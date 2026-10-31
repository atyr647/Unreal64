// Gameplay Ability System core. Ported from Unreal's GAS semantics and the project's C++.
// Attribute value = aggregate(base, active modifiers); instant/periodic effects change the base value; Pre/Post attribute
// callbacks follow GDAttributeSetBase; death follows GDCharacterBase::Die/FinishDying.
#include "u64_gas_core.h"
#include <math.h>
#include <stdio.h>

#define MAX_ASC 24
#define MAX_ATTR 24
#define MAX_TAGS 48
#define MAX_ACTIVE 96
#define MAX_ABIL 16

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
  float remaining, next_period; int inhibited;
  struct { const char *tag; float v; } sbc[4]; int nsbc;
} active_t;

static asc_t ascs[MAX_ASC];
static active_t active[MAX_ACTIVE];
const char *u64_intern(const char *s);

// ---------------------------------------------------------------------------------------------------- arrays
#define ARR_POOL 4096
#define ARR_MAX 128
static u64_v arr_items[ARR_POOL]; static int arr_used;
static struct { int off, n, cap; } arrs[ARR_MAX]; static int narr;
static u64_v arr_new_cap(int cap)
{
  u64_v r = {U64_ARRAY};
  if (narr >= ARR_MAX || arr_used + cap > ARR_POOL) return (u64_v){U64_NONE};
  arrs[narr].off = arr_used; arrs[narr].n = 0; arrs[narr].cap = cap; arr_used += cap;
  r.u.i = ++narr; return r;
}
u64_v u64_array_of(int n, const u64_v *items)
{
  u64_v a = arr_new_cap(n < 4 ? 4 : n * 2);
  if (a.t != U64_ARRAY) return a;
  for (int i = 0; i < n; i++) arr_items[arrs[a.u.i - 1].off + i] = items[i];
  arrs[a.u.i - 1].n = n; return a;
}
int u64_array_len(u64_v a) { return a.t == U64_ARRAY ? arrs[a.u.i - 1].n : 0; }
u64_v u64_array_get(u64_v a, int i)
{
  if (a.t != U64_ARRAY || i < 0 || i >= arrs[a.u.i - 1].n) return (u64_v){U64_NONE};
  return arr_items[arrs[a.u.i - 1].off + i];
}
void u64_array_push(u64_v *a, u64_v item)
{
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
static const u64_ge_def *find_ge(const char *name)
{
  const char *n = strip_c(name);
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
  if (a->na >= MAX_ATTR) return &a->a[MAX_ATTR - 1];
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
  if (!t) { if (a->nt >= MAX_TAGS) return; t = &a->t[a->nt++]; t->tag = u64_intern(tag); t->n = 0; }
  int before = t->n; t->n += d; if (t->n < 0) t->n = 0;
  if (before == 0 && t->n > 0 && !strcmp(tag, "State.Debuff.Stun"))      // AGDMinionCharacter::StunTagChanged
    cancel_abilities(a, "Ability", "Ability.NotCanceledByStun", 0);
  if (!a->in_inhibit_update) update_inhibition(a);
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
// GDAttributeSetBase::PreAttributeChange
static void pre_attribute_change(asc_t *a, const char *name, float *nv)
{
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
  if (!strcmp(name, "Health") && nv <= 0 && a->owner && !tag_count(a, "State.Dead") && u64_obj_alive(a->owner))
    u64_die(a->owner);        // AGDMinionCharacter::HealthChanged / AGDPlayerState::HealthChanged: !IsAlive && !Dead -> Die()
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
  float damage = src ? attr(src, "Damage")->cur : 0;      // captured from the Source (snapshot)
  int found; float sbc = spec_sbc(spec, "Data.Damage", &found);
  damage += sbc > 0 ? sbc : 0;                            // + max(SetByCaller Data.Damage, 0)
  float unmitigated = damage;
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
    if (!strcmp(d->execs[i], "GDDamageExecCalculation")) exec_damage(tgt, src, spec);
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
  if (d->dur == U64_DUR_INSTANT) { run_modifiers(tgt, src, d, 0, spec); return 0; }
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
    if (d->period > 0 && !e->inhibited) run_modifiers(tgt, src, d, e, spec);       // execute on application
    update_inhibition(tgt);
    u64_notify_stack_tasks(tgt->obj, d->name);
    return i + 1;
  }
  return 0;
}

void u64_gas_tick(float dt)
{
  for (int i = 0; i < MAX_ACTIVE; i++) {
    active_t *e = &active[i];
    if (!e->live) continue;
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
  for (int i = 0; i < MAX_ASC; i++) if (!ascs[i].used) {
    asc_t *a = &ascs[i]; memset(a, 0, sizeof *a); a->used = 1; a->owner = actor;
    char nmb[64]; snprintf(nmb, sizeof nmb, "%s.ASC", u64_obj_name(actor));
    a->obj = u64_new(u64_intern(nmb), "AbilitySystemComponent");
    *u64_ivar(a->obj, "asc_idx") = u64_int(i + 1);
    *u64_ivar(a->obj, "Owner") = u64_objv(actor);
    *slot = u64_objv(a->obj);
    return a->obj;
  }
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
    float blend; float len = u64_montage_length(nm(*dm), &blend);                 // PlayAnimMontage(DeathMontage)
    if (len > 0) { u64_timer_latent(len, u64_latent(u64_objv(actor), "FinishDying", 0)); return; }
  }
  u64_obj_destroy(actor);                                                         // FinishDying() -> Destroy()
}
void u64_kill(u64_obj *o) { u64_die(o); }

void u64_actor_init(u64_obj *actor)
{
  if (!actor || u64_ivar(actor, "DefaultAttributes")->t == U64_NONE) return;
  u64_obj *asc = u64_asc_of(actor); asc_t *a = asc_from(asc);
  if (!a || u64_ivar(actor, "gas_inited")->t != U64_NONE) return;
  *u64_ivar(actor, "gas_inited") = u64_bool(1);
  // InitializeAttributes: DefaultAttributes applied at GetCharacterLevel()
  u64_apply_ge(asc, asc, nm(*u64_ivar(actor, "DefaultAttributes")), attr(a, "CharacterLevel")->cur, 0);
  // Hero respawn path (PossessedBy): SetHealth/Mana/Stamina to max
  set_base(a, "Health", attr(a, "MaxHealth")->cur);
  set_base(a, "Mana", attr(a, "MaxMana")->cur);
  set_base(a, "Stamina", attr(a, "MaxStamina")->cur);
  u64_v se = *u64_ivar(actor, "StartupEffects");                                 // AddStartupEffects
  for (int i = 0; i < u64_array_len(se); i++) u64_apply_ge(asc, asc, nm(u64_array_get(se, i)), attr(a, "CharacterLevel")->cur, 0);
  u64_v ca = *u64_ivar(actor, "CharacterAbilities");                             // AddCharacterAbilities
  for (int i = 0; i < u64_array_len(ca) && a->nabil < MAX_ABIL; i++) {
    const char *cls = strip_c(nm(u64_array_get(ca, i)));
    char b[64]; snprintf(b, sizeof b, "%s.%s", u64_obj_name(actor), cls);
    u64_obj *ab = u64_new(u64_intern(b), u64_intern(cls));
    *u64_ivar(ab, "Avatar") = u64_objv(actor); *u64_ivar(ab, "ASC") = u64_objv(asc);
    a->abil[a->nabil++] = ab;
  }
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
  if (!cooldown_ok(a, ability_ge(ab, "CooldownGameplayEffectClass"))) return 0;
  if (!cost_ok(a, ability_ge(ab, "CostGameplayEffectClass"))) return 0;
  // activate: cancel abilities, grant owned tags, block tags, run the Blueprint
  u64_v cancel = *u64_ivar(ab, "CancelAbilitiesWithTag");
  for (int i = 0; i < u64_array_len(cancel); i++) cancel_abilities(a, tag_i(cancel, i), 0, ab);
  *u64_ivar(ab, "Active") = u64_bool(1); *u64_ivar(ab, "Ended") = u64_bool(0);
  u64_v owned = *u64_ivar(ab, "ActivationOwnedTags");
  for (int i = 0; i < u64_array_len(owned); i++) tag_add(a, tag_i(owned, i), 1);
  u64_v block = *u64_ivar(ab, "BlockAbilitiesWithTag");
  for (int i = 0; i < u64_array_len(block); i++) { char k[96]; snprintf(k, sizeof k, "blocked:%s", tag_i(block, i)); tag_add(a, k, 1); }
  if (u64_log) u64_log("  [ability] %s activated", u64_obj_name(ab));
  u64_call(ab, "K2_ActivateAbility", 0, 0);
  return 1;
}
int u64_ability_commit(u64_obj *ab, int cost, int cooldown)
{
  asc_t *a = asc_from(u64_ivar(ab, "ASC")->u.o);
  if (!a) return 1;                                      // an ability without an ASC (not given) cannot be checked
  const u64_ge_def *cd = ability_ge(ab, "CooldownGameplayEffectClass"), *co = ability_ge(ab, "CostGameplayEffectClass");
  if ((cooldown && !cooldown_ok(a, cd)) || (cost && !cost_ok(a, co))) return 0;
  u64_obj *asc = a->obj;
  if (cooldown && cd) u64_apply_ge(asc, asc, cd->name, 1, 0);
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
}
int u64_input_press(u64_obj *actor, const char *id)
{
  asc_t *a = asc_from(u64_asc_of(actor));
  for (int i = 0; a && i < a->nabil; i++)
    if (!strcmp(nm(*u64_ivar(a->abil[i], "AbilityInputID")), id)) return u64_ability_try_activate(a->abil[i]);
  return 0;
}
void u64_input_release_id(u64_obj *actor, const char *id)
{
  asc_t *a = asc_from(u64_asc_of(actor));
  for (int i = 0; a && i < a->nabil; i++)
    if (!strcmp(nm(*u64_ivar(a->abil[i], "AbilityInputID")), id) && u64_true(*u64_ivar(a->abil[i], "Active"))) u64_input_release(a->abil[i]);
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
int u64_overlapping(u64_obj *self, u64_obj **out, int max)       // AABB overlap of the actor's box/capsule shapes
{
  if (self && !strcmp(u64_obj_class(self), "Component")) self = u64_asobj(*u64_ivar(self, "Owner"));   // component -> its actor
  if (!self) return 0;
  const u64_shape_def *ss = shape_of(self);
  if (!ss) return 0;
  u64_v *l = u64_ivar(self, "Location");
  if (l->t != U64_VEC) return 0;
  float sx = l->u.v[0], sy = l->u.v[1], sz = l->u.v[2];
  float ex = ss->kind == 1 ? ss->a : ss->a, ey = ss->kind == 1 ? ss->b : ss->a, ez = ss->kind == 1 ? ss->c : ss->b;
  int n = 0;
  for (int i = 0; i < u64_obj_count() && n < max; i++) {
    u64_obj *o = u64_obj_at(i);
    if (o == self || !u64_obj_alive(o)) continue;
    const u64_shape_def *os = shape_of(o); u64_v *ol = u64_ivar(o, "Location");
    if (!os || ol->t != U64_VEC || os->kind != 2) continue;             // only characters (capsules) are overlapped
    float rx = os->a, rz = os->b;
    if (fabsf(ol->u.v[0] - sx) <= ex + rx && fabsf(ol->u.v[1] - sy) <= ey + rx && fabsf(ol->u.v[2] - sz) <= ez + rz) out[n++] = o;
  }
  return n;
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
