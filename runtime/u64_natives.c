// Engine natives that Blueprints call by name, plus the world loop (timers, spawning, BeginPlay).
#include "u64_rt.h"
#include <math.h>
#include <stdio.h>

void u64_mc_broadcast(u64_v *m, int argc, const u64_v *argv);
void u64_obj_destroy(u64_obj *o);

#define MAX_TIMERS 32
typedef struct { int live; u64_v dg; float t, period; } timer_t_;
static timer_t_ timers[MAX_TIMERS];
static float now_;
static int spawn_serial;
static char names[64][24];
static int nnames;

float u64_world_time(void) { return now_; }

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
    timers[i] = (timer_t_){1, arg(argc, a, 0), F(arg(argc, a, 1)), F(arg(argc, a, 1))};
    if (!u64_true(arg(argc, a, 2))) timers[i].period = 0;
    return u64_int(i + 1);
  }
  return u64_int(0);
}

static u64_obj *spawn(u64_v cls, u64_v xform)
{
  if (cls.t != U64_CLASS) return 0;
  char *nm = names[nnames++ & 63];
  snprintf(nm, 24, "%.14s_%d", cls.u.s, ++spawn_serial);
  u64_obj *o = u64_new(nm, cls.u.s);
  *u64_ivar(o, "Location") = xform.t == U64_VEC ? xform : u64_vec(0, 0, 0);
  if (u64_log) u64_log("  [spawn] %s at (%d, %d, %d) t=%d.%d", nm, (int)xform.u.v[0], (int)xform.u.v[1], (int)xform.u.v[2],
                       (int)now_, (int)(now_ * 10) % 10);
  return o;
}

u64_v u64_native(u64_obj *self, const char *fn, int argc, const u64_v *a)
{
  u64_v x = arg(argc, a, 0), y = arg(argc, a, 1);
#define IS(n) (!strcmp(fn, n))
#define HIT note(fn)
  // --- world / actors
  if (IS("HasAuthority")) { HIT; return u64_bool(1); }
  if (IS("IsValid")) { HIT; return u64_bool(x.t == U64_OBJ && u64_obj_alive(x.u.o)); }
  if (IS("IsAlive")) { HIT; return u64_bool(1); }
  if (IS("GetTransform") || IS("K2_GetActorLocation")) { HIT; return loc_of(self); }
  if (IS("K2_SetActorLocation") || IS("K2_SetRelativeLocation")) { HIT; *u64_ivar(self, "Location") = x; return u64_bool(1); }
  if (IS("GetOwner") || IS("GetInstigator") || IS("GetOwningActorFromActorInfo")) { HIT; return u64_objv(self); }
  if (IS("BeginDeferredActorSpawnFromClass")) { HIT; return u64_objv(spawn(y, arg(argc, a, 2))); }
  if (IS("FinishSpawningActor")) { HIT; if (x.t == U64_OBJ && x.u.o) u64_call(x.u.o, "ReceiveBeginPlay", 0, 0); return x; }
  if (IS("K2_DestroyActor")) { HIT; u64_kill(self); return u64_int(0); }
  if (IS("SetLifeSpan")) { HIT; return u64_int(0); }
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
  if (IS("MakeTransform")) { HIT; return x; }
  if (IS("Add_VectorVector")) { HIT; return u64_vec(x.u.v[0] + y.u.v[0], x.u.v[1] + y.u.v[1], x.u.v[2] + y.u.v[2]); }
  if (IS("Multiply_VectorFloat")) { HIT; return u64_vec(x.u.v[0] * F(y), x.u.v[1] * F(y), x.u.v[2] * F(y)); }
  if (IS("VSize")) { HIT; return u64_float(sqrtf(x.u.v[0] * x.u.v[0] + x.u.v[1] * x.u.v[1] + x.u.v[2] * x.u.v[2])); }
  // --- arrays (arrays are not modelled yet: length 0)
  if (IS("Array_Length")) { HIT; return u64_int(0); }
  if (IS("Array_Get")) { HIT; return u64_objv(0); }
  return (u64_v){U64_NONE};
}

void u64_world_begin_play(void)
{
  int n = u64_obj_count();
  for (int i = 0; i < n; i++) {
    u64_obj *o = u64_obj_at(i);
    if (u64_obj_alive(o)) u64_call(o, "ReceiveBeginPlay", 0, 0);
  }
}

void u64_world_tick(float dt)
{
  now_ += dt;
  for (int i = 0; i < MAX_TIMERS; i++) {
    if (!timers[i].live) continue;
    timers[i].t -= dt;
    if (timers[i].t > 0) continue;
    u64_v d = timers[i].dg;
    if (timers[i].period > 0) timers[i].t += timers[i].period; else timers[i].live = 0;
    if (d.t == U64_DELEGATE && u64_obj_alive(d.dobj)) u64_call(d.dobj, d.u.s, 0, 0);
  }
}

void u64_kill(u64_obj *o)
{
  if (!u64_obj_alive(o)) return;
  u64_v self = u64_objv(o);
  u64_mc_broadcast(u64_ivar(o, "OnCharacterDied"), 1, &self);
  u64_obj_destroy(o);
  if (u64_log) u64_log("  [kill] %s", u64_obj_name(o));
}
