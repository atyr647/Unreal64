#pragma once
#undef CURRENT_FILE_ID
#define CURRENT_FILE_ID GSGameplayEffectTypes
struct FGSGameplayEffectContext;
#define GSGameplayEffectTypes_17_GENERATED_BODY public:  typedef FGameplayEffectContext Super;  typedef FGSGameplayEffectContext ThisClass; static UClass *StaticClass() { static UClass c = {"FGSGameplayEffectContext", &ShimMake<FGSGameplayEffectContext>}; return &c; } static UScriptStruct *StaticStruct() { static UScriptStruct s; return &s; } 
