#pragma once
#undef CURRENT_FILE_ID
#define CURRENT_FILE_ID GSWeapon
class AGSWeapon;
#define GSWeapon_27_GENERATED_BODY public:  typedef AActor Super;  typedef AGSWeapon ThisClass; static UClass *StaticClass() { static UClass c = {"AGSWeapon", &ShimMake<AGSWeapon>}; return &c; } bool InstanceOf(const UClass *c) const override { return c == StaticClass() || Super::InstanceOf(c); } 
