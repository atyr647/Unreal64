// Static-mesh collision for line traces and component queries: convex hulls (planes in mesh space) of the components the Blueprints
// define, placed by the actor transform and the component hierarchy, filtered by the components' collision responses.
// Data (u64_meshes / u64_comp_defs) comes from the assets: mesh simple collision (BodySetup) and the SimpleConstructionScript.
// Not modelled: dynamically moved components (the door's timeline moves a component, see u64_comp_world), skeletal-mesh collision.
#include "u64_gas_core.h"
#include <math.h>
#include <stdio.h>

typedef struct { float r[9], t[3]; } xf_t;                                 // world = R * local + t (R includes scale), row-major
static void xf_identity(xf_t *x) { memset(x, 0, sizeof *x); x->r[0] = x->r[4] = x->r[8] = 1; }
static void xf_mul(const xf_t *a, const xf_t *b, xf_t *o)                  // a * b
{
  xf_t t;
  for (int i = 0; i < 3; i++) {
    for (int j = 0; j < 3; j++) t.r[i * 3 + j] = a->r[i * 3] * b->r[j] + a->r[i * 3 + 1] * b->r[3 + j] + a->r[i * 3 + 2] * b->r[6 + j];
    t.t[i] = a->r[i * 3] * b->t[0] + a->r[i * 3 + 1] * b->t[1] + a->r[i * 3 + 2] * b->t[2] + a->t[i];
  }
  *o = t;
}
// FRotationMatrix(FRotator(pitch, yaw, roll)) columns are the X, Y, Z axes: v_world = X*vx + Y*vy + Z*vz
static void xf_from(const float loc[3], const float rot[3], const float scale[3], xf_t *o)
{
  float p = rot[0] * 0.017453292f, y = rot[1] * 0.017453292f, r = rot[2] * 0.017453292f;
  float cp = cosf(p), sp = sinf(p), cy = cosf(y), sy = sinf(y), cr = cosf(r), sr = sinf(r);
  float X[3] = {cp * cy, cp * sy, sp};
  float Y[3] = {sr * sp * cy - cr * sy, sr * sp * sy + cr * cy, -sr * cp};
  float Z[3] = {-(cr * sp * cy + sr * sy), cy * sr - cr * sp * sy, cr * cp};
  for (int i = 0; i < 3; i++) { o->r[i * 3] = X[i] * scale[0]; o->r[i * 3 + 1] = Y[i] * scale[1]; o->r[i * 3 + 2] = Z[i] * scale[2]; o->t[i] = loc[i]; }
}
static int xf_inverse(const xf_t *x, xf_t *o)
{
  const float *m = x->r;
  float c0 = m[4] * m[8] - m[5] * m[7], c1 = m[5] * m[6] - m[3] * m[8], c2 = m[3] * m[7] - m[4] * m[6];
  float det = m[0] * c0 + m[1] * c1 + m[2] * c2;
  if (fabsf(det) < 1e-12f) return 0;
  float id = 1.0f / det;
  o->r[0] = c0 * id; o->r[1] = (m[2] * m[7] - m[1] * m[8]) * id; o->r[2] = (m[1] * m[5] - m[2] * m[4]) * id;
  o->r[3] = c1 * id; o->r[4] = (m[0] * m[8] - m[2] * m[6]) * id; o->r[5] = (m[2] * m[3] - m[0] * m[5]) * id;
  o->r[6] = c2 * id; o->r[7] = (m[1] * m[6] - m[0] * m[7]) * id; o->r[8] = (m[0] * m[4] - m[1] * m[3]) * id;
  for (int i = 0; i < 3; i++) o->t[i] = -(o->r[i * 3] * x->t[0] + o->r[i * 3 + 1] * x->t[1] + o->r[i * 3 + 2] * x->t[2]);
  return 1;
}
static void vget3(u64_obj *o, const char *n, float d[3], float dflt)
{
  u64_v *v = u64_ivar(o, n);
  for (int i = 0; i < 3; i++) d[i] = v->t == U64_VEC ? v->u.v[i] : dflt;
}
static void actor_xf(u64_obj *a, xf_t *o)
{
  float loc[3], rot[3], sc[3];
  vget3(a, "Location", loc, 0); vget3(a, "Rotation", rot, 0); vget3(a, "Scale", sc, 1);
  xf_from(loc, rot, sc, o);
}
static const u64_comp_def *find_def(const char *cls, const char *comp)
{
  for (const u64_comp_def *d = u64_comp_defs; d->cls; d++) if (!strcmp(d->comp, comp) && u64_class_is(cls, d->cls)) return d;
  return 0;
}
// A component's relative transform: the asset's, unless the game moved it (RelativeLocation set on the component object).
static void comp_local(u64_obj *actor, const u64_comp_def *d, xf_t *o)
{
  u64_obj *c = u64_comp(actor, d->comp).u.o;
  float loc[3] = {d->loc[0], d->loc[1], d->loc[2]}, rot[3] = {d->rot[0], d->rot[1], d->rot[2]};
  u64_v *rl = c ? u64_ivar(c, "RelativeLocation") : 0, *rr = c ? u64_ivar(c, "RelativeRotation") : 0;
  if (rl && rl->t == U64_VEC) memcpy(loc, rl->u.v, sizeof loc);
  if (rr && rr->t == U64_VEC) memcpy(rot, rr->u.v, sizeof rot);
  xf_from(loc, rot, d->scale, o);
}
void u64_comp_world(u64_obj *actor, const char *comp, float loc_out[3])      // world location of a component (also for components without collision data)
{
  xf_t m; actor_xf(actor, &m);
  const u64_comp_def *chain[8]; int n = 0; const char *name = comp;
  while (n < 8) { const u64_comp_def *d = find_def(u64_obj_class(actor), name); if (!d) break; chain[n++] = d; name = d->parent; if (!*name) break; }
  for (int i = n - 1; i >= 0; i--) { xf_t l; comp_local(actor, chain[i], &l); xf_mul(&m, &l, &m); }
  memcpy(loc_out, m.t, sizeof m.t);
}
static const u64_mesh_def *find_mesh(const char *name) { for (const u64_mesh_def *m = u64_meshes; m->name; m++) if (!strcmp(m->name, name)) return m; return 0; }

