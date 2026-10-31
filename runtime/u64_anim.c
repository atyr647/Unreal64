// Animation Blueprint runtime (see u64_anim.h). Evaluates sequence players, blend lists, state machines (with crossfades) and bone-layer blends
// for the bone chain in the graph tables; montage slots pass the pose through (montage poses are not evaluated yet).
#include "u64_anim.h"
#include "u64_gas_core.h"
#include <math.h>
#include <string.h>

enum { N_ROOT, N_SLOT, N_SM, N_STATERESULT, N_SEQ, N_BLENDBOOL, N_BLENDENUM, N_LAYERED, N_TRANSRESULT };
#define MAX_NODES 64
struct u64_an_inst {
  const u64_an_graph *g; u64_obj *self; int used;
  float acc[MAX_NODES]; unsigned char inited[MAX_NODES];
  float bw[MAX_NODES][8]; int bcur[MAX_NODES];
  int m_cur[4], m_next[4]; float m_xf[4], m_xt[4];
  int mont; float mont_pos, mont_rate, mont_eval;                                        // the playing montage (-1 = none)
  u64_an_pose out;
};
static u64_an_inst insts[8];

// ---- quaternion helpers (xyzw)
static void qmul(const float a[4], const float b[4], float o[4])
{
  float x = a[3] * b[0] + a[0] * b[3] + a[1] * b[2] - a[2] * b[1], y = a[3] * b[1] - a[0] * b[2] + a[1] * b[3] + a[2] * b[0];
  float z = a[3] * b[2] + a[0] * b[1] - a[1] * b[0] + a[2] * b[3], w = a[3] * b[3] - a[0] * b[0] - a[1] * b[1] - a[2] * b[2];
  o[0] = x; o[1] = y; o[2] = z; o[3] = w;
}
static void qrot(const float q[4], const float v[3], float o[3])
{
  float tx = 2 * (q[1] * v[2] - q[2] * v[1]), ty = 2 * (q[2] * v[0] - q[0] * v[2]), tz = 2 * (q[0] * v[1] - q[1] * v[0]);
  o[0] = v[0] + q[3] * tx + (q[1] * tz - q[2] * ty); o[1] = v[1] + q[3] * ty + (q[2] * tx - q[0] * tz); o[2] = v[2] + q[3] * tz + (q[0] * ty - q[1] * tx);
}
static void slerp(const float a[4], const float b0[4], float t, float o[4])
{
  float b[4] = {b0[0], b0[1], b0[2], b0[3]}, dot = a[0] * b[0] + a[1] * b[1] + a[2] * b[2] + a[3] * b[3];
  if (dot < 0) { for (int i = 0; i < 4; i++) b[i] = -b[i]; dot = -dot; }
  float wa, wb;
  if (dot > 0.9995f) { wa = 1 - t; wb = t; }
  else { float th = acosf(dot), s = sinf(th); wa = sinf((1 - t) * th) / s; wb = sinf(t * th) / s; }
  float l = 0; for (int i = 0; i < 4; i++) { o[i] = wa * a[i] + wb * b[i]; l += o[i] * o[i]; }
  l = sqrtf(l); if (l > 0) for (int i = 0; i < 4; i++) o[i] /= l;
}
static void pose_blend(const u64_an_pose *a, const u64_an_pose *b, float t, int nb, u64_an_pose *o)
{
  for (int i = 0; i < nb; i++) {
    for (int k = 0; k < 3; k++) o->t[i][k] = a->t[i][k] + (b->t[i][k] - a->t[i][k]) * t;
    slerp(a->q[i], b->q[i], t, o->q[i]);
  }
}

static u64_v *node_field(u64_an_inst *a, const char *node, const char *field) { return u64_member(u64_ivar(a->self, node), field); }

