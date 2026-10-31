// Gameplay Ability System core. Ported from Unreal's GAS semantics and the project's C++.
// Attribute value = aggregate(base, active modifiers); instant/periodic effects change the base value; Pre/Post attribute
// callbacks follow GDAttributeSetBase; death follows GDCharacterBase::Die/FinishDying.
#include "u64_gas_core.h"
#include <math.h>
#include <stdio.h>

#define MAX_ASC 160
#define MAX_ATTR 48
#define MAX_TAGS 160
#define MAX_ACTIVE 192
#define MAX_ABIL 48

typedef struct { const char *name; float base, cur; } attr_t;
typedef struct { const char *tag; int n; } tagc_t;
typedef struct {
  int used; u64_obj *obj, *owner;
  attr_t a[MAX_ATTR]; int na;
  tagc_t t[MAX_TAGS]; int nt;
  u64_obj *abil[MAX_ABIL]; int nabil;
  int in_inhibit_update;
} asc_t;
typedef struct {
  int live; const u64_ge_def *def; asc_t *target, *source; float level; int stack;
  float remaining, next_period; int inhibited, exec_pending; u64_obj *spec;
  struct { const char *tag; float v; } sbc[4]; int nsbc;
} active_t;

static asc_t ascs[MAX_ASC];
static active_t active[MAX_ACTIVE];
const char *u64_intern(const char *s);

// ---------------------------------------------------------------------------------------------------- arrays
#define ARR_POOL 32768
#define ARR_MAX 2048
static u64_v arr_items[ARR_POOL]; static int arr_used;
static struct { int off, n, cap, shared; } arrs[ARR_MAX]; static int narr;
static int arr_free_ids[ARR_MAX], n_arr_free;
static unsigned char arr_marked[ARR_MAX];
static u64_v const_cache[1024];                                        // shared constant arrays (class defaults)
static u64_v arr_new_cap(int cap)
{
  u64_v r = {U64_ARRAY};
  if ((!n_arr_free && narr >= ARR_MAX) || arr_used + cap > ARR_POOL) { if (!u64_rt_overflows++ && u64_log) u64_log("  [error] array pool exhausted"); return (u64_v){U64_NONE}; }
  int k = n_arr_free ? arr_free_ids[--n_arr_free] : narr++;
  arrs[k].off = arr_used; arrs[k].n = 0; arrs[k].cap = cap; arrs[k].shared = 0; arr_used += cap;
  r.u.i = k + 1; return r;
}
// Garbage collection of arrays: mark what the object graph reaches, free the rest and compact the item pool (ids stay stable).
void u64_array_gc_begin(void) { memset(arr_marked, 0, sizeof arr_marked); }
void u64_array_gc_mark(u64_v a)
{
  int k = a.u.i - 1;
  if (k < 0 || k >= narr || arr_marked[k] || arrs[k].off < 0) return;
  arr_marked[k] = 1;
  for (int i = 0; i < arrs[k].n; i++) u64_gc_mark_v(arr_items[arrs[k].off + i]);
}
void u64_array_gc_sweep(void)
{
  int live[ARR_MAX], nl = 0;
  for (int k = 0; k < narr; k++) {
    if (arrs[k].off < 0) continue;
    if (!arr_marked[k]) { arrs[k].off = -1; arr_free_ids[n_arr_free++] = k; continue; }
    int j = nl++; while (j > 0 && arrs[live[j - 1]].off > arrs[k].off) { live[j] = live[j - 1]; j--; } live[j] = k;
  }
  int top = 0;
  for (int i = 0; i < nl; i++) { int k = live[i]; if (arrs[k].off != top) memmove(&arr_items[top], &arr_items[arrs[k].off], (size_t)arrs[k].cap * sizeof(u64_v)); arrs[k].off = top; top += arrs[k].cap; }
  arr_used = top;
}
int u64_array_pressure(void) { return arr_used > ARR_POOL * 5 / 8 || (narr - n_arr_free) > ARR_MAX * 5 / 8; }
u64_v u64_array_of(int n, const u64_v *items)
{
  u64_v a = arr_new_cap(n < 4 ? 4 : n * 2);
  if (a.t != U64_ARRAY) return a;
  for (int i = 0; i < n; i++) arr_items[arrs[a.u.i - 1].off + i] = items[i];
  arrs[a.u.i - 1].n = n; return a;
}
int u64_array_len(u64_v a) { return a.t == U64_ARRAY && arrs[a.u.i - 1].off >= 0 ? arrs[a.u.i - 1].n : 0; }
u64_v u64_array_get(u64_v a, int i)
{
  if (a.t != U64_ARRAY || i < 0 || i >= arrs[a.u.i - 1].n) return (u64_v){U64_NONE};
  return arr_items[arrs[a.u.i - 1].off + i];
}
// Constant arrays (class defaults) are created once per call site and shared; pushing onto one copies it first.
u64_v u64_array_const(int id, int n, const u64_v *items)
{
  u64_v *cache = const_cache;
  if (id >= 0 && id < 1024 && cache[id].t == U64_ARRAY) return cache[id];
  u64_v a = u64_array_of(n, items);
  if (a.t == U64_ARRAY) arrs[a.u.i - 1].shared = 1;
  if (id >= 0 && id < 1024) cache[id] = a;
  return a;
}
void u64_array_push(u64_v *a, u64_v item)
{
  if (a->t == U64_ARRAY && arrs[a->u.i - 1].shared) {                 // copy-on-write
    u64_v old = *a; *a = u64_array_of(u64_array_len(old), &arr_items[arrs[old.u.i - 1].off]);
  }
  if (a->t != U64_ARRAY) *a = arr_new_cap(8);
  if (a->t != U64_ARRAY) return;
  int k = a->u.i - 1;
  if (arrs[k].n >= arrs[k].cap) {                       // grow by moving to a fresh block
    int old = arrs[k].off, n = arrs[k].n;
    if (arr_used + n * 2 > ARR_POOL) return;
    arrs[k].off = arr_used; arr_used += n * 2; arrs[k].cap = n * 2;
    for (int i = 0; i < n; i++) arr_items[arrs[k].off + i] = arr_items[old + i];
  }
  arr_items[arrs[k].off + arrs[k].n++] = item;
}

// ---------------------------------------------------------------------------------------------------- helpers
static const char *nm(u64_v v)
{
  if (v.t == U64_CLASS || v.t == U64_NAME || v.t == U64_STR) return v.u.s ? v.u.s : "";
  if (v.t == U64_OBJ && v.u.o) return u64_obj_name(v.u.o);
  return "";
}
static const char *strip_c(const char *s) { static char b[8][64]; static int i; char *o = b[i++ & 7]; snprintf(o, 64, "%s", s); size_t n = strlen(o); if (n > 2 && !strcmp(o + n - 2, "_C")) o[n - 2] = 0; return o; }
static const u64_ge_def *dyn_defs[48]; static int ndyn;
int u64_ge_register(const u64_ge_def *d)                    // effects built at run time; registering a name again replaces its definition
{
  for (int i = 0; i < ndyn; i++) if (!strcmp(dyn_defs[i]->name, d->name)) { dyn_defs[i] = d; return 1; }
  if (ndyn >= 48) { if (!u64_rt_overflows++ && u64_log) u64_log("  [error] runtime effect table full"); return 0; }
  dyn_defs[ndyn++] = d; return 1;
}
static const u64_ge_def *find_ge(const char *name)
{
  const char *n = strip_c(name);
  for (int i = 0; i < ndyn; i++) if (!strcmp(dyn_defs[i]->name, n)) return dyn_defs[i];
  for (int i = 0; i < u64_ge_count; i++) if (!strcmp(u64_ge_defs[i].name, n)) return &u64_ge_defs[i];
  return 0;
}
static int tag_match(const char *have, const char *query)   // GAS hierarchical match: "A.B" matches query "A"
{
  size_t q = strlen(query);
  return !strncmp(have, query, q) && (have[q] == 0 || have[q] == '.');
}
static asc_t *asc_from(u64_obj *o)
{
  if (!o) return 0;
  u64_v *idx = u64_ivar(o, "asc_idx");
  return idx->t == U64_INT && idx->u.i > 0 ? &ascs[idx->u.i - 1] : 0;
}
static attr_t *attr(asc_t *a, const char *n)
{
  for (int i = 0; i < a->na; i++) if (!strcmp(a->a[i].name, n)) return &a->a[i];
  if (a->na >= MAX_ATTR) { if (!u64_rt_overflows++ && u64_log) u64_log("  [error] attribute list full"); return &a->a[MAX_ATTR - 1]; }
  a->a[a->na] = (attr_t){u64_intern(n), 0, 0};
  return &a->a[a->na++];
}
static int tag_count(asc_t *a, const char *q) { int n = 0; for (int i = 0; i < a->nt; i++) if (a->t[i].n > 0 && tag_match(a->t[i].tag, q)) n += a->t[i].n; return n; }
int u64_tag_has(u64_obj *asc, const char *q) { asc_t *a = asc_from(asc); return a && tag_count(a, q) > 0; }
static int has_any(asc_t *a, const char *const *tags) { for (int i = 0; tags && tags[i]; i++) if (tag_count(a, tags[i])) return 1; return 0; }
static int has_all(asc_t *a, const char *const *tags) { for (int i = 0; tags && tags[i]; i++) if (!tag_count(a, tags[i])) return 0; return 1; }

