#pragma once
// AActor / APawn / ACharacter / AController / APlayerController: identity and the relations the gameplay code walks; state lives in the runtime.
#include "AttributeSet.h"
#define STATIC_CLASS_OF(C) static UClass *StaticClass() { static UClass c = {#C}; return &c; }
struct AController; struct APawn;
struct EEndPlayReason { enum Type { Destroyed, LevelTransition, EndPlayInEditor, RemovedFromWorld, Quit }; };
typedef EEndPlayReason::Type EEndPlayReasonShim; struct IRepChangedPropertyTracker; typedef IRepChangedPropertyTracker IRepChangedPropertyTrackerShim;
struct FRotator { float Pitch = 0, Yaw = 0, Roll = 0; };
struct AActor : UObject { template <class T = APawn> T *GetPawn() const { return nullptr; } virtual void BeginPlay() {} virtual void EndPlay(EEndPlayReasonShim) {} virtual void PreReplication(IRepChangedPropertyTrackerShim &) {} virtual void NotifyActorBeginOverlap(AActor *) {} virtual void NotifyActorEndOverlap(AActor *) {} virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty> &) const {} virtual void PostInitializeComponents() {} bool bReplicates = false; bool bNetUseOwnerRelevancy = false; float NetUpdateFrequency = 100; class USceneComponent *RootComponent = nullptr; void SetReplicates(bool) {} AActor *GetOwner() const { return nullptr; } bool bWantsPlayerState = false; struct { bool bCanEverTick = false; int TickGroup = 0; } PrimaryActorTick; STATIC_CLASS_OF(AActor) bool InstanceOf(const UClass *c) const override { return c == StaticClass(); } };
struct APawn : AActor { template <class T = struct APlayerState> T *GetPlayerState() const { return nullptr; } virtual void PossessedBy(AController *) {} virtual void OnRep_PlayerState() {} virtual void OnRep_Controller() {} virtual void UnPossessed() {} STATIC_CLASS_OF(APawn) bool InstanceOf(const UClass *c) const override { return c == StaticClass() || AActor::InstanceOf(c); }
  AController *Controller = nullptr; AController *GetController() const { return Controller; } };
struct UInputComponent;
inline void (*CharJumpHook)(void *, int) = nullptr; inline bool (*CharCanJumpHook)(void *) = nullptr;     // set by the runtime bridge
struct ACharacter : APawn { virtual void Jump() { if (CharJumpHook) CharJumpHook(rt, 1); } virtual void StopJumping() { if (CharJumpHook) CharJumpHook(rt, 0); } virtual bool CanJump() const { return CharCanJumpHook ? CharCanJumpHook(rt) : true; } virtual void SetupPlayerInputComponent(UInputComponent *) {} STATIC_CLASS_OF(ACharacter) bool InstanceOf(const UClass *c) const override { return c == StaticClass() || APawn::InstanceOf(c); } ACharacter() {} ACharacter(const FObjectInitializer &) {} };
struct AController : AActor { template <class T = APawn> T *GetPawn() const { return static_cast<T *>(Pawn); } STATIC_CLASS_OF(AController) bool InstanceOf(const UClass *c) const override { return c == StaticClass() || AActor::InstanceOf(c); }
  APawn *Pawn = nullptr; };
struct APlayerController : AController { template <class T = struct APlayerState> T *GetPlayerState() const { return nullptr; } bool IsLocalPlayerController() const { return true; } virtual void OnPossess(APawn *) {} virtual void OnRep_PlayerState() {} STATIC_CLASS_OF(APlayerController) bool InstanceOf(const UClass *c) const override { return c == StaticClass() || AController::InstanceOf(c); } };
