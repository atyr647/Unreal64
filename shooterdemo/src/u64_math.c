// Pure-function engine natives: KismetMathLibrary scalar/vector/rotator math and KismetArrayLibrary. No game state.
// Array functions receive the array variable by reference (the compiler passes u64_ref for Array_* targets).
#include "u64_gas_core.h"
#include <math.h>

#define IS(n) (!strcmp(fn, n))
#define RAD2DEG 57.29577951308232f
#define DEG2RAD 0.017453292519943295f
static u64_v A(int argc, const u64_v *a, int i) { return i < argc ? a[i] : (u64_v){U64_NONE}; }
static float F(u64_v v) { return v.t == U64_INT || v.t == U64_BOOL ? (float)v.u.i : v.t == U64_FLOAT ? v.u.f : 0.0f; }
static u64_v deref(u64_v v) { return v.t == U64_REF && v.u.r ? *v.u.r : v; }
static u64_v vec_of(u64_v v) { return v.t == U64_VEC ? v : u64_vec(0, 0, 0); }
static const char *nm(u64_v v) { return v.t == U64_OBJ && v.u.o ? u64_obj_name(v.u.o) : (v.t == U64_CLASS || v.t == U64_NAME || v.t == U64_STR) && v.u.s ? v.u.s : ""; }

// FRotator of a direction (FRotationMatrix::MakeFromX(...).Rotator): pitch above the horizon, yaw about Z, no roll.
static u64_v rotator_of(u64_v d)
{
  d = vec_of(d);
  return u64_vec(atan2f(d.u.v[2], sqrtf(d.u.v[0] * d.u.v[0] + d.u.v[1] * d.u.v[1])) * RAD2DEG, atan2f(d.u.v[1], d.u.v[0]) * RAD2DEG, 0);
}
// Equality of two array items the way the engine compares TArray elements of the types Blueprints use.
static int same(u64_v a, u64_v b)
{
  if (a.t == U64_OBJ || b.t == U64_OBJ) return a.t == b.t && a.u.o == b.u.o;
  if (a.t == U64_NAME || a.t == U64_STR || a.t == U64_CLASS) return (b.t == U64_NAME || b.t == U64_STR || b.t == U64_CLASS) && !strcmp(nm(a), nm(b));
  if (a.t == U64_VEC && b.t == U64_VEC) return a.u.v[0] == b.u.v[0] && a.u.v[1] == b.u.v[1] && a.u.v[2] == b.u.v[2];
  if (a.t == U64_FLOAT || b.t == U64_FLOAT) return F(a) == F(b);
  return a.u.i == b.u.i;
}
static int find(u64_v arr, u64_v item) { for (int i = 0; i < u64_array_len(arr); i++) if (same(u64_array_get(arr, i), item)) return i; return -1; }

// Seeded generator: the converted game must be reproducible for testing (Unreal's is not); one stream for all calls.
static uint32_t rng = 0x2545F491u;
static float rand01(void) { rng = rng * 1664525u + 1013904223u; return (float)(rng >> 8) / 16777216.0f; }

