#pragma once
#undef CURRENT_FILE_ID
#define CURRENT_FILE_ID GSGameplayCueManager
class UGSGameplayCueManager;
#define GSGameplayCueManager_15_GENERATED_BODY public:  typedef UGameplayCueManager Super;  typedef UGSGameplayCueManager ThisClass; static UClass *StaticClass() { static UClass c = {"UGSGameplayCueManager", &ShimMake<UGSGameplayCueManager>}; return &c; } bool InstanceOf(const UClass *c) const override { return c == StaticClass() || Super::InstanceOf(c); } 
