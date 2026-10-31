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
static char pool[65536]; static int pool_n;
const char *u64_intern(const char *s)
{
  for (int i = 0; i < pool_n; i += (int)strlen(pool + i) + 1) if (!strcmp(pool + i, s)) return pool + i;
  int n = (int)strlen(s) + 1;
  if (pool_n + n > (int)sizeof pool) { if (!u64_rt_overflows++ && u64_log) u64_log("  [error] string pool exhausted"); return "?"; }
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
static int is_asc(u64_obj *o) { return o && (u64_is_asc(o) || !strcmp(u64_obj_class(o), "AbilitySystemComponent")); }
static u64_obj *asc_for(u64_obj *self)                                    // an ASC, or the ability system of an actor / of the actor that owns a component
{
  if (!self || is_asc(self)) return self;
  u64_v *ow = u64_ivar(self, "Owner");
  if (ow->t == U64_OBJ && ow->u.o && !u64_obj_is_actor(self)) return u64_asc_of(ow->u.o);
  return u64_asc_of(self);
}
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
void u64_deliver_gameplay_event_extra(u64_obj *asc, const char *tag, int argc, const u64_v *argv);
void u64_deliver_gameplay_event(u64_obj *asc, const char *tag, int argc, const u64_v *argv)
{
  u64_deliver_gameplay_event_extra(asc, tag, argc, argv);
  u64_ability_triggers(asc, tag, argc > 1 ? argv[1] : (u64_v){0});
  u64_obj *snap[MAX_TASKS]; int n = 0;
  for (int i = 0; i < ntasks; i++) {
    u64_obj *t = tasks[i];
    if (strcmp(u64_obj_class(t), "AbilityTask_PlayMontageAndWaitForEvent")) continue;
    if (u64_asc_of(avatar_of_ability(obj_arg(*u64_ivar(t, "Ability")))) != asc) continue;
    u64_v tags = *u64_ivar(t, "EventTags"); int hit = u64_array_len(tags) == 0;
    for (int k = 0; k < u64_array_len(tags); k++) { const char *want = nm(u64_array_get(tags, k)); size_t q = strlen(want); if (!strncmp(tag, want, q) && (tag[q] == 0 || tag[q] == '.')) hit = 1; }
    if (hit) snap[n++] = t;
  }
  for (int i = 0; i < n; i++) u64_mc_broadcast(u64_ivar(snap[i], "EventReceived"), argc, argv);
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
  u64_obj *t = u64_new_copy(b, u64_intern(cls));
  *u64_ivar(t, "Ability") = u64_objv(ability);
  return t;
}
u64_obj *u64_task_new(const char *cls, u64_obj *ability) { return new_task(cls, ability); }       // for u64_gas_extra.c
void u64_task_track(u64_obj *t) { track(t); }
void u64_task_untrack(u64_obj *t) { untrack(t); }
int u64_tasks_snapshot(u64_obj **out, int max) { int n = 0; for (int i = 0; i < ntasks && n < max; i++) out[n++] = tasks[i]; return n; }
int u64_task_activate_extra(u64_obj *t);
static void activate_task(u64_obj *t)
{
  const char *c = u64_obj_class(t);
  track(t);
  if (u64_task_activate_extra(t)) return;
  if (!strcmp(c, "AbilityTask_WaitDelay")) u64_timer_broadcast(F(*u64_ivar(t, "Time")), t, "OnFinish", u64_float(F(*u64_ivar(t, "Time"))));
  else if (!strcmp(c, "AbilityTask_PlayMontageAndWaitForEvent")) {
    // GDAT_PlayMontageAndWaitForEvent: OnBlendOut when the montage starts blending out, OnCompleted when it ends.
    float blend, len = u64_montage_length(nm(*u64_ivar(t, "Montage")), &blend), rate = F(*u64_ivar(t, "Rate")); if (rate <= 0) rate = 1;
    u64_montage_play(avatar_of_ability(obj_arg(*u64_ivar(t, "Ability"))), nm(*u64_ivar(t, "Montage")), rate);   // schedules AnimNotifies
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
int u64_native_gas_extra(u64_obj *self, const char *fn, int argc, const u64_v *a, u64_v *out);
u64_v u64_native_gas(u64_obj *self, const char *fn, int argc, const u64_v *a)
{
  u64_v x = A(argc, a, 0), y = A(argc, a, 1), z = A(argc, a, 2);
  { u64_v out; if (u64_native_gas_extra(self, fn, argc, a, &out)) return out; }                       // target data, containers, more tasks
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
  if (IS("BreakHitResult")) {                                       // UE 4.27/5.x layout: 17 parameters (hit + 16 outputs)
    u64_obj *h = obj_arg(x);
    if (!h) { for (int i = 1; i < argc; i++) u64_out(a[i], u64_int(0)); return NONE_; }
    u64_v actor = *u64_ivar(h, "HitActor");
    u64_out(A(argc, a, 1), *u64_ivar(h, "bBlockingHit")); u64_out(A(argc, a, 2), u64_bool(0));
    u64_out(A(argc, a, 3), *u64_ivar(h, "Time")); u64_out(A(argc, a, 4), *u64_ivar(h, "Distance"));
    u64_out(A(argc, a, 5), *u64_ivar(h, "Location")); u64_out(A(argc, a, 6), *u64_ivar(h, "Location"));
    u64_out(A(argc, a, 7), *u64_ivar(h, "Normal")); u64_out(A(argc, a, 8), *u64_ivar(h, "Normal"));
    u64_out(A(argc, a, 9), u64_int(0)); u64_out(A(argc, a, 10), actor);
    u64_v hc = *u64_ivar(h, "HitComponent");                                         // the traced component, else the pawn capsule
    u64_out(A(argc, a, 11), hc.t == U64_OBJ ? hc : actor.t == U64_OBJ ? u64_comp(actor.u.o, "CapsuleComponent") : u64_int(0));
    u64_out(A(argc, a, 12), u64_name("None")); u64_out(A(argc, a, 13), u64_int(0)); u64_out(A(argc, a, 14), u64_int(0));
    u64_out(A(argc, a, 15), *u64_ivar(h, "TraceStart")); u64_out(A(argc, a, 16), *u64_ivar(h, "TraceEnd"));
    return NONE_;
  }
  if (IS("BreakGameplayCueParameters")) { for (int i = 1; i < argc; i++) u64_out(a[i], u64_int(0)); return NONE_; }
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
  // ---------------- character input, control rotation, rotator math ----------------
  if (IS("AddMovementInput")) {                                                                  // APawn::AddMovementInput
    u64_v *in = u64_ivar(self, "InputVec"); float sc = F(y);
    float cur[3] = {in->t == U64_VEC ? in->u.v[0] : 0, in->t == U64_VEC ? in->u.v[1] : 0, in->t == U64_VEC ? in->u.v[2] : 0};
    if (x.t == U64_VEC) *in = u64_vec(cur[0] + x.u.v[0] * sc, cur[1] + x.u.v[1] * sc, cur[2] + x.u.v[2] * sc);
    return NONE_;
  }
  if (IS("AddControllerYawInput") || IS("AddControllerPitchInput")) {                            // APlayerController::AddYawInput/AddPitchInput
    u64_v *cr = u64_ivar(self, "ControlRotation");
    float p = cr->t == U64_VEC ? cr->u.v[0] : 0, yw = cr->t == U64_VEC ? cr->u.v[1] : 0, r = cr->t == U64_VEC ? cr->u.v[2] : 0;
    if (IS("AddControllerYawInput")) yw += F(x) * 2.5f; else { p += F(x) * 2.5f; if (p > 89.9f) p = 89.9f; if (p < -89.9f) p = -89.9f; }   // InputYawScale/PitchScale = 2.5, view pitch +-89.9
    while (yw > 180) yw -= 360; while (yw <= -180) yw += 360;
    *cr = u64_vec(p, yw, r);
    return NONE_;
  }
  if (IS("GetControlRotation")) { u64_v *cr = u64_ivar(self, "ControlRotation"); return cr->t == U64_VEC ? *cr : u64_vec(0, 0, 0); }
  if (IS("MakeRot")) return u64_vec(F(y), F(z), F(x));                                         // (roll, pitch, yaw) -> (pitch, yaw, roll)
  if (IS("BreakRot")) { u64_out(y, u64_float(x.u.v[2])); u64_out(z, u64_float(x.u.v[0])); u64_out(A(argc, a, 3), u64_float(x.u.v[1])); return NONE_; }
  if (IS("GetForwardVector") || IS("GetRightVector")) {                                         // FRotationMatrix axes
    float p = x.u.v[0] * 0.0174533f, yw = x.u.v[1] * 0.0174533f, r = x.u.v[2] * 0.0174533f;
    float sp = sinf(p), cp = cosf(p), sy = sinf(yw), cy = cosf(yw), sr = sinf(r), cr = cosf(r);
    if (IS("GetForwardVector")) return u64_vec(cp * cy, cp * sy, sp);
    return u64_vec(sr * sp * cy - cr * sy, sr * sp * sy + cr * cy, -sr * cp);
  }
  if (IS("Jump")) { *u64_ivar(self, "bPressedJump") = u64_bool(1); return NONE_; }               // ACharacter::Jump
  if (IS("StopJumping")) { *u64_ivar(self, "bPressedJump") = u64_bool(0); return NONE_; }
  if (IS("CanJump")) return u64_bool(!u64_true(*u64_ivar(self, "bFalling")) && u64_ivar(self, "JumpCount")->u.i < 1);   // JumpMaxCount = 1
  if (IS("EqualEqual_BoolBool")) return u64_bool(u64_true(x) == u64_true(y));
  if (IS("GetAnimAssetPlayerTimeFromEndFraction")) return u64_float(0);
  // ---------------- C++ gameplay of the converted project ----------------
  if (IS("StartSprinting")) { *u64_ivar(self, "bSprinting") = u64_bool(1); return NONE_; }       // RequestToStartSprinting = true
  if (IS("StopSprinting"))  { *u64_ivar(self, "bSprinting") = u64_bool(0); return NONE_; }
  if (IS("StartAimDownSights")) { *u64_ivar(self, "bAiming") = u64_bool(1); return NONE_; }      // RequestToStartADS = true
  if (IS("StopAimDownSights"))  { *u64_ivar(self, "bAiming") = u64_bool(0); return NONE_; }
  if (IS("FinishDying")) { u64_obj_destroy(self); return NONE_; }                                // AGDCharacterBase::FinishDying -> Destroy()
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
  if (IS("LineTraceMultiForObjects")) {                            // (world, start, end, objectTypes[], complex, ignore[], draw, OutHits, ignoreSelf, ...)
    u64_v types = A(argc, a, 3); int pawn = u64_array_len(types) == 0;
    for (int i = 0; i < u64_array_len(types); i++) if (u64_array_get(types, i).u.i == 2) pawn = 1;      // ObjectTypeQuery3 = Pawn
    u64_obj *hits[16]; int n = 0;
    if (pawn) {
      u64_obj *ign = u64_true(A(argc, a, 8)) ? self : 0;
      if (y.t == U64_VEC && z.t == U64_VEC) n = u64_line_trace(y.u.v, z.u.v, ign, hits, 16);
    }
    u64_v ignore = A(argc, a, 5), arr = u64_array_of(0, 0);
    for (int i = 0; i < n; i++) {
      int skip = 0; u64_obj *act = u64_asobj(*u64_ivar(hits[i], "HitActor"));
      for (int k = 0; k < u64_array_len(ignore); k++) if (u64_asobj(u64_array_get(ignore, k)) == act) skip = 1;
      if (!skip) u64_array_push(&arr, u64_objv(hits[i]));
    }
    u64_out(A(argc, a, 7), arr);
    return u64_bool(u64_array_len(arr) > 0);
  }
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
  // ---------------- Gameplay Ability System ----------------
  if (IS("GetAbilitySystemComponent")) return u64_objv(u64_asc_of(x.t == U64_OBJ ? x.u.o : self));
  if (IS("GetActorInfo")) {
    u64_v *av = u64_ivar(self, "Avatar");
    u64_obj *info = u64_new_copy("ActorInfo", "GameplayAbilityActorInfo");
    u64_obj *avatar = av->t == U64_OBJ ? av->u.o : 0;
    *u64_ivar(info, "AvatarActor") = *av; *u64_ivar(info, "OwnerActor") = *av;     // the avatar owns its ability system here
    *u64_ivar(info, "AbilitySystemComponent") = *u64_ivar(self, "ASC");
    *u64_ivar(info, "PlayerController") = avatar ? *u64_ivar(avatar, "Controller") : u64_objv(0);
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
    *u64_ivar(spec, "snapDamage") = u64_float(u64_attr_get(asc_for(self), "Damage"));      // FGameplayEffectAttributeCaptureDefinition: Snapshot = true
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
  if (IS("HasAnyMatchingGameplayTags")) return u64_bool(tag_arg_has(asc_for(self), x));
  if (IS("HasTag")) {                                                // FGameplayTagContainer::HasTag(tag): a contained tag equals or is a child of it
    for (int i = 0; i < u64_array_len(x); i++) { const char *h = nm(u64_array_get(x, i)), *q = nm(y); size_t n = strlen(q); if (!strncmp(h, q, n) && (h[n] == 0 || h[n] == '.')) return u64_bool(1); }
    return u64_bool(0);
  }
  if (IS("K2_AddLooseGameplayTags") || IS("K2_RemoveLooseGameplayTags")) {          // (tags, count)
    int n = y.t == U64_NONE ? 1 : y.u.i, sign = IS("K2_AddLooseGameplayTags") ? 1 : -1; u64_obj *asc = asc_for(self);
    if (x.t == U64_ARRAY) for (int i = 0; i < u64_array_len(x); i++) u64_tag_add_loose(asc, nm(u64_array_get(x, i)), sign * n);
    else u64_tag_add_loose(asc, nm(x), sign * n);
    return NONE_;
  }
  if (IS("K2_GetTagCount")) return u64_int(u64_tag_count(asc_for(self), nm(x)));
  if (IS("GetTimeSeconds")) return u64_float(u64_world_time());
  if (IS("PrintString")) { if (u64_log) u64_log("  [print] %s", nm(y)); return NONE_; }
  if (IS("IsValidClass")) return u64_bool(*nm(x) != 0);
  if (IS("IsLocallyControlled")) return u64_bool(self && self == u64_player_pawn());                       // a pawn
  if (IS("IsLocalPlayerController")) { u64_obj *p = u64_player_pawn(); return u64_bool(self && p && obj_arg(*u64_ivar(p, "Controller")) == self); }   // a controller
  if (IS("GetAbilitySystemComponentFromActorInfo")) return *u64_ivar(self, "ASC");
  if (IS("GetCurrentSourceObject") || IS("K2_GetSourceObject")) return *u64_ivar(self, "SourceObject");
  if (IS("GetFloatAttributeFromAbilitySystemComponent")) {                         // (actor, attribute, out found)
    u64_obj *asc = asc_for(obj_arg(x)); u64_out(A(argc, a, 2), u64_bool(asc != 0));
    return u64_float(asc ? u64_attr_get(asc, nm(y)) : 0.0f);
  }
  if (IS("RemoveActiveGameplayEffect")) { u64_remove_ge(x.u.i); return u64_int(0); }
  if (IS("GetPrimaryAbilityInstanceFromClass")) {                                  // (ASC, class): the granted ability object of that class
    u64_obj *list[64]; int n = u64_asc_abilities(asc_for(obj_arg(x) ? obj_arg(x) : self), list, 64);
    for (int i = 0; i < n; i++) { char cb[64]; snprintf(cb, sizeof cb, "%s", nm(y)); size_t cl = strlen(cb); if (cl > 2 && !strcmp(cb + cl - 2, "_C")) cb[cl - 2] = 0; if (u64_class_is(u64_obj_class(list[i]), cb)) return u64_objv(list[i]); }
    return u64_objv(0);
  }
  if (IS("GetPrimaryAbilityInstanceFromHandle")) return y;                         // (ASC, handle): a handle is the ability object
  if (IS("IsAbilitySpecHandleValid")) return u64_bool(obj_arg(x) != 0);
  if (IS("IsPrimaryAbilityInstanceActive")) return u64_bool(obj_arg(y) && u64_true(*u64_ivar(obj_arg(y), "Active")));    // (ASC, handle)
  if (IS("TryActivateAbilitiesByTag")) {                                           // ASC: activate every ability whose tags match; true if any did
    u64_obj *asc = asc_for(self), *list[64]; int n = u64_asc_abilities(asc, list, 64), any = 0;
    for (int i = 0; i < n; i++) {
      u64_v tags = *u64_ivar(list[i], "AbilityTags");
      for (int k = 0; k < u64_array_len(tags); k++) {
        const char *h = nm(u64_array_get(tags, k));
        int match = 0; if (x.t == U64_ARRAY) { for (int j = 0; j < u64_array_len(x); j++) { const char *q = nm(u64_array_get(x, j)); size_t m = strlen(q); if (!strncmp(h, q, m) && (h[m] == 0 || h[m] == '.')) match = 1; } }
        if (match) { any |= u64_ability_try_activate(list[i]); break; }
      }
    }
    return u64_bool(any);
  }
  if (IS("K2_GetRootComponent")) return *u64_ivar(self, "RootComponent");
  if (IS("K2_GetComponentLocation")) {                                             // component world location = owner + R(yaw) * RelativeLocation
    u64_v *o = u64_ivar(self, "Owner"); u64_obj *owner = o->t == U64_OBJ ? o->u.o : self;
    u64_v base = *u64_ivar(owner, "Location"), rot = *u64_ivar(owner, "Rotation"), rel = *u64_ivar(self, "RelativeLocation");
    if (base.t != U64_VEC) base = u64_vec(0, 0, 0);
    if (rel.t != U64_VEC) rel = u64_vec(0, 0, 0);
    float yaw = (rot.t == U64_VEC ? rot.u.v[1] : 0) * 0.017453292f, c = cosf(yaw), sn = sinf(yaw);
    return u64_vec(base.u.v[0] + rel.u.v[0] * c - rel.u.v[1] * sn, base.u.v[1] + rel.u.v[0] * sn + rel.u.v[1] * c, base.u.v[2] + rel.u.v[2]);
  }
  // Presentation only: audio, UI widgets, HUD reticles, camera shakes and gameplay-cue visuals have no counterpart on the target
  // (no audio / widget system is ported). Accepted and ignored, not modelled.
  static const char *const presentation[] = {"PlaySound2D", "PlaySoundAtLocation", "SpawnSoundAttached", "ClientPlayCameraShake", "AddChildToCanvas",
    "RemoveFromParent", "SetActiveWidgetIndex", "SetAlignment", "SetAnchors", "SetAutoSize", "SetBrushFromAtlasInterface", "SetBrushTintColor",
    "SetColorAndOpacity", "SetFont", "SetPosition", "SetVectorParameter", "ResetHUDReticle", "SetHUDReticle", "AddGameplayCueLocal",
    "ExecuteGameplayCueLocal", "RemoveGameplayCueLocal", "K2_ExecuteGameplayCueWithParams", 0};
  for (int i = 0; presentation[i]; i++) if (IS(presentation[i])) return NONE_;
  if (IS("GetTagName")) return u64_name(nm(x));
  if (IS("EqualEqual_GameplayAttributeGameplayAttribute")) return u64_bool(!strcmp(nm(x), nm(y)));
  if (IS("K2_AddGameplayCue")) { if (u64_log) u64_log("  [cue] %s", nm(x)); return NONE_; }
  if (IS("SendGameplayEventToActor")) {
    if (u64_log) u64_log("  [event] %s -> %s", nm(y), x.t == U64_OBJ && x.u.o ? u64_obj_name(x.u.o) : "?");
    if (x.t == U64_OBJ && x.u.o) u64_send_gameplay_event(x.u.o, nm(y), z);
    return u64_int(1);
  }
  if (IS("K2_CommitAbility")) return u64_bool(u64_ability_commit(self, 1, 1));           // cooldown + cost
  if (IS("K2_CommitAbilityCost")) return u64_bool(u64_ability_commit(self, 1, 0));
  if (IS("K2_EndAbility")) { u64_ability_end(self, 0); return NONE_; }
  if (IS("K2_CancelAbility")) { u64_ability_end(self, 1); return NONE_; }
  // ---------------- ability tasks ----------------
  if (IS("WaitDelay")) { u64_obj *t = new_task("AbilityTask_WaitDelay", x.u.o); *u64_ivar(t, "Time") = y; return u64_objv(t); }
  if (IS("PlayMontageAndWaitForEvent")) {                          // (ability, taskName, montage, eventTags, rate, ...)
    u64_obj *t = new_task("AbilityTask_PlayMontageAndWaitForEvent", x.u.o);
    *u64_ivar(t, "Montage") = A(argc, a, 2); *u64_ivar(t, "EventTags") = A(argc, a, 3); *u64_ivar(t, "Rate") = A(argc, a, 4);
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

void u64_gc_roots_extra(void);
void u64_gc_roots_gas(void) { for (int i = 0; i < ntasks; i++) u64_gc_mark_obj(tasks[i]); u64_gc_roots_extra(); }
