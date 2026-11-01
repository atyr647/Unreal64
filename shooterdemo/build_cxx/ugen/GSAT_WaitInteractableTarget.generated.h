#pragma once
#undef CURRENT_FILE_ID
#define CURRENT_FILE_ID GSAT_WaitInteractableTarget
class UGSAT_WaitInteractableTarget;
#define GSAT_WaitInteractableTarget_20_GENERATED_BODY public: typedef UAbilityTask Super;  UGSAT_WaitInteractableTarget(const FObjectInitializer &i = FObjectInitializer()); typedef UGSAT_WaitInteractableTarget ThisClass; static UClass *StaticClass() { static UClass c = {"UGSAT_WaitInteractableTarget", &ShimMake<UGSAT_WaitInteractableTarget>}; return &c; } bool InstanceOf(const UClass *c) const override { return c == StaticClass() || Super::InstanceOf(c); } 