static void update_inhibition(asc_t *a);
static void cancel_abilities(asc_t *a, const char *include, const char *ignore, u64_obj *except);
static void tag_add(asc_t *a, const char *tag, int d)
{
  tagc_t *t = 0;
  for (int i = 0; i < a->nt; i++) if (!strcmp(a->t[i].tag, tag)) { t = &a->t[i]; break; }
  if (!t) { if (a->nt >= MAX_TAGS) { if (!u64_rt_overflows++ && u64_log) u64_log("  [error] tag list full"); return; } t = &a->t[a->nt++]; t->tag = u64_intern(tag); t->n = 0; }
  int before = t->n; t->n += d; if (t->n < 0) t->n = 0;
  if (before == 0 && t->n > 0 && !strcmp(tag, "State.Debuff.Stun"))      // AGDMinionCharacter::StunTagChanged
    cancel_abilities(a, "Ability", "Ability.NotCanceledByStun", 0);
  if (!a->in_inhibit_update) update_inhibition(a);
  u64_notify_tag_change(a->obj);
}

// ---------------------------------------------------------------------------------------------------- attributes
static float eval_mag(const active_t *e, const u64_modinfo *m, asc_t *tgt, asc_t *src);
static float aggregate(asc_t *a, const char *name, float base)
{
  float add = 0, mul = 1, div = 1, over = 0; int has_over = 0;
  for (int i = 0; i < MAX_ACTIVE; i++) {
    const active_t *e = &active[i];
    if (!e->live || e->target != a || e->inhibited || e->def->dur == U64_DUR_INSTANT || e->def->period > 0) continue;
    for (int k = 0; k < e->def->nmods; k++) {
      const u64_modinfo *m = &e->def->mods[k];
      if (strcmp(m->attr, name)) continue;
      float mag = eval_mag(e, m, a, e->source), s = (float)e->stack;
      switch (m->op) {
        case U64_OP_ADD: add += mag * s; break;
        case U64_OP_MUL: mul *= 1 + (mag - 1) * s; break;
        case U64_OP_DIV: div *= 1 + (mag - 1) * s; break;
        default: over = mag; has_over = 1;
      }
    }
  }
  return has_over ? over : (base + add) * mul / (div != 0 ? div : 1);
}
static void recompute(asc_t *a)
{
  for (int pass = 0; pass < 2; pass++)       // attribute-based modifiers read other attributes' current values
    for (int i = 0; i < a->na; i++) a->a[i].cur = aggregate(a, a->a[i].name, a->a[i].base);
}
float u64_attr_get(u64_obj *o, const char *n) { asc_t *a = asc_from(o); return a ? attr(a, n)->cur : 0; }

static float eval_mag(const active_t *e, const u64_modinfo *m, asc_t *tgt, asc_t *src)
{
  switch (m->calc) {
    case U64_CALC_ATTR: {
      asc_t *from = m->backing_target ? tgt : src;
      float v = from ? attr(from, m->backing)->cur : 0;
      return m->coef * (v + m->pre) + m->post;
    }
    case U64_CALC_SETBYCALLER:
      for (int i = 0; i < e->nsbc; i++) if (!strcmp(e->sbc[i].tag, m->sbc)) return e->sbc[i].v;
      return 0;
    default: return m->value;
  }
}

static void fire_attr_change(asc_t *a, const char *name, float nv, float old);
static void post_gameplay_effect_execute(asc_t *tgt, const char *name, asc_t *src);

static float apply_op(float base, int op, float mag)
{
  switch (op) { case U64_OP_ADD: return base + mag; case U64_OP_MUL: return base * mag; case U64_OP_DIV: return mag != 0 ? base / mag : base; default: return mag; }
}
// GDAttributeSetBase::AdjustAttributeForMaxChange (applies ApplyModToAttributeUnsafe: additive on the base value)
static void adjust_for_max_change(asc_t *a, const char *affected, const char *maxattr, float new_max)
{
  float cur_max = attr(a, maxattr)->cur;
  if (fabsf(cur_max - new_max) > 1e-4f) {
    float cur = attr(a, affected)->cur;
    float delta = cur_max > 0.f ? (cur * new_max / cur_max) - cur : new_max;
    attr(a, affected)->base += delta; recompute(a);
  }
}
const u64_gas_hooks *u64_gas_hooks_active;                   // a project port replaces the attribute-set rules (default: GDAttributeSetBase)
void u64_adjust_for_max_change(u64_obj *asc, const char *affected, const char *maxattr, float new_max)
{
  asc_t *a = asc_from(asc); if (a) adjust_for_max_change(a, affected, maxattr, new_max);
}
// GDAttributeSetBase::PreAttributeChange
static void pre_attribute_change(asc_t *a, const char *name, float *nv)
{
  if (u64_gas_hooks_active && u64_gas_hooks_active->pre_attribute_change) { u64_gas_hooks_active->pre_attribute_change(a->obj, name, nv); return; }
  if (!strcmp(name, "MaxHealth")) adjust_for_max_change(a, "Health", "MaxHealth", *nv);
  else if (!strcmp(name, "MaxMana")) adjust_for_max_change(a, "Mana", "MaxMana", *nv);
  else if (!strcmp(name, "MaxStamina")) adjust_for_max_change(a, "Stamina", "MaxStamina", *nv);
  else if (!strcmp(name, "MoveSpeed")) { if (*nv < 150) *nv = 150; if (*nv > 1000) *nv = 1000; }   // [150, 1000]
}
// Execute a modifier against the base value (instant effects, periodic ticks, execution outputs).
static void execute_mod(asc_t *tgt, asc_t *src, const char *name, int op, float mag)
{
  attr_t *at = attr(tgt, name);
  float old = at->cur;
  float nb = apply_op(at->base, op, mag);
  float nv = aggregate(tgt, name, nb);
  float requested = nv;
  pre_attribute_change(tgt, name, &nv);
  at = attr(tgt, name);                       // the pointer stays valid; adjust_for_max_change may have touched others
  at->base = nb + (nv - requested);
  recompute(tgt);
  if (at->cur != old) fire_attr_change(tgt, name, at->cur, old);
  post_gameplay_effect_execute(tgt, name, src);
}
static void set_base(asc_t *a, const char *name, float v)   // attribute setter used by the C++ (SetHealth etc.)
{
  attr_t *at = attr(a, name); float old = at->cur;
  at->base = v; recompute(a);
  if (at->cur != old) fire_attr_change(a, name, at->cur, old);
}

// ---------------------------------------------------------------------------------------------------- attribute-change listeners
static void fire_attr_change(asc_t *a, const char *name, float nv, float old)
{
  u64_v args[3] = {u64_name(name), u64_float(nv), u64_float(old)};
  u64_notify_attribute_tasks(a->obj, 3, args);
  if (!strcmp(name, "Health") && nv <= 0 && a->owner && !tag_count(a, "State.Dead") && u64_obj_alive(a->owner)) {
    if (u64_gas_hooks_active && u64_gas_hooks_active->zero_health) u64_gas_hooks_active->zero_health(a->owner);
    else u64_die(a->owner);        // AGDMinionCharacter::HealthChanged / AGDPlayerState::HealthChanged: !IsAlive && !Dead -> Die()
  }
}

// ---------------------------------------------------------------------------------------------------- effects
static int requirements_met(asc_t *a, const char *const *require, const char *const *ignore)
{
  return has_all(a, require) && !has_any(a, ignore);
}
static void grant_tags(asc_t *a, const u64_ge_def *d, int delta) { for (int i = 0; d->granted && d->granted[i]; i++) tag_add(a, d->granted[i], delta); }

static void update_inhibition(asc_t *a)
{
  if (a->in_inhibit_update) return;
  a->in_inhibit_update = 1;
  int changed;
  do {
    changed = 0;
    for (int i = 0; i < MAX_ACTIVE; i++) {
      active_t *e = &active[i];
      if (!e->live || e->target != a) continue;
      int inh = !requirements_met(a, e->def->ongoing_require, e->def->ongoing_ignore);
      if (inh != e->inhibited) {
        e->inhibited = inh; changed = 1;
        grant_tags(a, e->def, inh ? -1 : 1);     // an inhibited effect neither modifies nor grants tags
      }
    }
  } while (changed);
  a->in_inhibit_update = 0;
  recompute(a);
}

