#pragma once
#undef CURRENT_FILE_ID
#define CURRENT_FILE_ID GSDamageExecutionCalc
class UGSDamageExecutionCalc;
#define GSDamageExecutionCalc_15_GENERATED_BODY public:  typedef UGameplayEffectExecutionCalculation Super;  typedef UGSDamageExecutionCalc ThisClass; static UClass *StaticClass() { static UClass c = {"UGSDamageExecutionCalc", &ShimMake<UGSDamageExecutionCalc>}; return &c; } bool InstanceOf(const UClass *c) const override { return c == StaticClass() || Super::InstanceOf(c); } 
