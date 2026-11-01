#pragma once
#undef CURRENT_FILE_ID
#define CURRENT_FILE_ID GSProjectile
class AGSProjectile;
#define GSProjectile_12_GENERATED_BODY public: typedef AActor Super;  typedef AGSProjectile ThisClass; static UClass *StaticClass() { static UClass c = {"AGSProjectile", &ShimMake<AGSProjectile>}; return &c; } bool InstanceOf(const UClass *c) const override { return c == StaticClass() || Super::InstanceOf(c); } 
