#pragma once
// The GAS plugin's core types, as much as the gameplay code touches: ability specs / handles / target data / events, UGameplayAbility, UAbilityTask,
// the ability system component's surface. Semantics live in the Unreal64 runtime (runtime/u64_gas_core.c); these are the C++ faces over it.
#include "AttributeSet.h"
#include "GameFramework/Actors.h"

struct UPrimitiveComponent;
struct FHitResult { FHitResult() {} void *Rt = nullptr; TWeakObjectPtr<UPrimitiveComponent> Component; FVector Location, ImpactPoint, Normal, TraceStart, TraceEnd; float Time = 1, Distance = 0; bool bBlockingHit = false; TWeakObjectPtr<AActor> Actor; FName BoneName;
  AActor *GetActor() const { return Actor.Get(); } };
inline void FGameplayEffectContext::AddHitResult(const FHitResult &h, bool) { HitResultPtr = new FHitResult(h); }
struct FActiveGameplayEffectHandle { int32 Handle = -1; bool IsValid() const { return Handle >= 0; } bool operator==(const FActiveGameplayEffectHandle &o) const { return Handle == o.Handle; } };
struct FGameplayEffectSpecHandle { FGameplayEffectSpec *Data = nullptr; bool IsValid() const { return Data != nullptr; } };
enum class EGameplayAbilityActivationMode { Authority, NonAuthority, Predicting, Confirmed, Rejected };
struct FGameplayAbilityActivationInfo { EGameplayAbilityActivationMode ActivationMode = EGameplayAbilityActivationMode::Authority; FPredictionKey GetActivationPredictionKey() const { return FPredictionKey(); } };
struct FGameplayEventData { FGameplayEffectContextHandle ContextHandle; FGameplayTag EventTag; AActor *Instigator = nullptr, *Target = nullptr; const UObject *OptionalObject = nullptr; FGameplayTagContainer InstigatorTags, TargetTags; float EventMagnitude = 0; };
class UGameplayAbility;
struct FGameplayAbilitySpec { UGameplayAbility *GetPrimaryInstance() const { return Ability; } FGameplayAbilitySpecHandle Handle; UGameplayAbility *Ability = nullptr; int32 Level = 1; int32 InputID = -1; UObject *SourceObject = nullptr; bool IsActive() const { return false; } bool InputPressed = false; };

struct FGameplayAbilityTargetData { virtual ~FGameplayAbilityTargetData() {} virtual bool HasHitResult() const { return false; } virtual const FHitResult *GetHitResult() const { return nullptr; } };
struct FGameplayAbilityTargetData_SingleTargetHit : FGameplayAbilityTargetData { FHitResult HitResult; FGameplayAbilityTargetData_SingleTargetHit() {} FGameplayAbilityTargetData_SingleTargetHit(const FHitResult &h) : HitResult(h) {}
  bool HasHitResult() const override { return true; } const FHitResult *GetHitResult() const override { return &HitResult; } };
struct FGameplayAbilityTargetData_ActorArray : FGameplayAbilityTargetData { TArray<TWeakObjectPtr<AActor>> TargetActorArray; };
struct FGameplayAbilityTargetDataHandle { TArray<FGameplayAbilityTargetData *> Data; FGameplayAbilityTargetDataHandle() {} FGameplayAbilityTargetDataHandle(FGameplayAbilityTargetData *d) { Data.Add(d); } bool NetSerialize(FArchive &, UPackageMap *, bool &b) { b = true; return true; }
  int32 Num() const { return Data.Num(); } void Add(FGameplayAbilityTargetData *d) { Data.Add(d); } void Append(const FGameplayAbilityTargetDataHandle &o) { Data.Append(o.Data); } void Clear() { Data.Empty(); }
  FGameplayAbilityTargetData *Get(int32 i) { return Data.Num() > i ? Data[i] : nullptr; } const FGameplayAbilityTargetData *Get(int32 i) const { return Data.Num() > i ? Data[i] : nullptr; } };