// Collision response of a component to a channel: explicit entry, else the project's channel default (DefaultEngine.ini), else block.
extern const u64_resp u64_channel_defaults[];
static int resp_in(int enabled, int nresp, const u64_resp *resp, const char *channel)
{
  if (enabled == 0) return 0;
  for (int i = 0; i < nresp; i++) if (!strcmp(resp[i].chan, channel)) return resp[i].resp;
  for (const u64_resp *r = u64_channel_defaults; r->chan; r++) if (!strcmp(r->chan, channel)) return r->resp;
  return 2;
}
static int response_of(const u64_comp_def *d, const char *channel) { return resp_in(d->enabled, d->nresp, d->resp, channel); }
int u64_comp_response(u64_obj *comp, const char *channel)                   // GetCollisionResponseToChannel: 0 ignore, 1 overlap, 2 block
{
  u64_obj *owner = comp ? u64_asobj(*u64_ivar(comp, "Owner")) : 0;
  if (!owner) return 0;
  const char *full = u64_obj_name(comp), *nm = strrchr(full, '.'); nm = nm ? nm + 1 : full;
  const u64_comp_def *d = find_def(u64_obj_class(owner), nm);
  if (d) return response_of(d, channel);
  for (const u64_skel_comp_def *sd = u64_skel_comps; sd->cls; sd++)                   // the character Mesh and capsule
    if (!strcmp(sd->comp, nm) && u64_class_is(u64_obj_class(owner), sd->cls)) return resp_in(sd->enabled, sd->nresp, sd->resp, channel);
  return 0;
}
// Ray (p + t d, t in [0,1]) against a convex hull in the hull's own space: plane clipping. Returns the entry t and plane index.
static int ray_hull(const float p[3], const float d[3], const u64_hull *h, float *t_out, int *plane)
{
  float t0 = 0, t1 = 1; int hit = -1;
  for (int i = 0; i < h->n; i++) {
    const float *pl = h->planes + i * 4;
    float denom = pl[0] * d[0] + pl[1] * d[1] + pl[2] * d[2], dist = pl[3] - (pl[0] * p[0] + pl[1] * p[1] + pl[2] * p[2]);
    if (fabsf(denom) < 1e-9f) { if (dist < 0) return 0; continue; }      // parallel: outside this plane means a miss
    float t = dist / denom;
    if (denom < 0) { if (t > t0) { t0 = t; hit = i; } }                  // entering
    else if (t < t1) t1 = t;                                              // leaving
    if (t0 > t1) return 0;
  }
  if (hit < 0) return 0;                                                  // started inside: no entry hit (the engine reports a start-penetrating hit; not needed by the callers)
  *t_out = t0; *plane = hit; return 1;
}
// ---- shapes of skeletal meshes: ray against a body in the body's own space (R rows map local -> mesh space: local = R^T (x - t))
static void to_local(const u64_phys_body *b, const float p[3], const float d[3], float lp[3], float ld[3])
{
  float q[3] = {p[0] - b->t[0], p[1] - b->t[1], p[2] - b->t[2]};
  for (int i = 0; i < 3; i++) { lp[i] = b->r[i] * q[0] + b->r[3 + i] * q[1] + b->r[6 + i] * q[2]; ld[i] = b->r[i] * d[0] + b->r[3 + i] * d[1] + b->r[6 + i] * d[2]; }
}
static int ray_body(const u64_phys_body *b, const float p[3], const float d[3], float *t_out, float n_out[3])
{
  float lp[3], ld[3]; to_local(b, p, d, lp, ld);
  float t = 2, n[3] = {0, 0, 1};
  if (b->kind == 0) {                                                       // box: slabs
    float h[3] = {b->a, b->b, b->c}, t0 = 0, t1 = 1; int hit = -1, sgn = 1;
    for (int i = 0; i < 3; i++) {
      if (fabsf(ld[i]) < 1e-9f) { if (fabsf(lp[i]) > h[i]) return 0; continue; }
      float ta = (-h[i] - lp[i]) / ld[i], tb = (h[i] - lp[i]) / ld[i]; int s_ = 1;
      if (ta > tb) { float x = ta; ta = tb; tb = x; s_ = -1; }
      if (ta > t0) { t0 = ta; hit = i; sgn = s_; }
      if (tb < t1) t1 = tb;
      if (t0 > t1) return 0;
    }
    if (hit < 0) return 0;                                                  // started inside: no entry
    t = t0; n[0] = n[1] = n[2] = 0; n[hit] = (float)-sgn;
  } else {                                                                  // sphere (r = a) or capsule (r = a, half cylinder length b, axis z)
    float r = b->a, hl = b->kind == 2 ? b->b : 0;
    double best = 2; float bn[3] = {0, 0, 1};
    for (int cap = -1; cap <= 1; cap += 2) {                                // the two end spheres (a sphere is a capsule of length 0: one sphere at the center)
      if (b->kind == 1 && cap > 0) break;
      double cz = b->kind == 1 ? 0 : cap * hl, ox = lp[0], oy = lp[1], oz = lp[2] - cz;
      double A = (double)ld[0] * ld[0] + (double)ld[1] * ld[1] + (double)ld[2] * ld[2], B = ox * ld[0] + oy * ld[1] + oz * ld[2], C = ox * ox + oy * oy + oz * oz - (double)r * r;
      double disc = B * B - A * C; if (A < 1e-12 || disc < 0) continue;
      double tt = (-B - sqrt(disc)) / A; if (tt < 0 || tt > 1) continue;
      double z = lp[2] + tt * ld[2];
      if (b->kind == 2 && ((cap > 0 && z < hl) || (cap < 0 && z > -hl))) continue;   // that part of the sphere is inside the cylinder
      if (tt < best) { best = tt; double px = lp[0] + tt * ld[0], py = lp[1] + tt * ld[1], pz = z - cz; double l = sqrt(px * px + py * py + pz * pz); bn[0] = (float)(px / l); bn[1] = (float)(py / l); bn[2] = (float)(pz / l); }
    }
    if (b->kind == 2) {                                                     // the cylinder
      double A = (double)ld[0] * ld[0] + (double)ld[1] * ld[1], B = lp[0] * ld[0] + lp[1] * ld[1], C = (double)lp[0] * lp[0] + (double)lp[1] * lp[1] - (double)r * r;
      double disc = B * B - A * C;
      if (A > 1e-12 && disc >= 0) {
        double tt = (-B - sqrt(disc)) / A, z = lp[2] + tt * ld[2];
        if (tt >= 0 && tt <= 1 && fabs(z) <= hl && tt < best) { best = tt; double px = lp[0] + tt * ld[0], py = lp[1] + tt * ld[1], l = sqrt(px * px + py * py); bn[0] = (float)(px / l); bn[1] = (float)(py / l); bn[2] = 0; }
      }
    }
    if (best > 1) return 0;
    t = (float)best; memcpy(n, bn, sizeof n);
  }
  *t_out = t;
  for (int i = 0; i < 3; i++) n_out[i] = b->r[i * 3] * n[0] + b->r[i * 3 + 1] * n[1] + b->r[i * 3 + 2] * n[2];   // normal back to mesh space
  return 1;
}
#include "u64_anim.h"
__attribute__((weak)) u64_an_inst *u64_cpp_body_pose_of(u64_obj *actor) { (void)actor; return 0; }        // the project port: the actor's third-person animation instance
// The body in the animated pose: bone component-space transform * element-in-bone transform (falls back to the reference pose when the bone is not in the pose)
static int posed_body(const u64_an_inst *an, const u64_phys_body *b, u64_phys_body *o)
{
  float bt[3], bq[4];
  if (!an || !u64_anim_bone_cs(an, b->bone, bt, bq)) return 0;
  *o = *b;
  float x = bq[0], y = bq[1], z = bq[2], w = bq[3], R[9] = {1 - 2 * (y * y + z * z), 2 * (x * y - z * w), 2 * (x * z + y * w), 2 * (x * y + z * w), 1 - 2 * (x * x + z * z), 2 * (y * z - x * w), 2 * (x * z - y * w), 2 * (y * z + x * w), 1 - 2 * (x * x + y * y)};
  for (int i = 0; i < 3; i++) { o->t[i] = bt[i] + R[i * 3] * b->rt[0] + R[i * 3 + 1] * b->rt[1] + R[i * 3 + 2] * b->rt[2];
    for (int j = 0; j < 3; j++) o->r[i * 3 + j] = R[i * 3] * b->rr[j] + R[i * 3 + 1] * b->rr[3 + j] + R[i * 3 + 2] * b->rr[6 + j]; }
  return 1;
}
static const u64_phys_mesh *find_phys(const char *name) { for (const u64_phys_mesh *m = u64_phys_meshes; m->name; m++) if (!strcmp(m->name, name)) return m; return 0; }