static float spec_sbc(u64_obj *spec, const char *tag, int *found)
{
  *found = 0;
  if (!spec) return -1.0f;
  int n = u64_ivar(spec, "sbcN")->u.i;
  for (int i = 0; i < n; i++) {
    char k[16]; snprintf(k, sizeof k, "sbc%d_tag", i);
    if (!strcmp(u64_ivar(spec, u64_intern(k))->u.s ? u64_ivar(spec, u64_intern(k))->u.s : "", tag)) {
      snprintf(k, sizeof k, "sbc%d_val", i);
      *found = 1; return u64_ivar(spec, u64_intern(k))->u.f;
    }
  }
  return -1.0f;
}

// UGDDamageExecCalculation::Execute_Implementation
static void exec_damage(asc_t *tgt, asc_t *src, u64_obj *spec)
{
  float armor = attr(tgt, "Armor")->cur;                  // captured from the Target, not snapshotted
  if (armor < 0) armor = 0;
  float damage = src ? attr(src, "Damage")->cur : 0;      // captured from the Source (snapshot taken when the spec was made)
  if (spec && u64_ivar(spec, "snapDamage")->t == U64_FLOAT) damage = u64_ivar(spec, "snapDamage")->u.f;
  int found; float sbc = spec_sbc(spec, "Data.Damage", &found);
  damage += sbc > 0 ? sbc : 0;                            // + max(SetByCaller Data.Damage, 0)
  float unmitigated = damage;
  // UGSDamageExecutionCalc: effects tagged Effect.Damage.CanHeadShot do 1.5x when the context's hit is on bone b_head (the hit
  // result's BoneName; traces report "None" until body shapes from the physics asset are modelled) and add Effect.Damage.HeadShot.
  if (u64_gas_hooks_active && spec && u64_ivar(spec, "GEClass")->t != U64_NONE) {
    const u64_ge_def *gd = find_ge(nm(*u64_ivar(spec, "GEClass")));
    u64_obj *ctx = u64_ivar(spec, "Context")->t == U64_OBJ ? u64_ivar(spec, "Context")->u.o : 0;
    u64_obj *hit = ctx && u64_ivar(ctx, "HitResult")->t == U64_OBJ ? u64_ivar(ctx, "HitResult")->u.o : 0;
    int can = 0; for (int k = 0; gd && gd->asset_tags && gd->asset_tags[k]; k++) if (!strcmp(gd->asset_tags[k], "Effect.Damage.CanHeadShot")) can = 1;
    if (can && hit && !strcmp(nm(*u64_ivar(hit, "BoneName")), "b_head")) { unmitigated *= 1.5f; *u64_ivar(spec, "HeadShot") = u64_bool(1); }
  }
  float mitigated = unmitigated * (100.0f / (100.0f + armor));
  if (mitigated > 0.f) execute_mod(tgt, src, "Damage", U64_OP_ADD, mitigated);
  // UGDAbilitySystemComponent::ReceiveDamage -> OnDamage on WaitReceiveDamage tasks
  u64_v args[3] = {src ? u64_objv(src->owner) : u64_objv(0), u64_float(unmitigated), u64_float(mitigated)};
  u64_notify_damage_tasks(tgt->obj, 3, args);
}

static void run_modifiers(asc_t *tgt, asc_t *src, const u64_ge_def *d, active_t *e, u64_obj *spec)
{
  active_t tmp; memset(&tmp, 0, sizeof tmp); tmp.def = d; tmp.target = tgt; tmp.source = src; tmp.stack = 1;
  if (!e) { e = &tmp;
    for (int i = 0; i < d->nmods; i++) if (d->mods[i].calc == U64_CALC_SETBYCALLER && e->nsbc < 4) {
      int f; float v = spec_sbc(spec, d->mods[i].sbc, &f); e->sbc[e->nsbc].tag = d->mods[i].sbc; e->sbc[e->nsbc++].v = v; } }
  for (int i = 0; i < d->nmods; i++) {
    const u64_modinfo *m = &d->mods[i];
    execute_mod(tgt, src, m->attr, m->op, eval_mag(e, m, tgt, src) * (float)(e->stack ? e->stack : 1));
  }
  for (int i = 0; d->execs && d->execs[i]; i++)
    if (!strcmp(d->execs[i], "GDDamageExecCalculation") || !strcmp(d->execs[i], "GSDamageExecutionCalc")) exec_damage(tgt, src, spec);
    else if (u64_log) u64_log("  [gas] unported execution calculation %s", d->execs[i]);
}

static void remove_active_idx(int i)
{
  active_t *e = &active[i];
  if (!e->live) return;
  asc_t *a = e->target;
  e->live = 0;
  if (!e->inhibited) grant_tags(a, e->def, -1);
  recompute(a);
  update_inhibition(a);
  u64_notify_stack_tasks(a->obj, e->def->name);
}
void u64_remove_ge(int handle) { if (handle > 0 && handle <= MAX_ACTIVE) remove_active_idx(handle - 1); }

static void apply_conditionals(asc_t *tgt, asc_t *src, const u64_ge_def *d, float level)   // FConditionalGameplayEffect
{
  for (int i = 0; d->cond_effects && d->cond_effects[i]; i++) {
    char req[160]; snprintf(req, sizeof req, "%s", d->cond_required && d->cond_required[i] ? d->cond_required[i] : "");
    int ok = 1;
    for (char *tok = strtok(req, "|"); tok; tok = strtok(0, "|")) if (!src || !tag_count(src, tok)) ok = 0;
    if (ok) u64_apply_ge(tgt->obj, src ? src->obj : tgt->obj, d->cond_effects[i], level, 0);
  }
}

int u64_apply_ge(u64_obj *tasc, u64_obj *sasc, const char *ge_name, float level, u64_obj *spec)
{
  asc_t *tgt = asc_from(tasc), *src = asc_from(sasc ? sasc : tasc);
  const u64_ge_def *d = find_ge(ge_name);
  if (!tgt || !d) { if (u64_log) u64_log("  [gas] cannot apply %s (%s)", ge_name, d ? "no target" : "no such effect"); return 0; }
  if (!requirements_met(tgt, d->app_require, d->app_ignore)) return 0;               // ApplicationTagRequirements
  if (u64_log) u64_log("  [gas] %s <- %s", u64_obj_name(tgt->owner), d->name);
  // RemoveGameplayEffectsWithTags
  for (int k = 0; d->remove_tags && d->remove_tags[k]; k++)
    for (int i = 0; i < MAX_ACTIVE; i++)
      if (active[i].live && active[i].target == tgt) {
        for (int j = 0; active[i].def->asset_tags && active[i].def->asset_tags[j]; j++)
          if (tag_match(active[i].def->asset_tags[j], d->remove_tags[k])) { remove_active_idx(i); break; }
      }
  if (d->dur == U64_DUR_INSTANT) { run_modifiers(tgt, src, d, 0, spec); apply_conditionals(tgt, src, d, level); return 0; }
  // stacking
  if (d->stack_type) {
    for (int i = 0; i < MAX_ACTIVE; i++) {
      active_t *e = &active[i];
      if (e->live && e->def == d && e->target == tgt && (d->stack_type == 1 || e->source == src)) {
        if (e->stack < d->stack_limit) e->stack++;
        if (d->dur == U64_DUR_HAS_DURATION) e->remaining = d->dur_value;            // refresh on application
        recompute(tgt);
        u64_notify_stack_tasks(tgt->obj, d->name);
        return i + 1;
      }
    }
  }
  for (int i = 0; i < MAX_ACTIVE; i++) if (!active[i].live) {
    active_t *e = &active[i]; memset(e, 0, sizeof *e);
    e->live = 1; e->def = d; e->target = tgt; e->source = src; e->level = level; e->stack = 1;
    e->remaining = d->dur_value; e->next_period = d->period;
    for (int k = 0; k < d->nmods && e->nsbc < 4; k++) if (d->mods[k].calc == U64_CALC_SETBYCALLER) {
      int f; e->sbc[e->nsbc].tag = d->mods[k].sbc; e->sbc[e->nsbc++].v = spec_sbc(spec, d->mods[k].sbc, &f); }
    e->inhibited = !requirements_met(tgt, d->ongoing_require, d->ongoing_ignore);
    if (!e->inhibited) grant_tags(tgt, d, 1);
    recompute(tgt);
    // bExecutePeriodicEffectOnApplication: the first execution happens at the next tick (a timer with a ~0 first period), not
    // inside the Apply call. Otherwise an effect applied from an attribute-change callback re-enters it (GASShooter's KnockDown ->
    // GE_BleedingOut would kill the hero it just knocked down). Assumption from the shipping game's behaviour; see docs/PLAN.md.
    if (d->period > 0 && !e->inhibited) { e->exec_pending = 1; e->spec = spec; }
    update_inhibition(tgt);
    u64_notify_stack_tasks(tgt->obj, d->name);
    apply_conditionals(tgt, src, d, level);
    return i + 1;
  }
  if (!u64_rt_overflows++ && u64_log) u64_log("  [error] active effect list full");
  return 0;
}

