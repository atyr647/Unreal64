#pragma once
#undef CURRENT_FILE_ID
#define CURRENT_FILE_ID GSAT_WaitDelayOneFrame
class UGSAT_WaitDelayOneFrame;
#define GSAT_WaitDelayOneFrame_17_GENERATED_BODY public: typedef UAbilityTask Super;  UGSAT_WaitDelayOneFrame(const FObjectInitializer &i = FObjectInitializer()); typedef UGSAT_WaitDelayOneFrame ThisClass; static UClass *StaticClass() { static UClass c = {"UGSAT_WaitDelayOneFrame", &ShimMake<UGSAT_WaitDelayOneFrame>}; return &c; } bool InstanceOf(const UClass *c) const override { return c == StaticClass() || Super::InstanceOf(c); } 
