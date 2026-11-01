#pragma once
#undef CURRENT_FILE_ID
#define CURRENT_FILE_ID GSAbilitySystemGlobals
class UGSAbilitySystemGlobals;
#define GSAbilitySystemGlobals_16_GENERATED_BODY public:  typedef UAbilitySystemGlobals Super;  typedef UGSAbilitySystemGlobals ThisClass; static UClass *StaticClass() { static UClass c = {"UGSAbilitySystemGlobals", &ShimMake<UGSAbilitySystemGlobals>}; return &c; } bool InstanceOf(const UClass *c) const override { return c == StaticClass() || Super::InstanceOf(c); } 