struct FGameplayAbilityLocalAnimMontage { UAnimMontage *AnimMontage = nullptr; bool PlayBit = false; };
struct FGameplayAbilityRepAnimMontage { UAnimMontage *AnimMontage = nullptr; float PlayRate = 1; };
struct FGameplayCueParameters { float RawMagnitude = 0; AActor *Instigator = nullptr; };
class UAbilitySystemGlobals : public UObject {
public:
  static UClass *StaticClass() { static UClass c = {"UAbilitySystemGlobals"}; return &c; }
  bool InstanceOf(const UClass *c) const override { return c == StaticClass(); }
  virtual FGameplayEffectContext *AllocGameplayEffectContext() const { return new FGameplayEffectContext(); }
  virtual void InitGlobalTags() {} virtual void InitGlobalData() {}
  static UAbilitySystemGlobals &Get() { static UAbilitySystemGlobals g; return g; }
};
struct USkeletalMeshComponent : UObject {}; struct UAnimMontage : UObject {};
struct FGameplayAbilityActorInfoFull;
enum class EGameplayAbilityInstancingPolicy { NonInstanced, InstancedPerActor, InstancedPerExecution };
struct FPostLockDelegate { template <class... A> static FPostLockDelegate CreateUObject(A...) { return FPostLockDelegate(); } };
// runtime-backed parts of the ability base (set by the bridge, which owns the u64 objects): level, outgoing effect specs, applying specs to target data, source object
struct AbilityRtHooks {
  int (*level)(void *ab) = nullptr; FGameplayEffectSpecHandle (*make_spec)(void *ab, UClass *effect, int level) = nullptr;
  void (*apply)(void *ab, const FGameplayEffectSpecHandle &, const FGameplayAbilityTargetDataHandle &, TArray<FActiveGameplayEffectHandle> &) = nullptr; UObject *(*source)(void *ab) = nullptr; };
