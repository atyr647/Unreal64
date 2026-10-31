// Engine natives called by Blueprints: text, vectors/rotators (with out-parameters), animation queries, UI widget state,
// collision queries, attribute accessors, Gameplay Ability System entry points and ability tasks. The GAS rules live in
// u64_gas_core.c; this file adapts the Blueprint-callable surface onto them.
#include "u64_gas_core.h"
#include <math.h>
#include <stdio.h>

#define IS(n) (!strcmp(fn, n))
static u64_v A(int argc, const u64_v *a, int i) { return i < argc ? a[i] : (u64_v){U64_NONE}; }
static float F(u64_v v) { return v.t == U64_INT || v.t == U64_BOOL ? (float)v.u.i : v.t == U64_FLOAT ? v.u.f : 0.0f; }
static const u64_v NONE_ = {U64_NONE};

// ---- string pool ----
static char pool[8192]; static int pool_n;
const char *u64_intern(const char *s)
{
  for (int i = 0; i < pool_n; i += (int)strlen(pool + i) + 1) if (!strcmp(pool + i, s)) return pool + i;
  int n = (int)strlen(s) + 1;
  if (pool_n + n > (int)sizeof pool) return "?";
  memcpy(pool + pool_n, s, n); pool_n += n;
  return pool + pool_n - n;
}
static const char *key(const char *prefix, const char *name) { char b[96]; snprintf(b, sizeof b, "%s%s", prefix, name ? name : ""); return u64_intern(b); }
static const char *nm(u64_v v)
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
static int is_asc(u64_obj *o) { return o && !strcmp(u64_obj_class(o), "AbilitySystemComponent"); }
static u64_obj *asc_for(u64_obj *self) { return is_asc(self) ? self : u64_asc_of(self); }
static u64_obj *avatar_of_ability(u64_obj *ab) { u64_v *av = u64_ivar(ab, "Avatar"); return av->t == U64_OBJ ? av->u.o : ab; }
static u64_obj *obj_arg(u64_v v) { return v.t == U64_OBJ ? v.u.o : 0; }
static float norm180(float d) { while (d > 180) d -= 360; while (d <= -180) d += 360; return d; }
static int tag_arg_has(u64_obj *asc, u64_v tags)           // a tag or a container of tags: any match
{
  if (tags.t == U64_ARRAY) { for (int i = 0; i < u64_array_len(tags); i++) if (u64_tag_has(asc, nm(u64_array_get(tags, i)))) return 1; return 0; }
  return u64_tag_has(asc, nm(tags));
}

