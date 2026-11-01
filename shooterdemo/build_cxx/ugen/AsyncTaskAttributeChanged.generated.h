#pragma once
#undef CURRENT_FILE_ID
#define CURRENT_FILE_ID AsyncTaskAttributeChanged
class UAsyncTaskAttributeChanged;
#define AsyncTaskAttributeChanged_19_GENERATED_BODY public: typedef UBlueprintAsyncActionBase Super;  typedef UAsyncTaskAttributeChanged ThisClass; static UClass *StaticClass() { static UClass c = {"UAsyncTaskAttributeChanged", &ShimMake<UAsyncTaskAttributeChanged>}; return &c; } bool InstanceOf(const UClass *c) const override { return c == StaticClass() || Super::InstanceOf(c); } 
