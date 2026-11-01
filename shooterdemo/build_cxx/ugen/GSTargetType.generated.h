#pragma once
#undef CURRENT_FILE_ID
#define CURRENT_FILE_ID GSTargetType
class UGSTargetType;
class UGSTargetType_UseOwner;
class UGSTargetType_UseEventData;
#define GSTargetType_24_GENERATED_BODY public: typedef UObject Super;  typedef UGSTargetType ThisClass; static UClass *StaticClass() { static UClass c = {"UGSTargetType", &ShimMake<UGSTargetType>}; return &c; } bool InstanceOf(const UClass *c) const override { return c == StaticClass() || Super::InstanceOf(c); } 
#define GSTargetType_40_GENERATED_BODY public: typedef UGSTargetType Super;  typedef UGSTargetType_UseOwner ThisClass; static UClass *StaticClass() { static UClass c = {"UGSTargetType_UseOwner", &ShimMake<UGSTargetType_UseOwner>}; return &c; } bool InstanceOf(const UClass *c) const override { return c == StaticClass() || Super::InstanceOf(c); } 
#define GSTargetType_54_GENERATED_BODY public: typedef UGSTargetType Super;  typedef UGSTargetType_UseEventData ThisClass; static UClass *StaticClass() { static UClass c = {"UGSTargetType_UseEventData", &ShimMake<UGSTargetType_UseEventData>}; return &c; } bool InstanceOf(const UClass *c) const override { return c == StaticClass() || Super::InstanceOf(c); } 
