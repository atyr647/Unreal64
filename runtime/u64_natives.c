// Engine natives that Blueprints call by name, plus the world loop (timers, spawning, BeginPlay).
#include "u64_gas_core.h"
#include <math.h>
#include <stdio.h>

void u64_mc_broadcast(u64_v *m, int argc, const u64_v *argv);
void u64_obj_destroy(u64_obj *o);

#define MAX_TIMERS 256
typedef struct { int live; u64_v dg; float t, period; u64_obj *bt; const char *bname; u64_v barg; u64_obj *co; const char *cfn; } timer_t_;
static timer_t_ timers[MAX_TIMERS];
static void timers_full(void) { if (!u64_rt_overflows++ && u64_log) u64_log("  [error] timer table full"); }
static float now_;
static int spawn_serial;
static char names[64][24];
static int nnames;

float u64_world_time(void) { return now_; }
static float dt_;
float u64_world_dt(void) { return dt_; }

static float F(u64_v v) { return v.t == U64_INT || v.t == U64_BOOL ? (float)v.u.i : v.u.f; }
static u64_v arg(int argc, const u64_v *a, int i) { return i < argc ? a[i] : (u64_v){U64_NONE}; }
static u64_v loc_of(u64_obj *o) { u64_v *l = u64_ivar(o, "Location"); return l->t == U64_VEC ? *l : u64_vec(0, 0, 0); }

static int hits[256]; static const char *hit_names[256]; static int nhit;
static void note(const char *fn) {
  for (int i = 0; i < nhit; i++) if (hit_names[i] == fn || !strcmp(hit_names[i], fn)) { hits[i]++; return; }
  if (nhit < 256) { hit_names[nhit] = fn; hits[nhit++] = 1; }
}
int u64_native_hits(const char *fn) { for (int i = 0; i < nhit; i++) if (!strcmp(hit_names[i], fn)) return hits[i]; return -1; }

static u64_v set_timer(int argc, const u64_v *a)
{
  for (int i = 0; i < MAX_TIMERS; i++) if (!timers[i].live) {
    timers[i] = (timer_t_){1, arg(argc, a, 0), F(arg(argc, a, 1)), F(arg(argc, a, 1)), 0, 0, {U64_NONE}, 0, 0};
    if (!u64_true(arg(argc, a, 2))) timers[i].period = 0;
    return u64_int(i + 1);
  }
  timers_full();
  return u64_int(0);
}

static u64_obj *spawn(u64_v cls, u64_v xform)
{
  u64_v rot = u64_vec(0, 0, 0);
  if (xform.t == U64_OBJ && xform.u.o) { rot = *u64_ivar(xform.u.o, "Rotation"); xform = *u64_ivar(xform.u.o, "Location"); }   // a transform carries rotation
  if (cls.t != U64_CLASS) return 0;
  char nm[40];
  snprintf(nm, sizeof nm, "%.30s_%d", cls.u.s, ++spawn_serial);
  u64_obj *o = u64_new_copy(nm, cls.u.s);
  *u64_ivar(o, "Location") = xform.t == U64_VEC ? xform : u64_vec(0, 0, 0);
  *u64_ivar(o, "Rotation") = rot;
  if (u64_log) u64_log("  [spawn] %s at (%d, %d, %d) t=%d.%d", nm, (int)xform.u.v[0], (int)xform.u.v[1], (int)xform.u.v[2],
                       (int)now_, (int)(now_ * 10) % 10);
  return o;
}

void u64_timer_broadcast(float t, u64_obj *task, const char *name, u64_v arg)
{
  for (int i = 0; i < MAX_TIMERS; i++) if (!timers[i].live) {
    timers[i] = (timer_t_){1, {U64_NONE}, t, 0, task, name, arg, 0, 0}; return;
  }
  timers_full();
}
void u64_timer_call(float t, u64_obj *obj, const char *fn, u64_v arg)
{
  for (int i = 0; i < MAX_TIMERS; i++) if (!timers[i].live) { timers[i] = (timer_t_){1, {U64_NONE}, t, 0, 0, 0, arg, obj, fn}; return; }
  timers_full();
}
void u64_timer_latent(float t, u64_v dg)
{
  for (int i = 0; i < MAX_TIMERS; i++) if (!timers[i].live) {
    timers[i] = (timer_t_){1, dg, t, 0, 0, 0, {U64_NONE}, 0, 0}; return;
  }
  timers_full();
}
void u64_timers_cancel_for(u64_obj *ability)   // an ending ability cancels its tasks' pending timers
{
  for (int i = 0; i < MAX_TIMERS; i++)
    if (timers[i].live && timers[i].bt) {
      u64_v *ab = u64_ivar(timers[i].bt, "Ability");
      if (ab->t == U64_OBJ && ab->u.o == ability) timers[i].live = 0;
    }
}
u64_obj *u64_spawn_actor(const char *cls, u64_v loc) { return spawn(u64_cls(cls), loc); }

