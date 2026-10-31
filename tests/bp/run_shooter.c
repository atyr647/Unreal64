// GASShooter scenarios on the host. Every expected value is derived from the project's assets (GE_HeroAttributes, the weapon
// Blueprints' defaults) or its C++ (Source/GASShooter), cited per check.
#include "u64_gas_core.h"
#include <stdio.h>
#include <math.h>
#include <stdarg.h>
const u64_bpfn *u64_bp_registry(void); int u64_bp_registry_count(void);
static int fails;
static int verbose_ = 1;
#define CHECK(c) do { if (c) { if (verbose_) printf("ok   %s\n", #c); } else { printf("FAIL line %d: %s\n", __LINE__, #c); fails++; } } while (0)
static void tick(int n) { for (int i = 0; i < n; i++) u64_world_tick(1.0f / 30.0f); }
static void err_log(const char *f, ...) { va_list a; va_start(a, f); if (strstr(f, "error")) { vprintf(f, a); putchar('\n'); } va_end(a); }
static u64_obj *hero;
static float attr(const char *n) { return u64_attr_get(u64_asc_of(hero), n); }
static const char *s(u64_v v) { return v.u.s ? v.u.s : ""; }
static u64_obj *asc_of_hero(void) { return u64_asc_of(hero); }
static u64_v call(u64_obj *o, const char *fn) { return u64_call(o, fn, 0, 0); }

