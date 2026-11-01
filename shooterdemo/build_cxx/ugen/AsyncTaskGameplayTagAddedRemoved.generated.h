#pragma once
#undef CURRENT_FILE_ID
#define CURRENT_FILE_ID AsyncTaskGameplayTagAddedRemoved
class UAsyncTaskGameplayTagAddedRemoved;
#define AsyncTaskGameplayTagAddedRemoved_20_GENERATED_BODY public:  typedef UBlueprintAsyncActionBase Super;  typedef UAsyncTaskGameplayTagAddedRemoved ThisClass; static UClass *StaticClass() { static UClass c = {"UAsyncTaskGameplayTagAddedRemoved", &ShimMake<UAsyncTaskGameplayTagAddedRemoved>}; return &c; } bool InstanceOf(const UClass *c) const override { return c == StaticClass() || Super::InstanceOf(c); } 