// ---------------------------------------------------------------------------------------------------- ability tasks
#define MAX_TASKS 64
static u64_obj *tasks[MAX_TASKS]; static int ntasks;
static void track(u64_obj *t) { for (int i = 0; i < ntasks; i++) if (tasks[i] == t) return; if (ntasks < MAX_TASKS) tasks[ntasks++] = t; }
static void untrack(u64_obj *t) { for (int i = 0; i < ntasks; i++) if (tasks[i] == t) { tasks[i] = tasks[--ntasks]; return; } }
static void fire_tasks(const char *cls, const char *delegate, int argc, const u64_v *argv, u64_obj *asc)
{
  u64_obj *snap[MAX_TASKS]; int n = 0;
  for (int i = 0; i < ntasks; i++) {
    if (strcmp(u64_obj_class(tasks[i]), cls)) continue;
    if (asc && obj_arg(*u64_ivar(tasks[i], "ASC")) != asc) continue;
    snap[n++] = tasks[i];
  }
  for (int i = 0; i < n; i++) u64_mc_broadcast(u64_ivar(snap[i], delegate), argc, argv);
}
void u64_tasks_end_for(u64_obj *ability)
{
  for (int i = 0; i < ntasks; ) {
    u64_v *ab = u64_ivar(tasks[i], "Ability");
    if (ab->t == U64_OBJ && ab->u.o == ability) tasks[i] = tasks[--ntasks]; else i++;
  }
}
void u64_notify_attribute_tasks(u64_obj *asc, int argc, const u64_v *argv)      // AsyncTaskAttributeChanged
{
  u64_obj *snap[MAX_TASKS]; int n = 0;
  for (int i = 0; i < ntasks; i++) {
    if (strcmp(u64_obj_class(tasks[i]), "AsyncTaskAttributeChanged") || obj_arg(*u64_ivar(tasks[i], "ASC")) != asc) continue;
    u64_v list = *u64_ivar(tasks[i], "Attributes"); int hit = 0;
    for (int k = 0; k < u64_array_len(list); k++) if (!strcmp(nm(u64_array_get(list, k)), nm(argv[0]))) hit = 1;
    if (hit) snap[n++] = tasks[i];
  }
  for (int i = 0; i < n; i++) u64_mc_broadcast(u64_ivar(snap[i], "OnAttributeChanged"), argc, argv);
}
void u64_notify_damage_tasks(u64_obj *asc, int argc, const u64_v *argv) { fire_tasks("AbilityTask_WaitReceiveDamage", "OnDamage", argc, argv, asc); }
void u64_notify_stack_tasks(u64_obj *asc, const char *effect)
{
  (void)effect;
  for (int i = 0; i < ntasks; i++) {
    u64_obj *t = tasks[i];
    if (obj_arg(*u64_ivar(t, "ASC")) != asc) continue;
    if (!strcmp(u64_obj_class(t), "AsyncTaskEffectStackChanged")) {      // OnGameplayEffectStackChange(tag, handle, new, old)
      int now = u64_effect_stack_count(asc, nm(*u64_ivar(t, "Tag"))), old = u64_ivar(t, "Count")->u.i;
      if (now != old) {
        *u64_ivar(t, "Count") = u64_int(now);
        u64_v args[4] = {u64_name(nm(*u64_ivar(t, "Tag"))), u64_int(0), u64_int(now), u64_int(old)};
        u64_mc_broadcast(u64_ivar(t, "OnGameplayEffectStackChange"), 4, args);
      }
    } else if (!strcmp(u64_obj_class(t), "AsyncTaskCooldownChanged")) {  // OnCooldownBegin / OnCooldownEnd(tag, remaining, duration)
      u64_v list = *u64_ivar(t, "CooldownTags");
      for (int k = 0; k < u64_array_len(list); k++) {
        const char *tag = nm(u64_array_get(list, k)); float dur;
        float rem = u64_cooldown_remaining(asc, tag, &dur);
        const char *kk = key("cd:", tag); int was = u64_true(*u64_ivar(t, kk));
        if (rem > 0 && !was) { *u64_ivar(t, kk) = u64_bool(1); u64_v args[3] = {u64_name(tag), u64_float(rem), u64_float(dur)}; u64_mc_broadcast(u64_ivar(t, "OnCooldownBegin"), 3, args); }
        else if (rem <= 0 && was) { *u64_ivar(t, kk) = u64_bool(0); u64_v args[3] = {u64_name(tag), u64_float(-1), u64_float(dur)}; u64_mc_broadcast(u64_ivar(t, "OnCooldownEnd"), 3, args); }
      }
    }
  }
}

static u64_obj *new_task(const char *cls, u64_obj *ability)
{
  static int serial; char b[48]; snprintf(b, sizeof b, "%s_%d", cls, ++serial);
  u64_obj *t = u64_new(u64_intern(b), u64_intern(cls));
  *u64_ivar(t, "Ability") = u64_objv(ability);
  return t;
}
static void activate_task(u64_obj *t)
{
  const char *c = u64_obj_class(t);
  track(t);
  if (!strcmp(c, "AbilityTask_WaitDelay")) u64_timer_broadcast(F(*u64_ivar(t, "Time")), t, "OnFinish", u64_float(F(*u64_ivar(t, "Time"))));
  else if (!strcmp(c, "AbilityTask_PlayMontageAndWaitForEvent")) {
    // GDAT_PlayMontageAndWaitForEvent: OnBlendOut when the montage starts blending out, OnCompleted when it ends.
    float blend, len = u64_montage_length(nm(*u64_ivar(t, "Montage")), &blend), rate = F(*u64_ivar(t, "Rate")); if (rate <= 0) rate = 1;
    if (len < 0) { if (u64_log) u64_log("  [anim] montage %s has no data", nm(*u64_ivar(t, "Montage"))); len = 0; blend = 0; }
    float bo = (len - blend) / rate; if (bo < 0) bo = 0;
    u64_timer_broadcast(bo, t, "OnBlendOut", u64_name("None"));
    u64_timer_broadcast(len / rate, t, "OnCompleted", u64_name("None"));
  }
  else if (!strcmp(c, "AbilityTask_ApplyRootMotionConstantForce")) u64_timer_broadcast(F(*u64_ivar(t, "Duration")), t, "OnFinish", NONE_);
  else if (!strcmp(c, "AbilityTask_WaitNetSync")) u64_timer_broadcast(0.0f, t, "OnSync", NONE_);
  else if (!strcmp(c, "AbilityTask_WaitTargetData")) u64_timer_broadcast(0.0f, t, "ValidData", u64_objv(u64_new("TargetData", "GameplayAbilityTargetData")));
  else if (!strcmp(c, "AsyncTaskEffectStackChanged")) *u64_ivar(t, "Count") = u64_int(u64_effect_stack_count(obj_arg(*u64_ivar(t, "ASC")), nm(*u64_ivar(t, "Tag"))));
}
void u64_input_release(u64_obj *ability)
{
  u64_v t = u64_float(0);
  fire_tasks("AbilityTask_WaitInputRelease", "OnRelease", 1, &t, 0);
  (void)ability;
}