u64_v u64_native(u64_obj *self, const char *fn, int argc, const u64_v *a)
{
  u64_v x = arg(argc, a, 0), y = arg(argc, a, 1);
#define IS(n) (!strcmp(fn, n))
#define HIT note(fn)
  // --- world / actors
  if (IS("HasAuthority")) { HIT; return u64_bool(1); }
  if (IS("IsValid")) { HIT; return u64_bool(x.t == U64_OBJ && u64_obj_alive(x.u.o)); }
  if (IS("K2_GetActorLocation")) { HIT; return loc_of(self); }
  if (IS("GetTransform")) { HIT; u64_v *r = u64_ivar(self, "Rotation"); return u64_xform_new(loc_of(self), r->t == U64_VEC ? *r : u64_vec(0, 0, 0)); }
  if (IS("K2_SetActorLocation") || IS("K2_SetRelativeLocation")) { HIT; *u64_ivar(self, "Location") = x; return u64_bool(1); }
  if (IS("GetInstigator")) {                                                    // AActor::GetInstigator: who caused this actor to exist
    HIT; u64_v *i = u64_ivar(self, "Instigator");
    return i->t == U64_OBJ ? *i : u64_objv(self);
  }
  if (IS("GetOwner") || IS("GetOwningActorFromActorInfo")) {
    HIT;
    if (!strcmp(u64_obj_class(self), "Component")) return *u64_ivar(self, "Owner");     // a component's owner is its actor
    u64_v *o = u64_ivar(self, "Owner");
    return o->t == U64_OBJ ? *o : u64_objv(self);
  }
  if (IS("BeginDeferredActorSpawnFromClass")) { HIT; return u64_objv(spawn(y, arg(argc, a, 2))); }
  if (IS("FinishSpawningActor")) { HIT; if (x.t == U64_OBJ && x.u.o) { u64_actor_init(x.u.o); u64_call(x.u.o, "ReceiveBeginPlay", 0, 0); } return x; }
  if (IS("K2_DestroyActor")) { HIT; u64_obj_destroy(self); return u64_int(0); }
  if (IS("SetLifeSpan")) { HIT; if (F(x) > 0) u64_timer_call(F(x), self, "K2_DestroyActor", (u64_v){U64_NONE}); return u64_int(0); }   // AActor::SetLifeSpan
  // --- timers
  if (IS("K2_SetTimerDelegate")) { HIT; return set_timer(argc, a); }
  if (IS("K2_ClearAndInvalidateTimerHandle")) { HIT; return u64_int(0); }
  // --- math
  if (IS("Add_FloatFloat")) { HIT; return u64_float(F(x) + F(y)); }
  if (IS("Subtract_FloatFloat")) { HIT; return u64_float(F(x) - F(y)); }
  if (IS("Multiply_FloatFloat")) { HIT; return u64_float(F(x) * F(y)); }
  if (IS("Divide_FloatFloat")) { HIT; return u64_float(F(y) != 0 ? F(x) / F(y) : 0); }
  if (IS("FMax")) { HIT; return u64_float(F(x) > F(y) ? F(x) : F(y)); }
  if (IS("Add_IntInt")) { HIT; return u64_int(x.u.i + y.u.i); }
  if (IS("Greater_FloatFloat")) { HIT; return u64_bool(F(x) > F(y)); }
  if (IS("Less_FloatFloat")) { HIT; return u64_bool(F(x) < F(y)); }
  if (IS("LessEqual_FloatFloat")) { HIT; return u64_bool(F(x) <= F(y)); }
  if (IS("Less_IntInt")) { HIT; return u64_bool(x.u.i < y.u.i); }
  if (IS("EqualEqual_IntInt") || IS("EqualEqual_ByteByte")) { HIT; return u64_bool(x.u.i == y.u.i); }
  if (IS("NotEqual_ByteByte")) { HIT; return u64_bool(x.u.i != y.u.i); }
  if (IS("NearlyEqual_FloatFloat")) { HIT; return u64_bool(fabsf(F(x) - F(y)) <= F(arg(argc, a, 2))); }
  if (IS("EqualEqual_ObjectObject")) { HIT; return u64_bool(x.u.o == y.u.o); }
  if (IS("NotEqual_ObjectObject")) { HIT; return u64_bool(x.u.o != y.u.o); }
  if (IS("Not_PreBool")) { HIT; return u64_bool(!u64_true(x)); }
  if (IS("BooleanAND")) { HIT; return u64_bool(u64_true(x) && u64_true(y)); }
  if (IS("BooleanOR")) { HIT; return u64_bool(u64_true(x) || u64_true(y)); }
  if (IS("Conv_IntToFloat")) { HIT; return u64_float((float)x.u.i); }
  // --- vectors
  if (IS("MakeVector")) { HIT; return u64_vec(F(x), F(y), F(arg(argc, a, 2))); }
  if (IS("MakeTransform")) { HIT; return u64_xform_new(x, y); }
  if (IS("Add_VectorVector")) { HIT; return u64_vec(x.u.v[0] + y.u.v[0], x.u.v[1] + y.u.v[1], x.u.v[2] + y.u.v[2]); }
  if (IS("Multiply_VectorFloat")) { HIT; return u64_vec(x.u.v[0] * F(y), x.u.v[1] * F(y), x.u.v[2] * F(y)); }
  if (IS("VSize")) { HIT; return u64_float(sqrtf(x.u.v[0] * x.u.v[0] + x.u.v[1] * x.u.v[1] + x.u.v[2] * x.u.v[2])); }
  { u64_v out = {U64_NONE}; if (u64_native_math(self, fn, argc, a, &out)) { HIT; return out; } }   // u64_math.c: math + arrays
  return u64_native_gas(self, fn, argc, a);
}

