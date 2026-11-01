#pragma once
// The math the targeting code uses: rotators, rotation axes, FRandomStream (the engine's seed mutation and cone sampling). Formulas follow the engine's definitions
// (FRotator::Vector, FVector::Rotation, FVector::RotateAngleAxis, FRandomStream::GetFraction / VRandCone). Not verified against an engine build: the engine is not available here.
#include "ue_shim.h"
#define PI 3.1415926535897932f
struct FRotator { float Pitch = 0, Yaw = 0, Roll = 0; FRotator() {} FRotator(float p, float y, float r) : Pitch(p), Yaw(y), Roll(r) {}
  FVector Vector() const { float rp = Pitch * (PI / 180.f), ry = Yaw * (PI / 180.f); float CP = cosf(rp), SP = sinf(rp), CY = cosf(ry), SY = sinf(ry); return FVector(CP * CY, CP * SY, SP); }
  FVector UnitAxisY() const { float rp = Pitch * (PI / 180.f), ry = Yaw * (PI / 180.f), rr = Roll * (PI / 180.f); float SP = sinf(rp), CY = cosf(ry), SY = sinf(ry), SR = sinf(rr), CR = cosf(rr);
    return FVector(SR * SP * CY - CR * SY, SR * SP * SY + CR * CY, -SR * cosf(rp)); } };
struct FQuat { FRotator R; FRotator Rotator() const { return R; } static const FQuat Identity; };                  // the rotation is kept as the rotator it was made from
struct FTransform { FVector Location; FQuat Rotation; FVector GetLocation() const { return Location; } FQuat GetRotation() const { return Rotation; } };
inline FVector operator*(float f, const FVector &v) { return v * f; }
inline FVector FVectorRotateAngleAxis(const FVector &v, float deg, const FVector &a)
{
  float r = deg * (PI / 180.f), S = sinf(r), C = cosf(r), XX = a.X * a.X, YY = a.Y * a.Y, ZZ = a.Z * a.Z, XY = a.X * a.Y, YZ = a.Y * a.Z, ZX = a.Z * a.X, XS = a.X * S, YS = a.Y * S, ZS = a.Z * S, OMC = 1.f - C;
  return FVector((OMC * XX + C) * v.X + (OMC * XY - ZS) * v.Y + (OMC * ZX + YS) * v.Z, (OMC * XY + ZS) * v.X + (OMC * YY + C) * v.Y + (OMC * YZ - XS) * v.Z, (OMC * ZX - YS) * v.X + (OMC * YZ + XS) * v.Y + (OMC * ZZ + C) * v.Z);
}
inline FRotator FVectorRotation(const FVector &v) { FRotator r; r.Yaw = atan2f(v.Y, v.X) * (180.f / PI); r.Pitch = atan2f(v.Z, sqrtf(v.X * v.X + v.Y * v.Y)) * (180.f / PI); r.Roll = 0; return r; }
struct FRandomStream {
  int32 InitialSeed, Seed; FRandomStream(int32 s = 0) : InitialSeed(s), Seed(s) {}
  void MutateSeed() const { const_cast<FRandomStream *>(this)->Seed = (int32)((uint32)Seed * 196314165u + 907633515u); }
  float GetFraction() const { MutateSeed(); union { float f; int32 i; } r, t; t.f = 1.0f; r.i = (t.i & 0xff800000) | (Seed & 0x007fffff); return r.f - truncf(r.f); }
  FVector VRandCone(const FVector &Dir, float HorizontalHalf, float VerticalHalf) const
  {
    if (VerticalHalf > 0.f && HorizontalHalf > 0.f) {
      float RandU = GetFraction(), RandV = GetFraction(), Theta = 2.f * PI * RandU, Phi = acosf(2.f * RandV - 1.f);
      float c = cosf(Theta) / VerticalHalf, s = sinf(Theta) / HorizontalHalf, Half = sqrtf(1.f / (c * c + s * s));      // the ellipse's radius at angle Theta
      Phi = fmodf(Phi, Half);
      FRotator dr = FVectorRotation(Dir); FVector DirZ = dr.Vector(), DirY = dr.UnitAxisY();                         // FRotationMatrix(Dir.Rotation()): X and Y axes
      FVector Result = FVectorRotateAngleAxis(Dir, Phi * 180.f / PI, DirY); Result = FVectorRotateAngleAxis(Result, Theta * 180.f / PI, DirZ);
      return Result.GetSafeNormal();
    }
    return Dir.GetSafeNormal();
  }
};
inline const FQuat FQuat::Identity = FQuat();
