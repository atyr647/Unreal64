// GASShooter scenarios on the host. Every expected value is derived from the project's assets (GE_HeroAttributes, the weapon
// Blueprints' defaults) or its C++ (Source/GASShooter), cited per check.
#include "u64_gas_core.h"
#include <stdio.h>
#include <math.h>
const u64_bpfn *u64_bp_registry(void); int u64_bp_registry_count(void);
static int fails;
#define CHECK(c) do { if (c) printf("ok   %s\n", #c); else { printf("FAIL %s\n", #c); fails++; } } while (0)
static void tick(int n) { for (int i = 0; i < n; i++) u64_world_tick(1.0f / 30.0f); }
static u64_obj *hero;
static float attr(const char *n) { return u64_attr_get(u64_asc_of(hero), n); }
static const char *s(u64_v v) { return v.u.s ? v.u.s : ""; }
static u64_v call(u64_obj *o, const char *fn) { return u64_call(o, fn, 0, 0); }

int main(void)
{
  u64_gc_stress = 1;                       // collect after every tick: any dangling reference shows up as a failure
  u64_bp_table = u64_bp_registry(); u64_bp_count = u64_bp_registry_count();
  u64_level_load();
  u64_world_begin_play();
  u64_game_start();
  hero = u64_player_pawn();
  CHECK(hero && !strcmp(u64_obj_class(hero), "BP_HeroCharacter"));
  tick(3);                                   // PostInitializeComponents -> SetTimerForNextTick(SpawnDefaultInventory)
  u64_obj *asc = u64_asc_of(hero);
  // GE_HeroAttributes: MaxHealth/Mana/Stamina 100 raise Health/Mana/Stamina to 100 through AdjustAttributeForMaxChange (0 -> 100: delta = NewMax)
  CHECK(attr("MaxHealth") == 100 && attr("Health") == 100);
  CHECK(attr("MaxMana") == 100 && attr("Mana") == 100);
  CHECK(attr("MaxStamina") == 100 && attr("Stamina") == 100);
  CHECK(attr("MaxShield") == 100 && attr("Shield") == 100 && attr("Armor") == 20 && attr("MoveSpeed") == 600);
  CHECK(attr("MaxRifleReserveAmmo") == 300 && attr("RifleReserveAmmo") == 60);
  CHECK(attr("RocketReserveAmmo") == 24 && attr("ShotgunReserveAmmo") == 12);
  // SpawnDefaultInventory: DefaultInventoryWeaponClasses = [BP_Rifle], equipped (i == 0)
  CHECK(u64_as_int(call(hero, "GetNumWeapons")) == 1);
  u64_obj *rifle = call(hero, "GetCurrentWeapon").u.o;
  CHECK(rifle && !strcmp(u64_obj_class(rifle), "BP_Rifle"));
  CHECK(u64_as_int(call(hero, "GetPrimaryClipAmmo")) == 30 && u64_as_int(call(hero, "GetMaxPrimaryClipAmmo")) == 30);   // BP_Rifle: 30 / 30
  CHECK(u64_as_int(call(hero, "GetPrimaryReserveAmmo")) == 60);                      // Weapon.Ammo.Rifle -> RifleReserveAmmo
  CHECK(u64_tag_has(asc, "Weapon.Equipped.Rifle") && !u64_tag_has(asc, "Weapon.Equipped.None") == 1);   // CurrentWeaponTag = WeaponTag, loose
  // 7 CharacterAbilities (BP_HeroCharacter) + 5 weapon Abilities (BP_Rifle)
  CHECK(u64_asc_ability_count(asc) == 12);
  CHECK(u64_class_is(u64_obj_class(hero), "GSCharacterBase") && u64_true(call(hero, "IsAlive")));
  // PreAttributeChange: MoveSpeed is clamped to [150, 1000]
  {                                                                                // an Additive +5000 instant modifier: 5600 is clamped to 1000
    static const char *const none0[] = {0}; static u64_modinfo ms[1];
    static u64_ge_def spd = {.name = "TestSpeed", .dur = U64_DUR_INSTANT, .nmods = 1, .mods = ms, .execs = none0, .granted = none0, .asset_tags = none0,
      .remove_tags = none0, .app_require = none0, .app_ignore = none0, .ongoing_require = none0, .ongoing_ignore = none0};
    ms[0] = (u64_modinfo){"MoveSpeed", U64_OP_ADD, U64_CALC_SCALABLE, 5000.0f, "", 0, 1.0f, 0.0f, 0.0f, ""};
    u64_ge_register(&spd); u64_apply_ge(asc, asc, "TestSpeed", 1, 0);
    CHECK(attr("MoveSpeed") == 1000);
    ms[0].value = -5000.0f; u64_apply_ge(asc, asc, "TestSpeed", 1, 0);              // 1000 - 5000 -> clamped up to 150
    CHECK(attr("MoveSpeed") == 150);
    u64_attr_set_base(asc, "MoveSpeed", 600);
  }

  // --- inventory (AGSHeroCharacter::AddWeaponToInventory / NextWeapon / PreviousWeapon)
  u64_obj *shotgun = u64_spawn_actor("BP_Shotgun", u64_vec(0, 0, 0)), *rocket = u64_spawn_actor("BP_RocketLauncher", u64_vec(0, 0, 0));
  u64_actor_init(shotgun); u64_actor_init(rocket);
  CHECK(u64_true(u64_call(hero, "AddWeaponToInventory", 2, (u64_v[]){u64_objv(shotgun), u64_bool(0)})));
  CHECK(u64_true(u64_call(hero, "AddWeaponToInventory", 2, (u64_v[]){u64_objv(rocket), u64_bool(0)})));
  CHECK(u64_as_int(call(hero, "GetNumWeapons")) == 3 && call(hero, "GetCurrentWeapon").u.o == rifle);   // equip=false keeps the rifle
  CHECK(u64_asc_ability_count(asc) == 12 + 5 + 4);                                  // + Shotgun (5) + RocketLauncher (4) abilities
  call(hero, "NextWeapon");
  CHECK(call(hero, "GetCurrentWeapon").u.o == shotgun && u64_tag_has(asc, "Weapon.Equipped.Shotgun") && !u64_tag_has(asc, "Weapon.Equipped.Rifle"));
  CHECK(u64_as_int(call(hero, "GetPrimaryClipAmmo")) == 6 && u64_as_int(call(hero, "GetPrimaryReserveAmmo")) == 12);   // BP_Shotgun 6; Shotgun reserve 12
  call(hero, "NextWeapon"); call(hero, "NextWeapon");
  CHECK(call(hero, "GetCurrentWeapon").u.o == rifle);                               // (index + 1) % 3 wraps
  call(hero, "PreviousWeapon");
  CHECK(call(hero, "GetCurrentWeapon").u.o == rocket && u64_as_int(call(hero, "GetPrimaryClipAmmo")) == 3);   // (0 - 1 + 3) % 3 = 2
  // picking up a second rifle: DoesWeaponExistInInventory -> its clip ammo goes to the reserve (Additive), the pickup is destroyed
  u64_obj *rifle2 = u64_spawn_actor("BP_Rifle", u64_vec(0, 0, 0)); u64_actor_init(rifle2);
  CHECK(!u64_true(u64_call(hero, "AddWeaponToInventory", 2, (u64_v[]){u64_objv(rifle2), u64_bool(1)})));
  CHECK(attr("RifleReserveAmmo") == 90 && !u64_obj_alive(rifle2) && u64_as_int(call(hero, "GetNumWeapons")) == 3);   // 60 + 30
  // reserve ammo is clamped by UGSAmmoAttributeSet::PostGameplayEffectExecute to [0, MaxRifleReserveAmmo]
  for (int i = 0; i < 12; i++) { u64_obj *r = u64_spawn_actor("BP_Rifle", u64_vec(0, 0, 0)); u64_actor_init(r); u64_call(hero, "AddWeaponToInventory", 2, (u64_v[]){u64_objv(r), u64_bool(0)}); }
  CHECK(attr("RifleReserveAmmo") == 300);                                           // 90 + 12 * 30 = 450 -> 300
  u64_attr_set_base(asc, "RifleReserveAmmo", 60);

  // --- damage: UGSAttributeSetBase::PostGameplayEffectExecute (shield absorbs first)
  static const char *const none[] = {0};
  static u64_modinfo dm[1]; static u64_ge_def dmg = {.name = "TestDamage", .dur = U64_DUR_INSTANT, .nmods = 1, .mods = dm, .execs = none, .granted = none,
    .asset_tags = none, .remove_tags = none, .app_require = none, .app_ignore = none, .ongoing_require = none, .ongoing_ignore = none};
  dm[0] = (u64_modinfo){"Damage", U64_OP_ADD, U64_CALC_SCALABLE, 30.0f, "", 0, 1.0f, 0.0f, 0.0f, ""};
  u64_ge_register(&dmg);
  u64_apply_ge(asc, asc, "TestDamage", 1, 0);
  CHECK(attr("Shield") == 70 && attr("Health") == 100 && attr("Damage") == 0);       // 30 < shield: shield 100 -> 70, health untouched
  dm[0].value = 90.0f; u64_apply_ge(asc, asc, "TestDamage", 1, 0);
  CHECK(attr("Shield") == 0 && attr("Health") == 80);                                // 90 - 70 shield = 20 to health
  // health -> 0: AGSPlayerState::HealthChanged -> KnockDown (not yet knocked down): health and shield reset, KnockedDown effect applied
  dm[0].value = 500.0f; u64_apply_ge(asc, asc, "TestDamage", 1, 0);
  CHECK(u64_tag_has(asc, "State.KnockedDown") && attr("Health") == 100 && attr("Shield") == 0 && u64_obj_alive(hero));
  // knocked down and hit again: FinishDying -> weapons dropped, hero destroyed, respawn scheduled (RespawnDelay 5 s)
  u64_v hl = *u64_ivar(hero, "Location");
  dm[0].value = 500.0f; u64_apply_ge(asc, asc, "TestDamage", 1, 0);
  CHECK(!u64_obj_alive(hero));
  // RemoveAllWeaponsFromInventory drops weapon i at hero + 50 * (cos(i/N * 2pi), sin(i/N * 2pi)) with N = 3: rifle 0, shotgun 1, rocket 2
  CHECK(!u64_true(*u64_ivar(rifle, "OwningCharacter")) && u64_obj_alive(rifle));
  CHECK(fabsf(u64_ivar(rifle, "Location")->u.v[0] - (hl.u.v[0] + 50)) < 1e-3f && fabsf(u64_ivar(rifle, "Location")->u.v[1] - hl.u.v[1]) < 1e-3f);
  CHECK(fabsf(u64_ivar(shotgun, "Location")->u.v[0] - (hl.u.v[0] - 25)) < 1e-3f && fabsf(u64_ivar(shotgun, "Location")->u.v[1] - (hl.u.v[1] + 43.30127f)) < 1e-3f);
  CHECK(fabsf(u64_ivar(rocket, "Location")->u.v[0] - (hl.u.v[0] - 25)) < 1e-3f && fabsf(u64_ivar(rocket, "Location")->u.v[1] - (hl.u.v[1] - 43.30127f)) < 1e-3f);
  tick(4 * 30);
  CHECK(u64_player_pawn() == 0);                                                    // still waiting at 4 s
  tick(2 * 30);
  CHECK(u64_player_pawn() && u64_player_pawn() != hero && u64_obj_alive(u64_player_pawn()));     // respawned after 5 s
  hero = u64_player_pawn(); tick(3);
  CHECK(attr("Health") == 100 && attr("Shield") == 100 && u64_as_int(call(hero, "GetNumWeapons")) == 1);   // a fresh hero with the default inventory
  // --- garbage collection: 3000 spawn/destroy cycles (plus arrays and temporaries) would exhaust the 1024-object pool without it
  {
    int live0 = u64_gc_live();
    for (int i = 0; i < 3000; i++) {
      u64_obj *tmp = u64_spawn_actor("BP_Shotgun", u64_vec(0, 0, 0));
      *u64_ivar(tmp, "Junk") = u64_array_of(8, (u64_v[]){u64_int(1), u64_int(2), u64_int(3), u64_int(4), u64_int(5), u64_int(6), u64_int(7), u64_int(8)});
      u64_obj_destroy(tmp);
      if (i % 10 == 0) tick(1);
    }
    tick(2);
    printf("live0=%d live=%d overflows=%d\n", live0, u64_gc_live(), u64_rt_overflows);
    CHECK(u64_gc_live() < live0 + 80 && u64_rt_overflows == 0);
  }
  CHECK(u64_rt_overflows == 0);
  printf(fails ? "shooter: %d FAILED\n" : "shooter: ALL PASSED\n", fails);
  return fails != 0;
}
