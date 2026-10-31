#pragma once
// AActor / APawn / ACharacter / AController / APlayerController: identity and the relations the gameplay code walks; state lives in the runtime.
#include "AttributeSet.h"
#define STATIC_CLASS_OF(C) static UClass *StaticClass() { static UClass c = {#C}; return &c; }
struct AController;
struct FRotator { float Pitch = 0, Yaw = 0, Roll = 0; };
struct AActor : UObject { virtual void BeginPlay() {} STATIC_CLASS_OF(AActor) bool InstanceOf(const UClass *c) const override { return c == StaticClass(); } };
struct APawn : AActor { STATIC_CLASS_OF(APawn) bool InstanceOf(const UClass *c) const override { return c == StaticClass() || AActor::InstanceOf(c); }
  AController *Controller = nullptr; AController *GetController() const { return Controller; } };
struct ACharacter : APawn { STATIC_CLASS_OF(ACharacter) bool InstanceOf(const UClass *c) const override { return c == StaticClass() || APawn::InstanceOf(c); } ACharacter() {} ACharacter(const FObjectInitializer &) {} };
struct AController : AActor { STATIC_CLASS_OF(AController) bool InstanceOf(const UClass *c) const override { return c == StaticClass() || AActor::InstanceOf(c); }
  APawn *Pawn = nullptr; APawn *GetPawn() const { return Pawn; } };
struct APlayerController : AController { virtual void OnPossess(APawn *) {} virtual void OnRep_PlayerState() {} STATIC_CLASS_OF(APlayerController) bool InstanceOf(const UClass *c) const override { return c == StaticClass() || AController::InstanceOf(c); } };
