#pragma once
#undef CURRENT_FILE_ID
#define CURRENT_FILE_ID GSInteractable
class UGSInteractable;
class IGSInteractable;
#define GSInteractable_14_GENERATED_BODY public: typedef UInterface Super;  typedef UGSInteractable ThisClass; static UClass *StaticClass() { static UClass c = {"UGSInteractable", &ShimMake<UGSInteractable>}; return &c; } bool InstanceOf(const UClass *c) const override { return c == StaticClass() || Super::InstanceOf(c); } 
#define GSInteractable_22_GENERATED_BODY public:  typedef IGSInteractable ThisClass; static UClass *StaticClass() { static UClass c = {"IGSInteractable", &ShimMake<IGSInteractable>}; return &c; } 
