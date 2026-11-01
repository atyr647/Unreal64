#pragma once
// Engine-side types the gameplay code uses besides GAS: collision queries, world, components, widgets, movement. Identity and the calls the project makes;
// behaviour comes from the runtime (u64_collision.c for traces, u64_movement.c for movement).
#include "ue_gas.h"
struct UAnimSequenceBase;
struct FGameplayTargetDataFilterHandle { bool FilterPassesForActor(const TWeakObjectPtr<AActor> &) const { return true; } bool FilterPassesForActor(const AActor *) const { return true; } };
struct FWorldReticleParameters { FVector AOEScale; };
enum ETickingGroupShim { TG_PrePhysics, TG_PostPhysics, TG_PostUpdateWork };
struct FCollisionProfileName { FName Name; };
struct FCollisionQueryParams { bool bTraceComplex = false, bReturnPhysicalMaterial = false, bIgnoreBlocks = false; FCollisionQueryParams() {} FCollisionQueryParams(FName, bool c) : bTraceComplex(c) {}
  const AActor *Ignored[8]; int nIgnored = 0;
  void AddIgnoredActor(const AActor *a) { if (nIgnored < 8) Ignored[nIgnored++] = a; } void AddIgnoredActors(const TArray<TWeakObjectPtr<AActor>> &l) { for (int i = 0; i < l.Num(); i++) AddIgnoredActor(l[i].Get()); }
  void AddIgnoredActors(const TArray<AActor *> &l) { for (int i = 0; i < l.Num(); i++) AddIgnoredActor(l[i]); } };
#define SCENE_QUERY_STAT(X) FName(#X)
#define ABILITY_LOG(...) ((void)0)
#define ENABLE_DRAW_DEBUG 0
struct UActorComponent : UObject { template <class... A> void SetIsReplicated(A...) {} template <class... A> void RegisterComponent(A...) {} template <class... A> void SetActive(A...) {} };
struct USceneComponent : UActorComponent { template <class... A> void SetRelativeLocation(A...) {} template <class... A> void SetRelativeRotation(A...) {} template <class... A> void SetupAttachment(A...) {} template <class... A> void AttachToComponent(A...) {}
  template <class... A> void SetVisibility(A...) {} template <class... A> void SetHiddenInGame(A...) {} template <class... A> void SetCollisionEnabled(A...) {} template <class... A> void SetCollisionResponseToChannel(A...) {} template <class... A> void SetCollisionProfileName(A...) {}
  template <class... A> void SetCollisionObjectType(A...) {} template <class... A> void SetGenerateOverlapEvents(A...) {} FVector GetComponentLocation() const { return FVector(); } }; struct UCameraComponent : USceneComponent { bool bUsePawnControlRotation = false; float FieldOfView = 90; }; struct UWidgetComponent : USceneComponent {};
