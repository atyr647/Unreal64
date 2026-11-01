#pragma once
#undef CURRENT_FILE_ID
#define CURRENT_FILE_ID GSDamageTextWidgetComponent
class UGSDamageTextWidgetComponent;
#define GSDamageTextWidgetComponent_15_GENERATED_BODY public: typedef UWidgetComponent Super;  typedef UGSDamageTextWidgetComponent ThisClass; static UClass *StaticClass() { static UClass c = {"UGSDamageTextWidgetComponent", &ShimMake<UGSDamageTextWidgetComponent>}; return &c; } bool InstanceOf(const UClass *c) const override { return c == StaticClass() || Super::InstanceOf(c); } 
