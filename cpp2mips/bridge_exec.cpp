// Bridge to the project's own UGSDamageExecutionCalc (GASShooter, unmodified): the damage formula -- set-by-caller + captured Damage, headshot multiplier,
// armour mitigation 100 / (100 + Armor) -- runs as the project wrote it. The runtime resolves the captures (u64_exec_in) and applies the output modifier.
#include "Characters/Abilities/GSDamageExecutionCalc.h"
#include "Characters/Abilities/AttributeSets/GSAttributeSetBase.h"
extern "C" {
#include "u64_gas_core.h"
}
namespace {
const u64_exec_in *cur;
float capture(void *, const FGameplayEffectAttributeCaptureDefinition &d)       // the captured magnitude for a definition (the calc declares Armor from the Target, Damage from the Source)
{
  const char *n = d.AttributeToCapture.Property->Name.ToString();
  return !strcmp(n, "Armor") ? cur->armor : !strcmp(n, "Damage") ? cur->damage : 0.0f;
}
}
extern "C" int u64_cxx_exec_calls;
int u64_cxx_exec_calls;
extern "C" int u64_cxx_exec_damage(const u64_exec_in *in, u64_exec_out *out)
{
  static UGSDamageExecutionCalc calc; static UAbilitySystemComponent src, tgt;       // the calc only null-checks the components (it reads the avatars, unused)
  cur = in; ExecCaptureHook = capture;
  FGameplayEffectSpec spec; FGameplayEffectContext ctx; FHitResult hit;
  for (int i = 0; i < in->nsbc; i++) spec.SetByCallerTagMagnitudes.Add(FGameplayTag::RequestGameplayTag(FName(in->sbc_tag[i])), in->sbc_val[i]);
  for (const char *const *t = in->asset_tags; t && *t; t++) spec.AssetTags.AddTag(FGameplayTag::RequestGameplayTag(FName(*t)));
  if (in->has_hit) { hit.BoneName = FName(in->bone); ctx.HitResultPtr = &hit; spec.Context.d = &ctx; }
  FGameplayEffectCustomExecutionParameters params; params.Source = &src; params.Target = &tgt; params.Spec = &spec;
  FGameplayEffectCustomExecutionOutput output;
  calc.Execute_Implementation(params, output); u64_cxx_exec_calls++;
  out->mitigated = 0; for (int i = 0; i < output.Mods.Num(); i++) if (!strcmp(output.Mods[i].Attribute.Property->Name.ToString(), "Damage") && output.Mods[i].ModifierOp == EGameplayModOp::Additive) out->mitigated += output.Mods[i].Magnitude;
  out->headshot = spec.DynamicAssetTags.HasTag(FGameplayTag::RequestGameplayTag(FName("Effect.Damage.HeadShot")));
  return 1;
}
