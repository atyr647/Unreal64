// Remaining engine natives: text, animation queries, UI widgets, collision, Gameplay Ability System (GAS-lite),
// ability tasks, and the C++ gameplay functions of the converted project (movement, death).
//
// Fidelity notes. Ported faithfully from the project's C++: damage mitigation (Damage * 100/(100+Armor)),
// Die -> OnCharacterDied -> Destroy, sprint = 1.4x move speed. Approximated: GameplayEffect modifier magnitudes are
// keyed off the effect class name (modifier arrays are not parsed from the assets yet); attribute defaults are fixed.
#include "u64_rt.h"
#include <math.h>
#include <stdio.h>

#define IS(n) (!strcmp(fn, n))
static u64_v A(int argc, const u64_v *a, int i) { return i < argc ? a[i] : (u64_v){U64_NONE}; }
static float F(u64_v v) { return v.t == U64_INT || v.t == U64_BOOL ? (float)v.u.i : v.t == U64_FLOAT ? v.u.f : 0.0f; }
static const u64_v NONE_ = {U64_NONE};

// ---- string pool ------------------------------------------------------------------------------------------------
static char pool[8192]; static int pool_n;
const char *u64_intern(const char *s)
{
  for (int i = 0; i < pool_n; i += (int)strlen(pool + i) + 1) if (!strcmp(pool + i, s)) return pool + i;
  int n = (int)strlen(s) + 1;
  if (pool_n + n > (int)sizeof pool) return "?";
  memcpy(pool + pool_n, s, n); pool_n += n;
  return pool + pool_n - n;
}
static const char *key(const char *prefix, const char *name)
{
  char b[96]; snprintf(b, sizeof b, "%s%s", prefix, name ? name : ""); return u64_intern(b);
}
static const char *nm(u64_v v)   // class / name / string / object -> its name
{
  if (v.t == U64_CLASS || v.t == U64_NAME || v.t == U64_STR || v.t == U64_DELEGATE) return v.u.s ? v.u.s : "";
  if (v.t == U64_OBJ && v.u.o) return u64_obj_name(v.u.o);
  return "";
}
static const char *tostr(u64_v v, char *b, int n)
{
  switch (v.t) {
    case U64_INT: case U64_BOOL: snprintf(b, n, "%d", v.u.i); return b;
    case U64_FLOAT: snprintf(b, n, "%g", v.u.f); return b;
    case U64_VEC: snprintf(b, n, "(%g,%g,%g)", v.u.v[0], v.u.v[1], v.u.v[2]); return b;
    default: return nm(v);
  }
}
static u64_v text(const char *s) { return u64_str(u64_intern(s)); }

// ---- actors, attributes, tags -----------------------------------------------------------------------------------
static u64_obj *asc_of(u64_obj *actor)
{
  if (!actor) return 0;
  u64_v *slot = u64_ivar(actor, "AbilitySystemComponent");
  if (slot->t == U64_OBJ && slot->u.o) return slot->u.o;
  u64_obj *asc = u64_new(u64_intern(key(u64_obj_name(actor), ".ASC")), "AbilitySystemComponent");
  *u64_ivar(asc, "Owner") = u64_objv(actor);
  static const char *attrs[] = {"Health", "MaxHealth", "Mana", "MaxMana", "Stamina", "MaxStamina"};
  for (int i = 0; i < 6; i++) *u64_ivar(asc, attrs[i]) = u64_float(100);
  *u64_ivar(asc, "Armor") = u64_float(0);
  *u64_ivar(asc, "MoveSpeed") = u64_float(600);
  *slot = u64_objv(asc);
  return asc;
}
static u64_obj *owner_of(u64_obj *asc) { u64_v *o = u64_ivar(asc, "Owner"); return o->t == U64_OBJ ? o->u.o : 0; }
static float attr(u64_obj *asc, const char *n) { return F(*u64_ivar(asc, n)); }

#define MAX_TASKS 48
static u64_obj *tasks[MAX_TASKS]; static int ntasks;
static void track(u64_obj *t) { if (ntasks < MAX_TASKS) tasks[ntasks++] = t; }
static void untrack(u64_obj *t) { for (int i = 0; i < ntasks; i++) if (tasks[i] == t) { tasks[i] = tasks[--ntasks]; return; } }
static void fire_tasks(const char *cls, const char *delegate, int argc, const u64_v *argv)
{
  u64_obj *snap[MAX_TASKS]; int n = 0;
  for (int i = 0; i < ntasks; i++) if (!strcmp(u64_obj_class(tasks[i]), cls)) snap[n++] = tasks[i];
  for (int i = 0; i < n; i++) u64_mc_broadcast(u64_ivar(snap[i], delegate), argc, argv);
}

