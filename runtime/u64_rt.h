// Unreal64 Blueprint runtime interface. Generated code (tools/ue64/codegen.py) targets exactly this header.
// Values are dynamically typed for now: correctness first, typed specialisation is a later compiler pass.
#pragma once
#include <stdint.h>
#include <string.h>

typedef struct u64_obj u64_obj;
typedef enum { U64_NONE, U64_BOOL, U64_INT, U64_FLOAT, U64_STR, U64_NAME, U64_OBJ, U64_VEC, U64_CLASS, U64_DELEGATE, U64_REF, U64_ARRAY } u64_type;
typedef struct u64_v {
  u64_type t;
  union { int32_t i; float f; const char *s; u64_obj *o; float v[3]; struct u64_v *r; } u;
  u64_obj *dobj;  // delegate target object
  int aux;        // delegate: latent resume offset
} u64_v;

static inline u64_v u64_bool(int b)        { u64_v r = {U64_BOOL};  r.u.i = !!b; return r; }
static inline u64_v u64_int(int32_t i)     { u64_v r = {U64_INT};   r.u.i = i;   return r; }
static inline u64_v u64_float(float f)     { u64_v r = {U64_FLOAT}; r.u.f = f;   return r; }
static inline u64_v u64_str(const char *s) { u64_v r = {U64_STR};   r.u.s = s;   return r; }
static inline u64_v u64_name(const char *s){ u64_v r = {U64_NAME};  r.u.s = s;   return r; }
static inline u64_v u64_objv(u64_obj *o)   { u64_v r = {U64_OBJ};   r.u.o = o;   return r; }
static inline u64_v u64_cls(const char *s) { u64_v r = {U64_CLASS}; r.u.s = s; return r; }
static inline u64_v u64_ref(u64_v *p) { u64_v r = {U64_REF}; r.u.r = p; return r; }
/* Writes to an out-parameter: if dst is a reference, store through it. */
static inline void u64_out(u64_v dst, u64_v val) { if (dst.t == U64_REF && dst.u.r) *dst.u.r = val; }
static inline u64_v u64_latent(u64_v obj, const char *fn, int linkage) { u64_v r = {U64_DELEGATE}; r.u.s = fn; r.dobj = obj.u.o; r.aux = linkage; return r; }
static inline u64_v u64_vec(float x, float y, float z) { u64_v r = {U64_VEC}; r.u.v[0] = x; r.u.v[1] = y; r.u.v[2] = z; return r; }
static inline int   u64_true(u64_v v)      { return v.t == U64_FLOAT ? v.u.f != 0 : v.t == U64_OBJ ? v.u.o != 0 : v.u.i != 0; }
static inline int   u64_as_int(u64_v v)    { return v.t == U64_FLOAT ? (int)v.u.f : v.u.i; }

// Object model (implemented by the runtime): instance variables, named-object lookup, and function dispatch.
u64_v *u64_ivar(u64_obj *o, const char *name);
u64_obj *u64_find(const char *name);
// Calls a native or Blueprint function by name. Unimplemented natives are reported once and return U64_NONE.
u64_v u64_call(u64_obj *self, const char *fn, int argc, const u64_v *argv);
#define U64_CALL(self, fn, ...) u64_call((self), (fn), (int)(sizeof((u64_v[]){u64_int(0), ##__VA_ARGS__}) / sizeof(u64_v)) - 1, ((u64_v[]){u64_int(0), ##__VA_ARGS__}) + 1)

// Kismet execution-flow stack (PushExecutionFlow / PopExecutionFlow).
#define U64_FLOW_MAX 32
#define U64_FLOW_DECL void *flow[U64_FLOW_MAX]; int flow_n = 0
#define U64_PUSH(label) (flow[flow_n++] = (label))
#define U64_POP() (flow[--flow_n])

// Persistent per-object frame: Blueprint event graphs ("ubergraph") keep their locals here across latent actions.
u64_v *u64_pframe(u64_obj *o, const char *name);
// Delegates and arrays.
u64_v u64_delegate(u64_v obj, const char *fn);
void  u64_mc_add(u64_v *multicast, u64_v delegate);
void  u64_mc_remove(u64_v *multicast, u64_v delegate);
void  u64_mc_clear(u64_v *multicast);
void  u64_array_set(u64_v *array, int n, const u64_v *elems);
// Struct member access on a dynamic struct value (vectors, transforms, user structs).
u64_v *u64_member(u64_v *s, const char *name);

