#pragma once
#undef CURRENT_FILE_ID
#define CURRENT_FILE_ID GSAT_WaitTargetDataUsingActor
class UGSAT_WaitTargetDataUsingActor;
#define GSAT_WaitTargetDataUsingActor_22_GENERATED_BODY public:  typedef UAbilityTask Super;  UGSAT_WaitTargetDataUsingActor(const FObjectInitializer &i = FObjectInitializer()); typedef UGSAT_WaitTargetDataUsingActor ThisClass; static UClass *StaticClass() { static UClass c = {"UGSAT_WaitTargetDataUsingActor", &ShimMake<UGSAT_WaitTargetDataUsingActor>}; return &c; } bool InstanceOf(const UClass *c) const override { return c == StaticClass() || Super::InstanceOf(c); } 
