#pragma once
#undef CURRENT_FILE_ID
#define CURRENT_FILE_ID GSAT_PlayMontageForMeshAndWaitForEvent
class UGSAT_PlayMontageForMeshAndWaitForEvent;
#define GSAT_PlayMontageForMeshAndWaitForEvent_23_GENERATED_BODY public:  typedef UAbilityTask Super;  typedef UGSAT_PlayMontageForMeshAndWaitForEvent ThisClass; static UClass *StaticClass() { static UClass c = {"UGSAT_PlayMontageForMeshAndWaitForEvent", &ShimMake<UGSAT_PlayMontageForMeshAndWaitForEvent>}; return &c; } bool InstanceOf(const UClass *c) const override { return c == StaticClass() || Super::InstanceOf(c); } 