// ---- queries by collision profile (UWorld::LineTraceMultiByProfile)
// The profile's object type is the trace channel. A component answers min(profile response to the component's object type,
// component response to the trace channel): ignore < overlap < block. Static meshes are WorldStatic, character meshes and capsules Pawn.
// Hits are sorted by distance and the list ends at the first blocking one.
static int profile_response(const u64_profile_def *p, const char *object_type)
{
  if (p) for (int i = 0; i < p->nresp; i++) if (!strcmp(p->resp[i].chan, object_type)) return p->resp[i].resp;
  for (const u64_resp *r = u64_channel_defaults; r->chan; r++) if (!strcmp(r->chan, object_type)) return r->resp;
  return 2;                                                                       // engine channels default to block
}
int u64_ray_pawn_capsule(u64_obj *o, const float s[3], const float e[3], float *t, float n[3]);
int u64_trace_profile(const float s[3], const float e[3], const char *profile, u64_obj *ignore, u64_world_hit_t *out, int max)
{
  const u64_profile_def *pd = 0; const char *channel = profile;
  for (const u64_profile_def *p = u64_profiles; p->name; p++) if (!strcmp(p->name, profile)) { pd = p; channel = p->object_type; break; }
  float d[3] = {e[0] - s[0], e[1] - s[1], e[2] - s[2]};
  u64_world_hit_t found[32]; int nf = 0;
  int q_static = profile_response(pd, "WorldStatic"), q_pawn = profile_response(pd, "Pawn");
  for (const u64_world_mesh *wm = u64_world_meshes; wm->mesh && nf < 30; wm++) {      // placed static meshes: WorldStatic, default responses
    int resp = resp_in(2, 0, 0, channel); if (q_static < resp) resp = q_static;
    const u64_mesh_def *mesh = find_mesh(wm->mesh);
    if (resp == 0 || !mesh) continue;
    xf_t m, inv; xf_from(wm->loc, wm->rot, wm->scale, &m);
    if (!xf_inverse(&m, &inv)) continue;
    float lp[3], ld[3], best = 2; int bp = -1; const u64_hull *bh = 0;
    for (int k = 0; k < 3; k++) {
      lp[k] = inv.r[k * 3] * s[0] + inv.r[k * 3 + 1] * s[1] + inv.r[k * 3 + 2] * s[2] + inv.t[k];
      ld[k] = inv.r[k * 3] * d[0] + inv.r[k * 3 + 1] * d[1] + inv.r[k * 3 + 2] * d[2];
    }
    for (int hi = 0; hi < mesh->nhulls; hi++) { float t; int pi; if (ray_hull(lp, ld, &mesh->hulls[hi], &t, &pi) && t < best) { best = t; bp = pi; bh = &mesh->hulls[hi]; } }
    if (!bh) continue;
    u64_world_hit_t *h = &found[nf++]; h->actor = 0; h->comp = 0; h->t = best; h->blocking = resp == 2; h->bone = 0;
    for (int k = 0; k < 3; k++) h->loc[k] = s[k] + d[k] * best;
    const float *pl = bh->planes + bp * 4;
    float nw[3] = {inv.r[0] * pl[0] + inv.r[3] * pl[1] + inv.r[6] * pl[2], inv.r[1] * pl[0] + inv.r[4] * pl[1] + inv.r[7] * pl[2], inv.r[2] * pl[0] + inv.r[5] * pl[1] + inv.r[8] * pl[2]};
    float l = sqrtf(nw[0] * nw[0] + nw[1] * nw[1] + nw[2] * nw[2]); if (l < 1e-9f) l = 1;
    for (int k = 0; k < 3; k++) h->normal[k] = nw[k] / l;
  }
  for (int i = 0; i < u64_obj_count() && nf < 30; i++) {
    u64_obj *a = u64_obj_at(i);
    if (a == ignore || !u64_obj_is_actor(a)) continue;
    const char *cls = u64_obj_class(a);
    for (const u64_comp_def *def = u64_comp_defs; def->cls && nf < 30; def++) {         // static meshes
      if (!u64_class_is(cls, def->cls)) continue;
      int resp = response_of(def, channel); if (q_static < resp) resp = q_static;
      if (resp == 0) continue;
      const u64_mesh_def *mesh = find_mesh(def->mesh);
      if (!mesh) continue;
      xf_t m; actor_xf(a, &m);
      const u64_comp_def *chain[8]; int n = 0; const u64_comp_def *c = def;
      while (c && n < 8) { chain[n++] = c; c = *c->parent ? find_def(cls, c->parent) : 0; }
      for (int k = n - 1; k >= 0; k--) { xf_t l; comp_local(a, chain[k], &l); xf_mul(&m, &l, &m); }
      xf_t inv; if (!xf_inverse(&m, &inv)) continue;
      float lp[3], ld[3];
      for (int k = 0; k < 3; k++) {
        lp[k] = inv.r[k * 3] * s[0] + inv.r[k * 3 + 1] * s[1] + inv.r[k * 3 + 2] * s[2] + inv.t[k];
        ld[k] = inv.r[k * 3] * d[0] + inv.r[k * 3 + 1] * d[1] + inv.r[k * 3 + 2] * d[2];
      }
      float best = 2; int bp = -1; const u64_hull *bh = 0;
      for (int hi = 0; hi < mesh->nhulls; hi++) { float t; int pi; if (ray_hull(lp, ld, &mesh->hulls[hi], &t, &pi) && t < best) { best = t; bp = pi; bh = &mesh->hulls[hi]; } }
      if (!bh) continue;
      u64_world_hit_t *h = &found[nf++];
      h->actor = a; h->comp = u64_comp(a, def->comp).u.o; h->t = best; h->blocking = resp == 2; h->bone = 0;
      for (int k = 0; k < 3; k++) h->loc[k] = s[k] + d[k] * best;
      const float *pl = bh->planes + bp * 4;                              // normal of a plane under the transform: R^-T n
      float nw[3] = {inv.r[0] * pl[0] + inv.r[3] * pl[1] + inv.r[6] * pl[2], inv.r[1] * pl[0] + inv.r[4] * pl[1] + inv.r[7] * pl[2], inv.r[2] * pl[0] + inv.r[5] * pl[1] + inv.r[8] * pl[2]};
      float l = sqrtf(nw[0] * nw[0] + nw[1] * nw[1] + nw[2] * nw[2]); if (l < 1e-9f) l = 1;
      for (int k = 0; k < 3; k++) h->normal[k] = nw[k] / l;
    }
    int has_skel = 0;
    for (const u64_skel_comp_def *sd = u64_skel_comps; sd->cls && nf < 30; sd++) {      // character mesh bodies and the capsule
      if (!u64_class_is(cls, sd->cls)) continue;
      has_skel = 1;
      int resp = resp_in(sd->enabled, sd->nresp, sd->resp, channel); if (q_pawn < resp) resp = q_pawn;
      if (resp == 0) continue;
      u64_obj *comp = u64_comp(a, sd->comp).u.o;
      if (!sd->mesh[0]) {                                                             // the collision capsule
        float t, n[3]; if (!u64_ray_pawn_capsule(a, s, e, &t, n)) continue;
        u64_world_hit_t *h = &found[nf++]; h->actor = a; h->comp = comp; h->t = t; h->blocking = resp == 2; h->bone = 0;
        for (int k = 0; k < 3; k++) { h->loc[k] = s[k] + d[k] * t; h->normal[k] = n[k]; }
        continue;
      }
      const u64_phys_mesh *pm = find_phys(sd->mesh);
      if (!pm) continue;
      float loc[3] = {sd->loc[0], sd->loc[1], sd->loc[2]}, rot[3] = {sd->rot[0], sd->rot[1], sd->rot[2]};
      u64_v *rl = comp ? u64_ivar(comp, "RelativeLocation") : 0, *rr = comp ? u64_ivar(comp, "RelativeRotation") : 0;
      if (rl && rl->t == U64_VEC) memcpy(loc, rl->u.v, sizeof loc);
      if (rr && rr->t == U64_VEC) memcpy(rot, rr->u.v, sizeof rot);
      xf_t m, l; actor_xf(a, &m); xf_from(loc, rot, sd->scale, &l); xf_mul(&m, &l, &m);
      xf_t inv; if (!xf_inverse(&m, &inv)) continue;
      float lp[3], ld[3];                                                             // the ray in mesh space
      for (int k = 0; k < 3; k++) {
        lp[k] = inv.r[k * 3] * s[0] + inv.r[k * 3 + 1] * s[1] + inv.r[k * 3 + 2] * s[2] + inv.t[k];
        ld[k] = inv.r[k * 3] * d[0] + inv.r[k * 3 + 1] * d[1] + inv.r[k * 3 + 2] * d[2];
      }
      float best = 2, bn[3] = {0, 0, 1}; const char *bone = 0;
      u64_an_inst *pose = u64_cpp_body_pose_of(a);
      for (int bi = 0; bi < pm->n; bi++) { float t, n[3]; u64_phys_body pb; const u64_phys_body *bd = posed_body(pose, &pm->bodies[bi], &pb) ? &pb : &pm->bodies[bi]; if (ray_body(bd, lp, ld, &t, n) && t < best) { best = t; bone = pm->bodies[bi].bone; memcpy(bn, n, sizeof bn); } }
      if (!bone) continue;
      u64_world_hit_t *h = &found[nf++]; h->actor = a; h->comp = comp; h->t = best; h->blocking = resp == 2; h->bone = bone;
      for (int k = 0; k < 3; k++) h->loc[k] = s[k] + d[k] * best;
      for (int k = 0; k < 3; k++) h->normal[k] = m.r[k * 3] * bn[0] + m.r[k * 3 + 1] * bn[1] + m.r[k * 3 + 2] * bn[2];   // (rotation part; scale is 1 for characters)
    }
    if (!has_skel && nf < 30) {                                                       // a character without collision data (a Blueprint that does not set it): a default pawn capsule
      float t, n[3];
      int resp = resp_in(2, 0, 0, channel); if (q_pawn < resp) resp = q_pawn;
      if (resp > 0 && u64_ray_pawn_capsule(a, s, e, &t, n)) {
        u64_world_hit_t *h = &found[nf++]; h->actor = a; h->comp = u64_comp(a, "CapsuleComponent").u.o; h->t = t; h->blocking = resp == 2; h->bone = 0;
        for (int k = 0; k < 3; k++) { h->loc[k] = s[k] + d[k] * t; h->normal[k] = n[k]; }
      }
    }
  }
  for (int i = 1; i < nf; i++) for (int j = i; j > 0 && found[j].t < found[j - 1].t; j--) { u64_world_hit_t tmp = found[j]; found[j] = found[j - 1]; found[j - 1] = tmp; }
  int n = 0;
  for (int i = 0; i < nf && n < max; i++) { out[n++] = found[i]; if (found[i].blocking) break; }
  return n;
}
// A HitResult object for a hit of the query above (FHitResult: Time, Distance, Location, ImpactPoint, Normal, Actor, Component, BoneName, TraceStart / End)
u64_obj *u64_hit_result_new(const u64_world_hit_t *h, const float s[3], const float e[3])
{
  u64_obj *r = u64_new("HitResult", "HitResult");
  float d[3] = {e[0] - s[0], e[1] - s[1], e[2] - s[2]}, len = sqrtf(d[0] * d[0] + d[1] * d[1] + d[2] * d[2]);
  *u64_ivar(r, "Time") = u64_float(h->t); *u64_ivar(r, "Distance") = u64_float(h->t * len); *u64_ivar(r, "bBlockingHit") = u64_bool(h->blocking);
  *u64_ivar(r, "Location") = u64_vec(h->loc[0], h->loc[1], h->loc[2]); *u64_ivar(r, "Normal") = u64_vec(h->normal[0], h->normal[1], h->normal[2]);
  *u64_ivar(r, "HitActor") = u64_objv(h->actor); *u64_ivar(r, "HitComponent") = u64_objv(h->comp);
  *u64_ivar(r, "BoneName") = u64_name(h->bone ? h->bone : "None");
  *u64_ivar(r, "TraceStart") = u64_vec(s[0], s[1], s[2]); *u64_ivar(r, "TraceEnd") = u64_vec(e[0], e[1], e[2]);
  return r;
}