void u64_tasks_tick_extra(float dt);
void u64_gas_tick(float dt)
{
  u64_tasks_tick_extra(dt);
  if (u64_gas_hooks_active && u64_gas_hooks_active->world_tick) u64_gas_hooks_active->world_tick(dt);
  for (int i = 0; i < MAX_ACTIVE; i++) {
    active_t *e = &active[i];
    if (!e->live) continue;
    if (e->exec_pending && !e->inhibited) {
      e->exec_pending = 0; e->next_period = e->def->period + dt;      // (this tick's decrement below then leaves a full period)
      run_modifiers(e->target, e->source, e->def, e, e->spec);
      if (!e->live) continue;
    }
    if (e->def->dur == U64_DUR_HAS_DURATION) { e->remaining -= dt; if (e->remaining <= 0) { remove_active_idx(i); continue; } }
    if (e->def->period > 0 && !e->inhibited) {
      e->next_period -= dt;
      while (e->next_period <= 0 && e->live) { e->next_period += e->def->period; run_modifiers(e->target, e->source, e->def, e, 0); }
    }
  }
}

// ---------------------------------------------------------------------------------------------------- AGDAttributeSetBase::PostGameplayEffectExecute
static void post_gameplay_effect_execute(asc_t *tgt, const char *name, asc_t *src)
{
  if (u64_gas_hooks_active && u64_gas_hooks_active->post_execute && u64_gas_hooks_active->post_execute(tgt->obj, name, src ? src->obj : 0)) return;
  if (!strcmp(name, "Damage")) {
    float local = attr(tgt, "Damage")->cur;
    set_base(tgt, "Damage", 0);
    if (local > 0.0f) {
      int was_alive = attr(tgt, "Health")->cur > 0;
      float maxh = attr(tgt, "MaxHealth")->cur, nh = attr(tgt, "Health")->cur - local;
      set_base(tgt, "Health", nh < 0 ? 0 : (nh > maxh ? maxh : nh));
      if (tgt->owner && was_alive) {
        if (u64_log) u64_log("  [dmg] %s takes %d", u64_obj_name(tgt->owner), (int)(local + 0.5f));
        u64_call(tgt->owner, "PlayHitReact", 0, 0);                              // front by default (no hit result)
        if (attr(tgt, "Health")->cur <= 0 && src && src != tgt) {                // bounty to the source
          execute_mod(src, tgt, "XP", U64_OP_ADD, attr(tgt, "XPBounty")->cur);
          execute_mod(src, tgt, "Gold", U64_OP_ADD, attr(tgt, "GoldBounty")->cur);
        }
      }
    }
  } else if (!strcmp(name, "Health")) { attr_t *h = attr(tgt, "Health"), *m = attr(tgt, "MaxHealth"); float v = h->cur < 0 ? 0 : (h->cur > m->cur ? m->cur : h->cur); if (v != h->base) set_base(tgt, "Health", v); }
  else if (!strcmp(name, "Mana")) { attr_t *h = attr(tgt, "Mana"), *m = attr(tgt, "MaxMana"); float v = h->cur < 0 ? 0 : (h->cur > m->cur ? m->cur : h->cur); if (v != h->base) set_base(tgt, "Mana", v); }
  else if (!strcmp(name, "Stamina")) { attr_t *h = attr(tgt, "Stamina"), *m = attr(tgt, "MaxStamina"); float v = h->cur < 0 ? 0 : (h->cur > m->cur ? m->cur : h->cur); if (v != h->base) set_base(tgt, "Stamina", v); }
}

// ---------------------------------------------------------------------------------------------------- characters
u64_obj *u64_asc_of(u64_obj *actor)
{
  if (!actor) return 0;
  u64_v *slot = u64_ivar(actor, "AbilitySystemComponent");
  if (slot->t == U64_OBJ && slot->u.o) return slot->u.o;
  if (!u64_obj_is_actor(actor) && strcmp(u64_obj_class(actor), "Component")) { /* only actors own an ability system */ }
  for (int i = 0; i < MAX_ASC; i++) if (!ascs[i].used) {
    asc_t *a = &ascs[i]; memset(a, 0, sizeof *a); a->used = 1; a->owner = actor;
    char nmb[64]; snprintf(nmb, sizeof nmb, "%s.ASC", u64_obj_name(actor));
    a->obj = u64_new(u64_intern(nmb), "AbilitySystemComponent");
    *u64_ivar(a->obj, "asc_idx") = u64_int(i + 1);
    *u64_ivar(a->obj, "Owner") = u64_objv(actor);
    *slot = u64_objv(a->obj);
    return a->obj;
  }
  u64_rt_overflows++; if (u64_log) u64_log("  [error] ability system pool exhausted");
  return 0;
}

void u64_die(u64_obj *actor)                      // AGDCharacterBase::Die
{
  if (!u64_obj_alive(actor)) return;
  asc_t *a = asc_from(u64_asc_of(actor));
  if (a && tag_count(a, "State.Dead")) return;
  if (u64_log) u64_log("  [die] %s", u64_obj_name(actor));
  u64_v self = u64_objv(actor);
  u64_mc_broadcast(u64_ivar(actor, "OnCharacterDied"), 1, &self);                 // OnCharacterDied.Broadcast(this)
  if (a) {
    cancel_abilities(a, 0, 0, 0);                                                 // CancelAllAbilities
    for (int i = 0; i < MAX_ACTIVE; i++)                                          // RemoveActiveEffectsWithTags(Effect.RemoveOnDeath)
      if (active[i].live && active[i].target == a)
        for (int j = 0; active[i].def->asset_tags && active[i].def->asset_tags[j]; j++)
          if (tag_match(active[i].def->asset_tags[j], "Effect.RemoveOnDeath")) { remove_active_idx(i); break; }
    tag_add(a, "State.Dead", 1);                                                  // AddLooseGameplayTag(DeadTag)
  }
  u64_v *dm = u64_ivar(actor, "DeathMontage");
  if (dm->t == U64_OBJ || dm->t == U64_CLASS || dm->t == U64_NAME) {
    u64_montage_play(actor, nm(*dm), 1.0f);                                       // PlayAnimMontage(DeathMontage): its Die notify calls FinishDying
    return;
  }
  u64_obj_destroy(actor);                                                         // no montage: FinishDying() -> Destroy()
}
void u64_kill(u64_obj *o) { u64_die(o); }

void u64_actor_init(u64_obj *actor)
{
  if (!actor) return;
  if (u64_ivar(actor, "cpp_inited")->t == U64_NONE) {                            // the C++ half of spawning, when a project port defines it
    u64_v r;
    *u64_ivar(actor, "cpp_inited") = u64_bool(1);
    u64_cpp_dispatch(actor, "PostInitializeComponents", 0, 0, &r);
    u64_cpp_dispatch(actor, "BeginPlay", 0, 0, &r);
  }
  if (!actor || u64_ivar(actor, "DefaultAttributes")->t == U64_NONE) return;
  u64_obj *asc = u64_asc_of(actor); asc_t *a = asc_from(asc);
  if (!a || u64_ivar(actor, "gas_inited")->t != U64_NONE) return;
  *u64_ivar(actor, "gas_inited") = u64_bool(1);
  // InitializeAttributes: DefaultAttributes applied at GetCharacterLevel()
  u64_apply_ge(asc, asc, nm(*u64_ivar(actor, "DefaultAttributes")), attr(a, "CharacterLevel")->cur, 0);
  // Hero respawn path (PossessedBy): SetHealth/Mana/Stamina to max
  if (!u64_gas_hooks_active) {                                                   // (GASDocumentation: Health/Mana/Stamina to max)
    set_base(a, "Health", attr(a, "MaxHealth")->cur);
    set_base(a, "Mana", attr(a, "MaxMana")->cur);
    set_base(a, "Stamina", attr(a, "MaxStamina")->cur);
  }
  u64_v se = *u64_ivar(actor, "StartupEffects");                                 // AddStartupEffects
  for (int i = 0; i < u64_array_len(se); i++) u64_apply_ge(asc, asc, nm(u64_array_get(se, i)), attr(a, "CharacterLevel")->cur, 0);
  u64_v ca = *u64_ivar(actor, "CharacterAbilities");                             // AddCharacterAbilities
  for (int i = 0; i < u64_array_len(ca) && a->nabil < MAX_ABIL; i++) {
    const char *cls = strip_c(nm(u64_array_get(ca, i)));
    char b[64]; snprintf(b, sizeof b, "%s.%s", u64_obj_name(actor), cls);
    u64_obj *ab = u64_new_copy(b, u64_intern(cls));
    *u64_ivar(ab, "Avatar") = u64_objv(actor); *u64_ivar(ab, "ASC") = u64_objv(asc);
    *u64_ivar(ab, "SourceObject") = u64_objv(actor);
    a->abil[a->nabil++] = ab;
  }
}
u64_obj *u64_asc_owner(u64_obj *asc) { asc_t *a = asc_from(asc); return a ? a->owner : 0; }
void u64_tag_add_loose(u64_obj *asc, const char *tag, int delta) { asc_t *a = asc_from(asc); if (a) tag_add(a, tag, delta); }   // Add/RemoveLooseGameplayTag(s)
int u64_tag_count(u64_obj *asc, const char *tag) { asc_t *a = asc_from(asc); return a ? tag_count(a, tag) : 0; }
// UAbilitySystemComponent::GiveAbility(FGameplayAbilitySpec(Class, Level, InputID, SourceObject)) -> the granted ability instance
u64_obj *u64_give_ability(u64_obj *asc_obj, const char *cls, int level, const char *input_id, u64_obj *source)
{
  asc_t *a = asc_from(asc_obj);
  if (!a || a->nabil >= MAX_ABIL) { if (a && !u64_rt_overflows++ && u64_log) u64_log("  [error] ability list full"); return 0; }
  char b[96]; snprintf(b, sizeof b, "%s.%s.%d", a->owner ? u64_obj_name(a->owner) : "asc", strip_c(cls), a->nabil);
  u64_obj *ab = u64_new_copy(b, u64_intern(strip_c(cls)));
  *u64_ivar(ab, "Avatar") = u64_objv(a->owner); *u64_ivar(ab, "ASC") = u64_objv(asc_obj);
  *u64_ivar(ab, "Level") = u64_int(level); *u64_ivar(ab, "SourceObject") = u64_objv(source);
  if (input_id && *input_id) *u64_ivar(ab, "AbilityInputID") = u64_name(input_id);
  a->abil[a->nabil++] = ab;
  return ab;
}
void u64_clear_ability(u64_obj *asc_obj, u64_obj *ab)                      // ClearAbility: ends it if running, then removes the spec
{
  asc_t *a = asc_from(asc_obj);
  for (int i = 0; a && i < a->nabil; i++) if (a->abil[i] == ab) {
    if (u64_true(*u64_ivar(ab, "Active"))) u64_ability_end(ab, 1);
    a->abil[i] = a->abil[--a->nabil]; u64_obj_destroy(ab); return;
  }
}
int u64_asc_abilities(u64_obj *asc_obj, u64_obj **out, int max)
{
  asc_t *a = asc_from(asc_obj); int n = 0;
  for (int i = 0; a && i < a->nabil && n < max; i++) out[n++] = a->abil[i];
  return n;
}