static void set_attr(u64_obj *asc, const char *n, float v)
{
  float old = attr(asc, n);
  char mx[32]; snprintf(mx, sizeof mx, "Max%s", n);
  float cap = (!strcmp(n, "Health") || !strcmp(n, "Mana") || !strcmp(n, "Stamina")) ? attr(asc, mx) : 1e9f;
  if (v > cap) v = cap;
  if (v < 0) v = 0;
  *u64_ivar(asc, n) = u64_float(v);
  u64_v args[3] = {u64_name(n), u64_float(v), u64_float(old)};
  fire_tasks("AbilityTask_ListenForAttributesChange", "AttributeChanged", 3, args);
}

// ---- gameplay effects -------------------------------------------------------------------------------------------
#define MAX_EFFECTS 32
static struct { int live; u64_obj *asc; const char *cls; const char *tag; } effects[MAX_EFFECTS];

static void die(u64_obj *actor)    // C++ AGDCharacterBase::Die: broadcast OnCharacterDied, then FinishDying -> Destroy
{
  if (u64_log) u64_log("  [die] %s", u64_obj_name(actor));
  u64_kill(actor);
}

static int apply_effect(u64_obj *asc, const char *cls, float level, u64_obj *spec)
{
  if (!asc) return 0;
  u64_obj *owner = owner_of(asc);
  if (u64_log) u64_log("  [gas] %s <- %s (level %d)", owner ? u64_obj_name(owner) : "?", cls, (int)level);
  const char *tag = 0;
  if (strstr(cls, "Damage") && !strstr(cls, "Receive")) {
    float dmg = spec ? F(*u64_ivar(spec, "sbc:Data.Damage")) : 0.0f;
    if (dmg <= 0) dmg = 25.0f * level;                                  // fallback when no SetByCaller magnitude
    float armor = attr(asc, "Armor");
    float mitigated = dmg * (100.0f / (100.0f + armor));                // GDDamageExecCalculation
    set_attr(asc, "Health", attr(asc, "Health") - mitigated);
    u64_v args[3] = {u64_objv(0), u64_float(dmg), u64_float(mitigated)};
    fire_tasks("AbilityTask_WaitReceiveDamage", "OnDamage", 3, args);   // UGDAbilitySystemComponent::ReceiveDamage
    if (attr(asc, "Health") <= 0 && owner) die(owner);
    return 0;                                                            // instant effect: no handle
  }
  if (strstr(cls, "Regen") || strstr(cls, "Heal") || strstr(cls, "HealthManaStamina")) {
    set_attr(asc, "Health", attr(asc, "Health") + 20 * level);
    set_attr(asc, "Mana", attr(asc, "Mana") + 20 * level);
    set_attr(asc, "Stamina", attr(asc, "Stamina") + 20 * level);
    return 0;
  }
  if (strstr(cls, "AimDownSightRemoval")) { *u64_ivar(asc, "tag:State.AimDownSights") = u64_bool(0); if (owner) *u64_ivar(owner, "bAiming") = u64_bool(0); return 0; }
  if (strstr(cls, "AimDownSight")) { tag = "State.AimDownSights"; if (owner) *u64_ivar(owner, "bAiming") = u64_bool(1); }
  else if (strstr(cls, "Sprint")) { tag = "State.Sprinting"; if (owner) *u64_ivar(owner, "bSprinting") = u64_bool(1); }
  else if (strstr(cls, "Armor")) { set_attr(asc, "Armor", attr(asc, "Armor") + 10 * level); }
  if (tag) *u64_ivar(asc, key("tag:", tag)) = u64_bool(1);
  for (int i = 0; i < MAX_EFFECTS; i++) if (!effects[i].live) {
    effects[i].live = 1; effects[i].asc = asc; effects[i].cls = u64_intern(cls); effects[i].tag = tag ? u64_intern(tag) : 0;
    return i + 1;
  }
  return 0;
}
static void remove_effect(int i)
{
  if (i < 0 || i >= MAX_EFFECTS || !effects[i].live) return;
  if (effects[i].tag) {
    *u64_ivar(effects[i].asc, key("tag:", effects[i].tag)) = u64_bool(0);
    u64_obj *o = owner_of(effects[i].asc);
    if (o && !strcmp(effects[i].tag, "State.Sprinting")) *u64_ivar(o, "bSprinting") = u64_bool(0);
    if (o && !strcmp(effects[i].tag, "State.AimDownSights")) *u64_ivar(o, "bAiming") = u64_bool(0);
  }
  effects[i].live = 0;
}

