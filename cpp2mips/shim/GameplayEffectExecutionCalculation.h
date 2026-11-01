#pragma once
// UGameplayEffectExecutionCalculation and what an execution reads / writes: attribute capture definitions, the owning spec, evaluated captures, output modifiers.
// The Unreal64 runtime fills the parameters (captured magnitudes, set-by-caller values, asset tags, the context's hit) and applies the output modifiers.
#include "ue_engine.h"
enum class EGameplayEffectAttributeCaptureSource { Source, Target };
struct FGameplayEffectAttributeCaptureDefinition { FGameplayAttribute AttributeToCapture; EGameplayEffectAttributeCaptureSource AttributeSource = EGameplayEffectAttributeCaptureSource::Source; bool bSnapshot = false;
  FGameplayEffectAttributeCaptureDefinition() {} FGameplayEffectAttributeCaptureDefinition(FGameplayAttribute a, EGameplayEffectAttributeCaptureSource s, bool snap) : AttributeToCapture(a), AttributeSource(s), bSnapshot(snap) {} };
#define DECLARE_ATTRIBUTE_CAPTUREDEF(P) FProperty *P##Property; FGameplayEffectAttributeCaptureDefinition P##Def;
#define DEFINE_ATTRIBUTE_CAPTUREDEF(S, P, T, B) { P##Property = FindFieldChecked<FProperty>(S::StaticClass(), GET_MEMBER_NAME_CHECKED(S, P)); P##Def = FGameplayEffectAttributeCaptureDefinition(P##Property, EGameplayEffectAttributeCaptureSource::T, B); }
struct FAggregatorEvaluateParameters { const FGameplayTagContainer *SourceTags = nullptr, *TargetTags = nullptr; };
inline float (*ExecCaptureHook)(void *ctx, const FGameplayEffectAttributeCaptureDefinition &) = nullptr;   // set by the bridge: the runtime's captured value
struct FGameplayEffectCustomExecutionParameters {
  UAbilitySystemComponent *Source = nullptr, *Target = nullptr; FGameplayEffectSpec *Spec = nullptr; void *Ctx = nullptr;
  UAbilitySystemComponent *GetTargetAbilitySystemComponent() const { return Target; } UAbilitySystemComponent *GetSourceAbilitySystemComponent() const { return Source; }
  const FGameplayEffectSpec &GetOwningSpec() const { return *Spec; } FGameplayEffectSpec *GetOwningSpecForPreExecuteMod() const { return Spec; }
  bool AttemptCalculateCapturedAttributeMagnitude(const FGameplayEffectAttributeCaptureDefinition &d, const FAggregatorEvaluateParameters &, float &out) const { if (!ExecCaptureHook) return false; out = ExecCaptureHook(Ctx, d); return true; }
};
struct FGameplayEffectCustomExecutionOutput { TArray<FGameplayModifierEvaluatedData> Mods; void AddOutputModifier(const FGameplayModifierEvaluatedData &m) { Mods.Add(m); } };
class UGameplayEffectExecutionCalculation : public UObject {
public:
  TArray<FGameplayEffectAttributeCaptureDefinition> RelevantAttributesToCapture;
  static UClass *StaticClass() { static UClass c = {"UGameplayEffectExecutionCalculation"}; return &c; }
  bool InstanceOf(const UClass *c) const override { return c == StaticClass(); }
  virtual void Execute_Implementation(const FGameplayEffectCustomExecutionParameters &, FGameplayEffectCustomExecutionOutput &) const {}
};
