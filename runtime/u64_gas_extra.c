// Gameplay Ability System, second layer (engine side, project independent): target data handles, effect contexts and hit results
// on specs, effect application to target data, RegisterGameplayTagEvent listeners and the ability tasks that need per-frame
// updates or input / event delivery (WaitChangeFOV, MoveSceneComponentRelativeLocation, RepeatAction, WaitDelayOneFrame,
// WaitInputPressWithTags, WaitGameplayEvent, WaitGameplayTagAdded/Removed, AsyncTaskGameplayTagAddedRemoved, PlayMontageForMesh...,
// WaitTargetDataUsingActor with a reusable trace target actor).
//
// Data model: a target data handle is an array of target data objects ("TargetDataHit": HitResult, "TargetDataActors": Actors);
// a spec is an object ("GameplayEffectSpec": GEClass, Level, Source, Context, set-by-caller values); a context is an object
// ("GameplayEffectContext": HitResult, Instigator, EffectCauser, SourceObject, TargetData). Multiplayer prediction, replicated
// target data and reticle actors are not modelled (a single local authority).
#include "u64_gas_core.h"
#include <math.h>
#include <stdio.h>

#define IS(n) (!strcmp(fn, n))
static u64_v A(int argc, const u64_v *a, int i) { return i < argc ? a[i] : (u64_v){U64_NONE}; }
static const u64_v NONE_ = {U64_NONE};
static float F(u64_v v) { return v.t == U64_INT || v.t == U64_BOOL ? (float)v.u.i : v.t == U64_FLOAT ? v.u.f : 0.0f; }
static u64_obj *O(u64_v v) { return v.t == U64_OBJ ? v.u.o : 0; }
static u64_v deref(u64_v v) { return v.t == U64_REF && v.u.r ? *v.u.r : v; }
static const char *nm(u64_v v)
{
  if (v.t == U64_CLASS || v.t == U64_NAME || v.t == U64_STR) return v.u.s ? v.u.s : "";
  if (v.t == U64_OBJ && v.u.o) return u64_obj_name(v.u.o);
  return "";
}
static const char *strip_c(const char *s) { static char b[4][64]; static int i; char *o = b[i++ & 3]; snprintf(o, 64, "%s", s); size_t n = strlen(o); if (n > 2 && !strcmp(o + n - 2, "_C")) o[n - 2] = 0; return o; }
static int tag_matches(const char *have, const char *query) { size_t q = strlen(query); return !strncmp(have, query, q) && (have[q] == 0 || have[q] == '.'); }
static u64_obj *avatar_of(u64_obj *ab) { u64_v *av = u64_ivar(ab, "Avatar"); return av->t == U64_OBJ ? av->u.o : 0; }
static u64_obj *asc_for(u64_obj *o) { return o && (u64_is_asc(o) || !strcmp(u64_obj_class(o), "AbilitySystemComponent")) ? o : u64_asc_of(o); }

// ---------------------------------------------------------------------------------------------------- target data
static u64_v td_hit(u64_obj *hit) { u64_obj *d = u64_new("TargetDataHit", "TargetDataHit"); *u64_ivar(d, "HitResult") = u64_objv(hit); return u64_objv(d); }
static u64_v td_actors(u64_v actors) { u64_obj *d = u64_new("TargetDataActors", "TargetDataActors"); *u64_ivar(d, "Actors") = actors; return u64_objv(d); }
static int is_td(u64_v v, const char *cls) { return v.t == U64_OBJ && v.u.o && !strcmp(u64_obj_class(v.u.o), cls); }
u64_v u64_td_handle_from_hits(u64_v hits)                          // MakeGameplayAbilityTargetDataHandleFromHitResults / MakeTargetData
{
  u64_v h = u64_array_of(0, 0);
  for (int i = 0; i < u64_array_len(hits); i++) u64_array_push(&h, td_hit(O(u64_array_get(hits, i))));
  return h;
}
// The actors a target data object applies to: the hit actor, or every actor of an actor array.
static int td_actor_list(u64_v data, u64_obj **out, int max)
{
  int n = 0;
  if (is_td(data, "TargetDataHit")) { u64_obj *h = O(*u64_ivar(data.u.o, "HitResult")); u64_obj *act = h ? O(*u64_ivar(h, "HitActor")) : 0; if (act) out[n++] = act; }
  else if (is_td(data, "TargetDataActors")) { u64_v l = *u64_ivar(data.u.o, "Actors"); for (int i = 0; i < u64_array_len(l) && n < max; i++) if (O(u64_array_get(l, i))) out[n++] = O(u64_array_get(l, i)); }
  return n;
}
// FGameplayAbilityTargetData::ApplyGameplayEffectSpec: the spec is copied with this data (its hit) added to the context, then applied
// to every target actor's ability system.
static u64_v td_apply_spec(u64_v data, u64_obj *spec, u64_obj *fallback_source)
{
  u64_v handles = u64_array_of(0, 0);
  u64_obj *targets[32]; int n = td_actor_list(data, targets, 32);
  for (int i = 0; i < n; i++) {
    u64_obj *tasc = asc_for(targets[i]);
    if (!tasc) continue;
    u64_obj *copy = u64_obj_clone(spec);
    u64_v ctxv = *u64_ivar(spec, "Context");
    u64_obj *ctx = O(ctxv) ? u64_obj_clone(O(ctxv)) : u64_new("EffectContext", "GameplayEffectContext");
    *u64_ivar(copy, "Context") = u64_objv(ctx);
    if (is_td(data, "TargetDataHit")) *u64_ivar(ctx, "HitResult") = *u64_ivar(data.u.o, "HitResult");       // AddTargetDataToContext: the hit
    u64_obj *src = O(*u64_ivar(spec, "Source")); if (!src) src = fallback_source;
    int h = u64_apply_ge(tasc, src, nm(*u64_ivar(spec, "GEClass")), F(*u64_ivar(spec, "Level")), copy);
    u64_array_push(&handles, u64_int(h));
  }
  return handles;
}
static u64_v apply_spec_to_handle(u64_v spec, u64_v handle, u64_obj *fallback_source)
{
  u64_v all = u64_array_of(0, 0);
  u64_obj *so = O(spec);
  if (!so) return all;
  for (int i = 0; i < u64_array_len(handle); i++) {
    u64_v hs = td_apply_spec(u64_array_get(handle, i), so, fallback_source);
    for (int k = 0; k < u64_array_len(hs); k++) u64_array_push(&all, u64_array_get(hs, k));
  }
  return all;
}