// World-space centre of a character's physics body on `bone` (first element), in the current animated pose
int u64_body_world_center(u64_obj *a, const char *bone, float out[3])
{
  for (const u64_skel_comp_def *sd = u64_skel_comps; sd->cls; sd++) {
    if (!sd->mesh[0] || !u64_class_is(u64_obj_class(a), sd->cls)) continue;
    const u64_phys_mesh *pm = find_phys(sd->mesh); if (!pm) continue;
    for (int bi = 0; bi < pm->n; bi++) {
      if (strcmp(pm->bodies[bi].bone, bone)) continue;
      u64_phys_body pb; const u64_phys_body *bd = posed_body(u64_cpp_body_pose_of(a), &pm->bodies[bi], &pb) ? &pb : &pm->bodies[bi];
      xf_t m, l; actor_xf(a, &m);
      float loc[3] = {sd->loc[0], sd->loc[1], sd->loc[2]}, rot[3] = {sd->rot[0], sd->rot[1], sd->rot[2]};
      xf_from(loc, rot, sd->scale, &l); xf_mul(&m, &l, &m);
      for (int k = 0; k < 3; k++) out[k] = m.r[k * 3] * bd->t[0] + m.r[k * 3 + 1] * bd->t[1] + m.r[k * 3 + 2] * bd->t[2] + m.t[k];
      return 1;
    }
  }
  return 0;
}

