#pragma once
#undef CURRENT_FILE_ID
#define CURRENT_FILE_ID GSAT_WaitChangeFOV
class UGSAT_WaitChangeFOV;
#define GSAT_WaitChangeFOV_17_GENERATED_BODY public:  typedef UAbilityTask Super;  UGSAT_WaitChangeFOV(const FObjectInitializer &i = FObjectInitializer()); typedef UGSAT_WaitChangeFOV ThisClass; static UClass *StaticClass() { static UClass c = {"UGSAT_WaitChangeFOV", &ShimMake<UGSAT_WaitChangeFOV>}; return &c; } bool InstanceOf(const UClass *c) const override { return c == StaticClass() || Super::InstanceOf(c); } 