inline AbilityRtHooks AbilityRt;
inline bool (*AbilityCommitHook)(void *) = nullptr; inline void (*AbilityEndHook)(void *, bool) = nullptr;         // set by the runtime bridge
class UGameplayAbility : public UObject {
public:
  int32 ScopeLockCount = 0; TArray<FPostLockDelegate> WaitingToExecute;
  bool HasAuthorityOrPredictionKey(const FGameplayAbilityActorInfo *, const FGameplayAbilityActivationInfo *) const { return true; }
  static UClass *StaticClass() { static UClass c = {"UGameplayAbility"}; return &c; }
  bool InstanceOf(const UClass *c) const override { return c == StaticClass(); }
  FGameplayTagContainer AbilityTags, CancelAbilitiesWithTag, BlockAbilitiesWithTag, ActivationOwnedTags, ActivationRequiredTags, ActivationBlockedTags;
  TSubclassOf<UGameplayEffect> CostGameplayEffectClass, CooldownGameplayEffectClass;
  FGameplayAbilitySpecHandle CurrentSpecHandle; const FGameplayAbilityActorInfo *CurrentActorInfo = nullptr; FGameplayAbilityActivationInfo CurrentActivationInfo;
  virtual void OnAvatarSet(const FGameplayAbilityActorInfo *, const FGameplayAbilitySpec &) {}
  virtual bool CanActivateAbility(const FGameplayAbilitySpecHandle, const FGameplayAbilityActorInfo *, const FGameplayTagContainer * = nullptr, const FGameplayTagContainer * = nullptr, OUT FGameplayTagContainer * = nullptr) const { return true; }
  virtual bool CheckCost(const FGameplayAbilitySpecHandle, const FGameplayAbilityActorInfo *, OUT FGameplayTagContainer * = nullptr) const { return true; }
  virtual void ApplyCost(const FGameplayAbilitySpecHandle, const FGameplayAbilityActorInfo *, const FGameplayAbilityActivationInfo) const {}
  virtual void ActivateAbility(const FGameplayAbilitySpecHandle, const FGameplayAbilityActorInfo *, const FGameplayAbilityActivationInfo, const FGameplayEventData * = nullptr) {}
  virtual void EndAbility(const FGameplayAbilitySpecHandle, const FGameplayAbilityActorInfo *, const FGameplayAbilityActivationInfo, bool, bool bWasCancelled) { if (AbilityEndHook) AbilityEndHook(rt, bWasCancelled); }
  virtual void CancelAbility(const FGameplayAbilitySpecHandle, const FGameplayAbilityActorInfo *, const FGameplayAbilityActivationInfo, bool) { if (AbilityEndHook) AbilityEndHook(rt, true); }
  virtual bool CommitAbility(const FGameplayAbilitySpecHandle, const FGameplayAbilityActorInfo *, const FGameplayAbilityActivationInfo) { return AbilityCommitHook ? AbilityCommitHook(rt) : true; }
  const FGameplayAbilityActorInfo *GetCurrentActorInfo() const { return CurrentActorInfo; }
  FGameplayAbilitySpecHandle GetCurrentAbilitySpecHandle() const { return CurrentSpecHandle; }
  FGameplayAbilityActivationInfo GetCurrentActivationInfo() const { return CurrentActivationInfo; }
  AActor *GetAvatarActorFromActorInfo() const { return CurrentActorInfo && CurrentActorInfo->AvatarActor.IsValid() ? CurrentActorInfo->AvatarActor.Get() : nullptr; }
  UAbilitySystemComponent *GetAbilitySystemComponentFromActorInfo() const { return CurrentActorInfo ? CurrentActorInfo->AbilitySystemComponent.Get() : nullptr; }
  UAbilitySystemComponent *GetAbilitySystemComponentFromActorInfo_Checked() const { return GetAbilitySystemComponentFromActorInfo(); }
  AActor *GetOwningActorFromActorInfo() const { return CurrentActorInfo && CurrentActorInfo->OwnerActor.IsValid() ? CurrentActorInfo->OwnerActor.Get() : nullptr; }
  int32 GetAbilityLevel() const { return AbilityRt.level ? AbilityRt.level(rt) : 1; }
  FGameplayEffectSpecHandle MakeOutgoingGameplayEffectSpec(TSubclassOf<UGameplayEffect> c, float level = 1) const { return AbilityRt.make_spec ? AbilityRt.make_spec(rt, c.Get(), (int)level) : FGameplayEffectSpecHandle(); }
  TArray<FActiveGameplayEffectHandle> K2_ApplyGameplayEffectSpecToTarget(const FGameplayEffectSpecHandle &h, const FGameplayAbilityTargetDataHandle &t) const { TArray<FActiveGameplayEffectHandle> r; if (AbilityRt.apply) AbilityRt.apply(rt, h, t, r); return r; }
  UObject *GetSourceObject(FGameplayAbilitySpecHandle, const FGameplayAbilityActorInfo *) const { return AbilityRt.source ? AbilityRt.source(rt) : nullptr; }
  FGameplayAbilitySpec CurrentSpecShim; FGameplayAbilitySpec *GetCurrentAbilitySpec() const { return const_cast<FGameplayAbilitySpec *>(&CurrentSpecShim); } bool IsPredictingClient() const { return false; }
  virtual void InputReleased(const FGameplayAbilitySpecHandle, const FGameplayAbilityActorInfo *, FGameplayAbilityActivationInfo) {}
  virtual void InputPressed(const FGameplayAbilitySpecHandle, const FGameplayAbilityActorInfo *, FGameplayAbilityActivationInfo) {}
  EGameplayAbilityInstancingPolicy InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor; int NetExecutionPolicy = 0;
  bool IsActive() const { return false; } bool IsInstantiated() const { return true; }
  void K2_EndAbility() {} void K2_CancelAbility() {}
};
