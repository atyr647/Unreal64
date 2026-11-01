#pragma once
#undef CURRENT_FILE_ID
#define CURRENT_FILE_ID GSGATA_Trace
class AGSGATA_Trace;
#define GSGATA_Trace_22_GENERATED_BODY public:  typedef AGameplayAbilityTargetActor Super;  typedef AGSGATA_Trace ThisClass; static UClass *StaticClass() { static UClass c = {"AGSGATA_Trace", &ShimMake<AGSGATA_Trace>}; return &c; } bool InstanceOf(const UClass *c) const override { return c == StaticClass() || Super::InstanceOf(c); } 
