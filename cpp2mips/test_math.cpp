// Prints the shim's FRandomStream / VRandCone values for fixed inputs (checked against an independent Python implementation by tests/test_random_stream.py).
#include "GameFramework/Actors.h"
#include <stdio.h>
#include <initializer_list>
int main()
{
  for (int seed : {0, 1, 12345, -7}) { FRandomStream r(seed); float a = r.GetFraction(), b = r.GetFraction(), c = r.GetFraction(); printf("frac %d %.9g %.9g %.9g\n", seed, a, b, c); }
  struct { float x, y, z, h; int seed; } cases[] = {{1, 0, 0, 0.1f, 12345}, {0.6f, 0.3f, 0.7f, 0.2f, 99}, {0, 0, 1, 0.05f, -3}, {-0.2f, 0.9f, -0.4f, 0.35f, 2024}, {1, 0, 0, 0.0f, 5}};
  for (auto &c : cases) { FRandomStream r(c.seed); FVector v = r.VRandCone(FVector(c.x, c.y, c.z), c.h, c.h); printf("cone %d %.9g %.9g %.9g\n", c.seed, v.X, v.Y, v.Z); }
  FVector a = FVector(1, 2, 3).RotateAngleAxis(40.f, FVector(0, 0, 1)); printf("rot %.9g %.9g %.9g\n", a.X, a.Y, a.Z);
  FRotator q = FVector(0.5f, -0.3f, 0.8f).Rotation(); printf("rotation %.9g %.9g %.9g\n", q.Pitch, q.Yaw, q.Roll);
  return 0;
}
