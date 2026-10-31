// Runs the project's own, unmodified GSAmmoAttributeSet.cpp (compiled against the UE shim) and checks its rules with exact values:
//   PostGameplayEffectExecute clamps the changed reserve ammo attribute to [0, Max...]; the tag helpers map Weapon.Ammo.* to attributes.
#include "Characters/Abilities/AttributeSets/GSAmmoAttributeSet.h"
#include <stdio.h>

static int fails;
#define CHECK(c) do { if (c) printf("ok   %s\n", #c); else { printf("FAIL %s (line %d)\n", #c, __LINE__); fails++; } } while (0)
struct MockASC : UAbilitySystemComponent {                          // stands in for the runtime's ability system: writes straight into the set
  UAttributeSet *set;
  void SetNumericAttributeBase(const FGameplayAttribute &a, float v) override { FGameplayAttributeData *d = set->FindAttributeData(a.Property->Name); if (d) { d->SetBaseValue(v); d->SetCurrentValue(v); } }
  void ApplyModToAttributeUnsafe(const FGameplayAttribute &a, EGameplayModOp, float m) override { FGameplayAttributeData *d = set->FindAttributeData(a.Property->Name); if (d) { d->SetBaseValue(d->BaseValue + m); d->SetCurrentValue(d->CurrentValue + m); } }
};

int main() {
  UGSAmmoAttributeSet s; MockASC asc; asc.set = &s; s.Owner = &asc;
  s.InitMaxRifleReserveAmmo(300); s.InitRifleReserveAmmo(60);
  s.InitMaxRocketReserveAmmo(30); s.InitRocketReserveAmmo(24);
  FGameplayEffectModCallbackData d(asc);
  s.RifleReserveAmmo.SetCurrentValue(350);                          // an effect pushed the value over the max
  d.EvaluatedData.Attribute = UGSAmmoAttributeSet::GetRifleReserveAmmoAttribute();
  s.PostGameplayEffectExecute(d);
  CHECK(s.GetRifleReserveAmmo() == 300);
  s.RifleReserveAmmo.SetCurrentValue(-5);
  s.PostGameplayEffectExecute(d);
  CHECK(s.GetRifleReserveAmmo() == 0);
  s.RocketReserveAmmo.SetCurrentValue(40);
  d.EvaluatedData.Attribute = UGSAmmoAttributeSet::GetRocketReserveAmmoAttribute();
  s.PostGameplayEffectExecute(d);
  CHECK(s.GetRocketReserveAmmo() == 30);
  s.RocketReserveAmmo.SetCurrentValue(7);                           // another attribute changed: the rocket ammo is left alone
  d.EvaluatedData.Attribute = UGSAmmoAttributeSet::GetRifleReserveAmmoAttribute();
  s.PostGameplayEffectExecute(d);
  CHECK(s.GetRocketReserveAmmo() == 7);
  FGameplayTag rifle = FGameplayTag::RequestGameplayTag(FName("Weapon.Ammo.Rifle")), none = FGameplayTag::RequestGameplayTag(FName("Weapon.Ammo.Pistol"));
  CHECK(UGSAmmoAttributeSet::GetReserveAmmoAttributeFromTag(rifle) == UGSAmmoAttributeSet::GetRifleReserveAmmoAttribute());
  CHECK(UGSAmmoAttributeSet::GetMaxReserveAmmoAttributeFromTag(rifle) == UGSAmmoAttributeSet::GetMaxRifleReserveAmmoAttribute());
  CHECK(!UGSAmmoAttributeSet::GetReserveAmmoAttributeFromTag(none).IsValid());
  printf(fails ? "ammo: %d FAILED\n" : "ammo: ALL PASSED\n", fails);
  return fails != 0;
}
