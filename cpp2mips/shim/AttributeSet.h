#pragma once
#include "ue_shim.h"
#include "Net/UnrealNetwork.h"

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

enum class EGameplayModOp { Additive, Multiplicitive, Division, Override };

class UAbilitySystemComponent {                                   // implemented by the Unreal64 runtime (bridge): the C++ only calls these
public:
  virtual ~UAbilitySystemComponent() {}
  virtual void SetNumericAttributeBase(const FGameplayAttribute &a, float v) = 0;
  virtual void ApplyModToAttributeUnsafe(const FGameplayAttribute &a, EGameplayModOp op, float magnitude) = 0;
};

struct FGameplayModifierEvaluatedData { FGameplayAttribute Attribute; float Magnitude = 0; };
struct FGameplayEffectModCallbackData { FGameplayModifierEvaluatedData EvaluatedData; };

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