int u64_native_math(u64_obj *self, const char *fn, int argc, const u64_v *a, u64_v *out)
{
  (void)self;
  u64_v x = A(argc, a, 0), y = A(argc, a, 1), z = A(argc, a, 2);
  // --- scalars
  if (IS("Abs")) { *out = u64_float(fabsf(F(x))); return 1; }
  if (IS("Square")) { *out = u64_float(F(x) * F(x)); return 1; }
  if (IS("FTrunc")) { *out = u64_int((int)F(x)); return 1; }
  if (IS("Min")) { *out = u64_int(x.u.i < y.u.i ? x.u.i : y.u.i); return 1; }            // KismetMathLibrary::Min is the int version
  if (IS("Lerp")) { *out = u64_float(F(x) + (F(y) - F(x)) * F(z)); return 1; }
  if (IS("DegSin")) { *out = u64_float(sinf(F(x) * DEG2RAD)); return 1; }
  if (IS("DegCos")) { *out = u64_float(cosf(F(x) * DEG2RAD)); return 1; }
  if (IS("GreaterEqual_FloatFloat")) { *out = u64_bool(F(x) >= F(y)); return 1; }
  if (IS("GreaterEqual_IntInt")) { *out = u64_bool(x.u.i >= y.u.i); return 1; }
  if (IS("Greater_IntInt")) { *out = u64_bool(x.u.i > y.u.i); return 1; }
  if (IS("LessEqual_IntInt")) { *out = u64_bool(x.u.i <= y.u.i); return 1; }
  if (IS("Multiply_IntInt")) { *out = u64_int(x.u.i * y.u.i); return 1; }
  if (IS("Subtract_IntInt")) { *out = u64_int(x.u.i - y.u.i); return 1; }
  if (IS("RandomFloatInRange")) { *out = u64_float(F(x) + (F(y) - F(x)) * rand01()); return 1; }
  if (IS("RandomInteger")) { int m = x.u.i; *out = u64_int(m > 0 ? (int)(rand01() * (float)m) % m : 0); return 1; }   // [0, Max)
  if (IS("EqualEqual_GameplayTag")) { *out = u64_bool(!strcmp(nm(x), nm(y))); return 1; }
  // --- vectors / rotators
  if (IS("Add_VectorFloat")) { u64_v v = vec_of(x); *out = u64_vec(v.u.v[0] + F(y), v.u.v[1] + F(y), v.u.v[2] + F(y)); return 1; }
  if (IS("Vector_DistanceSquared")) {
    u64_v p = vec_of(x), q = vec_of(y); float dx = p.u.v[0] - q.u.v[0], dy = p.u.v[1] - q.u.v[1], dz = p.u.v[2] - q.u.v[2];
    *out = u64_float(dx * dx + dy * dy + dz * dz); return 1;
  }
  if (IS("Conv_VectorToRotator")) { *out = rotator_of(x); return 1; }
  if (IS("FindLookAtRotation")) { u64_v s = vec_of(x), t = vec_of(y); *out = rotator_of(u64_vec(t.u.v[0] - s.u.v[0], t.u.v[1] - s.u.v[1], t.u.v[2] - s.u.v[2])); return 1; }
  // --- arrays (x is the array variable, by reference)
  if (IS("Array_Length")) { *out = u64_int(u64_array_len(deref(x))); return 1; }
  if (IS("Array_Get")) { u64_v it = u64_array_get(deref(x), y.u.i); if (z.t == U64_REF) u64_out(z, it); *out = it; return 1; }
  if (IS("Array_Last")) { u64_v arr = deref(x); *out = u64_array_get(arr, u64_array_len(arr) - 1); return 1; }
  if (IS("Array_Add")) { if (x.t == U64_REF && x.u.r) { u64_array_push(x.u.r, y); *out = u64_int(u64_array_len(*x.u.r) - 1); } return 1; }
  if (IS("Array_AddUnique")) {
    if (x.t != U64_REF || !x.u.r) return 1;
    int i = find(*x.u.r, y);
    if (i < 0) { u64_array_push(x.u.r, y); i = u64_array_len(*x.u.r) - 1; }
    *out = u64_int(i); return 1;
  }
  if (IS("Array_Contains")) { *out = u64_bool(find(deref(x), y) >= 0); return 1; }
  if (IS("Array_Find")) { *out = u64_int(find(deref(x), y)); return 1; }
  if (IS("Array_Clear")) { if (x.t == U64_REF && x.u.r) *x.u.r = u64_array_of(0, 0); return 1; }
  if (IS("Array_Remove") || IS("Array_RemoveItem")) {                       // Remove(index) / RemoveItem(item): shift later items down
    if (x.t != U64_REF || !x.u.r) return 1;
    u64_v arr = *x.u.r; int n = u64_array_len(arr), at = IS("Array_Remove") ? y.u.i : find(arr, y);
    if (at < 0 || at >= n) { *out = u64_bool(0); return 1; }
    u64_v keep[2048]; int k = 0;
    if (n > 2048) return 1;
    for (int i = 0; i < n; i++) if (i != at) keep[k++] = u64_array_get(arr, i);
    *x.u.r = u64_array_of(k, keep); *out = u64_bool(1); return 1;
  }
  return 0;
}