// ---- ability tasks ----------------------------------------------------------------------------------------------
static u64_obj *new_task(const char *cls, u64_obj *ability)
{
  static int serial; char b[40]; snprintf(b, sizeof b, "%s_%d", cls, ++serial);
  u64_obj *t = u64_new(u64_intern(b), u64_intern(cls));
  *u64_ivar(t, "Ability") = u64_objv(ability);
  return t;
}
static void activate_task(u64_obj *t)
{
  const char *c = u64_obj_class(t);
  track(t);
  if (!strcmp(c, "AbilityTask_WaitDelay")) u64_timer_broadcast(F(*u64_ivar(t, "Time")), t, "OnFinish", u64_float(F(*u64_ivar(t, "Time"))));
  else if (!strcmp(c, "AbilityTask_PlayMontageAndWaitForEvent")) u64_timer_broadcast(1.0f, t, "OnCompleted", u64_name("None"));
  else if (!strcmp(c, "AbilityTask_ApplyRootMotionConstantForce")) u64_timer_broadcast(F(*u64_ivar(t, "Duration")), t, "OnFinish", NONE_);
  else if (!strcmp(c, "AbilityTask_WaitNetSync")) u64_timer_broadcast(0.0f, t, "OnSync", NONE_);
  else if (!strcmp(c, "AbilityTask_WaitTargetData")) u64_timer_broadcast(0.0f, t, "ValidData", u64_objv(u64_new("TargetData", "GameplayAbilityTargetData")));
}
void u64_input_release(u64_obj *ability)
{
  (void)ability;
  u64_v t = u64_float(0);
  fire_tasks("AbilityTask_WaitInputRelease", "OnRelease", 1, &t);
}

// ---- helpers for rotators / text ----------------------------------------------------------------------------------
static float norm180(float d) { while (d > 180) d -= 360; while (d <= -180) d += 360; return d; }