// ---------------------------------------------------------------------------------------------------- tag events
// RegisterGameplayTagEvent(Tag, NewOrRemoved): count of the tag crossing zero. Listeners are tasks; each remembers the last count.
static void tag_listener_check(u64_obj *t, u64_obj *asc)
{
  u64_v tags = *u64_ivar(t, "ListenTags");
  for (int i = 0; i < u64_array_len(tags); i++) {
    const char *tag = nm(u64_array_get(tags, i)); char key[80]; snprintf(key, sizeof key, "last:%s", tag);
    int now = u64_tag_count(asc, tag), was = u64_ivar(t, u64_intern(key))->u.i;
    if ((now > 0) == (was > 0)) { *u64_ivar(t, u64_intern(key)) = u64_int(now); continue; }
    *u64_ivar(t, u64_intern(key)) = u64_int(now);
    const char *cls = u64_obj_class(t);
    if (!strcmp(cls, "AsyncTaskGameplayTagAddedRemoved")) { u64_v a = u64_name(tag); u64_mc_broadcast(u64_ivar(t, now > 0 ? "OnTagAdded" : "OnTagRemoved"), 1, &a); }
    else if (!strcmp(cls, "AbilityTask_WaitGameplayTagAdded") && now > 0) {
      u64_mc_broadcast(u64_ivar(t, "Added"), 0, 0);
      if (u64_true(*u64_ivar(t, "OnlyTriggerOnce"))) u64_task_untrack(t);
    } else if (!strcmp(cls, "AbilityTask_WaitGameplayTagRemoved") && now <= 0) {
      u64_mc_broadcast(u64_ivar(t, "Removed"), 0, 0);
      if (u64_true(*u64_ivar(t, "OnlyTriggerOnce"))) u64_task_untrack(t);
    }
  }
}
void u64_notify_tag_change(u64_obj *asc)
{
  u64_obj *snap[96]; int n = u64_tasks_snapshot(snap, 96);
  for (int i = 0; i < n; i++) {
    u64_v tags = *u64_ivar(snap[i], "ListenTags");
    if (tags.t != U64_ARRAY || O(*u64_ivar(snap[i], "ListenASC")) != asc) continue;
    tag_listener_check(snap[i], asc);
  }
}

// ---------------------------------------------------------------------------------------------------- timelines
// UTimelineComponent for the Timeline nodes of Blueprints: float tracks of curve keys; each evaluation writes the track variables on
// the owner, sets the direction variable and calls the generated <Name>__UpdateFunc; reaching the end calls <Name>__FinishedFunc.
float u64_curve_eval(const u64_curve_key *k, int n, float t)                  // FRichCurve::Eval
{
  if (n <= 0) return 0;
  if (t <= k[0].time) return k[0].value;
  if (t >= k[n - 1].time) return k[n - 1].value;
  int i = 0; while (i + 1 < n && t >= k[i + 1].time) i++;
  float dt = k[i + 1].time - k[i].time, a = dt > 0 ? (t - k[i].time) / dt : 0;
  if (k[i].interp == 1) return k[i].value;                                    // constant
  if (k[i].interp == 2) {                                                     // cubic (Hermite with the keys' tangents)
    float a2 = a * a, a3 = a2 * a, t0 = k[i].leave * dt, t1 = k[i + 1].arrive * dt;
    return (2 * a3 - 3 * a2 + 1) * k[i].value + (a3 - 2 * a2 + a) * t0 + (a3 - a2) * t1 + (-2 * a3 + 3 * a2) * k[i + 1].value;
  }
  return k[i].value + (k[i + 1].value - k[i].value) * a;
}
static u64_obj *tls[32]; static int ntls;
void u64_gc_roots_extra(void) { for (int i = 0; i < ntls; i++) u64_gc_mark_obj(tls[i]); }
static const u64_timeline_def *tl_def(u64_obj *comp)
{
  u64_obj *owner = O(*u64_ivar(comp, "Owner"));
  const char *full = u64_obj_name(comp), *var = strrchr(full, '.'); var = var ? var + 1 : full;
  for (const u64_timeline_def *d = u64_timelines; owner && d->cls; d++) if (!strcmp(d->var, var) && u64_class_is(u64_obj_class(owner), d->cls)) return d;
  return 0;
}
static void tl_apply(u64_obj *comp, const u64_timeline_def *d, int fire_update)
{
  u64_obj *owner = O(*u64_ivar(comp, "Owner"));
  float pos = F(*u64_ivar(comp, "tl_pos"));
  for (int i = 0; i < d->ntracks; i++) *u64_ivar(owner, d->tracks[i].prop) = u64_float(u64_curve_eval(d->tracks[i].keys, d->tracks[i].nkeys, pos));
  *u64_ivar(owner, d->direction) = u64_int(u64_true(*u64_ivar(comp, "tl_rev")) ? 1 : 0);        // ETimelineDirection
  if (fire_update) u64_call(owner, d->update, 0, 0);
}
static void tl_track(u64_obj *comp) { for (int i = 0; i < ntls; i++) if (tls[i] == comp) return; if (ntls < 32) tls[ntls++] = comp; }
static void tl_tick(float dt)
{
  for (int i = 0; i < ntls; ) {
    u64_obj *c = tls[i]; const u64_timeline_def *d = u64_obj_alive(c) ? tl_def(c) : 0;
    if (!d || !u64_true(*u64_ivar(c, "tl_playing"))) { tls[i] = tls[--ntls]; continue; }
    int rev = u64_true(*u64_ivar(c, "tl_rev")), finished = 0;
    float pos = F(*u64_ivar(c, "tl_pos")) + (rev ? -dt : dt);
    if (rev && pos <= 0) { if (d->loop) pos += d->length; else { pos = 0; finished = 1; } }
    else if (!rev && pos >= d->length) { if (d->loop) pos -= d->length; else { pos = d->length; finished = 1; } }
    *u64_ivar(c, "tl_pos") = u64_float(pos);
    if (finished) *u64_ivar(c, "tl_playing") = u64_bool(0);
    tl_apply(c, d, 1);
    if (finished) u64_call(O(*u64_ivar(c, "Owner")), d->finished, 0, 0);
    i++;
  }
}

