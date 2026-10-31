// Exact-value checks for u64_math.c (KismetMathLibrary / KismetArrayLibrary natives). Expected values are computed by hand.
#include <stdio.h>
#include <math.h>
#include "u64_gas_core.h"
static int fails;
#define CHECK(c) do { if (c) printf("ok   %s\n", #c); else { printf("FAIL %s\n", #c); fails++; } } while (0)
static u64_v call(const char *fn, int n, u64_v a, u64_v b, u64_v c) { u64_v args[3] = {a, b, c}, out = {U64_NONE}; if (!u64_native_math(0, fn, n, args, &out)) printf("unhandled %s\n", fn); return out; }
int main(void)
{
  CHECK(fabsf(call("Abs", 1, u64_float(-2.5f), u64_int(0), u64_int(0)).u.f - 2.5f) < 1e-6);
  CHECK(call("FTrunc", 1, u64_float(-3.9f), u64_int(0), u64_int(0)).u.i == -3);
  CHECK(call("Min", 2, u64_int(4), u64_int(-2), u64_int(0)).u.i == -2);
  CHECK(fabsf(call("Lerp", 3, u64_float(10), u64_float(20), u64_float(0.25f)).u.f - 12.5f) < 1e-5);
  CHECK(fabsf(call("DegSin", 1, u64_float(90), u64_int(0), u64_int(0)).u.f - 1.0f) < 1e-6);
  CHECK(fabsf(call("DegCos", 1, u64_float(60), u64_int(0), u64_int(0)).u.f - 0.5f) < 1e-6);
  CHECK(call("Vector_DistanceSquared", 2, u64_vec(1, 2, 3), u64_vec(4, 6, 3), u64_int(0)).u.f == 25.0f);
  u64_v r = call("FindLookAtRotation", 2, u64_vec(0, 0, 0), u64_vec(0, 100, 100), u64_int(0));   // yaw 90, pitch 45
  CHECK(fabsf(r.u.v[0] - 45) < 1e-3 && fabsf(r.u.v[1] - 90) < 1e-3 && r.u.v[2] == 0);
  CHECK(call("GreaterEqual_IntInt", 2, u64_int(3), u64_int(3), u64_int(0)).u.i == 1);
  CHECK(call("Greater_IntInt", 2, u64_int(3), u64_int(3), u64_int(0)).u.i == 0);
  u64_v arr = u64_array_of(0, 0);
  u64_v ref = u64_ref(&arr);
  CHECK(call("Array_AddUnique", 2, ref, u64_name("A"), u64_int(0)).u.i == 0);
  CHECK(call("Array_AddUnique", 2, ref, u64_name("B"), u64_int(0)).u.i == 1);
  CHECK(call("Array_AddUnique", 2, ref, u64_name("A"), u64_int(0)).u.i == 0);       // already present: returns its index, no growth
  CHECK(call("Array_Length", 1, ref, u64_int(0), u64_int(0)).u.i == 2);
  CHECK(call("Array_Contains", 2, ref, u64_name("B"), u64_int(0)).u.i == 1);
  CHECK(call("Array_Remove", 2, ref, u64_int(0), u64_int(0)).u.i == 1);
  CHECK(call("Array_Length", 1, ref, u64_int(0), u64_int(0)).u.i == 1 && !strcmp(call("Array_Get", 2, ref, u64_int(0), u64_int(0)).u.s, "B"));
  CHECK(call("Array_Find", 2, ref, u64_name("Z"), u64_int(0)).u.i == -1);
  call("Array_Clear", 1, ref, u64_int(0), u64_int(0));
  CHECK(u64_array_len(arr) == 0);
  for (int i = 0; i < 1000; i++) { int k = call("RandomInteger", 1, u64_int(6), u64_int(0), u64_int(0)).u.i; if (k < 0 || k > 5) { CHECK(0); break; } }
  printf(fails ? "math: %d FAILED\n" : "math: ALL PASSED\n", fails);
  return fails != 0;
}
