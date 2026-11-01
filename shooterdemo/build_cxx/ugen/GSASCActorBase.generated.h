#pragma once
#undef CURRENT_FILE_ID
#define CURRENT_FILE_ID GSASCActorBase
class AGSASCActorBase;
#define GSASCActorBase_16_GENERATED_BODY public: typedef AActor Super;  typedef AGSASCActorBase ThisClass; static UClass *StaticClass() { static UClass c = {"AGSASCActorBase", &ShimMake<AGSASCActorBase>}; return &c; } bool InstanceOf(const UClass *c) const override { return c == StaticClass() || Super::InstanceOf(c); } 
