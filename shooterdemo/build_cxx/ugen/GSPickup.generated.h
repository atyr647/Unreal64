#pragma once
#undef CURRENT_FILE_ID
#define CURRENT_FILE_ID GSPickup
class AGSPickup;
#define GSPickup_15_GENERATED_BODY public: typedef AActor Super;  typedef AGSPickup ThisClass; static UClass *StaticClass() { static UClass c = {"AGSPickup", &ShimMake<AGSPickup>}; return &c; } bool InstanceOf(const UClass *c) const override { return c == StaticClass() || Super::InstanceOf(c); } 