// ---------------------------------------------------------------------------------------------------- tasks
// Tasks keep their own timing in the world clock: StartTime / Duration / Period are seconds of u64_world_time().
static void finish(u64_obj *t) { u64_task_untrack(t); }
static u64_v curve_value(u64_v curve, float x)                       // UCurveFloat::GetFloatValue: unsupported curves fall back to identity
{
  if (curve.t != U64_OBJ && curve.t != U64_CLASS) return u64_float(x);
  return u64_float(x);
}
static void set_rel_loc(u64_obj *comp, u64_v v) { if (comp) *u64_ivar(comp, "RelativeLocation") = v; }
void u64_tasks_tick_extra(float dt)
{
  tl_tick(dt);
  u64_obj *snap[96]; int n = u64_tasks_snapshot(snap, 96);
  float now = u64_world_time();
  for (int i = 0; i < n; i++) {
    u64_obj *t = snap[i]; const char *c = u64_obj_class(t);
    if (!strcmp(c, "GSAT_WaitChangeFOV")) {                                   // UGSAT_WaitChangeFOV::TickTask
      u64_obj *cam = O(*u64_ivar(t, "CameraComponent"));
      if (!cam) { finish(t); continue; }
      float start = F(*u64_ivar(t, "StartTime")), dur = F(*u64_ivar(t, "Duration")), fov0 = F(*u64_ivar(t, "StartFOV")), fov1 = F(*u64_ivar(t, "TargetFOV"));
      if (now >= start + dur) {
        *u64_ivar(cam, "FieldOfView") = u64_float(fov1);
        u64_mc_broadcast(u64_ivar(t, "OnTargetFOVReached"), 0, 0);
        finish(t);
      } else {
        float frac = F(curve_value(*u64_ivar(t, "LerpCurve"), (now - start) / dur));
        *u64_ivar(cam, "FieldOfView") = u64_float(fov0 + (fov1 - fov0) * frac);
      }
    } else if (!strcmp(c, "GSAT_MoveSceneCompRelLocation")) {                // UGSAT_MoveSceneCompRelLocation::TickTask
      u64_obj *comp = O(*u64_ivar(t, "Component"));
      if (!comp) { finish(t); continue; }
      float start = F(*u64_ivar(t, "StartTime")), dur = F(*u64_ivar(t, "Duration"));
      u64_v from = *u64_ivar(t, "StartLocation"), to = *u64_ivar(t, "TargetLocation");
      if (from.t != U64_VEC) from = u64_vec(0, 0, 0);
      if (to.t != U64_VEC) to = u64_vec(0, 0, 0);
      if (now >= start + dur) { set_rel_loc(comp, to); u64_mc_broadcast(u64_ivar(t, "OnFinishMove"), 0, 0); finish(t); }
      else {
        float f = F(curve_value(*u64_ivar(t, "LerpCurve"), (now - start) / dur));
        set_rel_loc(comp, u64_vec(from.u.v[0] + (to.u.v[0] - from.u.v[0]) * f, from.u.v[1] + (to.u.v[1] - from.u.v[1]) * f, from.u.v[2] + (to.u.v[2] - from.u.v[2]) * f));
      }
    } else if (!strcmp(c, "AbilityTask_Repeat")) {                            // UAbilityTask_Repeat: first action at once, then every TimeBetweenActions
      if (now + 1e-5f >= F(*u64_ivar(t, "NextTime"))) {
        int count = u64_ivar(t, "ActionCounter")->u.i, total = u64_ivar(t, "ActionsToPerform")->u.i;
        u64_v arg = u64_int(count);
        *u64_ivar(t, "ActionCounter") = u64_int(count + 1);
        *u64_ivar(t, "NextTime") = u64_float(F(*u64_ivar(t, "NextTime")) + F(*u64_ivar(t, "TimeBetweenActions")));
        u64_mc_broadcast(u64_ivar(t, "OnPerformAction"), 1, &arg);
        if (count + 1 >= total) { u64_v fin = u64_int(count + 1); u64_mc_broadcast(u64_ivar(t, "OnFinished"), 1, &fin); finish(t); }
      }
    } else if (!strcmp(c, "GSAT_WaitDelayOneFrame")) {                        // SetTimerForNextTick
      if (u64_ivar(t, "Armed")->t != U64_NONE) { u64_mc_broadcast(u64_ivar(t, "OnFinish"), 0, 0); finish(t); }
      else *u64_ivar(t, "Armed") = u64_bool(1);
    }
  }
}

