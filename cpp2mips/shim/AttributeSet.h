#pragma once
#include "ue_shim.h"
#include "Net/UnrealNetwork.h"

enum class EGameplayModOp { Additive, Multiplicitive, Division, Override };
struct FGameplayTag {
  FName Name;
  static FGameplayTag RequestGameplayTag(FName n) { FGameplayTag t; t.Name = n; return t; }
  bool operator==(const FGameplayTag &o) const { return Name == o.Name; }
  bool operator!=(const FGameplayTag &o) const { return Name != o.Name; }
  bool IsValid() const { return Name != FName(); }
  const char *ToString() const { return Name.ToString(); }
};

struct FGameplayAttribute {                                      // an attribute is named by its property
  FProperty *Property = nullptr;
  FGameplayAttribute() {}
  FGameplayAttribute(FProperty *p) : Property(p) {}
  bool IsValid() const { return Property != nullptr; }
  bool operator==(const FGameplayAttribute &o) const { return Property && o.Property ? Property->Name == o.Property->Name : Property == o.Property; }
  bool operator!=(const FGameplayAttribute &o) const { return !(*this == o); }
};

struct FGameplayAttributeData {
  float BaseValue = 0, CurrentValue = 0;
  float GetBaseValue() const { return BaseValue; }
  float GetCurrentValue() const { return CurrentValue; }
  void SetBaseValue(float v) { BaseValue = v; }
  void SetCurrentValue(float v) { CurrentValue = v; }
};


