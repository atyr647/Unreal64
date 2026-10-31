#pragma once
#undef CURRENT_FILE_ID
#define CURRENT_FILE_ID GSPlayerController
class AGSPlayerController;
#define GSPlayerController_18_GENERATED_BODY public: typedef APlayerController Super; typedef AGSPlayerController ThisClass; static UClass *StaticClass() { static UClass c = {"AGSPlayerController"}; return &c; } bool InstanceOf(const UClass *c) const override { return c == StaticClass() || Super::InstanceOf(c); } 
