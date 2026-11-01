#pragma once
#undef CURRENT_FILE_ID
#define CURRENT_FILE_ID GSHUDWidget
class UGSHUDWidget;
#define GSHUDWidget_18_GENERATED_BODY public:  typedef UUserWidget Super;  typedef UGSHUDWidget ThisClass; static UClass *StaticClass() { static UClass c = {"UGSHUDWidget", &ShimMake<UGSHUDWidget>}; return &c; } bool InstanceOf(const UClass *c) const override { return c == StaticClass() || Super::InstanceOf(c); } 
