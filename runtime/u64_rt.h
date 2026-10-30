// Unreal64 Blueprint runtime interface. Generated code (tools/ue64/codegen.py) targets exactly this header.
// Values are dynamically typed for now: correctness first, typed specialisation is a later compiler pass.
#pragma once
#include <stdint.h>
#include <string.h>

typedef struct u64_obj u64_obj;
typedef enum { U64_NONE, U64_BOOL, U64_INT, U64_FLOAT, U64_STR, U64_NAME, U64_OBJ, U64_VEC } u64_type;
typedef struct u64_v {
  u64_type t;
  union { int32_t i; float f; const char *s; u64_obj *o; float v[3]; } u;
} u64_v;

static inline u64_v u64_bool(int b)        { u64_v r = {U64_BOOL};  r.u.i = !!b; return r; }
static inline u64_v u64_int(int32_t i)     { u64_v r = {U64_INT};   r.u.i = i;   return r; }
static inline u64_v u64_float(float f)     { u64_v r = {U64_FLOAT}; r.u.f = f;   return r; }
static inline u64_v u64_str(const char *s) { u64_v r = {U64_STR};   r.u.s = s;   return r; }
static inline u64_v u64_name(const char *s){ u64_v r = {U64_NAME};  r.u.s = s;   return r; }
static inline u64_v u64_objv(u64_obj *o)   { u64_v r = {U64_OBJ};   r.u.o = o;   return r; }
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