struct UAnimInstance : UObject {}; struct UUserWidget : UObject { template <class... A> void AddToViewport(A...) {} template <class... A> void SetVisibility(A...) {} }; struct UBlueprintFunctionLibrary : UObject {}; struct UBlueprintAsyncActionBase : UObject {};
struct APlayerState : AActor {}; struct AGameModeBase : AActor {}; struct UCharacterMovementComponent : UActorComponent { virtual float GetMaxSpeed() const { return 0; } virtual struct FNetworkPredictionData_Client_Character *GetPredictionData_Client() const { return nullptr; } virtual void UpdateFromCompressedFlags(uint8) {} }; struct UProjectileMovementComponent : UActorComponent { float InitialSpeed = 0, MaxSpeed = 0, ProjectileGravityScale = 1; bool bRotationFollowsVelocity = false; };
enum class EWorldType { None, Game, Editor, PIE };
enum ENetMode { NM_Standalone, NM_DedicatedServer, NM_ListenServer, NM_Client };
struct FQuatIdentityShim {}; struct FCollisionShape { float Radius = 0; static FCollisionShape MakeSphere(float r) { FCollisionShape s; s.Radius = r; return s; } };
// world queries are the runtime's (u64_collision.c); the bridge sets the hooks. LineTraceMultiByProfile: profile responses decide blocking / overlap, sorted, stops at the first block.
inline bool (*WorldLineTraceHook)(const FVector &, const FVector &, FName, const struct FCollisionQueryParams &, TArray<FHitResult> &) = nullptr;
inline bool (*WorldSweepHook)(const FVector &, const FVector &, float, FName, const struct FCollisionQueryParams &, TArray<FHitResult> &) = nullptr;
struct UWorld : UObject { EWorldType WorldType = EWorldType::Game; float GetTimeSeconds() const { return 0; } ENetMode GetNetMode() const { return NM_Standalone; }
  bool LineTraceMultiByProfile(TArray<FHitResult> &out, const FVector &a, const FVector &b, FName profile, const FCollisionQueryParams &p) const { return WorldLineTraceHook ? WorldLineTraceHook(a, b, profile, p, out) : false; }
  bool SweepMultiByProfile(TArray<FHitResult> &out, const FVector &a, const FVector &b, const FQuat &, FName profile, const FCollisionShape &sh, const FCollisionQueryParams &p) const { return WorldSweepHook ? WorldSweepHook(a, b, sh.Radius, profile, p, out) : false; }
  template <class T> T *SpawnActor(TSubclassOf<T>, const FVector &, const FRotator &) const { ShimUnimplemented("SpawnActor"); return nullptr; } };
inline UWorld *UObject::GetWorld() const { static UWorld w; return &w; }
struct UGameplayStatics { template <class... A> static UObject *GetPlayerController(A...) { return nullptr; } };
#define UPARAM(...)
#define GENERATED_UCLASS_BODY() GENERATED_BODY()
struct IRepChangedPropertyTracker {}; struct FLinearColor { float R = 0, G = 0, B = 0, A = 1; };  struct FSubsystemCollectionBase {}; struct USubsystem : UObject { virtual void Initialize(FSubsystemCollectionBase &) {} virtual void Deinitialize() {} };
enum class EAbilityTaskNetSyncType { BothWait, OnlyServerWait, OnlyClientWait };
struct FOnAttributeChangeData { FGameplayAttribute Attribute; float NewValue = 0, OldValue = 0; };
struct FGameplayAbilityTargetingLocationInfo { FTransform LiteralTransform; FTransform GetTargetingTransform() const { return LiteralTransform; } };
struct UPrimitiveComponent : USceneComponent {};
struct UGameplayCueManager : UObject { virtual bool ShouldAsyncLoadRuntimeObjectLibraries() const { return true; } };
struct AGameplayAbilityWorldReticle : AActor { bool bSnapToTargetedActor = false; template <class... A> void Destroy(A...) {} void SetActorHiddenInGame(bool) {} void SetActorLocation(const FVector &) {} void SetIsTargetAnActor(bool) {} void SetReplicates(bool) {}
  template <class... A> void InitializeReticle(A...) {} };
struct FTargetDataDelegateShim { void (*fn)(void *, const FGameplayAbilityTargetDataHandle &) = nullptr; void *ctx = nullptr;               // TargetDataReadyDelegate / CanceledDelegate: one bound receiver, set by the bridge
  void Broadcast(const FGameplayAbilityTargetDataHandle &h) const { if (fn) fn(ctx, h); } void Clear() { fn = nullptr; ctx = nullptr; } };