static void start_listen(u64_obj *t, u64_obj *asc, u64_v tags)           // remember the starting counts, then watch for changes
{
  *u64_ivar(t, "ListenASC") = u64_objv(asc); *u64_ivar(t, "ListenTags") = tags;
  for (int i = 0; i < u64_array_len(tags); i++) { char key[80]; snprintf(key, sizeof key, "last:%s", nm(u64_array_get(tags, i))); *u64_ivar(t, u64_intern(key)) = u64_int(u64_tag_count(asc, nm(u64_array_get(tags, i)))); }
}

// Task activation for the classes of this file. Returns 1 when handled.
int u64_task_activate_extra(u64_obj *t)
{
  const char *c = u64_obj_class(t);
  if (!strcmp(c, "AbilityTask_WaitGameplayTagAdded") || !strcmp(c, "AbilityTask_WaitGameplayTagRemoved")) {
    u64_obj *asc = O(*u64_ivar(t, "ListenASC"));
    int add = !strcmp(c, "AbilityTask_WaitGameplayTagAdded"), has = asc && u64_tag_count(asc, nm(*u64_ivar(t, "Tag"))) > 0;
    if ((add && has) || (!add && asc && !has)) {                              // the condition already holds: fire at activation
      u64_mc_broadcast(u64_ivar(t, add ? "Added" : "Removed"), 0, 0);
      if (u64_true(*u64_ivar(t, "OnlyTriggerOnce"))) u64_task_untrack(t);
    }
    return 1;
  }
  if (!strcmp(c, "GSAT_WaitChangeFOV") || !strcmp(c, "GSAT_MoveSceneCompRelLocation") || !strcmp(c, "AsyncTaskGameplayTagAddedRemoved")) return 1;
  if (!strcmp(c, "AbilityTask_Repeat")) { *u64_ivar(t, "NextTime") = u64_float(u64_world_time()); return 1; }
  if (!strcmp(c, "GSAT_WaitDelayOneFrame")) return 1;
  if (!strcmp(c, "GSAT_WaitInputPressWithTags")) {                           // bTestAlreadyPressed: an input already down counts
    u64_obj *ab = O(*u64_ivar(t, "Ability"));
    *u64_ivar(t, "StartTime") = u64_float(u64_world_time());
    if (u64_true(*u64_ivar(t, "bTestInitialState")) && ab && u64_true(*u64_ivar(ab, "InputPressed"))) u64_ability_input_pressed(ab);
    return 1;
  }
  if (!strcmp(c, "AbilityTask_WaitGameplayEvent")) return 1;
  if (!strcmp(c, "GSAT_WaitTargetDataUsingActor")) {                         // InitializeTargetActor / FinalizeTargetActor
    u64_obj *ta = O(*u64_ivar(t, "TargetActor")), *ab = O(*u64_ivar(t, "Ability"));
    if (!ta || !ab) { finish(t); return 1; }
    *u64_ivar(ta, "OwningAbility") = u64_objv(ab); *u64_ivar(ta, "SourceActor") = u64_objv(avatar_of(ab));
    *u64_ivar(ta, "WaitingTask") = u64_objv(t);                               // TargetDataReadyDelegate / CanceledDelegate -> this task
    if (u64_as_int(*u64_ivar(t, "ConfirmationType")) == 0) u64_call(ta, "ConfirmTargetingAndContinue", 0, 0);   // Instant: ConfirmTargeting
    return 1;
  }
  return 0;
}

// Delivery of a trace target actor's result to the task waiting on it (OnTargetDataReadyCallback).
void u64_target_data_ready(u64_obj *target_actor, u64_v handle)
{
  u64_obj *t = O(*u64_ivar(target_actor, "WaitingTask"));
  if (!t) return;
  u64_mc_broadcast(u64_ivar(t, "ValidData"), 1, &handle);
  if (u64_as_int(*u64_ivar(t, "ConfirmationType")) != 3) { *u64_ivar(target_actor, "WaitingTask") = u64_objv(0); finish(t); }   // CustomMulti keeps going
}

void u64_ability_input_pressed(u64_obj *ab)                                  // GSAT_WaitInputPressWithTags::OnPressCallback
{
  u64_obj *asc = O(*u64_ivar(ab, "ASC"));
  u64_obj *snap[96]; int n = u64_tasks_snapshot(snap, 96);
  for (int i = 0; i < n; i++) {
    u64_obj *t = snap[i];
    if (strcmp(u64_obj_class(t), "GSAT_WaitInputPressWithTags") || O(*u64_ivar(t, "Ability")) != ab) continue;
    u64_v req = *u64_ivar(t, "RequiredTags"), ign = *u64_ivar(t, "IgnoredTags");
    int blocked = 0;
    for (int k = 0; k < u64_array_len(ign); k++) if (u64_tag_count(asc, nm(u64_array_get(ign, k)))) blocked = 1;
    for (int k = 0; k < u64_array_len(req); k++) if (!u64_tag_count(asc, nm(u64_array_get(req, k)))) blocked = 1;
    if (u64_tag_count(asc, "State.Interacting") > u64_tag_count(asc, "State.InteractingRemoval")) blocked = 1;
    if (blocked) continue;                                                    // Reset(): keep waiting
    u64_v elapsed = u64_float(u64_world_time() - F(*u64_ivar(t, "StartTime")));
    u64_mc_broadcast(u64_ivar(t, "OnPress"), 1, &elapsed);
    finish(t);
  }
}