// ---------------------------------------------------------------------------------------------------- abilities
static int arr_has_tag_match(u64_v arr, const char *query)
{
  for (int i = 0; i < u64_array_len(arr); i++) if (tag_match(nm(u64_array_get(arr, i)), query)) return 1;
  return 0;
}
static const char *tag_i(u64_v arr, int i) { return nm(u64_array_get(arr, i)); }
static const u64_ge_def *ability_ge(u64_obj *ab, const char *prop)
{
  u64_v *v = u64_ivar(ab, prop);
  return v->t == U64_CLASS || v->t == U64_OBJ || v->t == U64_NAME ? find_ge(nm(*v)) : 0;
}
static int cost_ok(asc_t *a, const u64_ge_def *cost)     // UAbilitySystemComponent::CanApplyAttributeModifiers
{
  if (!cost) return 1;
  for (int i = 0; i < cost->nmods; i++)
    if (cost->mods[i].op == U64_OP_ADD && attr(a, cost->mods[i].attr)->cur + cost->mods[i].value < 0.f) return 0;
  return 1;
}
static int cooldown_ok(asc_t *a, const u64_ge_def *cd) { return !cd || !has_any(a, cd->granted); }

static void cancel_abilities(asc_t *a, const char *include, const char *ignore, u64_obj *except)
{
  u64_obj *list[MAX_ABIL]; int n = 0;
  for (int i = 0; i < a->nabil; i++) {
    u64_obj *ab = a->abil[i];
    if (ab == except || !u64_true(*u64_ivar(ab, "Active"))) continue;
    u64_v tags = *u64_ivar(ab, "AbilityTags");
    if (include && !arr_has_tag_match(tags, include)) continue;
    if (ignore && arr_has_tag_match(tags, ignore)) continue;
    list[n++] = ab;
  }
  for (int i = 0; i < n; i++) u64_ability_end(list[i], 1);
}

int u64_ability_try_activate(u64_obj *ab)               // UGameplayAbility::CanActivateAbility + CallActivateAbility
{
  asc_t *a = asc_from(u64_ivar(ab, "ASC")->u.o);
  if (!a || u64_true(*u64_ivar(ab, "Active"))) return 0;
  u64_v blocked = *u64_ivar(ab, "ActivationBlockedTags"), required = *u64_ivar(ab, "ActivationRequiredTags");
  for (int i = 0; i < u64_array_len(blocked); i++) if (tag_count(a, tag_i(blocked, i))) return 0;
  for (int i = 0; i < u64_array_len(required); i++) if (!tag_count(a, tag_i(required, i))) return 0;
  u64_v mytags = *u64_ivar(ab, "AbilityTags");
  for (int i = 0; i < u64_array_len(mytags); i++) { char k[96]; snprintf(k, sizeof k, "blocked:%s", tag_i(mytags, i)); if (tag_count(a, k)) return 0; }
  if (u64_gas_hooks_active && u64_gas_hooks_active->can_activate && !u64_gas_hooks_active->can_activate(ab)) return 0;
  if (!u64_cpp_ability_can_activate(ab)) return 0;
  if (!cooldown_ok(a, ability_ge(ab, "CooldownGameplayEffectClass"))) return 0;
  if (!u64_ability_check_cost(ab)) return 0;
  // activate: cancel abilities, grant owned tags, block tags, run the Blueprint
  u64_v cancel = *u64_ivar(ab, "CancelAbilitiesWithTag");
  for (int i = 0; i < u64_array_len(cancel); i++) cancel_abilities(a, tag_i(cancel, i), 0, ab);
  *u64_ivar(ab, "Active") = u64_bool(1); *u64_ivar(ab, "Ended") = u64_bool(0);
  u64_v owned = *u64_ivar(ab, "ActivationOwnedTags");
  for (int i = 0; i < u64_array_len(owned); i++) tag_add(a, tag_i(owned, i), 1);
  u64_v block = *u64_ivar(ab, "BlockAbilitiesWithTag");
  for (int i = 0; i < u64_array_len(block); i++) { char k[96]; snprintf(k, sizeof k, "blocked:%s", tag_i(block, i)); tag_add(a, k, 1); }
  if (u64_log) u64_log("  [ability] %s activated", u64_obj_name(ab));
  if (!u64_cpp_ability_activate(ab)) u64_call(ab, "K2_ActivateAbility", 0, 0);       // a C++ override, else the Blueprint event
  return 1;
}
int u64_ability_check_cost(u64_obj *ab)                  // UGameplayAbility::CheckCost (+ the project's override)
{
  asc_t *a = asc_from(u64_ivar(ab, "ASC")->u.o);
  if (!a) return 1;
  if (!cost_ok(a, ability_ge(ab, "CostGameplayEffectClass"))) return 0;
  return !(u64_gas_hooks_active && u64_gas_hooks_active->check_cost) || u64_gas_hooks_active->check_cost(ab);
}
int u64_ability_commit(u64_obj *ab, int cost, int cooldown)
{
  asc_t *a = asc_from(u64_ivar(ab, "ASC")->u.o);
  if (!a) return 1;                                      // an ability without an ASC (not given) cannot be checked
  const u64_ge_def *cd = ability_ge(ab, "CooldownGameplayEffectClass"), *co = ability_ge(ab, "CostGameplayEffectClass");
  if ((cooldown && !cooldown_ok(a, cd)) || (cost && !u64_ability_check_cost(ab))) return 0;
  u64_obj *asc = a->obj;
  if (cooldown && cd) u64_apply_ge(asc, asc, cd->name, 1, 0);
  if (cost && u64_gas_hooks_active && u64_gas_hooks_active->apply_cost) u64_gas_hooks_active->apply_cost(ab);   // ApplyCost: GSApplyCost first, then Super
  if (cost && co) u64_apply_ge(asc, asc, co->name, 1, 0);
  return 1;
}
void u64_ability_end(u64_obj *ab, int cancelled)
{
  if (!u64_true(*u64_ivar(ab, "Active")) && !cancelled) return;
  asc_t *a = asc_from(u64_ivar(ab, "ASC")->u.o);
  *u64_ivar(ab, "Active") = u64_bool(0); *u64_ivar(ab, "Ended") = u64_bool(1);
  if (a) {
    u64_v owned = *u64_ivar(ab, "ActivationOwnedTags");
    for (int i = 0; i < u64_array_len(owned); i++) tag_add(a, tag_i(owned, i), -1);
    u64_v block = *u64_ivar(ab, "BlockAbilitiesWithTag");
    for (int i = 0; i < u64_array_len(block); i++) { char k[96]; snprintf(k, sizeof k, "blocked:%s", tag_i(block, i)); tag_add(a, k, -1); }
  }
  if (u64_log) u64_log("  [ability] %s %s", u64_obj_name(ab), cancelled ? "cancelled" : "ended");
  u64_timers_cancel_for(ab);
  u64_tasks_end_for(ab);
  u64_call(ab, "K2_OnEndAbility", 0, 0);
  if (u64_true(*u64_ivar(ab, "ClearOnEnd")) && a) u64_clear_ability(a->obj, ab);          // GiveAbilityAndActivateOnce: the spec goes away when the ability ends
}
int u64_input_press(u64_obj *actor, const char *id)          // AbilityLocalInputPressed: every ability bound to the input is considered; 1 if any reacted
{
  asc_t *a = asc_from(u64_asc_of(actor)); int any = 0;
  u64_obj *snap[MAX_ABIL]; int n = 0;
  for (int i = 0; a && i < a->nabil; i++) if (!strcmp(nm(*u64_ivar(a->abil[i], "AbilityInputID")), id)) snap[n++] = a->abil[i];
  for (int i = 0; i < n; i++) {
    u64_obj *ab = snap[i];
    u64_v *act = u64_ivar(ab, "bActivateOnInput");
    *u64_ivar(ab, "InputPressed") = u64_bool(1);                           // FGameplayAbilitySpec::InputPressed
    if (u64_true(*u64_ivar(ab, "Active"))) { u64_ability_input_pressed(ab); any = 1; continue; }   // AbilitySpecInputPressed on a running ability: the InputPressed event
    if (act->t != U64_NONE && !u64_true(*act)) continue;                   // UGSGameplayAbility::bActivateOnInput
    any |= u64_ability_try_activate(ab);
  }
  return any;
}
void u64_input_release_id(u64_obj *actor, const char *id)
{
  asc_t *a = asc_from(u64_asc_of(actor));
  for (int i = 0; a && i < a->nabil; i++)
    if (!strcmp(nm(*u64_ivar(a->abil[i], "AbilityInputID")), id)) *u64_ivar(a->abil[i], "InputPressed") = u64_bool(0);
  for (int i = 0; a && i < a->nabil; i++)
    if (!strcmp(nm(*u64_ivar(a->abil[i], "AbilityInputID")), id) && u64_true(*u64_ivar(a->abil[i], "Active"))) { u64_input_release(a->abil[i]); u64_cpp_ability_input_released(a->abil[i]); }
}