static void sample_seq(const u64_an_inst *a, const u64_an_seq *s, float time, u64_an_pose *out)
{
  int nb = a->g->nbones;
  for (int i = 0; i < nb; i++) {
    const u64_an_track *tr = &s->tracks[i];
    memcpy(out->t[i], a->g->bones[i].t, sizeof out->t[i]); memcpy(out->q[i], a->g->bones[i].q, sizeof out->q[i]);
    float pos = s->length > 0 ? time / s->length : 0; if (pos < 0) pos = 0; if (pos > 1) pos = 1;
    if (tr->npos == 1) memcpy(out->t[i], tr->pos, sizeof out->t[i]);
    else if (tr->npos > 1) { float p = pos * (tr->npos - 1); int k = (int)p; if (k > tr->npos - 2) k = tr->npos - 2; float f = p - k;
      for (int c = 0; c < 3; c++) out->t[i][c] = tr->pos[k * 3 + c] + (tr->pos[(k + 1) * 3 + c] - tr->pos[k * 3 + c]) * f; }
    if (tr->nrot == 1) memcpy(out->q[i], tr->rot, sizeof out->q[i]);
    else if (tr->nrot > 1) { float p = pos * (tr->nrot - 1); int k = (int)p; if (k > tr->nrot - 2) k = tr->nrot - 2; slerp(tr->rot + k * 4, tr->rot + (k + 1) * 4, p - k, out->q[i]); }
  }
}
static void eval(u64_an_inst *a, int n, float dt, u64_an_pose *out);
static void reset_subtree(u64_an_inst *a, int n)
{
  const u64_an_node *nd = &a->g->nodes[n]; a->inited[n] = 0; a->acc[n] = 0;
  if (nd->kind == N_SM) return;                                                  // a machine keeps its own state
  for (int i = 0; i < nd->nlinks; i++) reset_subtree(a, nd->links[i]);
}
static int blend_child(u64_an_inst *a, const u64_an_node *nd, int n)                  // BlendListByBool: pose 0 = true; ByEnum: EnumToPoseIndex[value]
{
  u64_v *v = node_field(a, nd->name, nd->kind == N_BLENDBOOL ? "bActiveValue" : "ActiveEnumValue");
  if (nd->kind == N_BLENDBOOL) return u64_true(*v) ? 0 : 1;
  int e = v->t == U64_INT ? v->u.i : 0; if (e < 0 || e >= 8) e = 0;
  (void)n; return nd->emap[e];
}
static void eval_machine(u64_an_inst *a, const u64_an_node *nd, float dt, u64_an_pose *out)
{
  const u64_an_machine *m = &a->g->machines[nd->machine]; int mi = nd->machine;
  if (!a->inited[nd - a->g->nodes]) { a->inited[nd - a->g->nodes] = 1; a->m_cur[mi] = m->initial; a->m_next[mi] = -1; reset_subtree(a, m->states[m->initial].root); }
  int cur = a->m_cur[mi];
  if (a->m_next[mi] < 0) {                                                       // not crossfading: look at the current state's exit transitions
    const u64_an_state *st = &m->states[cur];
    for (int i = 0; i < st->nexits; i++) {
      u64_v *v = node_field(a, a->g->nodes[st->exits[i].node].name, "bCanEnterTransition");
      if (u64_true(*v) == st->exits[i].desired) {
        const u64_an_trans *tr = &m->trans[st->exits[i].index];
        a->m_next[mi] = tr->next; a->m_xf[mi] = 0; a->m_xt[mi] = tr->time; reset_subtree(a, m->states[tr->next].root); break;
      }
    }
  }
  if (a->m_next[mi] < 0) { eval(a, m->states[cur].root, dt, out); return; }
  int nx = a->m_next[mi]; u64_an_pose pa, pb;
  a->m_xf[mi] += dt; float al = a->m_xt[mi] > 0 ? a->m_xf[mi] / a->m_xt[mi] : 1; if (al > 1) al = 1;
  eval(a, m->states[cur].root, dt, &pa); eval(a, m->states[nx].root, dt, &pb);
  pose_blend(&pa, &pb, al, a->g->nbones, out);
  if (al >= 1) { a->m_cur[mi] = nx; a->m_next[mi] = -1; }
}
static void eval(u64_an_inst *a, int n, float dt, u64_an_pose *out)
{
  const u64_an_node *nd = &a->g->nodes[n]; int nb = a->g->nbones;
  switch (nd->kind) {
  case N_ROOT: case N_STATERESULT: eval(a, nd->links[0], dt, out); break;
  case N_SLOT: {                                                                                // a playing montage of this slot blends over the source pose
    eval(a, nd->links[0], dt, out);
    if (a->mont < 0) break;
    const u64_an_montage *m = &a->g->montages[a->mont];
    if (strcmp(m->slot, nd->slot)) break;
    float w = 1, pos = a->mont_pos; a->mont_eval = pos;
    if (m->blend_in > 0 && pos < m->blend_in) w = pos / m->blend_in;
    float rem = m->length - pos; if (m->blend_out > 0 && rem < m->blend_out) { float wo = rem / m->blend_out; if (wo < w) w = wo; }
    if (w < 0) w = 0; if (w > 1) w = 1;
    u64_an_pose mp; sample_seq(a, &a->g->seqs[m->seq], m->start + pos * m->rate, &mp);
    pose_blend(out, &mp, w, nb, out); break; }
  case N_SM: eval_machine(a, nd, dt, out); break;
  case N_SEQ: {
    const u64_an_seq *s = &a->g->seqs[nd->seq];
    if (a->inited[n]) a->acc[n] += dt * nd->rate; a->inited[n] = 1;
    if (s->length > 0) { if (nd->loop) a->acc[n] = fmodf(a->acc[n], s->length); else if (a->acc[n] > s->length) a->acc[n] = s->length; }
    sample_seq(a, s, a->acc[n], out); break; }
  case N_BLENDBOOL: case N_BLENDENUM: {
    int tgt = blend_child(a, nd, n), nc = nd->nlinks; float *w = a->bw[n];
    if (!a->inited[n]) { a->inited[n] = 1; for (int i = 0; i < 8; i++) w[i] = i == tgt; a->bcur[n] = tgt; }
    for (int i = 0; i < nc; i++) { float T = nd->times[i] > 0 ? nd->times[i] : 1e-4f;
      if (i == tgt) { w[i] += dt / T; if (w[i] > 1) w[i] = 1; } else { w[i] -= dt / T; if (w[i] < 0) w[i] = 0; } }
    float tot = 0; for (int i = 0; i < nc; i++) tot += w[i]; if (tot <= 0) { w[tgt] = tot = 1; }
    u64_an_pose acc; float used = 0; int first = 1;
    for (int i = 0; i < nc; i++) {
      if (w[i] <= 0) continue;
      u64_an_pose p; eval(a, nd->links[i], dt, &p); float wi = w[i] / tot;
      if (first) { acc = p; first = 0; used = wi; } else { pose_blend(&acc, &p, wi / (used + wi), nb, &acc); used += wi; }
    }
    *out = acc; break; }
  case N_LAYERED: eval(a, nd->links[0], dt, out); break;                                      // no branch filters in the project's graphs: the base pose
  default: for (int i = 0; i < nb; i++) { memcpy(out->t[i], a->g->bones[i].t, 12); memcpy(out->q[i], a->g->bones[i].q, 16); }
  }
}

