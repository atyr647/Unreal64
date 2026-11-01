#pragma once
#undef CURRENT_FILE_ID
#define CURRENT_FILE_ID GSBlueprintFunctionLibrary
class UGSBlueprintFunctionLibrary;
#define GSBlueprintFunctionLibrary_20_GENERATED_BODY public: typedef UBlueprintFunctionLibrary Super;  typedef UGSBlueprintFunctionLibrary ThisClass; static UClass *StaticClass() { static UClass c = {"UGSBlueprintFunctionLibrary", &ShimMake<UGSBlueprintFunctionLibrary>}; return &c; } bool InstanceOf(const UClass *c) const override { return c == StaticClass() || Super::InstanceOf(c); } 