// ---------------------------------------------------------------------------------------------------- montages and overlaps
float u64_montage_length(const char *name, float *blend_out)
{
  for (const u64_montage_def *m = u64_montages; m->name; m++) if (!strcmp(m->name, name)) { if (blend_out) *blend_out = m->blend_out; return m->length; }
  return -1;
}
static const u64_shape_def *shape_of(u64_obj *o)
{
  const char *cls = u64_obj_class(o);
  for (const u64_shape_def *s = u64_shapes; s->cls; s++) if (!strcmp(s->cls, cls)) return s;
  return 0;
}
typedef struct { float x, y, z; } v3_;
static v3_ vget(u64_obj *o, const char *n) { u64_v *v = u64_ivar(o, n); return v->t == U64_VEC ? (v3_){v->u.v[0], v->u.v[1], v->u.v[2]} : (v3_){0, 0, 0}; }
static v3_ vget_def(u64_obj *o, const char *n, v3_ d) { u64_v *v = u64_ivar(o, n); return v->t == U64_VEC ? (v3_){v->u.v[0], v->u.v[1], v->u.v[2]} : d; }
// World transform of an actor's collision shape: centre, yaw (radians), scaled dimensions.
typedef struct { const u64_shape_def *s; float cx, cy, cz, yaw, a, b, c; } shape_t;
static int shape_world(u64_obj *o, shape_t *out)
{
  const u64_shape_def *s = shape_of(o);
  if (!s) return 0;
  v3_ loc = vget(o, "Location"), rot = vget(o, "Rotation"), sc = vget_def(o, "Scale", (v3_){1, 1, 1});
  float yaw = rot.y * 0.0174533f, cy = cosf(yaw), sy = sinf(yaw);
  float rx = s->rx * sc.x, ry = s->ry * sc.y;
  out->s = s; out->yaw = yaw + s->ryaw * 0.0174533f;
  out->cx = loc.x + rx * cy - ry * sy; out->cy = loc.y + rx * sy + ry * cy; out->cz = loc.z + s->rz * sc.z;
  out->a = s->a * sc.x; out->b = s->kind == 1 ? s->b * sc.y : (s->kind == 3 ? 0 : s->b * sc.z); out->c = s->kind == 1 ? s->c * sc.z : 0;
  if (s->kind == 2) {                                           // a capsule's size can come from C++ (InitCapsuleSize) or the Blueprint: the component's own values win
    u64_obj *comp = u64_comp(o, s->comp).u.o;
    u64_v *r = comp ? u64_ivar(comp, "CapsuleRadius") : 0, *h = comp ? u64_ivar(comp, "CapsuleHalfHeight") : 0;
    if (r && r->t == U64_FLOAT) out->a = r->u.f * sc.x;
    if (h && h->t == U64_FLOAT) out->b = h->u.f * sc.z;
  }
  return 1;
}
int u64_actor_capsule(u64_obj *o, float *cx, float *cy, float *cz, float *r, float *hh)   // the actor's collision capsule in the world; 0 if it has none
{
  shape_t sh;
  if (!shape_world(o, &sh) || sh.s->kind != 2) return 0;
  *cx = sh.cx; *cy = sh.cy; *cz = sh.cz; *r = sh.a; *hh = sh.b; return 1;
}
int u64_actor_is_pawn(u64_obj *o);
static int is_pawn(u64_obj *o)                                   // a character: its C++ base (GDCharacterBase / GSCharacterBase) or a character movement component
{
  return u64_ivar(o, "CharacterMovement")->t == U64_OBJ || u64_class_is(u64_obj_class(o), "GDCharacterBase") || u64_class_is(u64_obj_class(o), "GSCharacterBase");
}   // only characters' capsules block traces / sweeps / trigger overlaps
// vertical capsule (centre, radius r, half height hh) against a yawed box: exact in the box's local frame
static int capsule_box(const shape_t *cap, const shape_t *box)
{
  float dx = cap->cx - box->cx, dy = cap->cy - box->cy, c = cosf(-box->yaw), s = sinf(-box->yaw);
  float lx = dx * c - dy * s, ly = dx * s + dy * c;
  float r = cap->a, hh = cap->b, seg = hh - r; if (seg < 0) seg = 0;
  float ex = fmaxf(fabsf(lx) - box->a, 0), ey = fmaxf(fabsf(ly) - box->b, 0);
  float lo = cap->cz - seg, hi = cap->cz + seg, blo = box->cz - box->c, bhi = box->cz + box->c;
  float gz = fmaxf(0.f, fmaxf(lo - bhi, blo - hi));
  return ex * ex + ey * ey + gz * gz <= r * r;
}
int u64_overlapping(u64_obj *self, u64_obj **out, int max)       // actors whose capsule overlaps this actor's box
{
  if (self && !strcmp(u64_obj_class(self), "Component")) self = u64_asobj(*u64_ivar(self, "Owner"));   // component -> its actor
  if (!self) return 0;
  shape_t me;
  if (!shape_world(self, &me) || me.s->kind != 1) return 0;
  int n = 0;
  for (int i = 0; i < u64_obj_count() && n < max; i++) {
    u64_obj *o = u64_obj_at(i); shape_t other;
    if (o == self || !u64_obj_is_actor(o) || !is_pawn(o) || !shape_world(o, &other) || other.s->kind != 2) continue;   // characters (capsules)
    if (capsule_box(&other, &me)) out[n++] = o;
  }
  return n;
}

