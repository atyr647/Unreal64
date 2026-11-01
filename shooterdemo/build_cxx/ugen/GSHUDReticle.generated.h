#pragma once
#undef CURRENT_FILE_ID
#define CURRENT_FILE_ID GSHUDReticle
class UGSHUDReticle;
#define GSHUDReticle_15_GENERATED_BODY public:  typedef UUserWidget Super;  typedef UGSHUDReticle ThisClass; static UClass *StaticClass() { static UClass c = {"UGSHUDReticle", &ShimMake<UGSHUDReticle>}; return &c; } bool InstanceOf(const UClass *c) const override { return c == StaticClass() || Super::InstanceOf(c); } 
