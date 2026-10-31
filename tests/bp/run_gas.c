// Scenario tests of the ported Gameplay Ability System, driven through the project's own compiled Blueprints and data.
// Expected numbers come from the project's assets and C++ (comments show the derivation).
#include "u64_gas_core.h"
#include <math.h>
#include <stdio.h>
#include <stdarg.h>
const u64_bpfn *u64_bp_registry(void); int u64_bp_registry_count(void);
static int failures, verbose = 1, dash_calls;
static void logf_(const char *f, ...)
{
  va_list a; va_start(a, f);
  if (!strncmp(f, "[bp]", 4) && !strcmp(va_arg(a, const char *), "GA_Dash_BP")) dash_calls++;
  va_end(a);
}
#define CHECK(c) do { if (!(c)) { printf("FAIL line %d: %s\n", __LINE__, #c); failures++; } else if (verbose) printf("ok   %s\n", #c); } while (0)
#define NEAR(a, b) (fabsf((a) - (b)) < 0.01f)
static void run(float secs) { for (float t = 0; t < secs - 1e-4f; t += 0.05f) u64_world_tick(0.05f); }
static u64_v *iv(u64_obj *o, const char *n) { return u64_ivar(o, n); }
static const u64_ge_def *ge(const char *n) { for (int i = 0; i < u64_ge_count; i++) if (!strcmp(u64_ge_defs[i].name, n)) return &u64_ge_defs[i]; return 0; }
static u64_obj *give(u64_obj *hero, const char *cls) { char b[64]; snprintf(b, sizeof b, "%s.%s", u64_obj_name(hero), cls); return u64_find(b); }
static u64_v spec_with_damage(u64_obj *asc, const char *ge_name, float dmg)
{
  u64_v spec = u64_call(asc, "MakeOutgoingSpec", 2, (u64_v[]){u64_cls(ge_name), u64_float(1)});
  u64_call(asc, "AssignTagSetByCallerMagnitude", 3, (u64_v[]){spec, u64_name("Data.Damage"), u64_float(dmg)});
  return spec;
}

