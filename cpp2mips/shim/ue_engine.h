#pragma once
// Engine-side types the gameplay code uses besides GAS: collision queries, world, components, widgets, movement. Identity and the calls the project makes;
// behaviour comes from the runtime (u64_collision.c for traces, u64_movement.c for movement).
#include "ue_gas.h"
struct UAnimSequenceBase;
struct FCollisionProfileName { FName Name; };
struct FCollisionQueryParams { bool bTraceComplex = false, bReturnPhysicalMaterial = false, bIgnoreBlocks = false; FCollisionQueryParams() {} FCollisionQueryParams(FName, bool c) : bTraceComplex(c) {}
  void AddIgnoredActor(const AActor *) {} void AddIgnoredActors(const TArray<TWeakObjectPtr<AActor>> &) {} };
struct UActorComponent : UObject {}; struct USceneComponent : UActorComponent {}; struct UCameraComponent : USceneComponent {}; struct UWidgetComponent : USceneComponent {};
struct UAnimInstance : UObject {}; struct UUserWidget : UObject {}; struct UBlueprintFunctionLibrary : UObject {}; struct UBlueprintAsyncActionBase : UObject {};
struct APlayerState : AActor {}; struct AGameModeBase : AActor {}; struct UCharacterMovementComponent : UActorComponent {}; struct UProjectileMovementComponent : UActorComponent {};
struct UWorld : UObject { template <class... A> bool LineTraceMultiByProfile(A...) const { return false; } };
struct UGameplayStatics { template <class... A> static UObject *GetPlayerController(A...) { return nullptr; } };
#define UPARAM(...)
#define GENERATED_UCLASS_BODY() GENERATED_BODY()
struct IRepChangedPropertyTracker {}; struct FLinearColor { float R = 0, G = 0, B = 0, A = 1; }; struct FDelegateHandle {};  struct FSubsystemCollectionBase {}; struct USubsystem : UObject { virtual void Initialize(FSubsystemCollectionBase &) {} virtual void Deinitialize() {} };
enum class EAbilityTaskNetSyncType { BothWait, OnlyServerWait, OnlyClientWait };
struct FOnAttributeChangeData { FGameplayAttribute Attribute; float NewValue = 0, OldValue = 0; };
struct FGameplayAbilityTargetingLocationInfo { FVector LiteralTransform; FVector GetTargetingTransform() const { return LiteralTransform; } };
struct UPrimitiveComponent : USceneComponent {}; struct UGameplayEffectExecutionCalculation : UObject {}; struct UGameplayCueManager : UObject { virtual bool ShouldAsyncLoadRuntimeObjectLibraries() const { return true; } };
struct AGameplayAbilityTargetActor : AActor {}; struct UAbilityTask : UObject {}; struct UAnimNotify_PlaySound : UObject { virtual void Notify(USkeletalMeshComponent *, UAnimSequenceBase *) {} }; struct FSavedMove_Character { virtual ~FSavedMove_Character() {} virtual void Clear() {} virtual uint8 GetCompressedFlags() const { return 0; } virtual void SetMoveFor(ACharacter *, float, FVector const &, void *) {} virtual void PrepMoveFor(ACharacter *) {} };
struct FSimpleMulticastDelegate { template <class... A> void Broadcast(A...) const {} template <class... A> void AddUObject(A...) {} template <class... A> FDelegateHandle AddLambda(A...) { return FDelegateHandle(); } void Remove(FDelegateHandle) {} };
struct FGameplayTargetDataFilterHandle { bool FilterPassesForActor(const TWeakObjectPtr<AActor> &) const { return true; } bool FilterPassesForActor(const AActor *) const { return true; } };
struct AGameplayAbilityWorldReticle : AActor {}; struct UAnimSequenceBase : UObject {}; struct FSavedMovePtr {};
#define PURE_VIRTUAL(f, ...) { __VA_ARGS__ }
struct EDrawDebugTrace { enum Type { None, ForOneFrame, ForDuration, Persistent }; };
struct UInputComponent : UObject {}; struct UEngineSubsystem : USubsystem {};
struct UAnimNotify : UObject { virtual void Notify(USkeletalMeshComponent *, UAnimSequenceBase *) {} };
