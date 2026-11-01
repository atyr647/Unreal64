#pragma once
#undef CURRENT_FILE_ID
#define CURRENT_FILE_ID GSEngineSubsystem
class UGSEngineSubsystem;
#define GSEngineSubsystem_15_GENERATED_BODY public:  typedef UEngineSubsystem Super;  typedef UGSEngineSubsystem ThisClass; static UClass *StaticClass() { static UClass c = {"UGSEngineSubsystem", &ShimMake<UGSEngineSubsystem>}; return &c; } bool InstanceOf(const UClass *c) const override { return c == StaticClass() || Super::InstanceOf(c); } 