// ---------------------------------------------------------------------------------------------------- line traces
static int ray_vcapsule(const float p[3], const float d[3], float cx, float cy, float zlo, float zhi, float r, float *t, float nrm[3])
{
  double best = 1e30; int hit = 0;
  double ox = p[0] - cx, oy = p[1] - cy;
  double A = d[0] * d[0] + d[1] * d[1], B = 2 * (ox * d[0] + oy * d[1]), C = ox * ox + oy * oy - r * r;
  if (A > 1e-9f) {                                           // side of the cylinder
    double disc = B * B - 4 * A * C;
    if (disc >= 0) { double tt = (-B - sqrt(disc)) / (2 * A); double z = p[2] + tt * d[2];
      if (tt >= 0 && tt <= 1 && z >= zlo && z <= zhi && tt < best) { best = tt; hit = 1; nrm[0] = (ox + tt * d[0]) / r; nrm[1] = (oy + tt * d[1]) / r; nrm[2] = 0; } }
  }
  for (int cap = 0; cap < 2; cap++) {                        // the two end spheres
    double cz = cap ? zhi : zlo, sx = p[0] - cx, sy = p[1] - cy, sz = p[2] - cz;
    double a = d[0] * d[0] + d[1] * d[1] + d[2] * d[2], b = 2 * (sx * d[0] + sy * d[1] + sz * d[2]), c = sx * sx + sy * sy + sz * sz - r * r;
    double disc = b * b - 4 * a * c; if (disc < 0 || a < 1e-9f) continue;
    double tt = (-b - sqrt(disc)) / (2 * a); double z = p[2] + tt * d[2];
    if (tt >= 0 && tt <= 1 && ((cap && z >= zhi) || (!cap && z <= zlo)) && tt < best) {
      best = tt; hit = 1; nrm[0] = (sx + tt * d[0]) / r; nrm[1] = (sy + tt * d[1]) / r; nrm[2] = (sz + tt * d[2]) / r; }
  }
  if (hit) *t = (float)best;
  return hit;
}
int u64_trace_multi(const float start[3], const float end[3], float radius, u64_obj *ignore, u64_obj **hits, int max)   // radius 0: line trace; else a sphere sweep
{
  float d[3] = {end[0] - start[0], end[1] - start[1], end[2] - start[2]}, dist = sqrtf(d[0] * d[0] + d[1] * d[1] + d[2] * d[2]);
  struct { float t; u64_obj *o; float n[3]; } found[16]; int nf = 0;
  for (int i = 0; i < u64_obj_count() && nf < 16; i++) {
    u64_obj *o = u64_obj_at(i); shape_t sh;
    if (o == ignore || !u64_obj_is_actor(o) || !shape_world(o, &sh) || sh.s->kind != 2) continue;     // pawn capsules block object traces
    if (!is_pawn(o)) continue;                                                                      // pickups and weapons ignore these channels
    float seg = sh.b - sh.a; if (seg < 0) seg = 0;
    float t, n[3];
    if (radius > 0) {   // initial overlap (the sweep starts inside the capsule grown by the radius): a hit at time 0, as the engine reports bStartPenetrating
      double zc = start[2] < sh.cz - seg ? sh.cz - seg : start[2] > sh.cz + seg ? sh.cz + seg : start[2];
      double dx0 = start[0] - sh.cx, dy0 = start[1] - sh.cy, dz0 = start[2] - zc, rr = sh.a + radius;
      if (dx0 * dx0 + dy0 * dy0 + dz0 * dz0 <= rr * rr) {
        double l = sqrt(dx0 * dx0 + dy0 * dy0 + dz0 * dz0); t = 0; n[0] = l > 1e-6 ? (float)(dx0 / l) : 0; n[1] = l > 1e-6 ? (float)(dy0 / l) : 0; n[2] = l > 1e-6 ? (float)(dz0 / l) : 1;
        found[nf].t = t; found[nf].o = o; memcpy(found[nf].n, n, sizeof n); nf++; continue;
      }
    }
    if (ray_vcapsule(start, d, sh.cx, sh.cy, sh.cz - seg, sh.cz + seg, sh.a + radius, &t, n)) {   /* sphere vs capsule = ray vs capsule grown by the radius */ found[nf].t = t; found[nf].o = o; memcpy(found[nf].n, n, sizeof n); nf++; }
  }
  for (int i = 1; i < nf; i++) for (int j = i; j > 0 && found[j].t < found[j - 1].t; j--) { __typeof__(found[0]) tmp = found[j]; found[j] = found[j - 1]; found[j - 1] = tmp; }
  int n = 0;
  for (int i = 0; i < nf && n < max; i++) {
    u64_obj *h = u64_new("HitResult", "HitResult");
    float t = found[i].t;
    *u64_ivar(h, "Time") = u64_float(t); *u64_ivar(h, "Distance") = u64_float(t * dist); *u64_ivar(h, "bBlockingHit") = u64_bool(1);
    *u64_ivar(h, "Location") = u64_vec(start[0] + d[0] * t, start[1] + d[1] * t, start[2] + d[2] * t);
    *u64_ivar(h, "Normal") = u64_vec(found[i].n[0], found[i].n[1], found[i].n[2]);
    *u64_ivar(h, "HitActor") = u64_objv(found[i].o);
    *u64_ivar(h, "TraceStart") = u64_vec(start[0], start[1], start[2]); *u64_ivar(h, "TraceEnd") = u64_vec(end[0], end[1], end[2]);
    hits[n++] = h;
  }
  return n;
}

int u64_line_trace(const float start[3], const float end[3], u64_obj *ignore, u64_obj **hits, int max) { return u64_trace_multi(start, end, 0.0f, ignore, hits, max); }

// ---------------------------------------------------------------------------------------------------- montage notifies and gameplay events
void u64_montage_play(u64_obj *actor, const char *name, float rate)      // schedules the montage's AnimNotifies
{
  if (rate <= 0) rate = 1;
  for (const u64_montage_def *m = u64_montages; m->name; m++) {
    if (strcmp(m->name, name)) continue;
    *u64_ivar(actor, "CurrentMontage") = u64_name(m->name);
    for (int i = 0; i < m->nnotifies; i++) {
      char cls[64], nmb[64]; snprintf(cls, sizeof cls, "AnimNotify_%s", m->notifies[i].cls);
      snprintf(nmb, sizeof nmb, "%s.%s.%d", m->name, m->notifies[i].cls, i);
      u64_obj *nobj = u64_new(u64_intern(nmb), u64_intern(cls));
      if (m->notifies[i].tag[0]) *u64_ivar(nobj, "EventTag") = u64_name(m->notifies[i].tag);
      u64_timer_call(m->notifies[i].time / rate, nobj, "Received_Notify", u64_comp(actor, "Mesh"));   // arg: the mesh component
    }
    return;
  }
}
void u64_send_gameplay_event(u64_obj *actor, const char *tag, u64_v payload)
{
  u64_obj *asc = u64_asc_of(actor);
  u64_v args[2] = {u64_name(tag), payload};
  u64_deliver_gameplay_event(asc, tag, 2, args);
}

// ---------------------------------------------------------------------------------------------------- queries used by tasks and natives
float u64_cooldown_remaining(u64_obj *asc, const char *tag, float *duration)
{
  asc_t *a = asc_from(asc); float best = 0, dur = 0;
  for (int i = 0; a && i < MAX_ACTIVE; i++) {
    const active_t *e = &active[i];
    if (!e->live || e->target != a || e->def->dur != U64_DUR_HAS_DURATION) continue;
    for (int j = 0; e->def->granted && e->def->granted[j]; j++)
      if (tag_match(e->def->granted[j], tag) && e->remaining > best) { best = e->remaining; dur = e->def->dur_value; }
  }
  if (duration) *duration = dur;
  return best;
}
int u64_effect_stack_count(u64_obj *asc, const char *tag)    // total stacks of active effects whose tags match
{
  asc_t *a = asc_from(asc); int n = 0;
  for (int i = 0; a && i < MAX_ACTIVE; i++) {
    const active_t *e = &active[i];
    if (!e->live || e->target != a) continue;
    int hit = 0;
    for (int j = 0; e->def->granted && e->def->granted[j]; j++) if (tag_match(e->def->granted[j], tag)) hit = 1;
    for (int j = 0; e->def->asset_tags && e->def->asset_tags[j]; j++) if (tag_match(e->def->asset_tags[j], tag)) hit = 1;
    if (hit) n += e->stack;
  }
  return n;
}
void u64_remove_ge_class(u64_obj *asc, const char *ge_name)   // RemoveActiveGameplayEffectBySourceEffect
{
  asc_t *a = asc_from(asc); const u64_ge_def *d = find_ge(ge_name);
  for (int i = 0; a && d && i < MAX_ACTIVE; i++) if (active[i].live && active[i].target == a && active[i].def == d) remove_active_idx(i);
}
int u64_effect_handle_stack(int h) { return h > 0 && h <= MAX_ACTIVE && active[h - 1].live ? active[h - 1].stack : 0; }
u64_obj *u64_effect_handle_source(int h) { return h > 0 && h <= MAX_ACTIVE && active[h - 1].live && active[h - 1].source ? active[h - 1].source->obj : 0; }
int u64_remove_ge_with_asset_tags(u64_obj *asc, const char *tag)   // RemoveActiveEffectsWithTags: matches the effect's asset tags
{
  asc_t *a = asc_from(asc); int n = 0;
  for (int i = 0; a && i < MAX_ACTIVE; i++) {
    if (!active[i].live || active[i].target != a) continue;
    for (int j = 0; active[i].def->asset_tags && active[i].def->asset_tags[j]; j++)
      if (tag_match(active[i].def->asset_tags[j], tag)) { remove_active_idx(i); n++; break; }
  }
  return n;
}
void u64_cancel_abilities(u64_obj *asc, const char *tag)           // CancelAbilities(WithTags) / CancelAllAbilities (tag NULL)
{
  asc_t *a = asc_from(asc); if (a) cancel_abilities(a, tag, 0, 0);
}
void u64_remove_ge_with_tags(u64_obj *asc, const char *tag)   // RemoveActiveEffectsWithGrantedTags
{
  asc_t *a = asc_from(asc);
  for (int i = 0; a && i < MAX_ACTIVE; i++) {
    if (!active[i].live || active[i].target != a) continue;
    for (int j = 0; active[i].def->granted && active[i].def->granted[j]; j++)
      if (tag_match(active[i].def->granted[j], tag)) { remove_active_idx(i); break; }
  }
}
float u64_max_speed(u64_obj *actor)                            // UGDCharacterMovementComponent::GetMaxSpeed
{
  if (u64_gas_hooks_active && u64_gas_hooks_active->max_speed) { float v = u64_gas_hooks_active->max_speed(actor); if (v >= 0) return v; }
  asc_t *a = asc_from(u64_asc_of(actor));
  if (!a) return 0;
  if (attr(a, "Health")->cur <= 0) return 0;
  if (tag_count(a, "State.Debuff.Stun")) return 0;
  float ms = attr(a, "MoveSpeed")->cur;
  if (u64_true(*u64_ivar(actor, "bSprinting"))) return ms * 1.4f;     // SprintSpeedMultiplier = 1.4f (constructor)
  if (u64_true(*u64_ivar(actor, "bAiming"))) return ms * 0.5f;        // ADSSpeedMultiplier
  return ms;
}
int u64_asc_ability_count(u64_obj *asc) { asc_t *a = asc_from(asc); return a ? a->nabil : 0; }
void u64_attr_set_base(u64_obj *asc, const char *name, float v) { asc_t *a = asc_from(asc); if (a) set_base(a, name, v); }