int u64_anim_montage_play(u64_an_inst *a, const char *montage, float rate)
{
  for (int i = 0; i < a->g->nmontages; i++) if (!strcmp(a->g->montages[i].name, montage)) { a->mont = i; a->mont_pos = 0; a->mont_rate = rate > 0 ? rate : 1; return 1; }
  return 0;
}
u64_an_inst *u64_anim_new(const u64_an_graph *g, u64_obj *instance)
{
  for (int i = 0; i < 8; i++) if (!insts[i].used) { memset(&insts[i], 0, sizeof insts[i]); insts[i].used = 1; insts[i].g = g; insts[i].self = instance; insts[i].mont = -1; return &insts[i]; }
  return 0;
}
void u64_anim_update(u64_an_inst *a, float dt)
{
  const u64_an_graph *g = a->g; u64_v d = u64_float(dt);
  u64_call(a->self, "BlueprintUpdateAnimation", 1, &d);                                          // the Blueprint sets Speed, IsInAir, ... from the pawn
  for (int i = 0; i < g->ncopies; i++) *node_field(a, g->copies[i].node, g->copies[i].field) = *u64_ivar(a->self, g->copies[i].var);   // property access copies
  for (int i = 0; i < g->nfuncs; i++) u64_call(a->self, g->funcs[i], 0, 0);                    // exposed inputs (transition rules)
  int root = -1; for (int i = 0; i < g->nnodes; i++) if (g->nodes[i].kind == N_ROOT) root = i;
  if (root >= 0) eval(a, root, dt, &a->out);
  if (a->mont >= 0) { a->mont_pos += dt * a->mont_rate; if (a->mont_pos >= g->montages[a->mont].length) a->mont = -1; }    // after the pose: the first frame of a montage is at position 0
}
const u64_an_pose *u64_anim_pose(const u64_an_inst *a) { return &a->out; }
int u64_anim_bone_cs(const u64_an_inst *a, const char *bone, float t[3], float q[4])
{
  float ct[U64_AN_MAXBONES][3], cq[U64_AN_MAXBONES][4]; const u64_an_graph *g = a->g;
  for (int i = 0; i < g->nbones; i++) {
    int p = g->bones[i].parent;
    if (p < 0) { memcpy(ct[i], a->out.t[i], 12); memcpy(cq[i], a->out.q[i], 16); }
    else { float r[3]; qrot(cq[p], a->out.t[i], r); for (int k = 0; k < 3; k++) ct[i][k] = ct[p][k] + r[k]; qmul(cq[p], a->out.q[i], cq[i]); }
    if (!strcmp(g->bones[i].name, bone)) { memcpy(t, ct[i], 12); memcpy(q, cq[i], 16); return 1; }
  }
  return 0;
}
const u64_an_graph *u64_anim_find_graph(const char *abp) { for (const u64_an_graph *const *g = u64_an_graphs; *g; g++) if (!strcmp((*g)->name, abp)) return *g; return 0; }
float u64_anim_debug_acc(const u64_an_inst *a, int n) { return a->acc[n]; }
// GetInstanceAssetPlayerTimeFromEnd(AssetPlayerIndex): the index counts AnimNodeProperties from the other end (position N-1-index in the graph order)
float u64_anim_time_from_end(u64_obj *instance, int index)
{
  for (int i = 0; i < 8; i++) {
    u64_an_inst *a = &insts[i];
    if (!a->used || a->self != instance) continue;
    int n = a->g->nnodes - 1 - index;
    if (n < 0 || n >= a->g->nnodes || a->g->nodes[n].kind != N_SEQ) return 0;
    const u64_an_seq *s = &a->g->seqs[a->g->nodes[n].seq];
    float t = s->length - a->acc[n]; return t < 0 ? 0 : t;
  }
  return 0;
}
void u64_anim_free(u64_an_inst *a) { if (a) a->used = 0; }
float u64_anim_montage_eval_pos(const u64_an_inst *a) { return a->mont >= 0 ? a->mont_eval : -1.0f; }   // position of the playing montage in the last evaluated pose