u64_v u64_native_gas(u64_obj *self, const char *fn, int argc, const u64_v *a)
{
  u64_v x = A(argc, a, 0), y = A(argc, a, 1), z = A(argc, a, 2);
  char b1[48], b2[48];

  // ---------------- math / text ----------------
  if (IS("Add_DoubleDouble")) return u64_float(F(x) + F(y));
  if (IS("Conv_IntToText")) { snprintf(b1, sizeof b1, "%d", x.u.i); return text(b1); }
  if (IS("Conv_FloatToText")) {
    int maxfrac = argc > 7 ? (int)F(a[7]) : 3; if (maxfrac < 0 || maxfrac > 6) maxfrac = 3;
    snprintf(b1, sizeof b1, "%.*f", maxfrac, F(x));
    if (strchr(b1, '.')) { char *e = b1 + strlen(b1) - 1; while (*e == '0') *e-- = 0; if (*e == '.') *e = 0; }
    return text(b1);
  }
  if (IS("Conv_NameToString")) return text(nm(x));
  if (IS("Format")) {                                    // "{0}/{1}" style FText::Format
    char out[160]; int n = 0; const char *p = nm(x);
    while (*p && n < 150) {
      if (*p == '{' && p[1] >= '0' && p[1] <= '9' && p[2] == '}') { n += snprintf(out + n, sizeof out - n, "%s", tostr(A(argc, a, 1 + p[1] - '0'), b2, sizeof b2)); p += 3; }
      else out[n++] = *p++;
    }
    out[n] = 0; return text(out);
  }
  // ---------------- vectors / rotators (out-parameters arrive as references) ----------------
  if (IS("BreakVector")) { u64_out(y, u64_float(x.u.v[0])); u64_out(z, u64_float(x.u.v[1])); u64_out(A(argc, a, 3), u64_float(x.u.v[2])); return NONE_; }
  if (IS("BreakRotator")) { u64_out(y, u64_float(x.u.v[2])); u64_out(z, u64_float(x.u.v[0])); u64_out(A(argc, a, 3), u64_float(x.u.v[1])); return NONE_; }  // roll, pitch, yaw
  if (IS("BreakTransform")) { u64_out(y, x); u64_out(z, u64_vec(0, 0, 0)); u64_out(A(argc, a, 3), u64_vec(1, 1, 1)); return NONE_; }
  if (IS("BreakHitResult") || IS("BreakGameplayCueParameters")) { for (int i = 1; i < argc; i++) u64_out(a[i], u64_int(0)); return NONE_; }
  if (IS("NormalizedDeltaRotator")) return u64_vec(norm180(x.u.v[0] - y.u.v[0]), norm180(x.u.v[1] - y.u.v[1]), norm180(x.u.v[2] - y.u.v[2]));
  if (IS("K2_GetActorRotation") || IS("GetBaseAimRotation")) { u64_v *r = u64_ivar(self, "Rotation"); return r->t == U64_VEC ? *r : u64_vec(0, 0, 0); }
  if (IS("GetActorForwardVector")) { u64_v *r = u64_ivar(self, "Rotation"); float yaw = (r->t == U64_VEC ? r->u.v[1] : 0) * 0.0174533f; return u64_vec(cosf(yaw), sinf(yaw), 0); }
  if (IS("CalculateDirection")) {                        // angle of the velocity relative to where the actor faces
    float heading = atan2f(x.u.v[1], x.u.v[0]) * 57.29578f;
    return u64_float(norm180(heading - y.u.v[1]));
  }
  // ---------------- movement / animation queries ----------------
  if (IS("GetVelocity")) { u64_v *v = u64_ivar(self, "Velocity"); return v->t == U64_VEC ? *v : u64_vec(0, 0, 0); }
  if (IS("GetCurrentAcceleration")) { u64_v *v = u64_ivar(self, "Acceleration"); return v->t == U64_VEC ? *v : u64_vec(0, 0, 0); }
  if (IS("IsFalling")) return u64_bool(u64_true(*u64_ivar(self, "bFalling")));
  if (IS("GetMovementComponent")) { u64_v *m = u64_ivar(self, "MovementComponent"); return m->t == U64_OBJ ? *m : u64_objv(self); }
  if (IS("TryGetPawnOwner")) { u64_v *p = u64_ivar(self, "Pawn"); return p->t == U64_OBJ ? *p : u64_objv(self); }
  if (IS("PlayAnimation") || IS("GetInstanceAssetPlayerTimeFromEndFraction")) return u64_float(0);
  // ---------------- C++ gameplay of the converted project ----------------
  if (IS("StartSprinting")) { *u64_ivar(self, "bSprinting") = u64_bool(1); return NONE_; }
  if (IS("StopSprinting"))  { *u64_ivar(self, "bSprinting") = u64_bool(0); return NONE_; }
  if (IS("StartAimDownSights")) { *u64_ivar(self, "bAiming") = u64_bool(1); return NONE_; }
  if (IS("StopAimDownSights"))  { *u64_ivar(self, "bAiming") = u64_bool(0); return NONE_; }
  if (IS("FinishDying")) { u64_kill(self); return NONE_; }
  if (IS("GetAbilityLevel")) return u64_int(1);          // AGDCharacterBase::GetAbilityLevel returns 1
  // ---------------- UI widgets ----------------
  if (IS("SetText"))       { *u64_ivar(self, "Text") = x; return NONE_; }
  if (IS("SetPercent"))    { *u64_ivar(self, "Percent") = x; return NONE_; }
  if (IS("SetVisibility")) { *u64_ivar(self, "Visibility") = x; return NONE_; }
  if (IS("SetOpacity"))    { *u64_ivar(self, "Opacity") = x; return NONE_; }
  if (IS("GetUserWidgetObject")) { u64_v *w = u64_ivar(self, "Widget"); return w->t == U64_OBJ ? *w : u64_objv(self); }
  if (IS("GetOwningPlayer")) return u64_objv(u64_find("PlayerController"));
  // ---------------- property setters, components, collision, effects ----------------
  if (IS("SetObjectPropertyByName") || IS("SetDoublePropertyByName") || IS("SetStructurePropertyByName")) {
    if (x.t == U64_OBJ && x.u.o) *u64_ivar(x.u.o, u64_intern(nm(y))) = z;
    return NONE_;
  }
  if (IS("SetCollisionProfileNameProperty")) { *u64_ivar(self, "CollisionProfile") = y; return NONE_; }
  if (IS("K2_DestroyComponent")) { if (x.t == U64_OBJ) u64_kill(x.u.o); return NONE_; }
  if (IS("GetCollisionObjectType")) return u64_int(0);
  if (IS("LineTraceMultiForObjects")) return u64_bool(0);        // no physics world: nothing is ever hit
  if (IS("GetOverlappingActors")) { u64_out(A(argc, a, 2), u64_int(0)); return NONE_; }
  if (IS("SpawnEmitterAtLocation") || IS("SpawnEmitterAttached")) {
    if (u64_log) u64_log("  [fx] emitter %s", nm(x));
    return u64_objv(u64_new("Emitter", "ParticleSystemComponent"));
  }
  if (IS("BeginSpawningActor")) { u64_obj *o = u64_spawn_actor(nm(y), u64_vec(0, 0, 0)); return u64_objv(o); }
  // ---------------- Gameplay Ability System ----------------
  if (IS("GetAbilitySystemComponent")) {
    u64_obj *actor = x.t == U64_OBJ ? x.u.o : self;
    return u64_objv(asc_of(actor));
  }
  if (IS("GetActorInfo")) {                              // FGameplayAbilityActorInfo: avatar + its movement component
    u64_v *av = u64_ivar(self, "Avatar");
    u64_obj *info = u64_new(u64_intern(key(u64_obj_name(self), ".ActorInfo")), "GameplayAbilityActorInfo");
    *u64_ivar(info, "AvatarActor") = *av;
    *u64_ivar(info, "MovementComponent") = *av;          // movement is modelled on the actor itself
    return u64_objv(info);
  }
  if (IS("GetAvatarActorFromActorInfo") || IS("GetOwningActorFromActorInfo")) {
    u64_v *av = u64_ivar(self, "Avatar");
    if (av->t == U64_OBJ) return *av;
    u64_obj *o = owner_of(self);
    return u64_objv(o ? o : self);
  }
  if (IS("MakeEffectContext") || IS("GetEffectContext")) return u64_objv(u64_new("EffectContext", "GameplayEffectContext"));
  if (IS("EffectContextAddHitResult")) return NONE_;
  if (IS("EffectContextGetHitResult") || IS("GetHitResultFromTargetData")) return u64_int(0);
  if (IS("MakeOutgoingSpec") || IS("MakeOutgoingGameplayEffectSpec")) {
    int ia = IS("MakeOutgoingSpec") ? 0 : 0;                   // (class, level[, context]) after the receiver
    u64_obj *spec = u64_new("EffectSpec", "GameplayEffectSpec");
    *u64_ivar(spec, "GEClass") = u64_name(u64_intern(nm(A(argc, a, ia))));
    *u64_ivar(spec, "Level") = A(argc, a, ia + 1);
    return u64_objv(spec);
  }
  if (IS("AssignTagSetByCallerMagnitude")) {
    if (x.t == U64_OBJ && x.u.o) *u64_ivar(x.u.o, key("sbc:", nm(y))) = z;
    return x;
  }
  if (IS("BP_ApplyGameplayEffectToOwner")) {           // called on the ability: (class, level, stacks)
    u64_v *av = u64_ivar(self, "Avatar"); u64_obj *actor = av->t == U64_OBJ ? av->u.o : (owner_of(self) ? owner_of(self) : self);
    return u64_int(apply_effect(asc_of(actor), nm(x), F(y) > 0 ? F(y) : 1, 0));
  }
  if (IS("BP_ApplyGameplayEffectToSelf")) return u64_int(apply_effect(!strcmp(u64_obj_class(self), "AbilitySystemComponent") ? self : asc_of(self), nm(x), F(y) > 0 ? F(y) : 1, 0));
  if (IS("BP_ApplyGameplayEffectSpecToSelf")) {
    u64_obj *spec = x.t == U64_OBJ ? x.u.o : 0;
    u64_obj *asc = !strcmp(u64_obj_class(self), "AbilitySystemComponent") ? self : asc_of(self);
    return u64_int(spec ? apply_effect(asc, nm(*u64_ivar(spec, "GEClass")), F(*u64_ivar(spec, "Level")) > 0 ? F(*u64_ivar(spec, "Level")) : 1, spec) : 0);
  }
  if (IS("BP_RemoveGameplayEffectFromOwnerWithHandle")) { remove_effect(x.u.i - 1); return u64_int(1); }
  if (IS("BP_RemoveGameplayEffectFromOwnerWithGrantedTags") || IS("RemoveActiveGameplayEffectBySourceEffect")) {
    for (int i = 0; i < MAX_EFFECTS; i++) if (effects[i].live && effects[i].tag && (!strcmp(effects[i].tag, nm(x)) || strstr(nm(x), "Sprint"))) remove_effect(i);
    return u64_int(0);
  }
  if (IS("HasMatchingGameplayTag")) { u64_obj *asc = !strcmp(u64_obj_class(self), "AbilitySystemComponent") ? self : asc_of(self); return u64_bool(u64_true(*u64_ivar(asc, key("tag:", nm(x))))); }
  if (IS("GetTagName")) return u64_name(nm(x));
  if (IS("EqualEqual_GameplayAttributeGameplayAttribute")) return u64_bool(!strcmp(nm(x), nm(y)));
  if (IS("K2_AddGameplayCue")) { if (u64_log) u64_log("  [cue] %s", nm(x)); return NONE_; }
  if (IS("SendGameplayEventToActor")) { if (u64_log) u64_log("  [event] %s -> %s", nm(y), x.t == U64_OBJ && x.u.o ? u64_obj_name(x.u.o) : "?"); return u64_int(0); }
  if (IS("K2_CommitAbility") || IS("K2_CommitAbilityCost")) return u64_bool(1);
  if (IS("K2_EndAbility") || IS("K2_CancelAbility")) {
    if (u64_true(*u64_ivar(self, "Ended"))) return NONE_;
    if (u64_log) u64_log("  [ability] %s ended", u64_obj_name(self));
    *u64_ivar(self, "Ended") = u64_bool(1);
    u64_timers_cancel_for(self);                         // abort the ability's outstanding tasks
    for (int i = 0; i < ntasks; ) { u64_v *ab = u64_ivar(tasks[i], "Ability"); if (ab->t == U64_OBJ && ab->u.o == self) tasks[i] = tasks[--ntasks]; else i++; }
    u64_call(self, "K2_OnEndAbility", 0, 0);             // the ability's OnEndAbility event (e.g. StopSprinting)
    return NONE_;
  }
  // ---------------- ability tasks ----------------
  if (IS("WaitDelay")) { u64_obj *t = new_task("AbilityTask_WaitDelay", x.u.o); *u64_ivar(t, "Time") = y; return u64_objv(t); }
  if (IS("PlayMontageAndWaitForEvent")) return u64_objv(new_task("AbilityTask_PlayMontageAndWaitForEvent", x.u.o));
  if (IS("ApplyRootMotionConstantForce")) { u64_obj *t = new_task("AbilityTask_ApplyRootMotionConstantForce", x.u.o); *u64_ivar(t, "Duration") = A(argc, a, 4); return u64_objv(t); }
  if (IS("WaitInputRelease")) return u64_objv(new_task("AbilityTask_WaitInputRelease", x.u.o));
  if (IS("WaitTargetData")) return u64_objv(new_task("AbilityTask_WaitTargetData", x.u.o));
  if (IS("WaitNetSync")) return u64_objv(new_task("AbilityTask_WaitNetSync", x.u.o));
  if (IS("WaitReceiveDamage")) return u64_objv(new_task("AbilityTask_WaitReceiveDamage", x.u.o));
  if (IS("WaitForGameplayEffectStackChange")) return u64_objv(new_task("AbilityTask_WaitForGameplayEffectStackChange", x.u.o));
  if (IS("ListenForAttributesChange")) return u64_objv(new_task("AbilityTask_ListenForAttributesChange", x.u.o));
  if (IS("ListenForCooldownChange") || IS("ListenForGameplayEffectStackChange")) return u64_objv(new_task("AbilityTask_ListenForOther", x.u.o));
  if (IS("MakeTargetLocationInfoFromOwnerActor")) return u64_objv(new_task("TargetLocationInfo", x.u.o));
  if (IS("ReadyForActivation") || IS("Activate")) { if (x.t == U64_OBJ && x.u.o) activate_task(x.u.o); else if (self) activate_task(self); return NONE_; }
  if (IS("EndTask")) { untrack(self); return NONE_; }
  // ---------------- latent actions ----------------
  if (IS("Delay")) { if (y.t == U64_FLOAT && z.t == U64_DELEGATE) u64_timer_latent(F(y), z); return NONE_; }
  if (IS("WaitDelay_") || IS("OnExecute") || IS("StackChange") || IS("UpdateDamage")) return NONE_;
  return NONE_;
}
