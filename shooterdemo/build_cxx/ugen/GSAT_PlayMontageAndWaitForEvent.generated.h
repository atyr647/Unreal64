#pragma once
#undef CURRENT_FILE_ID
#define CURRENT_FILE_ID GSAT_PlayMontageAndWaitForEvent
class UGSAT_PlayMontageAndWaitForEvent;
#define GSAT_PlayMontageAndWaitForEvent_23_GENERATED_BODY public: typedef UAbilityTask Super;  typedef UGSAT_PlayMontageAndWaitForEvent ThisClass; static UClass *StaticClass() { static UClass c = {"UGSAT_PlayMontageAndWaitForEvent", &ShimMake<UGSAT_PlayMontageAndWaitForEvent>}; return &c; } bool InstanceOf(const UClass *c) const override { return c == StaticClass() || Super::InstanceOf(c); } 