struct AActor; struct AController; class UAbilitySystemComponent; class UGameplayEffect;
struct FGameplayTagContainer {
  FGameplayTag t[16]; int n = 0;
  void AddTagFast(const FGameplayTag &x) { if (n < 16) t[n++] = x; }
  void AddTag(const FGameplayTag &x) { if (!HasTag(x)) AddTagFast(x); }
  void RemoveTag(const FGameplayTag &x) { for (int i = 0; i < n; i++) if (t[i] == x) { t[i] = t[--n]; return; } }
  void AppendTags(const FGameplayTagContainer &o) { for (int i = 0; i < o.n; i++) AddTag(o.t[i]); }
  bool HasTagExact(const FGameplayTag &x) const { return HasTag(x); }
  bool HasTag(const FGameplayTag &x) const { for (int i = 0; i < n; i++) if (t[i] == x) return true; return false; }
};
struct FPredictionKey { bool IsValidKey() const { return false; } bool IsValidForMorePrediction() const { return false; } FString ToString() const { return FString("[0/0]"); } };
class UAbilitySystemComponent;
struct FGameplayAbilityActorInfo { TWeakObjectPtr<AActor> OwnerActor, AvatarActor; TWeakObjectPtr<AController> PlayerController; TWeakObjectPtr<UAbilitySystemComponent> AbilitySystemComponent; bool IsNetAuthority() const { return true; } bool IsLocallyControlledPlayer() const { return true; } };
struct FArchive {}; class UPackageMap; struct UScriptStruct {};
struct FHitResult;
struct FGameplayEffectContext {
  AActor *EffectCauser = nullptr; UAbilitySystemComponent *Instigator = nullptr; TArray<TWeakObjectPtr<AActor>> Actors; FHitResult *HitResultPtr = nullptr;
  virtual ~FGameplayEffectContext() {}
  virtual UScriptStruct *GetScriptStruct() const { static UScriptStruct s; return &s; }
  virtual FGameplayEffectContext *Duplicate() const { return new FGameplayEffectContext(*this); }
  virtual bool NetSerialize(FArchive &, UPackageMap *, bool &b) { b = true; return true; }
  void AddActors(const TArray<TWeakObjectPtr<AActor>> &a, bool = true) { Actors.Append(a); }
  void AddHitResult(const FHitResult &h, bool bReset = false);
  const FHitResult *GetHitResult() const { return HitResultPtr; }
  AActor *GetEffectCauser() const { return EffectCauser; }
};
template <class T> struct TStructOpsTypeTraitsBase2 {};
template <class T> struct TStructOpsTypeTraits : TStructOpsTypeTraitsBase2<T> {};
struct FGameplayEffectContextHandle {
  FGameplayEffectContext *d = nullptr;
  const FHitResult *GetHitResult() const { return d ? d->GetHitResult() : nullptr; }
  UAbilitySystemComponent *GetOriginalInstigatorAbilitySystemComponent() const { return d ? d->Instigator : nullptr; }
  AActor *GetEffectCauser() const { return d ? d->EffectCauser : nullptr; }
};
struct FTagCapture { FGameplayTagContainer agg; const FGameplayTagContainer *GetAggregatedTags() const { return &agg; } };
struct FGameplayEffectSpec {
  void *Rt = nullptr; FGameplayEffectContextHandle Context; FTagCapture CapturedSourceTags, CapturedTargetTags; TMap<FGameplayTag, float> SetByCallerTagMagnitudes;
  float GetSetByCallerMagnitude(FGameplayTag t, bool = true, float def = 0) const { const float *f = SetByCallerTagMagnitudes.Find(t); return f ? *f : def; } FGameplayTagContainer DynamicAssetTags, AssetTags;
  FGameplayEffectContextHandle GetContext() const { return Context; }
  void GetAllAssetTags(FGameplayTagContainer &o) const { o = AssetTags; o.AppendTags(DynamicAssetTags); }
};
struct FScalableFloat { float Value = 0; FScalableFloat() {} FScalableFloat(float v) : Value(v) {} };
enum class EGameplayEffectDurationType { Instant, Infinite, HasDuration };
struct FGameplayModifierInfo { FScalableFloat ModifierMagnitude; EGameplayModOp ModifierOp = EGameplayModOp::Additive; FGameplayAttribute Attribute; };
class UGameplayEffect : public UObject { public: EGameplayEffectDurationType DurationPolicy = EGameplayEffectDurationType::Instant; TArray<FGameplayModifierInfo> Modifiers; };
enum ELevelTick { LEVELTICK_All }; struct FActorComponentTickFunction {};
struct FGameplayCueParameters;
struct FGameplayAbilitySpecHandle { int32 Handle = -1; bool IsValid() const { return Handle >= 0; } bool operator==(const FGameplayAbilitySpecHandle &o) const { return Handle == o.Handle; } };
class UGameplayAbility; struct FGameplayAbilitySpec; struct USkeletalMeshComponent; struct UAnimMontage;
class UAbilitySystemComponent : public UObject {                                   // implemented by the Unreal64 runtime (bridge): the C++ only calls these
public:
  virtual ~UAbilitySystemComponent() {}
  static UClass *StaticClass() { static UClass c = {"UAbilitySystemComponent"}; return &c; }
  bool InstanceOf(const UClass *c) const override { return c == StaticClass(); }
  virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty> &) const {}
  virtual bool GetShouldTick() const { return false; }
  template <class... A> void SetReplicationMode(A...) {} template <class... A> void SetIsReplicated(A...) {} FGameplayAbilitySpec *FindAbilitySpecFromHandle(FGameplayAbilitySpecHandle) const { return nullptr; }
  virtual void TickComponent(float, ELevelTick, FActorComponentTickFunction *) {}
  virtual void InitAbilityActorInfo(AActor *, AActor *) {}
  virtual void AbilityLocalInputPressed(int32) {}
  virtual bool ShouldDoServerAbilityRPCBatch() const { return false; }
  virtual void NotifyAbilityEnded(FGameplayAbilitySpecHandle, UGameplayAbility *, bool) {}
  virtual int32 GetTagCount(const FGameplayTag &) const { return 0; }
  virtual bool TryActivateAbility(FGameplayAbilitySpecHandle, bool = true) { return false; }
  template <class E, class H, class K> FDelegateRemoveShim AbilityReplicatedEventDelegate(E, H, K) { return FDelegateRemoveShim(); } FDelegateBase GenericLocalConfirmCallbacks, GenericLocalCancelCallbacks;
  void AddLooseGameplayTag(const FGameplayTag &, int32 = 1) {} void AddLooseGameplayTags(const FGameplayTagContainer &, int32 = 1) {}
  void RemoveLooseGameplayTag(const FGameplayTag &, int32 = 1) {} void RemoveLooseGameplayTags(const FGameplayTagContainer &, int32 = 1) {}
  AActor *GetOwnerActor() const { return nullptr; } AActor *GetAvatarActor() const { return nullptr; }
  virtual void SetNumericAttributeBase(const FGameplayAttribute &, float) {}
  virtual void ApplyModToAttributeUnsafe(const FGameplayAttribute &, EGameplayModOp, float) {}
  template <class... A> FGameplayAbilitySpec *FindAbilitySpecFromClass(A...) const { return nullptr; }
  TSharedPtr<FGameplayAbilityActorInfo> AbilityActorInfo; FPredictionKey ScopedPredictionKey;
  virtual FGameplayEffectContextHandle MakeEffectContext() { return FGameplayEffectContextHandle(); }
  virtual void ApplyGameplayEffectToSelf(const UGameplayEffect *, float, FGameplayEffectContextHandle) {}
};