struct FActorTickFunctionShim { bool bCanEverTick = false; int TickGroup = 0; };
struct AGameplayAbilityTargetActor : AActor { FGameplayAbilityTargetingLocationInfo StartLocation; TSubclassOf<AGameplayAbilityWorldReticle> ReticleClass; FWorldReticleParameters ReticleParams; bool bDestroyOnConfirmation = true;
  UGameplayAbility *OwningAbility = nullptr; AActor *SourceActor = nullptr; APlayerController *MasterPC = nullptr; FGameplayTargetDataFilterHandle Filter; bool bDebug = false; bool ShouldProduceTargetDataOnServer = false;
  FCollisionProfileName TraceProfileBase; FTargetDataDelegateShim TargetDataReadyDelegate, CanceledDelegate; UAbilitySystemComponent *GenericDelegateBoundASC = nullptr; FDelegateHandle GenericCancelHandle; FActorTickFunctionShim PrimaryActorTick;
  virtual void StartTargeting(UGameplayAbility *) {} virtual void ConfirmTargetingAndContinue() {} virtual void ConfirmTargeting() {} virtual void CancelTargeting() {} virtual void Tick(float) {} virtual void StopTargeting() {}
  virtual bool IsConfirmTargetingAllowed() { return true; } virtual void BindToConfirmCancelInputs() {} virtual bool ShouldProduceTargetData() const { return true; }
  void SetActorTickEnabled(bool) {} void SetActorLocationAndRotation(const FVector &, const FRotator &) {} FVector GetActorLocation() const { return FVector(); } FRotator GetActorRotation() const { return FRotator(); }
  template <class... A> void Destroy(A...) {} }; struct EGameplayTargetingConfirmation { enum Type { Instant, UserConfirmed, Custom, CustomMulti }; };
struct UCurveFloat : UObject { float GetFloatValue(float) const { return 0; } };
struct FOnMontageBlendingOutStarted {}; struct FOnMontageEnded {};
struct UAbilityTask : UObject {
  virtual void ExternalConfirm(bool) {}
  UAbilityTask() {} UAbilityTask(const FObjectInitializer &) {}
  static UClass *StaticClass() { static UClass c = {"UAbilityTask"}; return &c; } bool InstanceOf(const UClass *c) const override { return c == StaticClass(); }
  UGameplayAbility *Ability = nullptr; UAbilitySystemComponent *AbilitySystemComponent = nullptr; bool bTickingTask = false; bool bIsPausable = true;
  virtual void Activate() {} virtual void OnDestroy(bool) {} virtual void TickTask(float) {} virtual void ExternalCancel() {} virtual void InitSimulatedTask(void *) {}
  virtual void ReadyForActivation() { Activate(); } void EndTask() { OnDestroy(false); } bool ShouldBroadcastAbilityTaskDelegates() const { return true; }
  virtual FString GetDebugString() const { return FString(); }
  template <class T> static T *NewAbilityTask(UGameplayAbility *a, FName = FName()) { T *t = new T(); t->Ability = a; return t; }
  AActor *GetAvatarActor() const { return nullptr; } void SetWaitingOnAvatar() {} void SetWaitingOnRemotePlayerData() {}
}; struct UAnimNotify_PlaySound : UObject { virtual void Notify(USkeletalMeshComponent *, UAnimSequenceBase *) {} }; struct FNetworkPredictionData_Client_Character; struct FSavedMove_Character { virtual ~FSavedMove_Character() {} virtual void Clear() {} virtual uint8 GetCompressedFlags() const { return 0; } virtual void SetMoveFor(ACharacter *, float, FVector const &, FNetworkPredictionData_Client_Character &) {} virtual bool CanCombineWith(const struct FSavedMovePtr &, ACharacter *, float) const { return false; } virtual void PrepMoveFor(ACharacter *) {} };
struct FSimpleMulticastDelegate { template <class... A> void Broadcast(A...) const {} template <class... A> void AddUObject(A...) {} template <class... A> FDelegateHandle AddLambda(A...) { return FDelegateHandle(); } void Remove(FDelegateHandle) {} };
 struct UAnimSequenceBase : UObject {}; struct FSavedMovePtr {};
