#pragma once
#undef CURRENT_FILE_ID
#define CURRENT_FILE_ID GSCharacterMovementComponent
class UGSCharacterMovementComponent;
class FGSSavedMove;
class FGSNetworkPredictionData_Client;
#define GSCharacterMovementComponent_16_GENERATED_BODY public:  typedef UCharacterMovementComponent Super;  typedef UGSCharacterMovementComponent ThisClass; static UClass *StaticClass() { static UClass c = {"UGSCharacterMovementComponent", &ShimMake<UGSCharacterMovementComponent>}; return &c; } bool InstanceOf(const UClass *c) const override { return c == StaticClass() || Super::InstanceOf(c); } 