int u64_run_scenarios(int verbose_)
{
  verbose = verbose_; failures = 0;
  u64_bp_table = u64_bp_registry(); u64_bp_count = u64_bp_registry_count(); u64_log = logf_;

  // 1. Initialisation (InitializeAttributes -> AddStartupEffects -> AddCharacterAbilities).
  //    GE_HeroAttributes overrides MaxHealth 100, Armor 15, MoveSpeed 600, StaminaRegenRate 2. Health is not set by the
  //    effect: it reaches 100 through AdjustAttributeForMaxChange when MaxHealth goes 0 -> 100.
  u64_obj *hero = u64_new("Hero", "BP_HeroCharacter");
  u64_actor_init(hero);
  u64_obj *asc = u64_asc_of(hero);
  CHECK(NEAR(u64_attr_get(asc, "MaxHealth"), 100) && NEAR(u64_attr_get(asc, "Health"), 100));
  CHECK(NEAR(u64_attr_get(asc, "Armor"), 15) && NEAR(u64_attr_get(asc, "MoveSpeed"), 600));
  CHECK(NEAR(u64_attr_get(asc, "StaminaRegenRate"), 2));
  CHECK(u64_asc_ability_count(asc) == 7);                          // BP_HeroCharacter.CharacterAbilities has 7 entries

  // 2. Damage execution calculation: Armor 15 -> 25 * 100/115 = 21.739. Then Health regenerates by HealthRegenRate (1)
  //    every 1.0 s (GE_HealthRegen, infinite, periodic).
  u64_v spec = spec_with_damage(asc, "GE_DamageVolume", 25);
  u64_call(asc, "BP_ApplyGameplayEffectSpecToSelf", 1, &spec);
  CHECK(NEAR(u64_attr_get(asc, "Health"), 100 - 25.0f * 100.0f / 115.0f));
  float h0 = u64_attr_get(asc, "Health");
  run(3.0f);
  CHECK(NEAR(u64_attr_get(asc, "Health"), h0 + 3));
  CHECK(NEAR(u64_attr_get(asc, "Damage"), 0));                    // the meta attribute is consumed

  // 3. Stacking: GE_PassiveArmor (+10 Armor, stack limit 4, aggregate by target) applied 6 times stacks to 4.
  for (int i = 0; i < 6; i++) u64_apply_ge(asc, asc, "GE_PassiveArmor", 1, 0);
  CHECK(NEAR(u64_attr_get(asc, "Armor"), 15 + 10 * 4));
  CHECK(u64_tag_has(asc, "Effect.Hero.PassiveArmor"));
  u64_remove_ge_class(asc, "GE_PassiveArmor");
  CHECK(NEAR(u64_attr_get(asc, "Armor"), 15) && !u64_tag_has(asc, "Effect.Hero.PassiveArmor"));

  // 4. Sprint ability through input: activation owns State.Sprinting, StartSprinting raises max speed to 600*1.4,
  //    releasing input ends it (K2_OnEndAbility -> StopSprinting).
  *iv(hero, "Acceleration") = u64_vec(1, 0, 0);
  CHECK(NEAR(u64_max_speed(hero), 600));
  CHECK(u64_input_press(hero, "EGDAbilityInputID::Sprint"));
  run(0.3f);
  CHECK(u64_true(*iv(hero, "bSprinting")) && NEAR(u64_max_speed(hero), 840));
  CHECK(u64_tag_has(asc, "State.Sprinting"));
  u64_input_release_id(hero, "EGDAbilityInputID::Sprint");
  run(0.3f);
  CHECK(!u64_true(*iv(hero, "bSprinting")) && NEAR(u64_max_speed(hero), 600));
  CHECK(!u64_tag_has(asc, "State.Sprinting"));

  // 5. Cost and blocking: Dash costs GE_DashCost stamina; it is refused when stamina is too low.
  float stam = u64_attr_get(asc, "Stamina");
  float dash_cost = ge("GE_DashCost")->mods[0].value;                 // -15 in the asset; the Blueprint calls CommitAbility
  CHECK(NEAR(dash_cost, -15));
  CHECK(u64_input_press(hero, "EGDAbilityInputID::Ability3"));
  CHECK(NEAR(u64_attr_get(asc, "Stamina"), stam + dash_cost));        // applied immediately, before any regeneration
  // The root-motion task ends Dash after its duration; the montage task is cancelled with the ability.
  run(0.4f);
  dash_calls = 0; run(2.0f);
  CHECK(dash_calls == 0);

  // 6. Cooldown: GE_MeteorCooldown grants Cooldown.Skill.Ability5 for 5 s; the ability cannot activate until it expires.
  u64_apply_ge(asc, asc, "GE_MeteorCooldown", 1, 0);
  CHECK(u64_tag_has(asc, "Cooldown.Skill.Ability5"));
  CHECK(!u64_input_press(hero, "EGDAbilityInputID::Ability5"));
  run(5.2f);
  CHECK(!u64_tag_has(asc, "Cooldown.Skill.Ability5"));

  // 7. Death: lethal damage runs Die(): OnCharacterDied is broadcast, State.Dead is added, and with a death montage the
  //    actor is destroyed only when the montage ends (FinishDying).
  u64_obj *dummy = u64_new("Dummy", "BP_HeroCharacter");
  u64_actor_init(dummy);
  u64_obj *dasc = u64_asc_of(dummy);
  *iv(dummy, "DeathMontage") = u64_cls("Death_3_Montage");
  u64_v lethal = spec_with_damage(dasc, "GE_DamageVolume", 1000);
  u64_call(dasc, "BP_ApplyGameplayEffectSpecToSelf", 1, &lethal);
  CHECK(NEAR(u64_attr_get(dasc, "Health"), 0));
  CHECK(u64_tag_has(dasc, "State.Dead"));
  CHECK(u64_obj_alive(dummy));
  run(1.0f);   CHECK(u64_obj_alive(dummy));                           // Death_3_Montage is 1.9 s long
  CHECK(NEAR(u64_attr_get(dasc, "Health"), 0));                        // GE_HealthRegen is inhibited while State.Dead
  run(1.2f);   CHECK(!u64_obj_alive(dummy));
  CHECK(!u64_input_press(dummy, "EGDAbilityInputID::Sprint"));        // dead actors cannot activate abilities

  // 8. Overlap queries use the real component shapes: BP_DamageVolume's box is (96, 96, 160) half extents.
  u64_obj *vol = u64_new("Vol", "BP_DamageVolume");
  *iv(vol, "Location") = u64_vec(0, 0, 0);
  *iv(hero, "Location") = u64_vec(80, 0, 0);                          // inside the box (96 + capsule radius 34)
  u64_obj *hit[8];
  CHECK(u64_overlapping(vol, hit, 8) == 1 && hit[0] == hero);
  *iv(hero, "Location") = u64_vec(300, 0, 0);
  CHECK(u64_overlapping(vol, hit, 8) == 0);

  // 9. Out-parameters and text.
  u64_v x = {0}, y = {0}, z = {0};
  u64_call(hero, "BreakVector", 4, (u64_v[]){u64_vec(1, 2, 3), u64_ref(&x), u64_ref(&y), u64_ref(&z)});
  CHECK(x.u.f == 1 && y.u.f == 2 && z.u.f == 3);
  u64_v t = u64_call(hero, "Format", 3, (u64_v[]){u64_str("{0}/{1}"), u64_int(42), u64_int(100)});
  CHECK(!strcmp(t.u.s, "42/100"));

  printf("scenarios: %s (%d failure%s)\n", failures ? "FAILED" : "ALL PASSED", failures, failures == 1 ? "" : "s");
  return failures;
}

#ifndef U64_NO_MAIN
int main(void) { return u64_run_scenarios(1) != 0; }
#endif