int u64_run_shooter_scenarios(int verbose)
{
  verbose_ = verbose;
  u64_gc_stress = 1; u64_log = err_log;                       // collect after every tick: any dangling reference shows up as a failure
  u64_bp_table = u64_bp_registry(); u64_bp_count = u64_bp_registry_count();
  u64_level_load();
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
  // The new hero starts among the weapons its previous life dropped (ring of radius 50 around the death spot, pickup capsule 40 + hero 34):
  // it picks them up on touch: the dropped rifle is a duplicate (its 30 rounds go to the reserve), the shotgun and rocket launcher join.
  CHECK(attr("Health") == 100 && attr("Shield") == 100 && u64_as_int(call(hero, "GetNumWeapons")) == 3);
  CHECK(attr("RifleReserveAmmo") == 90);                                              // 60 + 30
  for (int i = 0; i < 3 && strcmp(u64_obj_class(call(hero, "GetCurrentWeapon").u.o), "BP_Rifle"); i++) call(hero, "NextWeapon");
  CHECK(!strcmp(u64_obj_class(call(hero, "GetCurrentWeapon").u.o), "BP_Rifle"));
  rifle = call(hero, "GetCurrentWeapon").u.o;
  // --- firing the rifle (GA_RiflePrimary -> GA_RiflePrimaryInstant -> GSGATA_LineTrace -> GE_RifleDamage)
  {
    u64_obj *enemy = u64_find("BP_HeroCharacter2");
    tick(3);
    u64_obj *ehero = hero;
    hero = enemy; float en_shield0 = attr("Shield"), en_health0 = attr("Health"); hero = ehero;
    CHECK(en_shield0 == 100 && en_health0 == 100 && attr("Armor") == 20);
    // aim from the first-person camera at the enemy's capsule centre: control rotation = direction camera -> target
    call(hero, "GetCurrentWeapon");
    u64_v loc = *u64_ivar(hero, "Location"), el = *u64_ivar(enemy, "Location");
    u64_obj *cam = u64_comp(hero, "FirstPersonCamera").u.o;
    u64_v rel = cam ? *u64_ivar(cam, "RelativeLocation") : u64_vec(0, 0, 0);
    float cx = loc.u.v[0] + rel.u.v[0], cy = loc.u.v[1] + rel.u.v[1], cz = loc.u.v[2] + rel.u.v[2];
    float dx = el.u.v[0] - cx, dy = el.u.v[1] - cy, dz = el.u.v[2] - cz;
    *u64_ivar(hero, "ControlRotation") = u64_vec(atan2f(dz, sqrtf(dx * dx + dy * dy)) * 57.29578f, atan2f(dy, dx) * 57.29578f, 0);
    int clip0 = u64_as_int(call(hero, "GetPrimaryClipAmmo"));
    CHECK(u64_input_press(hero, "EGSAbilityInputID::PrimaryFire"));
    for (int i = 0; i < 30 && u64_as_int(call(hero, "GetPrimaryClipAmmo")) == clip0; i++) tick(1);       // until the first round is fired
    u64_input_release_id(hero, "EGSAbilityInputID::PrimaryFire");
    tick(20);
    CHECK(u64_as_int(call(hero, "GetPrimaryClipAmmo")) == clip0 - 1);                // AmmoCost 1 per shot, one shot (released at once)
    hero = enemy;
    // BulletDamage 10 (set by caller) * 100 / (100 + Armor 20) = 8.3333 to the shield first (100 -> 91.6667), health untouched
    CHECK(fabsf(attr("Shield") - (100 - 10.0f * 100.0f / 120.0f)) < 0.01f && attr("Health") == 100);
    hero = ehero;
    // --- reload (GA_RifleReload): the clip is topped up from the reserve; clip + reserve is conserved
    int clip1 = u64_as_int(call(hero, "GetPrimaryClipAmmo")), res1 = u64_as_int(call(hero, "GetPrimaryReserveAmmo"));
    CHECK(clip1 == 29 && res1 == 90);
    CHECK(u64_input_press(hero, "EGSAbilityInputID::Reload"));
    tick(150);                                                                          // the reload montage plays out
    int clip2 = u64_as_int(call(hero, "GetPrimaryClipAmmo")), res2 = u64_as_int(call(hero, "GetPrimaryReserveAmmo"));
    CHECK(clip2 == 30 && clip2 + res2 == clip1 + res1);
  }
  // --- shotgun (GA_ShotgunPrimaryInstant): 8 pellets at 6 damage each, 12 degrees of spread
  {
    u64_obj *enemy = u64_find("BP_HeroCharacter2"), *me = hero;
    u64_obj *sg = 0;                                                                                       // the shotgun picked up above
    for (int i = 0; i < u64_as_int(call(me, "GetNumWeapons")); i++) { call(me, "NextWeapon"); if (!strcmp(u64_obj_class(call(me, "GetCurrentWeapon").u.o), "BP_Shotgun")) { sg = call(me, "GetCurrentWeapon").u.o; break; } }
    CHECK(sg && call(me, "GetCurrentWeapon").u.o == sg && u64_as_int(call(me, "GetPrimaryClipAmmo")) == 6);
    u64_v el = *u64_ivar(enemy, "Location");
    *u64_ivar(me, "Location") = u64_vec(el.u.v[0] - 120.0f, el.u.v[1], 112.0f);                           // 120 units in front: the whole cone (radius 12.6) is inside the capsule
    u64_v cam = *u64_ivar(u64_comp(me, "FirstPersonCamera").u.o, "RelativeLocation");
    float cx = el.u.v[0] - 120.0f + cam.u.v[0], cy = el.u.v[1] + cam.u.v[1], cz = 112.0f + cam.u.v[2];
    float dx = el.u.v[0] - cx, dy = el.u.v[1] - cy, dz = el.u.v[2] - cz;
    *u64_ivar(me, "ControlRotation") = u64_vec(atan2f(dz, sqrtf(dx * dx + dy * dy)) * 57.29578f, atan2f(dy, dx) * 57.29578f, 0);
    tick(60);                                                                                              // let the weapon-change block and the enemy's shield settle
    hero = enemy; float sh0 = attr("Shield"); hero = me;
    CHECK(u64_input_press(me, "EGSAbilityInputID::PrimaryFire"));
    for (int i = 0; i < 30 && u64_as_int(call(me, "GetPrimaryClipAmmo")) == 6; i++) tick(1);
    u64_input_release_id(me, "EGSAbilityInputID::PrimaryFire");
    tick(30);
    hero = enemy;
    CHECK(fabsf((sh0 - attr("Shield")) - 8 * (6.0f * 100.0f / 120.0f)) < 0.01f);                         // 8 pellets * 6 * 100/120
    hero = me;
  }
  // --- aim down sights (GA_RifleSecondary): GE_RifleAiming and a FOV change to Targeting1PFOV (60) in 0.1 s; releasing restores Default1PFOV (90) in 0.05 s
  {
    u64_obj *me = hero;
    for (int i = 0; i < u64_as_int(call(me, "GetNumWeapons")) && strcmp(u64_obj_class(call(me, "GetCurrentWeapon").u.o), "BP_Rifle"); i++) call(me, "NextWeapon");
    tick(60);
    u64_obj *cam = u64_comp(me, "FirstPersonCamera").u.o;
    CHECK(!u64_tag_has(u64_asc_of(me), "Weapon.Rifle.Aiming"));
    CHECK(u64_input_press(me, "EGSAbilityInputID::SecondaryFire"));
    tick(1);
    float fov_mid = u64_ivar(cam, "FieldOfView")->u.f;
    CHECK(fov_mid < 90 && fov_mid > 60);                                              // mid-transition
    tick(5);
    CHECK(fabsf(u64_ivar(cam, "FieldOfView")->u.f - 60.0f) < 1e-4f && u64_tag_has(u64_asc_of(me), "Weapon.Rifle.Aiming"));
    u64_input_release_id(me, "EGSAbilityInputID::SecondaryFire");
    tick(4);
    CHECK(fabsf(u64_ivar(cam, "FieldOfView")->u.f - 90.0f) < 1e-4f);
    tick(30);
    CHECK(u64_tag_count(u64_asc_of(me), "Weapon.Rifle.Aiming") <= u64_tag_count(u64_asc_of(me), "Weapon.Rifle.AimingRemoval"));   // spread logic: aiming only while Aiming > AimingRemoval
  }
  // --- movement speed (UGSCharacterMovementComponent::GetMaxSpeed): MoveSpeed 600; sprinting x1.4, aiming down sights x0.8
  {
    u64_obj *me = hero;
    CHECK(u64_max_speed(me) == 600.0f);
    CHECK(u64_input_press(me, "EGSAbilityInputID::Sprint"));
    tick(3);
    CHECK(u64_tag_has(u64_asc_of(me), "State.Sprinting") && fabsf(u64_max_speed(me) - 600.0f * 1.4f) < 0.01f);
    u64_input_release_id(me, "EGSAbilityInputID::Sprint");
    tick(10);
    CHECK(u64_max_speed(me) == 600.0f && !u64_tag_has(u64_asc_of(me), "State.Sprinting"));
    CHECK(u64_input_press(me, "EGSAbilityInputID::SecondaryFire"));
    tick(10);
    CHECK(fabsf(u64_max_speed(me) - 600.0f * 0.8f) < 0.01f);
    u64_input_release_id(me, "EGSAbilityInputID::SecondaryFire");
    tick(10);
    CHECK(u64_max_speed(me) == 600.0f);
  }
  // --- rocket launcher (GA_RocketLauncherPrimaryInstant): a projectile, then an explosion (radius 200) of RocketDamage 60 -> 60 * 100/120 = 50
  {
    u64_obj *enemy = u64_find("BP_HeroCharacter2"), *me = hero;
    for (int i = 0; i < u64_as_int(call(me, "GetNumWeapons")) && strcmp(u64_obj_class(call(me, "GetCurrentWeapon").u.o), "BP_RocketLauncher"); i++) call(me, "NextWeapon");
    CHECK(!strcmp(u64_obj_class(call(me, "GetCurrentWeapon").u.o), "BP_RocketLauncher") && u64_as_int(call(me, "GetPrimaryClipAmmo")) == 3);
    u64_v el = *u64_ivar(enemy, "Location");
    *u64_ivar(me, "Location") = u64_vec(el.u.v[0] - 500.0f, el.u.v[1], 112.0f);
    u64_v cam = *u64_ivar(u64_comp(me, "FirstPersonCamera").u.o, "RelativeLocation");
    float cx = el.u.v[0] - 500.0f + cam.u.v[0], cy = el.u.v[1] + cam.u.v[1], cz = 112.0f + cam.u.v[2];
    float dx = el.u.v[0] - cx, dy = el.u.v[1] - cy, dz = el.u.v[2] - cz;
    *u64_ivar(me, "ControlRotation") = u64_vec(atan2f(dz, sqrtf(dx * dx + dy * dy)) * 57.29578f, atan2f(dy, dx) * 57.29578f, 0);
    tick(60);
    hero = enemy; float sh0 = attr("Shield"); hero = me;
    CHECK(u64_input_press(me, "EGSAbilityInputID::PrimaryFire"));
    for (int i = 0; i < 30 && u64_as_int(call(me, "GetPrimaryClipAmmo")) == 3; i++) tick(1);
    u64_input_release_id(me, "EGSAbilityInputID::PrimaryFire");
    tick(60);
    CHECK(u64_as_int(call(me, "GetPrimaryClipAmmo")) == 2);
    hero = enemy;
    CHECK(fabsf((sh0 - attr("Shield")) - 60.0f * 100.0f / 120.0f) < 0.01f);
    hero = me;
  }
  // --- pickups (AGSPickup): standing on one applies its effects; it respawns after RespawnTime (5 s) and, with the hero still on it, is taken again
  {
    u64_obj *hp = u64_find("BP_Pickup_Health_2"), *ap = u64_find("BP_Pickup_AmmoRifle_2");
    u64_v pl = *u64_ivar(hp, "Location");
    u64_remove_ge_class(asc_of_hero(), "GE_HealthRegen");                                   // so the health changes are only the pickup's
    u64_attr_set_base(asc_of_hero(), "Health", 50.0f);
    *u64_ivar(hero, "Location") = u64_vec(pl.u.v[0], pl.u.v[1], 112.0f);
    tick(2);
    CHECK(attr("Health") == 70 && !u64_true(*u64_ivar(hp, "bIsActive")));                 // GE_Pickup_Health: +20; the pickup is inactive
    tick(4 * 30);
    CHECK(attr("Health") == 70);                                                          // 4.07 s after the pickup: still waiting
    tick(1 * 30);
    CHECK(attr("Health") == 90 && !u64_true(*u64_ivar(hp, "bIsActive")));                 // respawned at 5 s and taken again by the hero standing on it
    u64_v al = *u64_ivar(ap, "Location");
    u64_attr_set_base(asc_of_hero(), "RifleReserveAmmo", 100.0f);
    *u64_ivar(hero, "Location") = u64_vec(al.u.v[0], al.u.v[1], 112.0f);
    tick(2);
    CHECK(attr("RifleReserveAmmo") == 130);                                               // GE_Pickup_AmmoRifle: +30
  }
  // --- interaction: GA_InteractPassive traces (InteractTrace profile: overlaps everything, so the first component hit decides) and finds the
  // chest's lock (the only component overlapping "Interactable"); Interact starts GA_InteractActive (a GameplayEvent trigger), which holds for
  // BP_Chest.OpenInteractionDuration (3 s), then PostInteract applies GE_ChestOpen (5 s, grants Object.Chest.State.Open) and spawns the loot
  {
    hero = u64_player_pawn();                                                              // the earlier scenarios may have respawned the hero
    u64_obj *ch = u64_find("BP_Chest_2"); float lx[3]; u64_comp_world(ch, "SM_Forge_Chest_Lock", lx);
    CHECK(fabsf(lx[0] - 393) < 0.01f && fabsf(lx[1] + 355) < 0.01f && fabsf(lx[2] - 47) < 0.01f);                                  // chest at (438,-355,20) yaw -180: the lock is at +45 in the chest's x
    int cnt_rocket0 = 0, cnt_hp0 = 0;
    for (int i = 0; i < u64_obj_count(); i++) { u64_obj *o = u64_obj_at(i); if (!u64_obj_is_actor(o)) continue; if (!strcmp(u64_obj_class(o), "BP_RocketLauncher")) cnt_rocket0++; if (!strcmp(u64_obj_class(o), "BP_Pickup_Health")) cnt_hp0++; }
    *u64_ivar(hero, "Location") = u64_vec(lx[0] - 150, lx[1], 112);
    u64_obj *cam = u64_comp(hero, "FirstPersonCamera").u.o; u64_v rel = *u64_ivar(cam, "RelativeLocation");
    float cx = lx[0] - 150 + rel.u.v[0], cy = lx[1] + rel.u.v[1], cz = 112 + rel.u.v[2], dx = lx[0] - cx, dy = lx[1] - cy, dz = lx[2] - cz;
    *u64_ivar(hero, "ControlRotation") = u64_vec(atan2f(dz, sqrtf(dx * dx + dy * dy)) * 57.29578f, atan2f(dy, dx) * 57.29578f, 0);   // look at the lock
    tick(12);
    CHECK(!u64_tag_has(u64_asc_of(ch), "Object.Chest.State.Open"));
    u64_input_press(hero, "EGSAbilityInputID::Interact");
    tick(2 * 30);
    CHECK(u64_tag_has(asc_of_hero(), "State.Interacting") && !u64_tag_has(u64_asc_of(ch), "Object.Chest.State.Open"));   // still holding after 2 s
    tick(1 * 30 + 6);
    CHECK(u64_tag_has(u64_asc_of(ch), "Object.Chest.State.Open") && !u64_tag_has(asc_of_hero(), "State.Interacting"));    // done after the 3 s hold
    u64_input_release_id(hero, "EGSAbilityInputID::Interact");                              // let go of the button
    tick(2);
    int cnt_rocket1 = 0, cnt_hp1 = 0;
    for (int i = 0; i < u64_obj_count(); i++) { u64_obj *o = u64_obj_at(i); if (!u64_obj_is_actor(o)) continue; if (!strcmp(u64_obj_class(o), "BP_RocketLauncher")) cnt_rocket1++; if (!strcmp(u64_obj_class(o), "BP_Pickup_Health")) cnt_hp1++; }
    CHECK(cnt_rocket1 == cnt_rocket0 + 1 && cnt_hp1 == cnt_hp0 + 1);                      // the chest's loot: a rocket launcher and a health pickup
    tick(4 * 30);
    CHECK(u64_tag_has(u64_asc_of(ch), "Object.Chest.State.Open"));                       // GE_ChestOpen lasts 5 s
    tick(2 * 30);
    CHECK(!u64_tag_has(u64_asc_of(ch), "Object.Chest.State.Open"));
  }
  // --- door: its button is the interactable component (instant: OpenInteractionDuration is 0 for the door). GA_InteractActive's PostInteract applies
  // GE_DoorOpen (5 s, Object.Door.State.Open); the tag listener plays DoorOpenTimeline (0.5 s) that lerps the Door component's relative y from its
  // start to -MaxDoorMovementDistance (-140). The door is yawed 90 and scaled 2.625, so local -140 is world +367.5 in x; the tag ending closes it.
  {
    u64_obj *door = u64_find("BP_Door_2"); float b[3], p0[3], p1[3]; u64_comp_world(door, "Button", b); u64_comp_world(door, "Door", p0);
    CHECK(fabsf(b[0] + 338.25f) < 0.01f && fabsf(b[1] + 930) < 0.01f && fabsf(p0[0] + 144) < 0.01f);       // button at local y 74 * 2.625 = 194.25 from the door
    *u64_ivar(hero, "Location") = u64_vec(b[0], b[1] + 100, b[2] - 70);
    u64_obj *cam = u64_comp(hero, "FirstPersonCamera").u.o; u64_v rel = *u64_ivar(cam, "RelativeLocation");
    float cx = b[0] + rel.u.v[0], cy = b[1] + 100 + rel.u.v[1], cz = b[2] - 70 + rel.u.v[2], dx = b[0] - cx, dy = b[1] - cy, dz = b[2] - cz;
    *u64_ivar(hero, "ControlRotation") = u64_vec(atan2f(dz, sqrtf(dx * dx + dy * dy)) * 57.29578f, atan2f(dy, dx) * 57.29578f, 0);   // look at the button
    tick(12);
    u64_input_press(hero, "EGSAbilityInputID::Interact");
    tick(3);
    u64_input_release_id(hero, "EGSAbilityInputID::Interact");
    CHECK(u64_tag_has(u64_asc_of(door), "Object.Door.State.Open"));
    tick(15);                                                                              // 0.5 s timeline done
    u64_comp_world(door, "Door", p1);
    CHECK(fabsf(p1[0] - (-144 + 367.5f)) < 0.05f && fabsf(p1[1] + 930) < 0.01f);
    tick(5 * 30);                                                                          // GE_DoorOpen ends: the door closes
    tick(18);
    u64_comp_world(door, "Door", p1);
    CHECK(!u64_tag_has(u64_asc_of(door), "Object.Door.State.Open") && fabsf(p1[0] + 144) < 0.05f);
  }
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
  return fails;
}

#ifndef U64_NO_MAIN
int main(void) { return u64_run_shooter_scenarios(1) != 0; }
#endif