// ---------------------------------------------------------------------------------------------------- wireframe of the level's collision (the N64 build draws it as its 3D world)
// A convex hull's edges: vertices are the intersections of plane triples that lie inside every plane; two vertices form an edge when they share two planes. World = m * local.
static int hull_edges(const u64_hull *h, const xf_t *m, float (*out)[6], int max)
{
  float v[32][3]; unsigned long long on[32]; int nv = 0, ne = 0; int np = h->n < 24 ? h->n : 24;
  for (int i = 0; i < np; i++) for (int j = i + 1; j < np; j++) for (int k = j + 1; k < np; k++) {
    const float *a = h->planes + i * 4, *b = h->planes + j * 4, *c = h->planes + k * 4;
    float cx[3] = {b[1] * c[2] - b[2] * c[1], b[2] * c[0] - b[0] * c[2], b[0] * c[1] - b[1] * c[0]};
    float det = a[0] * cx[0] + a[1] * cx[1] + a[2] * cx[2]; if (fabsf(det) < 1e-6f) continue;
    float bc[3] = {c[1] * a[2] - c[2] * a[1], c[2] * a[0] - c[0] * a[2], c[0] * a[1] - c[1] * a[0]}, ca[3] = {a[1] * b[2] - a[2] * b[1], a[2] * b[0] - a[0] * b[2], a[0] * b[1] - a[1] * b[0]};
    float p[3]; for (int q = 0; q < 3; q++) p[q] = (a[3] * cx[q] + b[3] * bc[q] + c[3] * ca[q]) / det;
    unsigned long long mask = 0; int inside = 1;
    for (int q = 0; q < np && inside; q++) { const float *pl = h->planes + q * 4; float e = pl[0] * p[0] + pl[1] * p[1] + pl[2] * p[2] - pl[3]; if (e > 1e-2f) inside = 0; else if (fabsf(e) < 1e-2f) mask |= 1ull << q; }
    if (!inside) continue;
    int dup = 0; for (int q = 0; q < nv; q++) if (fabsf(v[q][0] - p[0]) + fabsf(v[q][1] - p[1]) + fabsf(v[q][2] - p[2]) < 1e-2f) { on[q] |= mask; dup = 1; break; }
    if (!dup && nv < 32) { memcpy(v[nv], p, sizeof p); on[nv++] = mask; }
  }
  for (int i = 0; i < nv; i++) for (int j = i + 1; j < nv && ne < max; j++) {
    unsigned long long both = on[i] & on[j]; if (!both || (both & (both - 1)) == 0) continue;          // share at least two planes
    float *o = out[ne++];
    for (int q = 0; q < 3; q++) { o[q] = m->r[q * 3] * v[i][0] + m->r[q * 3 + 1] * v[i][1] + m->r[q * 3 + 2] * v[i][2] + m->t[q]; o[3 + q] = m->r[q * 3] * v[j][0] + m->r[q * 3 + 1] * v[j][1] + m->r[q * 3 + 2] * v[j][2] + m->t[q]; }
  }
  return ne;
}
int u64_pawn_capsule(u64_obj *o, float c[3], float *r, float *seg);
// Every edge of the placed static meshes' and the actors' static-mesh components' collision hulls, then each live character's capsule (a prism: 8 sides, rings at the cylinder's ends and a cross for the caps).
int u64_world_segments(float (*out)[6], int max)
{
  int n = 0;
  for (const u64_world_mesh *wm = u64_world_meshes; wm->mesh && n < max; wm++) {
    const u64_mesh_def *mesh = find_mesh(wm->mesh); if (!mesh) continue;
    xf_t m; xf_from(wm->loc, wm->rot, wm->scale, &m);
    for (int hi = 0; hi < mesh->nhulls && n < max; hi++) n += hull_edges(&mesh->hulls[hi], &m, out + n, max - n);
  }
  for (int i = 0; i < u64_obj_count() && n < max; i++) {
    u64_obj *a = u64_obj_at(i); if (!u64_obj_alive(a) || !u64_obj_is_actor(a)) continue;
    const char *cls = u64_obj_class(a);
    for (const u64_comp_def *def = u64_comp_defs; def->cls && n < max; def++) {
      if (!u64_class_is(cls, def->cls)) continue;
      const u64_mesh_def *mesh = find_mesh(def->mesh); if (!mesh) continue;
      xf_t m; actor_xf(a, &m);
      const u64_comp_def *chain[8]; int k = 0; const u64_comp_def *c = def;
      while (c && k < 8) { chain[k++] = c; c = *c->parent ? find_def(cls, c->parent) : 0; }
      for (int q = k - 1; q >= 0; q--) { xf_t l; comp_local(a, chain[q], &l); xf_mul(&m, &l, &m); }
      for (int hi = 0; hi < mesh->nhulls && n < max; hi++) n += hull_edges(&mesh->hulls[hi], &m, out + n, max - n);
    }
    float c[3], r, seg;
    if (n + 24 <= max && u64_pawn_capsule(a, c, &r, &seg)) {
      float top = c[2] + seg + r, bot = c[2] - seg - r, ang[9];
      for (int q = 0; q <= 8; q++) ang[q] = q * 0.78539816f;
      for (int q = 0; q < 8; q++) {
        float x0 = c[0] + r * cosf(ang[q]), y0 = c[1] + r * sinf(ang[q]), x1 = c[0] + r * cosf(ang[q + 1]), y1 = c[1] + r * sinf(ang[q + 1]);
        float *o = out[n++]; o[0] = x0; o[1] = y0; o[2] = bot + r; o[3] = x1; o[4] = y1; o[5] = bot + r;
        o = out[n++]; o[0] = x0; o[1] = y0; o[2] = top - r; o[3] = x1; o[4] = y1; o[5] = top - r;
        o = out[n++]; o[0] = x0; o[1] = y0; o[2] = bot + r; o[3] = x0; o[4] = y0; o[5] = top - r;
      }
    }
  }
  return n;
}
