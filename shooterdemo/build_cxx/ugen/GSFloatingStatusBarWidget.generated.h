#pragma once
#undef CURRENT_FILE_ID
#define CURRENT_FILE_ID GSFloatingStatusBarWidget
class UGSFloatingStatusBarWidget;
#define GSFloatingStatusBarWidget_15_GENERATED_BODY public: typedef UUserWidget Super;  typedef UGSFloatingStatusBarWidget ThisClass; static UClass *StaticClass() { static UClass c = {"UGSFloatingStatusBarWidget", &ShimMake<UGSFloatingStatusBarWidget>}; return &c; } bool InstanceOf(const UClass *c) const override { return c == StaticClass() || Super::InstanceOf(c); } 
