#pragma once
#undef CURRENT_FILE_ID
#define CURRENT_FILE_ID GSAT_WaitInputPressWithTags
class UGSAT_WaitInputPressWithTags;
#define GSAT_WaitInputPressWithTags_19_GENERATED_BODY public: typedef UAbilityTask Super;  UGSAT_WaitInputPressWithTags(const FObjectInitializer &i = FObjectInitializer()); typedef UGSAT_WaitInputPressWithTags ThisClass; static UClass *StaticClass() { static UClass c = {"UGSAT_WaitInputPressWithTags", &ShimMake<UGSAT_WaitInputPressWithTags>}; return &c; } bool InstanceOf(const UClass *c) const override { return c == StaticClass() || Super::InstanceOf(c); } 