struct FGameplayModifierEvaluatedData { FGameplayAttribute Attribute; EGameplayModOp ModifierOp = EGameplayModOp::Additive; float Magnitude = 0; FGameplayModifierEvaluatedData() {}
  FGameplayModifierEvaluatedData(const FGameplayAttribute &a, EGameplayModOp op, float m) : Attribute(a), ModifierOp(op), Magnitude(m) {} };
struct FGameplayEffectModCallbackData { FGameplayEffectSpec EffectSpec; FGameplayModifierEvaluatedData EvaluatedData; UAbilitySystemComponent &Target; FGameplayEffectModCallbackData(UAbilitySystemComponent &t) : Target(t) {} };

class UAttributeSet : public UObject {
public:
  UAbilitySystemComponent *Owner = nullptr;
  UAbilitySystemComponent *GetOwningAbilitySystemComponent() const { return Owner; }
  virtual void PreAttributeChange(const FGameplayAttribute &, float &) {}
  virtual void PostGameplayEffectExecute(const FGameplayEffectModCallbackData &) {}
  virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty> &) const {}
  virtual FGameplayAttributeData *FindAttributeData(FName) { return nullptr; }   // generated per class by uht_lite (reflection lookup)
};

#define GAMEPLAYATTRIBUTE_PROPERTY_GETTER(ClassName, PropertyName) \
  static FGameplayAttribute Get##PropertyName##Attribute() { static FProperty *Prop = FindFieldChecked<FProperty>(ClassName::StaticClass(), GET_MEMBER_NAME_CHECKED(ClassName, PropertyName)); return Prop; }
#define GAMEPLAYATTRIBUTE_VALUE_GETTER(PropertyName) FORCEINLINE float Get##PropertyName() const { return PropertyName.GetCurrentValue(); }
#define GAMEPLAYATTRIBUTE_VALUE_SETTER(PropertyName) \
  FORCEINLINE void Set##PropertyName(float NewVal) { UAbilitySystemComponent *AbilityComp = GetOwningAbilitySystemComponent(); if (ensure(AbilityComp)) AbilityComp->SetNumericAttributeBase(Get##PropertyName##Attribute(), NewVal); }
#define GAMEPLAYATTRIBUTE_VALUE_INITTER(PropertyName) FORCEINLINE void Init##PropertyName(float NewVal) { PropertyName.SetBaseValue(NewVal); PropertyName.SetCurrentValue(NewVal); }
#define GAMEPLAYATTRIBUTE_REPNOTIFY(ClassName, PropertyName, OldValue) ((void)0)