#define PURE_VIRTUAL(f, ...) { __VA_ARGS__ }
struct EDrawDebugTrace { enum Type { None, ForOneFrame, ForDuration, Persistent }; };
struct UInputComponent : UObject {}; struct UEngineSubsystem : USubsystem {};
struct UAnimNotify : UObject { virtual void Notify(USkeletalMeshComponent *, UAnimSequenceBase *) {} };
struct UCapsuleComponent : UPrimitiveComponent { template <class... A> void SetCollisionObjectType(A...) {} void InitCapsuleSize(float, float) {} template <class... A> void SetCapsuleSize(A...) {} template <class... A> void SetCollisionResponseToChannel(A...) {} template <class... A> void SetupAttachment(A...) {} };
struct FWaitTargetDataDelegate : FDelegateBase {};
struct USpringArmComponent : USceneComponent { float TargetArmLength = 0; bool bUsePawnControlRotation = false, bInheritPitch = true, bInheritYaw = true, bInheritRoll = true; FVector SocketOffset; template <class... A> void SetupAttachment(A...) {} };
struct USoundBase : UObject {}; struct UAbilitySystemBlueprintLibrary { template <class... A> static UAbilitySystemComponent *GetAbilitySystemComponent(A...) { return nullptr; } template <class... A> static void SendGameplayEventToActor(A...) {} };
enum class EGameplayEffectReplicationMode { Minimal, Mixed, Full };
struct UUserWidgetX {}; template <class T, class... A> T *CreateWidget(A...) { return nullptr; }
struct FNetworkPredictionData_Client_Character { virtual ~FNetworkPredictionData_Client_Character() {} virtual FSavedMovePtr AllocateNewMove() { return FSavedMovePtr(); } };
enum class ECollisionEnabled { NoCollision, QueryOnly, PhysicsOnly, QueryAndPhysics };

// ---- batch: enums, math, spawn, attachment, roles, sound, prediction
enum ENetRole { ROLE_None, ROLE_SimulatedProxy, ROLE_AutonomousProxy, ROLE_Authority };
enum ECollisionChannel { ECC_WorldStatic, ECC_WorldDynamic, ECC_Pawn, ECC_Visibility, ECC_Camera, ECC_PhysicsBody, ECC_Vehicle, ECC_Destructible, ECC_GameTraceChannel1, ECC_GameTraceChannel2, ECC_GameTraceChannel3, ECC_GameTraceChannel4, ECC_GameTraceChannel5 };
enum ECollisionResponse { ECR_Ignore, ECR_Overlap, ECR_Block };
enum class EAbilityGenericReplicatedEvent { GenericConfirm, GenericCancel, InputPressed, InputReleased, GenericSignalFromClient, GenericSignalFromServer, GameCustom1 };
enum class EGameplayCueEvent { OnActive, WhileActive, Executed, Removed };
enum class EGameplayAbilityNetExecutionPolicy { LocalPredicted, LocalOnly, ServerInitiated, ServerOnly };
enum class EGameplayTagEventType { NewOrRemoved, AnyCountChange };
enum class EVisibilityBasedAnimTickOption { AlwaysTickPoseAndRefreshBones, AlwaysTickPose, OnlyTickMontagesWhenNotRendered, OnlyTickPoseWhenRendered };
enum class ESpawnActorCollisionHandlingMethod { Undefined, AlwaysSpawn, AdjustIfPossibleButAlwaysSpawn, AdjustIfPossibleButDontSpawnIfColliding, DontSpawnIfColliding };
enum class EAttachLocation { KeepRelativeOffset, KeepWorldPosition, SnapToTarget };
enum class EAttachmentRule { KeepRelative, KeepWorld, SnapToTarget };
struct FAttachmentTransformRules { FAttachmentTransformRules(EAttachmentRule, bool) {} FAttachmentTransformRules(EAttachmentRule, EAttachmentRule, EAttachmentRule, bool) {} static const FAttachmentTransformRules SnapToTargetNotIncludingScale, KeepRelativeTransform; };
struct FDetachmentTransformRules { FDetachmentTransformRules(EAttachmentRule, bool) {} static const FDetachmentTransformRules KeepWorldTransform; };
struct FActorSpawnParameters { FName Name; AActor *Owner = nullptr; APawn *Instigator = nullptr; ESpawnActorCollisionHandlingMethod SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::Undefined; };
struct FScopedPredictionWindow { template <class... A> FScopedPredictionWindow(A...) {} };
#define SCENE_QUERY_STAT(x) FName(#x)
#define DECLARE_ATTRIBUTE_CAPTUREDEF(...)
#define DEFINE_ATTRIBUTE_CAPTUREDEF(...)
