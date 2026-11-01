#pragma once
#undef CURRENT_FILE_ID
#define CURRENT_FILE_ID GSAT_ServerWaitForClientTargetData
class UGSAT_ServerWaitForClientTargetData;
#define GSAT_ServerWaitForClientTargetData_16_GENERATED_BODY public:  typedef UAbilityTask Super;  UGSAT_ServerWaitForClientTargetData(const FObjectInitializer &i = FObjectInitializer()); typedef UGSAT_ServerWaitForClientTargetData ThisClass; static UClass *StaticClass() { static UClass c = {"UGSAT_ServerWaitForClientTargetData", &ShimMake<UGSAT_ServerWaitForClientTargetData>}; return &c; } bool InstanceOf(const UClass *c) const override { return c == StaticClass() || Super::InstanceOf(c); } 