// ---------------------------------------------------------------------------------------------------- natives
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
  if (IS("Format")) {
    char out[160]; int n = 0; const char *p = nm(x);
    while (*p && n < 150) {
      if (*p == '{' && p[1] >= '0' && p[1] <= '9' && p[2] == '}') { n += snprintf(out + n, sizeof out - n, "%s", tostr(A(argc, a, 1 + p[1] - '0'), b2, sizeof b2)); p += 3; }
      else out[n++] = *p++;
    }
    out[n] = 0; return text(out);
  }
  // ---------------- vectors / rotators ----------------
  if (IS("BreakVector")) { u64_out(y, u64_float(x.u.v[0])); u64_out(z, u64_float(x.u.v[1])); u64_out(A(argc, a, 3), u64_float(x.u.v[2])); return NONE_; }
  if (IS("BreakRotator")) { u64_out(y, u64_float(x.u.v[2])); u64_out(z, u64_float(x.u.v[0])); u64_out(A(argc, a, 3), u64_float(x.u.v[1])); return NONE_; }  // roll, pitch, yaw
  if (IS("BreakTransform")) { u64_out(y, x); u64_out(z, u64_vec(0, 0, 0)); u64_out(A(argc, a, 3), u64_vec(1, 1, 1)); return NONE_; }
  if (IS("BreakHitResult") || IS("BreakGameplayCueParameters")) { for (int i = 1; i < argc; i++) u64_out(a[i], u64_int(0)); return NONE_; }
  if (IS("NormalizedDeltaRotator")) return u64_vec(norm180(x.u.v[0] - y.u.v[0]), norm180(x.u.v[1] - y.u.v[1]), norm180(x.u.v[2] - y.u.v[2]));
  if (IS("K2_GetActorRotation") || IS("GetBaseAimRotation")) { u64_v *r = u64_ivar(self, "Rotation"); return r->t == U64_VEC ? *r : u64_vec(0, 0, 0); }
  if (IS("GetActorForwardVector")) { u64_v *r = u64_ivar(self, "Rotation"); float yaw = (r->t == U64_VEC ? r->u.v[1] : 0) * 0.0174533f; return u64_vec(cosf(yaw), sinf(yaw), 0); }
  if (IS("CalculateDirection")) return u64_float(norm180(atan2f(x.u.v[1], x.u.v[0]) * 57.29578f - y.u.v[1]));
  // ---------------- movement / animation queries ----------------
  if (IS("GetVelocity")) { u64_v *v = u64_ivar(self, "Velocity"); return v->t == U64_VEC ? *v : u64_vec(0, 0, 0); }
  if (IS("GetCurrentAcceleration")) { u64_v *v = u64_ivar(self, "Acceleration"); return v->t == U64_VEC ? *v : u64_vec(0, 0, 0); }
  if (IS("IsFalling")) return u64_bool(u64_true(*u64_ivar(self, "bFalling")));
  if (IS("GetMovementComponent")) { u64_v *m = u64_ivar(self, "MovementComponent"); return m->t == U64_OBJ ? *m : u64_objv(self); }
  if (IS("TryGetPawnOwner")) { u64_v *p = u64_ivar(self, "Pawn"); return p->t == U64_OBJ ? *p : u64_objv(self); }
  if (IS("GetMaxSpeed")) return u64_float(u64_max_speed(self));
  if (IS("PlayAnimation") || IS("GetInstanceAssetPlayerTimeFromEndFraction")) return u64_float(0);
  // ---------------- C++ gameplay of the converted project ----------------
  if (IS("StartSprinting")) { *u64_ivar(self, "bSprinting") = u64_bool(1); return NONE_; }       // RequestToStartSprinting = true
  if (IS("StopSprinting"))  { *u64_ivar(self, "bSprinting") = u64_bool(0); return NONE_; }
  if (IS("StartAimDownSights")) { *u64_ivar(self, "bAiming") = u64_bool(1); return NONE_; }      // RequestToStartADS = true
  if (IS("StopAimDownSights"))  { *u64_ivar(self, "bAiming") = u64_bool(0); return NONE_; }
  if (IS("FinishDying")) { u64_obj_destroy(self); return NONE_; }                                // Destroy()
  if (IS("GetAbilityLevel")) return u64_int(1);                                                  // AGDCharacterBase::GetAbilityLevel
  if (IS("IsAlive")) { u64_obj *asc = u64_asc_of(self); return u64_bool(u64_obj_alive(self) && (!asc || u64_attr_get(asc, "Health") > 0)); }
  if (IS("GetHealth") || IS("GetMaxHealth") || IS("GetMana") || IS("GetMaxMana") || IS("GetStamina") || IS("GetMaxStamina") ||
      IS("GetHealthRegenRate") || IS("GetManaRegenRate") || IS("GetStaminaRegenRate") || IS("GetArmor") || IS("GetMoveSpeed") ||
      IS("GetCharacterLevel") || IS("GetXP") || IS("GetXPBounty") || IS("GetGold") || IS("GetGoldBounty"))
    return u64_float(u64_attr_get(u64_asc_of(self), fn + 3));                                    // AGDCharacterBase attribute getters
  if (IS("SetHealth") || IS("SetMana") || IS("SetStamina")) { return NONE_; }
  // ---------------- UI widgets ----------------
  if (IS("SetText"))       { *u64_ivar(self, "Text") = x; return NONE_; }
  if (IS("SetPercent"))    { *u64_ivar(self, "Percent") = x; return NONE_; }
  if (IS("SetVisibility")) { *u64_ivar(self, "Visibility") = x; return NONE_; }
  if (IS("SetOpacity"))    { *u64_ivar(self, "Opacity") = x; return NONE_; }
  if (IS("GetUserWidgetObject")) { u64_v *w = u64_ivar(self, "Widget"); return w->t == U64_OBJ ? *w : u64_objv(self); }
  if (IS("GetOwningPlayer")) return u64_objv(u64_find("PlayerController"));
  // ---------------- property setters, components, collision ----------------
  if (IS("SetObjectPropertyByName") || IS("SetDoublePropertyByName") || IS("SetStructurePropertyByName")) {
    if (x.t == U64_OBJ && x.u.o) *u64_ivar(x.u.o, u64_intern(nm(y))) = z;
    return NONE_;
  }
  if (IS("SetCollisionProfileNameProperty")) { *u64_ivar(self, "CollisionProfile") = y; return NONE_; }
  if (IS("K2_DestroyComponent")) { if (x.t == U64_OBJ) u64_obj_destroy(x.u.o); return NONE_; }
  if (IS("GetCollisionObjectType")) return u64_int(0);
  if (IS("LineTraceMultiForObjects")) return u64_bool(0);        // needs collision meshes (editor export): reports no hit
  if (IS("GetOverlappingActors")) {
    u64_obj *hit[16]; int n = u64_overlapping(self, hit, 16);
    u64_v arr = u64_array_of(0, 0);
    for (int i = 0; i < n; i++) u64_array_push(&arr, u64_objv(hit[i]));
    u64_out(A(argc, a, argc > 1 ? 1 : 0), arr);
    return NONE_;
  }
  if (IS("SpawnEmitterAtLocation") || IS("SpawnEmitterAttached")) {
    if (u64_log) u64_log("  [fx] emitter %s", nm(x));
    return u64_objv(u64_new("Emitter", "ParticleSystemComponent"));
  }
  if (IS("BeginSpawningActor")) return u64_objv(u64_spawn_actor(nm(y), u64_vec(0, 0, 0)));
  // ---------------- arrays ----------------
  if (IS("Array_Length")) return u64_int(u64_array_len(x));
  if (IS("Array_Get")) { u64_v item = u64_array_get(x, y.u.i); u64_out(z, item); return item; }
  // ---------------- Gameplay Ability System ----------------
  if (IS("GetAbilitySystemComponent")) return u64_objv(u64_asc_of(x.t == U64_OBJ ? x.u.o : self));
  if (IS("GetActorInfo")) {
    u64_v *av = u64_ivar(self, "Avatar");
    u64_obj *info = u64_new(u64_intern(key(u64_obj_name(self), ".ActorInfo")), "GameplayAbilityActorInfo");
    *u64_ivar(info, "AvatarActor") = *av;
    *u64_ivar(info, "MovementComponent") = *av;                  // movement is modelled on the actor itself
    return u64_objv(info);
  }
  if (IS("GetAvatarActorFromActorInfo") || IS("GetOwningActorFromActorInfo")) return u64_objv(avatar_of_ability(self));
  if (IS("MakeEffectContext") || IS("GetEffectContext")) return u64_objv(u64_new("EffectContext", "GameplayEffectContext"));
  if (IS("EffectContextAddHitResult")) return NONE_;
  if (IS("EffectContextGetHitResult") || IS("GetHitResultFromTargetData")) return u64_int(0);
  if (IS("MakeOutgoingSpec") || IS("MakeOutgoingGameplayEffectSpec")) {
    u64_obj *spec = u64_new("EffectSpec", "GameplayEffectSpec");
    *u64_ivar(spec, "GEClass") = u64_name(u64_intern(nm(x)));
    *u64_ivar(spec, "Level") = y;
    *u64_ivar(spec, "Source") = u64_objv(asc_for(self));
    return u64_objv(spec);
  }
  if (IS("AssignTagSetByCallerMagnitude")) {
    u64_obj *spec = obj_arg(x);
    if (spec) {
      int n = u64_ivar(spec, "sbcN")->u.i; char k[16];
      snprintf(k, sizeof k, "sbc%d_tag", n); *u64_ivar(spec, u64_intern(k)) = u64_name(u64_intern(nm(y)));
      snprintf(k, sizeof k, "sbc%d_val", n); *u64_ivar(spec, u64_intern(k)) = z;
      *u64_ivar(spec, "sbcN") = u64_int(n + 1);
    }
    return x;
  }
  if (IS("BP_ApplyGameplayEffectToOwner")) {                      // on the ability: (class, level, stacks)
    u64_obj *asc = u64_asc_of(avatar_of_ability(self));
    return u64_int(u64_apply_ge(asc, asc, nm(x), F(y), 0));
  }
  if (IS("BP_ApplyGameplayEffectToSelf")) { u64_obj *asc = asc_for(self); return u64_int(u64_apply_ge(asc, asc, nm(x), F(y), 0)); }
  if (IS("BP_ApplyGameplayEffectSpecToSelf")) {
    u64_obj *spec = obj_arg(x);
    if (!spec) return u64_int(0);
    return u64_int(u64_apply_ge(asc_for(self), obj_arg(*u64_ivar(spec, "Source")), nm(*u64_ivar(spec, "GEClass")), F(*u64_ivar(spec, "Level")), spec));
  }
  if (IS("BP_RemoveGameplayEffectFromOwnerWithHandle")) { u64_remove_ge(x.u.i); return u64_int(1); }
  if (IS("BP_RemoveGameplayEffectFromOwnerWithGrantedTags")) {
    u64_obj *asc = u64_asc_of(avatar_of_ability(self));
    if (x.t == U64_ARRAY) for (int i = 0; i < u64_array_len(x); i++) u64_remove_ge_with_tags(asc, nm(u64_array_get(x, i)));
    else u64_remove_ge_with_tags(asc, nm(x));
    return u64_int(0);
  }
  if (IS("RemoveActiveGameplayEffectBySourceEffect")) { u64_remove_ge_class(asc_for(self), nm(x)); return u64_int(0); }
  if (IS("HasMatchingGameplayTag")) return u64_bool(tag_arg_has(asc_for(self), x));
  if (IS("GetTagName")) return u64_name(nm(x));
  if (IS("EqualEqual_GameplayAttributeGameplayAttribute")) return u64_bool(!strcmp(nm(x), nm(y)));
  if (IS("K2_AddGameplayCue")) { if (u64_log) u64_log("  [cue] %s", nm(x)); return NONE_; }
  if (IS("SendGameplayEventToActor")) { if (u64_log) u64_log("  [event] %s -> %s", nm(y), x.t == U64_OBJ && x.u.o ? u64_obj_name(x.u.o) : "?"); return u64_int(0); }
  if (IS("K2_CommitAbility")) return u64_bool(u64_ability_commit(self, 1, 1));           // cooldown + cost
  if (IS("K2_CommitAbilityCost")) return u64_bool(u64_ability_commit(self, 1, 0));
  if (IS("K2_EndAbility")) { u64_ability_end(self, 0); return NONE_; }
  if (IS("K2_CancelAbility")) { u64_ability_end(self, 1); return NONE_; }
  // ---------------- ability tasks ----------------
  if (IS("WaitDelay")) { u64_obj *t = new_task("AbilityTask_WaitDelay", x.u.o); *u64_ivar(t, "Time") = y; return u64_objv(t); }
  if (IS("PlayMontageAndWaitForEvent")) {                          // (ability, taskName, montage, eventTags, rate, ...)
    u64_obj *t = new_task("AbilityTask_PlayMontageAndWaitForEvent", x.u.o);
    *u64_ivar(t, "Montage") = A(argc, a, 2); *u64_ivar(t, "Rate") = A(argc, a, 4);
    return u64_objv(t);
  }
  if (IS("ApplyRootMotionConstantForce")) { u64_obj *t = new_task("AbilityTask_ApplyRootMotionConstantForce", x.u.o); *u64_ivar(t, "Duration") = A(argc, a, 4); return u64_objv(t); }
  if (IS("WaitInputRelease")) return u64_objv(new_task("AbilityTask_WaitInputRelease", x.u.o));
  if (IS("WaitTargetData")) return u64_objv(new_task("AbilityTask_WaitTargetData", x.u.o));
  if (IS("WaitNetSync")) return u64_objv(new_task("AbilityTask_WaitNetSync", x.u.o));
  if (IS("WaitReceiveDamage")) { u64_obj *t = new_task("AbilityTask_WaitReceiveDamage", x.u.o); *u64_ivar(t, "ASC") = u64_objv(asc_for(obj_arg(x))); return u64_objv(t); }
  if (IS("WaitForGameplayEffectStackChange")) return u64_objv(new_task("AbilityTask_WaitForGameplayEffectStackChange", x.u.o));
  if (IS("ListenForAttributesChange")) {                           // (ASC, attributes[])
    u64_obj *t = new_task("AsyncTaskAttributeChanged", 0); *u64_ivar(t, "ASC") = x; *u64_ivar(t, "Attributes") = y; return u64_objv(t);
  }
  if (IS("ListenForGameplayEffectStackChange")) {                  // (ASC, effect tag)
    u64_obj *t = new_task("AsyncTaskEffectStackChanged", 0); *u64_ivar(t, "ASC") = x; *u64_ivar(t, "Tag") = y; return u64_objv(t);
  }
  if (IS("ListenForCooldownChange")) {                             // (ASC, cooldown tags, use server cooldown)
    u64_obj *t = new_task("AsyncTaskCooldownChanged", 0); *u64_ivar(t, "ASC") = x; *u64_ivar(t, "CooldownTags") = y; return u64_objv(t);
  }
  if (IS("MakeTargetLocationInfoFromOwnerActor")) return u64_objv(new_task("TargetLocationInfo", x.u.o));
  if (IS("ReadyForActivation") || IS("Activate")) { u64_obj *t = x.t == U64_OBJ && x.u.o ? x.u.o : self; if (t) activate_task(t); return NONE_; }
  if (IS("EndTask")) { untrack(self); return NONE_; }
  // ---------------- latent actions ----------------
  if (IS("Delay")) { if (y.t == U64_FLOAT && z.t == U64_DELEGATE) u64_timer_latent(F(y), z); return NONE_; }
  if (IS("OnExecute") || IS("StackChange") || IS("UpdateDamage")) return NONE_;
  return NONE_;
}
