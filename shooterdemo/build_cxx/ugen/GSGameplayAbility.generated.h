#pragma once
#undef CURRENT_FILE_ID
#define CURRENT_FILE_ID GSGameplayAbility
struct FAbilityMeshMontage;
class UGSGameplayAbility;
#define GSGameplayAbility_16_GENERATED_BODY public:  typedef FAbilityMeshMontage ThisClass; static UClass *StaticClass() { static UClass c = {"FAbilityMeshMontage", &ShimMake<FAbilityMeshMontage>}; return &c; } static UScriptStruct *StaticStruct() { static UScriptStruct s; return &s; } 
#define GSGameplayAbility_41_GENERATED_BODY public: typedef UGameplayAbility Super;  typedef UGSGameplayAbility ThisClass; static UClass *StaticClass() { static UClass c = {"UGSGameplayAbility", &ShimMake<UGSGameplayAbility>}; return &c; } bool InstanceOf(const UClass *c) const override { return c == StaticClass() || Super::InstanceOf(c); } 