// ---------------------------------------------------------------------------------------------------- component overlap events
static const u64_shape_def *shape_for(u64_obj *o, const char *comp)
{
  const char *cls = u64_obj_class(o);
  for (const char *c = cls; c; c = u64_parent_of(c))
    for (const u64_shape_def *s = u64_shapes; s->cls; s++) if (!strcmp(s->cls, c) && !strcmp(s->comp, comp)) return s;
  return 0;
}
static int world_shape_of(u64_obj *o, const u64_shape_def *def, shape_t *out)
{
  const u64_shape_def *save = def;
  v3_ loc = vget(o, "Location"), rot = vget(o, "Rotation"), sc = vget_def(o, "Scale", (v3_){1, 1, 1});
  float yaw = rot.y * 0.0174533f, cy = cosf(yaw), sy = sinf(yaw), rx = save->rx * sc.x, ry = save->ry * sc.y;
  out->s = save; out->yaw = yaw + save->ryaw * 0.0174533f;
  out->cx = loc.x + rx * cy - ry * sy; out->cy = loc.y + rx * sy + ry * cy; out->cz = loc.z + save->rz * sc.z;
  out->a = save->a * sc.x; out->b = save->kind == 1 ? save->b * sc.y : (save->kind == 3 ? 0 : save->b * sc.z); out->c = save->kind == 1 ? save->c * sc.z : 0;
  return 1;
}
static int sphere_capsule(const shape_t *sp, const shape_t *cap)       // sphere vs vertical capsule
{
  float seg = cap->b - cap->a; if (seg < 0) seg = 0;
  float dz = sp->cz < cap->cz - seg ? (cap->cz - seg) - sp->cz : (sp->cz > cap->cz + seg ? sp->cz - (cap->cz + seg) : 0);
  float dx = sp->cx - cap->cx, dy = sp->cy - cap->cy, r = sp->a + cap->a;
  return dx * dx + dy * dy + dz * dz <= r * r;
}
static int comp_overlaps_capsule(const shape_t *c, const shape_t *cap) { return c->s->kind == 3 ? sphere_capsule(c, cap) : (c->s->kind == 1 ? capsule_box(cap, c) : 0); }

#define MAX_PAIRS 192
static struct { u64_obj *o, *other; const char *comp; } pairs[MAX_PAIRS]; static int npairs;
static int pair_find(u64_obj *o, const char *comp, u64_obj *other) { for (int i = 0; i < npairs; i++) if (pairs[i].o == o && pairs[i].other == other && !strcmp(pairs[i].comp, comp)) return i; return -1; }

void u64_overlap_events_tick(void)             // OnComponentBeginOverlap / EndOverlap for components bound in the Blueprint graph
{
  for (const u64_bound_event_def *d = u64_bound_events; d->cls; d++) {
    int primary = 1;                           // handle each (class, component) once; it dispatches begin and end handlers
    for (const u64_bound_event_def *e = u64_bound_events; e < d; e++) if (!strcmp(e->cls, d->cls) && !strcmp(e->comp, d->comp)) primary = 0;
    if (!primary) continue;
    for (int i = 0; i < u64_obj_count(); i++) {
      u64_obj *o = u64_obj_at(i);
      if (!u64_obj_is_actor(o) || !u64_class_is(u64_obj_class(o), d->cls)) continue;
      const u64_shape_def *def = shape_for(o, d->comp);
      if (!def || def->kind == 2) continue;
      shape_t me; world_shape_of(o, def, &me);
      for (int j = 0; j < u64_obj_count(); j++) {
        u64_obj *oo = u64_obj_at(j); shape_t other;
        if (oo == o || !u64_obj_is_actor(oo) || !is_pawn(oo) || !shape_world(oo, &other) || other.s->kind != 2) continue;
        int now = comp_overlaps_capsule(&me, &other), pi = pair_find(o, d->comp, oo);
        if (now == (pi >= 0)) continue;
        if (now && npairs < MAX_PAIRS) { pairs[npairs].o = o; pairs[npairs].other = oo; pairs[npairs++].comp = d->comp; }
        if (!now) pairs[pi] = pairs[--npairs];
        for (const u64_bound_event_def *h = u64_bound_events; h->cls; h++) {
          if (strcmp(h->cls, d->cls) || strcmp(h->comp, d->comp) || h->end == now) continue;      // begin handlers on enter, end handlers on leave
          u64_v args[6] = {u64_comp(o, d->comp), u64_objv(oo), u64_comp(oo, "CapsuleComponent"), u64_int(0), u64_bool(0), u64_int(0)};
          u64_call(o, h->fn, 6, args);
        }
        if (!u64_obj_alive(o)) break;
      }
    }
  }
}

// Sphere sweep (ProjectileMovementComponent): earliest contact with a pawn capsule along start -> end, ignoring one actor.
int u64_sweep_sphere(const float start[3], const float end[3], float radius, u64_obj *ignore, float *t_out)
{
  float d[3] = {end[0] - start[0], end[1] - start[1], end[2] - start[2]}, best = 2; int hit = 0;
  for (int i = 0; i < u64_obj_count(); i++) {
    u64_obj *o = u64_obj_at(i); shape_t sh;
    if (o == ignore || !u64_obj_is_actor(o) || !is_pawn(o) || !shape_world(o, &sh) || sh.s->kind != 2) continue;
    float seg = sh.b - sh.a; if (seg < 0) seg = 0;
    float t, n[3];
    if (ray_vcapsule(start, d, sh.cx, sh.cy, sh.cz - seg, sh.cz + seg, sh.a + radius, &t, n) && t < best) { best = t; hit = 1; }
  }
  if (hit) *t_out = best;
  return hit;
}

int u64_actor_is_pawn(u64_obj *o) { return is_pawn(o); }
// GC roots: every ability system whose avatar lives, its abilities, active effects (their specs and sources), overlap pairs and the
// constant arrays. A destroyed actor's ability system and effects are released here so their slots can be reused.
void u64_gc_roots_core(void)
{
  for (int i = 0; i < MAX_ASC; i++) {
    asc_t *a = &ascs[i];
    if (!a->used) continue;
    if (!u64_obj_alive(a->owner)) {
      for (int k = 0; k < MAX_ACTIVE; k++) if (active[k].live && (active[k].target == a || active[k].source == a)) active[k].live = 0;
      a->used = 0; continue;
    }
    u64_gc_mark_obj(a->obj); u64_gc_mark_obj(a->owner);
    for (int k = 0; k < a->nabil; k++) u64_gc_mark_obj(a->abil[k]);
  }
  for (int k = 0; k < MAX_ACTIVE; k++) if (active[k].live) { u64_gc_mark_obj(active[k].spec); if (active[k].source) u64_gc_mark_obj(active[k].source->obj); }
  for (int k = 0; k < npairs; k++) { u64_gc_mark_obj(pairs[k].o); u64_gc_mark_obj(pairs[k].other); }
  for (int k = 0; k < 1024; k++) if (const_cache[k].t == U64_ARRAY) u64_array_gc_mark(const_cache[k]);
}
