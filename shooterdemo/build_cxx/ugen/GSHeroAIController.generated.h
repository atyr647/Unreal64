#pragma once
#undef CURRENT_FILE_ID
#define CURRENT_FILE_ID GSHeroAIController
class AGSHeroAIController;
#define GSHeroAIController_15_GENERATED_BODY public: typedef AAIController Super;  typedef AGSHeroAIController ThisClass; static UClass *StaticClass() { static UClass c = {"AGSHeroAIController", &ShimMake<AGSHeroAIController>}; return &c; } bool InstanceOf(const UClass *c) const override { return c == StaticClass() || Super::InstanceOf(c); } 
