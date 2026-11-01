#pragma once
#undef CURRENT_FILE_ID
#define CURRENT_FILE_ID GSPlayerState
class AGSPlayerState;
#define GSPlayerState_19_GENERATED_BODY public:  typedef APlayerState Super;  typedef AGSPlayerState ThisClass; static UClass *StaticClass() { static UClass c = {"AGSPlayerState", &ShimMake<AGSPlayerState>}; return &c; } bool InstanceOf(const UClass *c) const override { return c == StaticClass() || Super::InstanceOf(c); } 
