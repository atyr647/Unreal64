#pragma once
#undef CURRENT_FILE_ID
#define CURRENT_FILE_ID GSAbilitySystemComponent
struct FGameplayAbilityLocalAnimMontageForMesh;
struct FGameplayAbilityRepAnimMontageForMesh;
class UGSAbilitySystemComponent;
#define GSAbilitySystemComponent_17_GENERATED_BODY public:  typedef FGameplayAbilityLocalAnimMontageForMesh ThisClass; static UClass *StaticClass() { static UClass c = {"FGameplayAbilityLocalAnimMontageForMesh", &ShimMake<FGameplayAbilityLocalAnimMontageForMesh>}; return &c; } static UScriptStruct *StaticStruct() { static UScriptStruct s; return &s; } 
#define GSAbilitySystemComponent_47_GENERATED_BODY public:  typedef FGameplayAbilityRepAnimMontageForMesh ThisClass; static UClass *StaticClass() { static UClass c = {"FGameplayAbilityRepAnimMontageForMesh", &ShimMake<FGameplayAbilityRepAnimMontageForMesh>}; return &c; } static UScriptStruct *StaticStruct() { static UScriptStruct s; return &s; } 
#define GSAbilitySystemComponent_72_GENERATED_BODY public: typedef UAbilitySystemComponent Super;  typedef UGSAbilitySystemComponent ThisClass; static UClass *StaticClass() { static UClass c = {"UGSAbilitySystemComponent", &ShimMake<UGSAbilitySystemComponent>}; return &c; } bool InstanceOf(const UClass *c) const override { return c == StaticClass() || Super::InstanceOf(c); } 