// Blueprint function registry (generated) and host hooks.
typedef void (*u64_bp_impl)(u64_obj *self, const u64_v *args, int argc);
typedef struct { const char *cls, *fn; u64_bp_impl impl; } u64_bpfn;
typedef u64_v (*u64_native_fn)(u64_obj *self, const char *fn, int argc, const u64_v *argv);
typedef void (*u64_log_fn)(const char *fmt, ...);
extern const u64_bpfn *u64_bp_table; extern int u64_bp_count;
extern u64_native_fn u64_native_hook; extern u64_log_fn u64_log;
u64_obj *u64_new(const char *name, const char *cls);

// World simulation (u64_natives.c): timers, actor spawning, event dispatch.
void u64_apply_defaults(u64_obj *o, const char *cls);   // generated (u64_defaults.c)
void u64_level_load(void);                              // generated (u64_level.c)
void u64_world_begin_play(void);                        // BeginPlay on every actor currently in the world
void u64_world_tick(float dt);                          // advances time, fires timers, Tick events
float u64_world_time(void);
void u64_kill(u64_obj *o);                              // destroys an actor and broadcasts OnCharacterDied
const char *u64_obj_name(u64_obj *o);
const char *u64_obj_class(u64_obj *o);
int u64_obj_count(void);
u64_obj *u64_obj_at(int i);
int u64_obj_alive(u64_obj *o);
int u64_native_hits(const char *fn);                   // times a native ran; -1 if never implemented

// Natives implemented in u64_gas.c (Gameplay Ability System, UI, animation, tasks).
u64_v u64_xform_new(u64_v loc, u64_v rot);   // a transform keeps location + rotation
u64_v u64_array_get(u64_v a, int i);
int u64_native_math(u64_obj *self, const char *fn, int argc, const u64_v *a, u64_v *out);   // 1 if handled
u64_v u64_native_gas(u64_obj *self, const char *fn, int argc, const u64_v *argv);
void u64_timer_broadcast(float t, u64_obj *task, const char *delegate_name, u64_v arg);
void u64_timer_latent(float t, u64_v delegate);
u64_obj *u64_spawn_actor(const char *cls, u64_v location);
void u64_mc_broadcast(u64_v *m, int argc, const u64_v *argv);
void u64_input_release(u64_obj *ability);   // player released the ability's input
const char *u64_intern(const char *s);      // stable copy of a string (variable names built at run time)
void u64_timers_cancel_for(u64_obj *ability);

// Hooks between the GAS core and the ability-task natives (u64_gas.c).
void u64_notify_attribute_tasks(u64_obj *asc, int argc, const u64_v *argv);
void u64_notify_damage_tasks(u64_obj *asc, int argc, const u64_v *argv);
void u64_notify_stack_tasks(u64_obj *asc, const char *effect_name);
void u64_tasks_end_for(u64_obj *ability);
void u64_obj_destroy(u64_obj *o);

// Safe object access: a value that is not an object reference yields NULL (never a reinterpreted pointer).
static inline u64_obj *u64_asobj(u64_v v) { return v.t == U64_OBJ ? v.u.o : 0; }
// Sub-objects (components) of an actor, created on first use and owned by it.
u64_v u64_comp(u64_obj *owner, const char *name);
int u64_obj_is_actor(u64_obj *o);   // placed or spawned actors (not components, tasks or specs)
void u64_timer_call(float t, u64_obj *obj, const char *fn, u64_v arg);   // call a Blueprint function later
void u64_deliver_gameplay_event(u64_obj *asc, const char *tag, int argc, const u64_v *argv);
void u64_set_return(u64_v v);   // a Blueprint function's return value, picked up by u64_call

// Class hierarchy (Blueprint -> C++ parents) and the ports of the project's C++ gameplay classes (u64_cpp_game.c).
const char *u64_parent_of(const char *cls);
int u64_class_is(const char *cls, const char *ancestor);
void u64_cpp_construct(u64_obj *o, const char *cls);                         // C++ constructor defaults of exactly this class
void u64_cpp_register_class(const char *cls, const char *parent, void (*ctor)(u64_obj *));   // a project's C++ class (constructor defaults)
void u64_cpp_register_method(const char *cls, const char *fn, u64_v (*f)(u64_obj *self, int argc, const u64_v *argv));
int u64_cpp_dispatch(u64_obj *self, const char *fn, int argc, const u64_v *argv, u64_v *ret);   // member functions along the chain
float u64_world_dt(void);
void u64_obj_set_alive(u64_obj *o, int v);
extern int u64_rt_overflows;   // > 0 if object or variable storage ran out
extern int u64_trace_calls;   // set to log every call
