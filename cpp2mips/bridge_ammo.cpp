// Bridge between the Unreal64 runtime and the project's own UGSAmmoAttributeSet (GASShooter, unmodified, compiled against the UE shim).
// The runtime keeps attribute values in its ability system; UGSAmmoAttributeSet keeps them in FGameplayAttributeData members. For each call the set's
// members are loaded from the runtime, the engine callback runs, and everything the callback writes goes straight back through the runtime's setter.
#include "Characters/Abilities/AttributeSets/GSAmmoAttributeSet.h"
extern "C" {
#include "u64_gas_core.h"
}

namespace {
struct RtASC : UAbilitySystemComponent {                       // UAbilitySystemComponent over the runtime's ASC object
  u64_obj *asc = nullptr; UAttributeSet *set = nullptr;
  void SetNumericAttributeBase(const FGameplayAttribute &a, float v) override
  {
    u64_attr_set_base(asc, a.Property->Name.ToString(), v);    // the runtime's setter (PreAttributeChange, clamping rules) ...
    if (FGameplayAttributeData *d = set->FindAttributeData(a.Property->Name)) { d->SetBaseValue(v); d->SetCurrentValue(v); }   // ... and the set's own copy
  }
  void ApplyModToAttributeUnsafe(const FGameplayAttribute &a, EGameplayModOp op, float m) override
  {
    if (op == EGameplayModOp::Additive) SetNumericAttributeBase(a, u64_attr_get(asc, a.Property->Name.ToString()) + m);
  }
};
struct Slot { u64_obj *asc; UGSAmmoAttributeSet set; RtASC rt; };
Slot slots[16]; int nslots;
Slot *slot_for(u64_obj *asc)
{
  for (int i = 0; i < nslots; i++) if (slots[i].asc == asc) return &slots[i];
  if (nslots >= 16) return nullptr;
  Slot *s = &slots[nslots++]; s->asc = asc; s->rt.asc = asc; s->rt.set = &s->set; s->set.Owner = &s->rt;
  return s;
}
const char *const names[] = {"RifleReserveAmmo", "MaxRifleReserveAmmo", "RocketReserveAmmo", "MaxRocketReserveAmmo", "ShotgunReserveAmmo", "MaxShotgunReserveAmmo", nullptr};
}

// UGSAmmoAttributeSet::PostGameplayEffectExecute for the attribute `name` of the ability system `asc`. Returns 0 when the name is not one of its attributes.
extern "C" int u64_cxx_ammo_calls;
int u64_cxx_ammo_calls;
extern "C" int u64_cxx_ammo_post_execute(u64_obj *asc, const char *name)
{
  bool mine = false;
  for (int i = 0; names[i]; i++) if (!strcmp(names[i], name)) mine = true;
  Slot *s = mine ? slot_for(asc) : nullptr;
  if (!s) return 0;
  for (int i = 0; names[i]; i++) { FGameplayAttributeData *d = s->set.FindAttributeData(FName(names[i])); float v = u64_attr_get(asc, names[i]); d->SetBaseValue(v); d->SetCurrentValue(v); }
  FGameplayEffectModCallbackData data(*s->set.Owner);
  data.EvaluatedData.Attribute = FGameplayAttribute(FindFieldChecked<FProperty>(nullptr, FName(name)));
  s->set.PostGameplayEffectExecute(data);
  u64_cxx_ammo_calls++;
  return 1;
}
