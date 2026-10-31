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
  void AppendTags(const FGameplayTagContainer &o) { for (int i = 0; i < o.n; i++) AddTag(o.t[i]); }
  bool HasTag(const FGameplayTag &x) const { for (int i = 0; i < n; i++) if (t[i] == x) return true; return false; }
};
struct FGameplayAbilityActorInfo { TWeakObjectPtr<AActor> AvatarActor; TWeakObjectPtr<AController> PlayerController; };
struct FGameplayEffectContext { AActor *EffectCauser = nullptr; UAbilitySystemComponent *Instigator = nullptr; };
struct FGameplayEffectContextHandle {
  FGameplayEffectContext *d = nullptr;
  UAbilitySystemComponent *GetOriginalInstigatorAbilitySystemComponent() const { return d ? d->Instigator : nullptr; }
  AActor *GetEffectCauser() const { return d ? d->EffectCauser : nullptr; }
};
struct FTagCapture { FGameplayTagContainer agg; const FGameplayTagContainer *GetAggregatedTags() const { return &agg; } };
struct FGameplayEffectSpec {
  FGameplayEffectContextHandle Context; FTagCapture CapturedSourceTags; FGameplayTagContainer DynamicAssetTags, AssetTags;
  FGameplayEffectContextHandle GetContext() const { return Context; }
  void GetAllAssetTags(FGameplayTagContainer &o) const { o = AssetTags; o.AppendTags(DynamicAssetTags); }
};
struct FScalableFloat { float Value = 0; FScalableFloat() {} FScalableFloat(float v) : Value(v) {} };
enum class EGameplayEffectDurationType { Instant, Infinite, HasDuration };
struct FGameplayModifierInfo { FScalableFloat ModifierMagnitude; EGameplayModOp ModifierOp = EGameplayModOp::Additive; FGameplayAttribute Attribute; };
class UGameplayEffect : public UObject { public: EGameplayEffectDurationType DurationPolicy = EGameplayEffectDurationType::Instant; TArray<FGameplayModifierInfo> Modifiers; };
class UAbilitySystemComponent {                                   // implemented by the Unreal64 runtime (bridge): the C++ only calls these
public:
  virtual ~UAbilitySystemComponent() {}
  virtual void SetNumericAttributeBase(const FGameplayAttribute &a, float v) = 0;
  virtual void ApplyModToAttributeUnsafe(const FGameplayAttribute &a, EGameplayModOp op, float magnitude) = 0;
  TSharedPtr<FGameplayAbilityActorInfo> AbilityActorInfo;
  virtual FGameplayEffectContextHandle MakeEffectContext() { return FGameplayEffectContextHandle(); }
  virtual void ApplyGameplayEffectToSelf(const UGameplayEffect *, float, FGameplayEffectContextHandle) {}
};

struct FGameplayModifierEvaluatedData { FGameplayAttribute Attribute; float Magnitude = 0; };
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
