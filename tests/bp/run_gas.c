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

  // 8b. Overlap geometry is exact and honours yaw: capsule radius 34 vs a 96-half-extent box. At x=130 the capsule just touches
  //     the face; at 131 it misses; rotating the box 45 degrees brings its corner (96*sqrt2 = 135.8) over x=131.
  *iv(vol, "Location") = u64_vec(0, 0, 0);
  *iv(hero, "Location") = u64_vec(130, 0, 0); CHECK(u64_overlapping(vol, hit, 8) == 1);
  *iv(hero, "Location") = u64_vec(131, 0, 0); CHECK(u64_overlapping(vol, hit, 8) == 0);
  *iv(vol, "Rotation") = u64_vec(0, 45, 0);   CHECK(u64_overlapping(vol, hit, 8) == 1);
  *iv(vol, "Rotation") = u64_vec(0, 0, 0);

  // 8c. Character movement (UCharacterMovementComponent::CalcVelocity): MaxAcceleration 2048, braking 2048, friction 8.
  u64_obj *runner = u64_new("Runner", "BP_HeroCharacter");
  *iv(runner, "Location") = u64_vec(0, 5000, 0); u64_actor_init(runner);
  u64_v fwd = u64_vec(1, 0, 0), one = u64_float(1);
  for (int i = 0; i < 20; i++) { u64_call(runner, "AddMovementInput", 2, (u64_v[]){fwd, one}); u64_world_tick(0.05f); }
  CHECK(NEAR(iv(runner, "Velocity")->u.v[0], 600));                  // reaches MoveSpeed (GetMaxSpeed) and is clamped there
  float x1 = iv(runner, "Location")->u.v[0];
  CHECK(x1 > 300 && x1 < 600);                                        // accelerating from rest covers less than 600 in 1 s
  for (int i = 0; i < 20; i++) u64_world_tick(0.05f);                 // no input: braking + friction stop it
  CHECK(NEAR(iv(runner, "Velocity")->u.v[0], 0));
  *iv(runner, "bSprinting") = u64_bool(1);
  for (int i = 0; i < 30; i++) { u64_call(runner, "AddMovementInput", 2, (u64_v[]){fwd, one}); u64_world_tick(0.05f); }
  CHECK(NEAR(iv(runner, "Velocity")->u.v[0], 840));                  // sprint multiplier 1.4
  *iv(runner, "bSprinting") = u64_bool(0);

  // 8d. Jump: JumpZVelocity 420 and gravity 980 -> apex 420^2 / (2*980) = 90 cm, flight time 0.857 s; landing resets the jump.
  u64_obj *jumper = u64_new("Jumper", "BP_HeroCharacter");
  *iv(jumper, "Location") = u64_vec(0, 9000, 0); u64_actor_init(jumper);
  u64_call(jumper, "Jump", 0, 0);
  float apex = 0;
  for (int i = 0; i < 24; i++) { u64_world_tick(0.05f); if (iv(jumper, "Location")->u.v[2] > apex) apex = iv(jumper, "Location")->u.v[2]; }
  CHECK(apex > 88 && apex < 91.5f);
  CHECK(!u64_true(*iv(jumper, "bFalling")) && NEAR(iv(jumper, "Location")->u.v[2], 0));
  CHECK(u64_true(u64_call(jumper, "CanJump", 0, 0)));

  // 8e. Line traces (pawn capsules) and hit results: a capsule of radius 34 at x=200 is entered at x=166 with normal -X.
  u64_obj *target = u64_new("Target", "BP_RedMinion"); *iv(target, "Location") = u64_vec(200, 0, 0); u64_actor_init(target);
  u64_v hits = {0};
  u64_v tr = u64_call(hero, "LineTraceMultiForObjects", 9, (u64_v[]){u64_objv(hero), u64_vec(0, 0, 40), u64_vec(400, 0, 40),
              u64_array_of(1, (u64_v[]){u64_int(2)}), u64_bool(0), u64_array_of(0, 0), u64_int(0), u64_ref(&hits), u64_bool(1)});
  CHECK(u64_true(tr) && u64_array_len(hits) == 1);
  u64_v loc = {0}, nrm = {0}, tt = {0};
  u64_call(hero, "BreakHitResult", 17, (u64_v[]){u64_array_get(hits, 0), u64_ref(&(u64_v){0}), u64_ref(&(u64_v){0}), u64_ref(&tt), u64_ref(&(u64_v){0}),
              u64_ref(&loc), u64_ref(&(u64_v){0}), u64_ref(&nrm), u64_ref(&(u64_v){0}), u64_ref(&(u64_v){0}), u64_ref(&(u64_v){0}),
              u64_ref(&(u64_v){0}), u64_ref(&(u64_v){0}), u64_ref(&(u64_v){0}), u64_ref(&(u64_v){0}), u64_ref(&(u64_v){0}), u64_ref(&(u64_v){0})});
  CHECK(NEAR(loc.u.v[0], 166) && NEAR(nrm.u.v[0], -1) && NEAR(tt.u.f, 166.0f / 400.0f));
  u64_v miss = {0};
  tr = u64_call(hero, "LineTraceMultiForObjects", 9, (u64_v[]){u64_objv(hero), u64_vec(0, 100, 40), u64_vec(400, 100, 40),
              u64_array_of(1, (u64_v[]){u64_int(2)}), u64_bool(0), u64_array_of(0, 0), u64_int(0), u64_ref(&miss), u64_bool(1)});
  CHECK(!u64_true(tr));                                               // 100 to the side is outside radius 34

  // 8f. Montage notifies and the C++ fire-gun ability: the montage's GenericEventByTag notifies send gameplay events,
  //     UGDGA_FireGun::EventReceived spawns a projectile (InitialSpeed 7000) and EndAbility ends the ability at 0.039 s.
  u64_obj *shooter = u64_new("Shooter", "BP_HeroCharacter");
  *iv(shooter, "Location") = u64_vec(0, 20000, 0); *iv(shooter, "ControlRotation") = u64_vec(0, 90, 0);   // facing +Y
  u64_actor_init(shooter);
  CHECK(u64_input_press(shooter, "EGDAbilityInputID::Ability1"));
  run(0.06f);                                                                // before its lifespan (Range / InitialSpeed = 1000/7000 = 0.143 s) ends
  u64_obj *proj = 0;
  for (int i = 0; i < u64_obj_count(); i++) if (!strcmp(u64_obj_class(u64_obj_at(i)), "BP_GunProjectile") && u64_obj_alive(u64_obj_at(i))) proj = u64_obj_at(i);
  CHECK(proj != 0);
  CHECK(proj && iv(proj, "Velocity")->u.v[1] > 6900 && fabsf(iv(proj, "Velocity")->u.v[0]) < 400);   // flies along the camera forward (+Y)
  CHECK(!u64_true(*iv(u64_find("Shooter.GA_FireGun"), "Active")));  // Event.Montage.EndAbility ended it
  run(0.2f);
  CHECK(proj && !u64_obj_alive(proj));                                 // SetLifeSpan(Range / InitialSpeed) destroyed it

  // 8g. Game mode: the hero's death runs FinishDying -> HeroDied -> RespawnHero after RespawnDelay (5 s) at the PlayerStart.
  u64_obj *start = u64_new("PlayerStart", "PlayerStart"); *iv(start, "Location") = u64_vec(10, 20, 30);
  u64_obj *pl = u64_new("PlayerHero", "BP_HeroCharacter"); *iv(pl, "Location") = u64_vec(0, 30000, 0); *iv(pl, "IsPlayer") = u64_bool(1);
  u64_actor_init(pl);
  u64_v kill = spec_with_damage(u64_asc_of(pl), "GE_DamageVolume", 10000);
  u64_call(u64_asc_of(pl), "BP_ApplyGameplayEffectSpecToSelf", 1, &kill);
  run(1.7f);  CHECK(!u64_obj_alive(pl));                              // destroyed by its Die notify at 1.526 s
  run(2.0f);  CHECK(u64_player_pawn() == 0);                          // no pawn while waiting (spectator)
  run(3.0f);  CHECK(u64_player_pawn() != 0 && NEAR(iv(u64_player_pawn(), "Location")->u.v[0], 10) && u64_attr_get(u64_asc_of(u64_player_pawn()), "Health") > 99);

  // 8h. Hero input axes (AGDHeroCharacter::MoveForward uses the controller yaw): facing +Y, forward input moves along +Y.
  u64_obj *pawn = u64_player_pawn();
  *iv(pawn, "ControlRotation") = u64_vec(0, 90, 0);
  for (int i = 0; i < 10; i++) { u64_input_axis(pawn, "MoveForward", 1); u64_world_tick(0.05f); }
  CHECK(iv(pawn, "Velocity")->u.v[1] > 100 && fabsf(iv(pawn, "Velocity")->u.v[0]) < 1);

  // 8i. Conditional effects and attribute snapshots (runtime-registered effects; the project's assets use neither).
  {
    static const char *const child_only[] = {"GE_TestChild", 0}, *const needs_tag[] = {"State.Sprinting", 0};
    static const u64_modinfo hp_mod[] = {{"Mana", U64_OP_ADD, U64_CALC_SCALABLE, -30.0f, "", 0, 1, 0, 0, ""}, {0}};
    static const u64_ge_def child = {"GE_TestChild", U64_DUR_INSTANT, 0, 0, 1, hp_mod, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
    static const u64_ge_def parent = {"GE_TestParent", U64_DUR_INSTANT, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, child_only, needs_tag};
    u64_ge_register(&child); u64_ge_register(&parent);
    u64_obj *src = u64_new("CondSrc", "BP_HeroCharacter"); *iv(src, "Location") = u64_vec(0, 40000, 0); u64_actor_init(src);
    u64_obj *sa = u64_asc_of(src);
    float mana0 = u64_attr_get(sa, "Mana");
    u64_apply_ge(sa, sa, "GE_TestParent", 1, 0);                      // source lacks State.Sprinting: the conditional child is skipped
    CHECK(NEAR(u64_attr_get(sa, "Mana"), mana0));
    u64_apply_ge(sa, sa, "GE_StandardSprint", 1, 0);                  // grants State.Sprinting
    u64_apply_ge(sa, sa, "GE_TestParent", 1, 0);
    CHECK(NEAR(u64_attr_get(sa, "Mana"), mana0 - 30));                // now it applies

    // Snapshot: the spec captured the source's Damage (10) when made; later changes (99) do not affect it.
    u64_obj *tgt = u64_new("SnapTgt", "BP_HeroCharacter"); *iv(tgt, "Location") = u64_vec(0, 41000, 0); u64_actor_init(tgt);
    u64_obj *ta = u64_asc_of(tgt);
    u64_attr_set_base(sa, "Damage", 10);                                 // the source's Damage attribute when the spec is made
    u64_v sp = u64_call(sa, "MakeOutgoingSpec", 2, (u64_v[]){u64_cls("GE_DamageVolume_C"), u64_float(1)});
    u64_attr_set_base(sa, "Damage", 99);                                 // changes afterwards must not matter (Snapshot = true)
    u64_call(sa, "AssignTagSetByCallerMagnitude", 3, (u64_v[]){sp, u64_name("Data.Damage"), u64_float(5)});
    float h0 = u64_attr_get(ta, "Health");
    u64_call(ta, "BP_ApplyGameplayEffectSpecToSelf", 1, &sp);
    CHECK(NEAR(h0 - u64_attr_get(ta, "Health"), (10 + 5) * 100.0f / (100.0f + 15)));      // (snapshot 10 + SetByCaller 5) mitigated by Armor 15
  }

  // 8j. The whole shooting chain on the real Blueprints: input -> UGDGA_FireGun (C++) -> montage notify events -> projectile
  //     (7000 u/s, swept) -> BP overlap event -> trace -> damage spec -> GDDamageExecCalculation -> attribute set.
  u64_obj *gunner = u64_new("Gunner", "BP_HeroCharacter");
  *iv(gunner, "Location") = u64_vec(0, 60000, 0); *iv(gunner, "ControlRotation") = u64_vec(0, 90, 0); u64_actor_init(gunner);
  u64_obj *dummy2 = u64_new("ShotTarget", "BP_RedMinion"); *iv(dummy2, "Location") = u64_vec(0, 60400, 0); u64_actor_init(dummy2);
  u64_obj *da = u64_asc_of(dummy2);
  float armor = u64_attr_get(da, "Armor"), hp0 = u64_attr_get(da, "Health");
  u64_input_press(gunner, "EGDAbilityInputID::Ability1");
  run(0.6f);
  CHECK(NEAR(hp0 - u64_attr_get(da, "Health"), 12.0f * 100.0f / (100.0f + armor)));   // Damage 12 (GDGA_FireGun) mitigated by the target's Armor
  CHECK(NEAR(u64_attr_get(u64_asc_of(gunner), "Health"), 100));              // a projectile never damages its own instigator
  {                                                                           // the projectile Blueprint relies on GetInstigator/GetOwner
    u64_obj *pj = u64_new("InstTest", "BP_GunProjectile");
    *iv(pj, "Instigator") = u64_objv(gunner); *iv(pj, "Owner") = u64_objv(gunner);
    CHECK(u64_asobj(u64_call(pj, "GetInstigator", 0, 0)) == gunner);
    CHECK(u64_asobj(u64_call(pj, "GetOwner", 0, 0)) == gunner);
  }
  int live_proj = 0;
  for (int i = 0; i < u64_obj_count(); i++) if (!strcmp(u64_obj_class(u64_obj_at(i)), "BP_GunProjectile") && u64_obj_alive(u64_obj_at(i)) && iv(u64_obj_at(i), "Location")->u.v[1] > 59000) live_proj++;
  CHECK(live_proj == 0);                                                                // the projectile was consumed by the hit

  // 9. Out-parameters and text.
  u64_v x = {0}, y = {0}, z = {0};
  u64_call(hero, "BreakVector", 4, (u64_v[]){u64_vec(1, 2, 3), u64_ref(&x), u64_ref(&y), u64_ref(&z)});
  CHECK(x.u.f == 1 && y.u.f == 2 && z.u.f == 3);
  u64_v t = u64_call(hero, "Format", 3, (u64_v[]){u64_str("{0}/{1}"), u64_int(42), u64_int(100)});
  CHECK(!strcmp(t.u.s, "42/100"));

  CHECK(u64_rt_overflows == 0);                                        // no storage was exhausted (nothing aliased)
  printf("scenarios: %s (%d failure%s)\n", failures ? "FAILED" : "ALL PASSED", failures, failures == 1 ? "" : "s");
  return failures;
}

#ifndef U64_NO_MAIN
int main(void) { return u64_run_scenarios(1) != 0; }
#endif