void u64_deliver_gameplay_event_extra(u64_obj *asc, const char *tag, int argc, const u64_v *argv)    // AbilityTask_WaitGameplayEvent
{
  u64_obj *snap[96]; int n = u64_tasks_snapshot(snap, 96);
  for (int i = 0; i < n; i++) {
    u64_obj *t = snap[i];
    if (strcmp(u64_obj_class(t), "AbilityTask_WaitGameplayEvent")) continue;
    u64_obj *target = O(*u64_ivar(t, "OptionalExternalTarget")), *tasc = target ? asc_for(target) : asc_for(avatar_of(O(*u64_ivar(t, "Ability"))));
    if (tasc != asc) continue;
    const char *want = nm(*u64_ivar(t, "Tag"));
    int match = u64_true(*u64_ivar(t, "OnlyMatchExact")) ? !strcmp(tag, want) : tag_matches(tag, want);
    if (!match) continue;
    u64_mc_broadcast(u64_ivar(t, "EventReceived"), argc, argv);
    if (u64_true(*u64_ivar(t, "OnlyTriggerOnce"))) finish(t);
  }
}

// ---------------------------------------------------------------------------------------------------- natives
int u64_native_gas_extra(u64_obj *self, const char *fn, int argc, const u64_v *a, u64_v *out)
{
  u64_v x = A(argc, a, 0), y = A(argc, a, 1), z = A(argc, a, 2);
  *out = NONE_;
  // --- target data handles (AbilitySystemBlueprintLibrary / the project's GSBlueprintFunctionLibrary)
  if (IS("GetDataCountFromTargetData")) { *out = u64_int(u64_array_len(deref(x))); return 1; }
  if (IS("TargetDataHasHitResult")) { u64_v d = u64_array_get(deref(x), y.u.i); *out = u64_bool(is_td(d, "TargetDataHit") && O(*u64_ivar(d.u.o, "HitResult"))); return 1; }
  if (IS("GetHitResultFromTargetData")) { u64_v d = u64_array_get(deref(x), y.u.i); *out = is_td(d, "TargetDataHit") ? *u64_ivar(d.u.o, "HitResult") : u64_objv(0); return 1; }
  if (IS("ClearTargetData")) { if (x.t == U64_REF && x.u.r) *x.u.r = u64_array_of(0, 0); return 1; }
  if (IS("MakeGameplayTagContainerFromArray")) { *out = deref(x); return 1; }
  if (IS("AddTargetsToEffectContainerSpec")) {                              // FGSGameplayEffectContainerSpec::AddTargets(Specs' TargetData, HitResults, TargetActors)
    u64_obj *spec = O(deref(x));
    if (!spec) return 1;
    u64_v *td = u64_ivar(spec, "TargetData");
    if (td->t != U64_ARRAY) *td = u64_array_of(0, 0);
    u64_v handles = deref(y), hits = deref(z), actors = deref(A(argc, a, 3));
    for (int i = 0; i < u64_array_len(handles); i++) { u64_v h = u64_array_get(handles, i); for (int k = 0; k < u64_array_len(h); k++) u64_array_push(td, u64_array_get(h, k)); }
    for (int i = 0; i < u64_array_len(hits); i++) u64_array_push(td, td_hit(O(u64_array_get(hits, i))));
    if (u64_array_len(actors) > 0) u64_array_push(td, td_actors(actors));
    return 1;
  }
  if (IS("ApplyExternalEffectContainerSpec")) {                            // apply every effect spec to the container's own target data
    u64_obj *cs = O(deref(x)); u64_v all = u64_array_of(0, 0);
    if (cs) { u64_v specs = *u64_ivar(cs, "TargetGameplayEffectSpecs"), td = *u64_ivar(cs, "TargetData");
      for (int i = 0; i < u64_array_len(specs); i++) { u64_v h = apply_spec_to_handle(u64_array_get(specs, i), td, 0); for (int k = 0; k < u64_array_len(h); k++) u64_array_push(&all, u64_array_get(h, k)); } }
    *out = all; return 1;
  }
  if (IS("K2_ApplyGameplayEffectSpecToTarget")) { *out = apply_spec_to_handle(x, deref(y), asc_for(avatar_of(self))); return 1; }   // UGameplayAbility: (SpecHandle, TargetData)
  if (IS("ApplyEffectContainerSpec")) {                                    // UGSGameplayAbility::ApplyEffectContainerSpec
    u64_obj *cs = O(deref(x)); u64_v all = u64_array_of(0, 0);
    if (cs) { u64_v specs = *u64_ivar(cs, "TargetGameplayEffectSpecs"), td = *u64_ivar(cs, "TargetData");
      for (int i = 0; i < u64_array_len(specs); i++) { u64_v h = apply_spec_to_handle(u64_array_get(specs, i), td, asc_for(avatar_of(self))); for (int k = 0; k < u64_array_len(h); k++) u64_array_push(&all, u64_array_get(h, k)); } }
    *out = all; return 1;
  }
  if (IS("BP_ApplyGameplayEffectToTarget") && u64_class_is(u64_obj_class(self), "AbilitySystemComponent")) {   // UAbilitySystemComponent: (GEClass, Target ASC, level, context)
    u64_obj *target = O(y) ? asc_for(O(y)) : 0;
    if (target) { *out = u64_int(u64_apply_ge(target, self, u64_intern(strip_c(nm(x))), z.t == U64_FLOAT ? z.u.f : 1.0f, 0)); return 1; }
    *out = u64_int(0); return 1;
  }
  if (IS("BP_ApplyGameplayEffectToTarget")) {                              // UGameplayAbility: (TargetData, GEClass, level, stacks)
    u64_obj *spec = u64_new("EffectSpec", "GameplayEffectSpec");
    *u64_ivar(spec, "GEClass") = u64_name(u64_intern(strip_c(nm(y)))); *u64_ivar(spec, "Level") = z; *u64_ivar(spec, "Source") = u64_objv(asc_for(avatar_of(self)));
    *out = apply_spec_to_handle(u64_objv(spec), deref(x), asc_for(avatar_of(self))); return 1;
  }
  // --- effect contexts
  if (IS("GetEffectContext")) {                                            // the spec's context, created on first use
    u64_obj *spec = O(x);
    if (!spec) { *out = u64_objv(u64_new("EffectContext", "GameplayEffectContext")); return 1; }
    u64_v *c = u64_ivar(spec, "Context");
    if (!O(*c)) { u64_obj *ctx = u64_new("EffectContext", "GameplayEffectContext"); *u64_ivar(ctx, "Instigator") = u64_objv(avatar_of(self)); *u64_ivar(ctx, "EffectCauser") = u64_objv(avatar_of(self)); *c = u64_objv(ctx); }
    *out = *c; return 1;
  }
  if (IS("MakeEffectContext")) {                                           // ASC::MakeEffectContext: instigator = owner, causer = avatar
    u64_obj *ctx = u64_new("EffectContext", "GameplayEffectContext"); u64_obj *asc = asc_for(self), *own = asc ? u64_asc_owner(asc) : 0;
    *u64_ivar(ctx, "Instigator") = u64_objv(own); *u64_ivar(ctx, "EffectCauser") = u64_objv(own);
    *out = u64_objv(ctx); return 1;
  }
  if (IS("EffectContextAddHitResult")) { u64_obj *ctx = O(x); if (ctx) *u64_ivar(ctx, "HitResult") = y; return 1; }     // (ctx, hit, bReset)
  if (IS("EffectContextGetHitResult")) { u64_obj *ctx = O(x); *out = ctx ? *u64_ivar(ctx, "HitResult") : u64_objv(0); return 1; }
  if (IS("EffectContextGetTargetData")) { u64_obj *ctx = O(x); u64_v d = ctx ? *u64_ivar(ctx, "TargetData") : NONE_; *out = d.t == U64_ARRAY ? d : u64_array_of(0, 0); return 1; }
  if (IS("EffectContextAddTargetData")) {                                  // FGSGameplayEffectContext::AddTargetData
    u64_obj *ctx = O(x); if (!ctx) return 1;
    u64_v *td = u64_ivar(ctx, "TargetData"); if (td->t != U64_ARRAY) *td = u64_array_of(0, 0);
    u64_v h = deref(y); for (int i = 0; i < u64_array_len(h); i++) u64_array_push(td, u64_array_get(h, i));
    return 1;
  }
  if (IS("EffectContextGetSourceObject")) { u64_obj *ctx = O(x); *out = ctx ? *u64_ivar(ctx, "SourceObject") : u64_objv(0); return 1; }
  if (IS("EffectContextIsInstigatorLocallyControlled")) { u64_obj *ctx = O(x); u64_obj *i = ctx ? O(*u64_ivar(ctx, "Instigator")) : 0; *out = u64_bool(i && i == u64_player_pawn()); return 1; }
  if (IS("MakeGameplayCueParameters")) {                                   // the struct of its (named) inputs; only the ones the BPs pass are kept
    u64_obj *p = u64_new("CueParameters", "GameplayCueParameters");
    static const char *const names[] = {"NormalizedMagnitude", "RawMagnitude", "EffectContext", "MatchedTagName", "OriginalTag", "AggregatedSourceTags", "AggregatedTargetTags",
                                        "Location", "Normal", "Instigator", "EffectCauser", "SourceObject", "TargetAttachComponent", "PhysicalMaterial", "GameplayEffectLevel", "AbilityLevel", "TargetAttachComponent2"};
    for (int i = 0; i < argc && i < 17; i++) *u64_ivar(p, names[i]) = a[i];
    *out = u64_objv(p); return 1;
  }
  // --- ability system component / controller helpers
  if (IS("FindAbilitySpecHandleForClass")) {                               // exact class, optionally the given source object
    u64_obj *list[64]; int n = u64_asc_abilities(asc_for(self), list, 64);
    for (int i = 0; i < n; i++) {
      if (strcmp(u64_obj_class(list[i]), strip_c(nm(x)))) continue;
      if (!O(y) || O(*u64_ivar(list[i], "SourceObject")) == O(y)) { *out = u64_objv(list[i]); return 1; }
    }
    *out = u64_objv(0); return 1;
  }
  if (IS("BatchRPCTryActivateAbility")) {                                  // UGSAbilitySystemComponent::BatchRPCTryActivateAbility(Handle, EndAbilityImmediately)
    u64_obj *ab = O(x);
    if (!ab || !u64_obj_alive(ab)) { *out = u64_bool(0); return 1; }
    int activated = u64_ability_try_activate(ab);
    if (u64_true(y) && u64_true(*u64_ivar(ab, "Active"))) u64_ability_end(ab, 0);                // ExternalEndAbility
    *out = u64_bool(activated); return 1;
  }
  if (IS("GetController")) { *out = *u64_ivar(O(x) ? O(x) : self, "Controller"); return 1; }
  if (IS("K2_GetPawn")) { *out = *u64_ivar(self, "Pawn"); return 1; }
  if (IS("GetPlayerController")) { u64_obj *p = u64_player_pawn(); *out = p ? *u64_ivar(p, "Controller") : u64_objv(0); return 1; }
  // --- ability-level helpers
  if (IS("K2_CheckAbilityCost")) { *out = u64_bool(u64_ability_check_cost(self)); return 1; }
  if (IS("IsInputPressed")) { *out = u64_bool(u64_true(*u64_ivar(self, "InputPressed"))); return 1; }
  if (IS("GetActiveGameplayEffectStackCount")) { *out = u64_int(u64_effect_handle_stack(x.u.i)); return 1; }
  if (IS("SendGameplayEvent")) { u64_send_gameplay_event(avatar_of(self), nm(x), y); return 1; }
  // --- timelines (self is the timeline component)
  if ((IS("Play") || IS("PlayFromStart") || IS("Reverse") || IS("ReverseFromEnd") || IS("Stop") || IS("SetNewTime") || IS("IsPlaying")) && self && tl_def(self)) {
    const u64_timeline_def *d = tl_def(self);
    if (IS("IsPlaying")) { *out = u64_bool(u64_true(*u64_ivar(self, "tl_playing"))); return 1; }
    if (IS("Stop")) { *u64_ivar(self, "tl_playing") = u64_bool(0); return 1; }
    if (IS("SetNewTime")) { float t = F(x); *u64_ivar(self, "tl_pos") = u64_float(t < 0 ? 0 : t > d->length ? d->length : t); tl_apply(self, d, 1); return 1; }
    int rev = IS("Reverse") || IS("ReverseFromEnd");
    if (IS("PlayFromStart")) *u64_ivar(self, "tl_pos") = u64_float(0);
    if (IS("ReverseFromEnd")) *u64_ivar(self, "tl_pos") = u64_float(d->length);
    *u64_ivar(self, "tl_rev") = u64_bool(rev); *u64_ivar(self, "tl_playing") = u64_bool(1);
    tl_track(self); return 1;
  }
  if (IS("GetPlayLength") && self && tl_def(self)) { *out = u64_float(tl_def(self)->length); return 1; }
  if (IS("GetPlayLength")) { float len = u64_montage_length(nm(x.t == U64_NONE ? NONE_ : x), 0); *out = u64_float(len < 0 ? 0.0f : len); return 1; }    // an animation asset's length
  if (IS("SphereTraceMultiForObjects")) {                                  // (World, Start, End, Radius, ObjectTypes, bComplex, ActorsToIgnore, DrawDebug, &OutHits, bIgnoreSelf, ...) -> bHit
    u64_v st = A(argc, a, 1), en = A(argc, a, 2); float radius = F(A(argc, a, 3));
    u64_v ign = deref(A(argc, a, 6)); u64_obj *hits[16];
    float s3[3] = {st.u.v[0], st.u.v[1], st.u.v[2]}, e3[3] = {en.u.v[0], en.u.v[1], en.u.v[2]};
    int n = u64_trace_multi(s3, e3, radius, u64_array_len(ign) ? O(u64_array_get(ign, 0)) : 0, hits, 16);
    u64_v arr = u64_array_of(0, 0); for (int i = 0; i < n; i++) u64_array_push(&arr, u64_objv(hits[i]));
    u64_out(A(argc, a, 8), arr); *out = u64_bool(n > 0); return 1;
  }
  // --- scene component setters used by the tasks and abilities
  if (IS("Activate") && self && !strcmp(u64_obj_class(self), "Component")) { *u64_ivar(self, "bIsActive") = u64_bool(1); return 1; }        // components: movement / particle systems
  if (IS("Deactivate")) { if (self) *u64_ivar(self, "bIsActive") = u64_bool(0); return 1; }
  if (IS("SetFieldOfView")) { if (self) *u64_ivar(self, "FieldOfView") = x; return 1; }
  if (IS("SetBoolPropertyByName") || IS("SetObjectPropertyByName") || IS("SetStructurePropertyByName") || IS("SetFloatPropertyByName") || IS("SetIntPropertyByName")) {
    if (O(x)) *u64_ivar(O(x), u64_intern(nm(y))) = z;                       // KismetSystemLibrary::Set*PropertyByName: the variable of that name on the object
    return 1;
  }
  if (IS("SetUpdatedComponent")) { if (self) *u64_ivar(self, "UpdatedComponent") = x; return 1; }
  if (IS("StopMovementImmediately")) { if (self) *u64_ivar(self, "Velocity") = u64_vec(0, 0, 0); return 1; }
  // --- ability task factories
  if (IS("RepeatAction")) {                                                // UAbilityTask_Repeat::RepeatAction(Ability, TimeBetweenActions, TotalActionCount)
    u64_obj *t = u64_task_new("AbilityTask_Repeat", O(x));
    *u64_ivar(t, "TimeBetweenActions") = y; *u64_ivar(t, "ActionsToPerform") = z; *u64_ivar(t, "ActionCounter") = u64_int(0);
    *out = u64_objv(t); return 1;
  }
  if (IS("WaitDelayOneFrame")) { *out = u64_objv(u64_task_new("GSAT_WaitDelayOneFrame", O(x))); return 1; }
  if (IS("WaitChangeFOV")) {                                               // (Ability, TaskInstanceName, Camera, TargetFOV, Duration, Curve)
    u64_obj *t = u64_task_new("GSAT_WaitChangeFOV", O(x)), *cam = O(A(argc, a, 2));
    float dur = F(A(argc, a, 4)); if (dur < 0.001f) dur = 0.001f;
    *u64_ivar(t, "CameraComponent") = A(argc, a, 2); *u64_ivar(t, "TargetFOV") = A(argc, a, 3); *u64_ivar(t, "Duration") = u64_float(dur);
    *u64_ivar(t, "StartFOV") = cam && u64_ivar(cam, "FieldOfView")->t == U64_FLOAT ? *u64_ivar(cam, "FieldOfView") : u64_float(90.0f);   // UCameraComponent::FieldOfView defaults to 90
    *u64_ivar(t, "StartTime") = u64_float(u64_world_time()); *u64_ivar(t, "LerpCurve") = A(argc, a, 5);
    *out = u64_objv(t); return 1;
  }
  if (IS("MoveSceneComponentRelativeLocation")) {                          // (Ability, Name, Component, Location, Duration, Curve, VectorCurve)
    u64_obj *t = u64_task_new("GSAT_MoveSceneCompRelLocation", O(x)), *comp = O(A(argc, a, 2));
    float dur = F(A(argc, a, 4)); if (dur < 0.001f) dur = 0.001f;
    *u64_ivar(t, "Component") = A(argc, a, 2); *u64_ivar(t, "TargetLocation") = A(argc, a, 3); *u64_ivar(t, "Duration") = u64_float(dur);
    *u64_ivar(t, "StartLocation") = comp ? *u64_ivar(comp, "RelativeLocation") : u64_vec(0, 0, 0);
    *u64_ivar(t, "StartTime") = u64_float(u64_world_time()); *u64_ivar(t, "LerpCurve") = A(argc, a, 5);
    *out = u64_objv(t); return 1;
  }
  if (IS("WaitInputPressWithTags")) {                                      // (Ability, RequiredTags, IgnoredTags, bTestAlreadyPressed)
    u64_obj *t = u64_task_new("GSAT_WaitInputPressWithTags", O(x));
    *u64_ivar(t, "RequiredTags") = y; *u64_ivar(t, "IgnoredTags") = z; *u64_ivar(t, "bTestInitialState") = A(argc, a, 3);
    *out = u64_objv(t); return 1;
  }
  if (IS("WaitGameplayEvent")) {                                           // (Ability, Tag, OptionalExternalTarget, OnlyTriggerOnce, OnlyMatchExact)
    u64_obj *t = u64_task_new("AbilityTask_WaitGameplayEvent", O(x));
    *u64_ivar(t, "Tag") = y; *u64_ivar(t, "OptionalExternalTarget") = z; *u64_ivar(t, "OnlyTriggerOnce") = A(argc, a, 3); *u64_ivar(t, "OnlyMatchExact") = A(argc, a, 4);
    *out = u64_objv(t); return 1;
  }
  if (IS("WaitGameplayTagAdd") || IS("WaitGameplayTagRemove")) {           // (Ability, Tag, OptionalExternalTarget, OnlyTriggerOnce)
    int add = IS("WaitGameplayTagAdd");
    u64_obj *t = u64_task_new(add ? "AbilityTask_WaitGameplayTagAdded" : "AbilityTask_WaitGameplayTagRemoved", O(x));
    u64_obj *asc = O(z) ? asc_for(O(z)) : asc_for(avatar_of(O(x)));
    *u64_ivar(t, "Tag") = y; *u64_ivar(t, "OnlyTriggerOnce") = A(argc, a, 3);
    start_listen(t, asc, u64_array_of(1, &y));
    *out = u64_objv(t); return 1;
  }
  if (IS("ListenForGameplayTagAddedOrRemoved")) {                          // UAsyncTaskGameplayTagAddedRemoved (ASC, tags)
    u64_obj *asc = asc_for(O(x)); u64_v tags = deref(y);
    if (!asc || u64_array_len(tags) < 1) { *out = u64_objv(0); return 1; }
    u64_obj *t = u64_task_new("AsyncTaskGameplayTagAddedRemoved", 0);
    start_listen(t, asc, tags); u64_task_track(t);
    *out = u64_objv(t); return 1;
  }
  if (IS("PlayMontageForMeshAndWaitForEvent")) {                           // (Ability, Name, Mesh, Montage, EventTags, Rate, StartSection, bStopWhenAbilityEnds, RootMotionScale)
    u64_v shifted[8] = {x, A(argc, a, 1), A(argc, a, 3), A(argc, a, 4), A(argc, a, 5), A(argc, a, 6), A(argc, a, 7), A(argc, a, 8)};
    u64_v t = u64_call(self, "PlayMontageAndWaitForEvent", 8, shifted);
    if (O(t)) *u64_ivar(O(t), "Mesh") = A(argc, a, 2);
    *out = t; return 1;
  }
  if (IS("WaitTargetDataWithReusableActor")) {                             // (Ability, Name, ConfirmationType, TargetActor, bCreateKeyIfNotValidForMorePredicting)
    u64_obj *t = u64_task_new("GSAT_WaitTargetDataUsingActor", O(x));
    *u64_ivar(t, "ConfirmationType") = z; *u64_ivar(t, "TargetActor") = A(argc, a, 3);
    *out = u64_objv(t); return 1;
  }
  if (IS("ServerWaitForClientTargetData")) {                               // single authority: the server is the client, so the data is local; never completes by itself
    *out = u64_objv(u64_task_new("GSAT_ServerWaitForClientTargetData", O(x))); return 1;
  }
  return 0;
}