void u64_world_begin_play(void)
{
  int n = u64_obj_count();
  for (int i = 0; i < n; i++) {
    u64_obj *o = u64_obj_at(i);
    if (u64_obj_alive(o)) { u64_actor_init(o); u64_call(o, "ReceiveBeginPlay", 0, 0); }
  }
}

static void projectile_tick(float dt)
{
  for (int i = 0; i < u64_obj_count(); i++) {
    u64_obj *o = u64_obj_at(i);
    if (!u64_obj_is_actor(o)) continue;
    for (const u64_projectile_def *p = u64_projectiles; p->cls; p++) {
      if (strcmp(p->cls, u64_obj_class(o)) || p->initial_speed <= 0) continue;
      u64_v *loc = u64_ivar(o, "Location"), *rot = u64_ivar(o, "Rotation");
      float pitch = rot->t == U64_VEC ? rot->u.v[0] * 0.0174533f : 0, yaw = rot->t == U64_VEC ? rot->u.v[1] * 0.0174533f : 0;
      u64_v *vel = u64_ivar(o, "Velocity");
      if (vel->t != U64_VEC) *vel = u64_vec(cosf(pitch) * cosf(yaw) * p->initial_speed, cosf(pitch) * sinf(yaw) * p->initial_speed, sinf(pitch) * p->initial_speed);
      vel->u.v[2] += -980.0f * p->gravity_scale * dt;
      float from[3] = {loc->u.v[0], loc->u.v[1], loc->u.v[2]};
      float to[3] = {from[0] + vel->u.v[0] * dt, from[1] + vel->u.v[1] * dt, from[2] + vel->u.v[2] * dt};
      float radius = 16.0f, t = 1.0f;                                   // the projectile's collision sphere
      for (const u64_shape_def *sd = u64_shapes; sd->cls; sd++) if (!strcmp(sd->cls, u64_obj_class(o)) && sd->kind == 3) radius = sd->a;
      u64_v *inst = u64_ivar(o, "Instigator");
      if (u64_sweep_sphere(from, to, radius, inst->t == U64_OBJ ? inst->u.o : 0, &t)) {       // sweep: stop at first contact so the overlap fires
        t += 0.002f; for (int k = 0; k < 3; k++) to[k] = from[k] + (to[k] - from[k]) * t;
      }
      *loc = u64_vec(to[0], to[1], to[2]);
    }
  }
}

void u64_world_tick(float dt)
{
  now_ += dt; dt_ = dt;
  u64_movement_tick(dt);
  projectile_tick(dt);
  u64_overlap_events_tick();
  u64_gas_tick(dt);
  for (int i = 0; i < MAX_TIMERS; i++) {
    if (!timers[i].live) continue;
    timers[i].t -= dt;
    if (timers[i].t > 0) continue;
    u64_v d = timers[i].dg;
    if (timers[i].period > 0) timers[i].t += timers[i].period; else timers[i].live = 0;
    if (timers[i].co) { u64_v av = timers[i].barg; u64_call(timers[i].co, timers[i].cfn, 1, &av); continue; }
    if (timers[i].bt) { u64_v av = timers[i].barg; u64_mc_broadcast(u64_ivar(timers[i].bt, timers[i].bname), 1, &av); continue; }
    if (d.t == U64_DELEGATE && u64_obj_alive(d.dobj)) {
      if (d.aux) { u64_v entry = u64_int(d.aux); u64_call(d.dobj, d.u.s, 1, &entry); }   // latent: re-enter the event graph
      else u64_call(d.dobj, d.u.s, 0, 0);
    }
  }
  if (u64_gc_stress || u64_gc_pressure() || u64_array_pressure()) u64_gc_collect();   // safe point: no Blueprint code is running
}

void u64_gc_roots_natives(void)
{
  for (int i = 0; i < MAX_TIMERS; i++) {
    if (!timers[i].live) continue;
    u64_gc_mark_v(timers[i].dg); u64_gc_mark_v(timers[i].barg); u64_gc_mark_obj(timers[i].bt); u64_gc_mark_obj(timers[i].co);
  }
}
