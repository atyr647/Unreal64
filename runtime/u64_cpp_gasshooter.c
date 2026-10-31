// Ports of GASShooter's C++ gameplay classes (Source/GASShooter), hand-translated function by function from the
// project's own sources: AGSCharacterBase, AGSHeroCharacter (inventory / weapons / knock down / dying), AGSWeapon,
// UGSAttributeSetBase + UGSAmmoAttributeSet (as attribute rules) and AGASShooterGameModeBase.
// Compiled only into builds of this project; it registers itself with the runtime's class / method tables.
// Differences that come from the runtime, not the game: the hero owns its ability system component (the project keeps it on
// the PlayerState, which only matters across respawns), and authority / replication code paths are always the server's.
#include "u64_gas_core.h"
#include "u64_anim.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

static const char *nm(u64_v v)
{
  if (v.t == U64_CLASS || v.t == U64_NAME || v.t == U64_STR) return v.u.s ? v.u.s : "";
  if (v.t == U64_OBJ && v.u.o) return u64_obj_name(v.u.o);
  return "";
}
static u64_v A(int argc, const u64_v *a, int i) { return i < argc ? a[i] : (u64_v){U64_NONE}; }
static u64_obj *O(u64_v v) { return v.t == U64_OBJ ? v.u.o : 0; }
static float fv(u64_v v) { return v.t == U64_FLOAT ? v.u.f : (v.t == U64_INT || v.t == U64_BOOL) ? (float)v.u.i : 0; }
static const u64_v NONE_ = {U64_NONE};
static void tag_push(u64_obj *o, const char *prop, const char *tag) { u64_array_push(u64_ivar(o, prop), u64_name(tag)); }
static int strip_len(const char *c) { int n = (int)strlen(c); return n > 2 && !strcmp(c + n - 2, "_C") ? n - 2 : n; }
static int is_class(u64_obj *o, const char *c) { return o && u64_class_is(u64_obj_class(o), c); }

// ---------------------------------------------------------------------------------------------------- ammo attributes
// UGSAmmoAttributeSet::GetReserveAmmoAttributeFromTag / GetMaxReserveAmmoAttributeFromTag
static const char *ammo_attr(const char *tag, int max)
{
  if (!strcmp(tag, "Weapon.Ammo.Rifle")) return max ? "MaxRifleReserveAmmo" : "RifleReserveAmmo";
  if (!strcmp(tag, "Weapon.Ammo.Rocket")) return max ? "MaxRocketReserveAmmo" : "RocketReserveAmmo";
  if (!strcmp(tag, "Weapon.Ammo.Shotgun")) return max ? "MaxShotgunReserveAmmo" : "ShotgunReserveAmmo";
  return 0;                                                       // FGameplayAttribute() (invalid)
}

// ---------------------------------------------------------------------------------------------------- attribute rules
static float clampf(float v, float lo, float hi) { return v < lo ? lo : v > hi ? hi : v; }
static void gs_pre_attribute_change(u64_obj *asc, const char *name, float *nv)      // UGSAttributeSetBase::PreAttributeChange
{
#ifdef U64_CXX_BASE                                                // the project's own C++ (UGSAttributeSetBase.cpp, unmodified): see cpp2mips/
  { extern int u64_cxx_base_pre_change(u64_obj *, const char *, float *); if (u64_cxx_base_pre_change(asc, name, nv)) return; }
#endif
  if (!strcmp(name, "MaxHealth")) u64_adjust_for_max_change(asc, "Health", "MaxHealth", *nv);
  else if (!strcmp(name, "MaxMana")) u64_adjust_for_max_change(asc, "Mana", "MaxMana", *nv);
  else if (!strcmp(name, "MaxStamina")) u64_adjust_for_max_change(asc, "Stamina", "MaxStamina", *nv);
  else if (!strcmp(name, "MoveSpeed")) *nv = clampf(*nv, 150, 1000);
}
static int is_alive(u64_obj *ch) { return u64_obj_alive(ch) && u64_attr_get(u64_asc_of(ch), "Health") > 0; }   // GetHealth() > 0
static int gs_post_execute(u64_obj *tgt, const char *name, u64_obj *src)            // UGSAttributeSetBase / UGSAmmoAttributeSet::PostGameplayEffectExecute
{
#ifdef U64_CXX_BASE
  { extern int u64_cxx_base_post_execute(u64_obj *, const char *, u64_obj *); if (u64_cxx_base_post_execute(tgt, name, src)) return 1; }
#endif
  u64_obj *target_actor = u64_asc_owner(tgt), *source_actor = src ? u64_asc_owner(src) : 0;
  if (!strcmp(name, "Damage")) {
    float local = u64_attr_get(tgt, "Damage");
    u64_attr_set_base(tgt, "Damage", 0.0f);
    if (local > 0.0f) {
      int was_alive = target_actor ? is_alive(target_actor) : 1;
      float old_shield = u64_attr_get(tgt, "Shield"), after_shield = local - old_shield;
      if (old_shield > 0) u64_attr_set_base(tgt, "Shield", clampf(old_shield - local, 0.0f, u64_attr_get(tgt, "MaxShield")));
      if (after_shield > 0) u64_attr_set_base(tgt, "Health", clampf(u64_attr_get(tgt, "Health") - after_shield, 0.0f, u64_attr_get(tgt, "MaxHealth")));
      if (target_actor && was_alive && !is_alive(target_actor) && src && src != tgt) {     // killed: bounty to a different controller
        static u64_modinfo mods[2][2]; static u64_ge_def defs[2]; static const char *const none[] = {0}; static int n;
        int k = n++ & 1;                                                                   // two alternating slots: the effect is applied immediately
        mods[k][0] = (u64_modinfo){"XP", U64_OP_ADD, U64_CALC_SCALABLE, u64_attr_get(tgt, "XPBounty"), "", 0, 1.0f, 0.0f, 0.0f, ""};
        mods[k][1] = (u64_modinfo){"Gold", U64_OP_ADD, U64_CALC_SCALABLE, u64_attr_get(tgt, "GoldBounty"), "", 0, 1.0f, 0.0f, 0.0f, ""};
        defs[k] = (u64_ge_def){.name = k ? "Bounty1" : "Bounty0", .dur = U64_DUR_INSTANT, .nmods = 2, .mods = mods[k], .execs = none, .granted = none,
                               .asset_tags = none, .remove_tags = none, .app_require = none, .app_ignore = none, .ongoing_require = none, .ongoing_ignore = none};
        u64_ge_register(&defs[k]);
        u64_apply_ge(src, src, defs[k].name, 1.0f, 0);
      }
    }
    (void)source_actor;
    return 1;
  }
  if (!strcmp(name, "Health")) { u64_attr_set_base(tgt, "Health", clampf(u64_attr_get(tgt, "Health"), 0.0f, u64_attr_get(tgt, "MaxHealth"))); return 1; }
  if (!strcmp(name, "Mana")) { u64_attr_set_base(tgt, "Mana", clampf(u64_attr_get(tgt, "Mana"), 0.0f, u64_attr_get(tgt, "MaxMana"))); return 1; }
  if (!strcmp(name, "Stamina")) { u64_attr_set_base(tgt, "Stamina", clampf(u64_attr_get(tgt, "Stamina"), 0.0f, u64_attr_get(tgt, "MaxStamina"))); return 1; }
  if (!strcmp(name, "Shield")) { u64_attr_set_base(tgt, "Shield", clampf(u64_attr_get(tgt, "Shield"), 0.0f, u64_attr_get(tgt, "MaxShield"))); return 1; }
#ifdef U64_CXX_AMMO                                                // the project's own C++ (UGSAmmoAttributeSet.cpp, unmodified) instead of the port below: see cpp2mips/
  { extern int u64_cxx_ammo_post_execute(u64_obj *, const char *); if (u64_cxx_ammo_post_execute(tgt, name)) return 1; }
#endif
  static const char *const ammo[] = {"Rifle", "Rocket", "Shotgun"};
  for (int i = 0; i < 3; i++) {
    char cur[32], max[32]; snprintf(cur, sizeof cur, "%sReserveAmmo", ammo[i]); snprintf(max, sizeof max, "Max%sReserveAmmo", ammo[i]);
    if (!strcmp(name, cur)) { u64_attr_set_base(tgt, cur, clampf(u64_attr_get(tgt, cur), 0.0f, u64_attr_get(tgt, max))); return 1; }
  }
  return 1;
}

// ---------------------------------------------------------------------------------------------------- AGSCharacterBase
static u64_v cb_is_alive(u64_obj *self, int argc, const u64_v *a) { (void)argc; (void)a; return u64_bool(is_alive(self)); }
static u64_v cb_attr(u64_obj *self, const char *n) { return u64_float(u64_attr_get(u64_asc_of(self), n)); }
static u64_v cb_get_shield(u64_obj *s, int c, const u64_v *a) { (void)c; (void)a; return cb_attr(s, "Shield"); }
static u64_v cb_get_max_shield(u64_obj *s, int c, const u64_v *a) { (void)c; (void)a; return cb_attr(s, "MaxShield"); }
static u64_v cb_get_move_speed_base(u64_obj *s, int c, const u64_v *a)    // GetMoveSpeedBaseValue: the attribute's base value
{ (void)c; (void)a; return cb_attr(s, "MoveSpeed"); }

// ---------------------------------------------------------------------------------------------------- AGSWeapon
static void ctor_weapon(u64_obj *o)                                // AGSWeapon::AGSWeapon
{
  *u64_ivar(o, "PrimaryClipAmmo") = u64_int(0); *u64_ivar(o, "MaxPrimaryClipAmmo") = u64_int(0);
  *u64_ivar(o, "SecondaryClipAmmo") = u64_int(0); *u64_ivar(o, "MaxSecondaryClipAmmo") = u64_int(0);
  *u64_ivar(o, "bInfiniteAmmo") = u64_bool(0);
  *u64_ivar(o, "bSpawnWithCollision") = u64_bool(1);
  *u64_ivar(o, "PrimaryAmmoType") = u64_name("Weapon.Ammo.None"); *u64_ivar(o, "SecondaryAmmoType") = u64_name("Weapon.Ammo.None");
  *u64_ivar(o, "WeaponMesh3PickupRelativeLocation") = u64_vec(0.0f, -25.0f, 0.0f);
  *u64_ivar(o, "WeaponPrimaryInstantAbilityTag") = u64_name("Ability.Weapon.Primary.Instant");
  *u64_ivar(o, "WeaponSecondaryInstantAbilityTag") = u64_name("Ability.Weapon.Secondary.Instant");
  *u64_ivar(o, "WeaponAlternateInstantAbilityTag") = u64_name("Ability.Weapon.Alternate.Instant");
  *u64_ivar(o, "WeaponIsFiringTag") = u64_name("Weapon.IsFiring");
  *u64_ivar(o, "FireMode") = u64_name("Weapon.FireMode.None");
  tag_push(o, "RestrictedPickupTags", "State.Dead"); tag_push(o, "RestrictedPickupTags", "State.KnockedDown");
  u64_obj *c = O(u64_comp(o, "CollisionComponent"));                  // CollisionComp->InitCapsuleSize(40, 50)
  if (c) { *u64_ivar(c, "CapsuleRadius") = u64_float(40.0f); *u64_ivar(c, "CapsuleHalfHeight") = u64_float(50.0f); }
}
static u64_v w_get_ivar(u64_obj *s, const char *n) { return *u64_ivar(s, n); }
static u64_v w_get_primary_clip(u64_obj *s, int c, const u64_v *a) { (void)c; (void)a; return w_get_ivar(s, "PrimaryClipAmmo"); }
static u64_v w_get_max_primary_clip(u64_obj *s, int c, const u64_v *a) { (void)c; (void)a; return w_get_ivar(s, "MaxPrimaryClipAmmo"); }
static u64_v w_get_secondary_clip(u64_obj *s, int c, const u64_v *a) { (void)c; (void)a; return w_get_ivar(s, "SecondaryClipAmmo"); }
static u64_v w_get_max_secondary_clip(u64_obj *s, int c, const u64_v *a) { (void)c; (void)a; return w_get_ivar(s, "MaxSecondaryClipAmmo"); }
static u64_v w_has_infinite_ammo(u64_obj *s, int c, const u64_v *a) { (void)c; (void)a; return u64_bool(u64_true(w_get_ivar(s, "bInfiniteAmmo"))); }
static u64_v w_mesh1p(u64_obj *s, int c, const u64_v *a) { (void)c; (void)a; return u64_comp(s, "WeaponMesh1P"); }
static u64_v w_mesh3p(u64_obj *s, int c, const u64_v *a) { (void)c; (void)a; return u64_comp(s, "WeaponMesh3P"); }
static void set_ammo(u64_obj *s, const char *prop, const char *delegate, int nv)   // SetXClipAmmo: store, then broadcast (Old, New)
{
  u64_v old = *u64_ivar(s, prop), args[2];
  *u64_ivar(s, prop) = u64_int(nv);
  args[0] = u64_int(old.t == U64_NONE ? 0 : u64_as_int(old)); args[1] = u64_int(nv);
  u64_mc_broadcast(u64_ivar(s, delegate), 2, args);
}
static u64_v w_set_primary_clip(u64_obj *s, int c, const u64_v *a) { set_ammo(s, "PrimaryClipAmmo", "OnPrimaryClipAmmoChanged", u64_as_int(A(c, a, 0))); return NONE_; }
static u64_v w_set_max_primary_clip(u64_obj *s, int c, const u64_v *a) { set_ammo(s, "MaxPrimaryClipAmmo", "OnMaxPrimaryClipAmmoChanged", u64_as_int(A(c, a, 0))); return NONE_; }
static u64_v w_set_secondary_clip(u64_obj *s, int c, const u64_v *a) { set_ammo(s, "SecondaryClipAmmo", "OnSecondaryClipAmmoChanged", u64_as_int(A(c, a, 0))); return NONE_; }
static u64_v w_set_max_secondary_clip(u64_obj *s, int c, const u64_v *a) { set_ammo(s, "MaxSecondaryClipAmmo", "OnMaxSecondaryClipAmmoChanged", u64_as_int(A(c, a, 0))); return NONE_; }
static u64_v w_reset(u64_obj *s, int c, const u64_v *a)            // ResetWeapon
{ (void)c; (void)a; *u64_ivar(s, "FireMode") = *u64_ivar(s, "DefaultFireMode"); *u64_ivar(s, "StatusText") = *u64_ivar(s, "DefaultStatusText"); return NONE_; }
static u64_v w_set_owning_character(u64_obj *s, int c, const u64_v *a)   // SetOwningCharacter
{
  u64_obj *ch = O(A(c, a, 0));
  *u64_ivar(s, "OwningCharacter") = u64_objv(ch);
  *u64_ivar(s, "AbilitySystemComponent") = u64_objv(ch ? u64_asc_of(ch) : 0);
  *u64_ivar(s, "Owner") = u64_objv(ch);
  return NONE_;
}
static u64_v w_equip(u64_obj *s, int c, const u64_v *a) { (void)c; (void)a; if (!O(*u64_ivar(s, "OwningCharacter"))) return NONE_; return NONE_; }   // mesh attach/visibility only
static u64_v w_unequip(u64_obj *s, int c, const u64_v *a) { (void)s; (void)c; (void)a; return NONE_; }
static struct { const char *cls; u64_obj *o; } cdo_cache[64]; static int cdo_n;
static u64_obj *class_default(const char *cls)                    // GetDefaultObject(): one cached, inert instance per class
{
  for (int i = 0; i < cdo_n; i++) if (!strcmp(cdo_cache[i].cls, cls)) return cdo_cache[i].o;
  if (cdo_n >= 64) return 0;
  char b[80]; snprintf(b, sizeof b, "Default__%s", cls);
  u64_obj *o = u64_new(u64_intern(b), u64_intern(cls));
  u64_obj_set_alive(o, 0);
  cdo_cache[cdo_n].cls = u64_intern(cls); cdo_cache[cdo_n++].o = o;
  return o;
}
static u64_v w_add_abilities(u64_obj *s, int c, const u64_v *a)    // AddAbilities: GiveAbility(Spec(Class, level 1, InputID, this)) for every listed ability
{
  (void)c; (void)a;
  u64_obj *ch = O(*u64_ivar(s, "OwningCharacter")), *asc = ch ? u64_asc_of(ch) : 0;
  if (!asc) return NONE_;
  u64_v list = *u64_ivar(s, "Abilities"), handles = *u64_ivar(s, "AbilitySpecHandles");
  for (int i = 0; i < u64_array_len(list); i++) {
    const char *cls = nm(u64_array_get(list, i));
    u64_obj *def = class_default(cls);                              // the ability's class default object: AbilityInputID
    const char *input = def ? nm(*u64_ivar(def, "AbilityInputID")) : "";
    u64_obj *ab = u64_give_ability(asc, cls, 1, input, s);
    if (ab) u64_array_push(&handles, u64_objv(ab));
  }
  *u64_ivar(s, "AbilitySpecHandles") = handles;
  return NONE_;
}
static u64_v w_remove_abilities(u64_obj *s, int c, const u64_v *a)
{
  (void)c; (void)a;
  u64_obj *ch = O(*u64_ivar(s, "OwningCharacter")), *asc = ch ? u64_asc_of(ch) : 0;
  u64_v handles = *u64_ivar(s, "AbilitySpecHandles");
  for (int i = 0; asc && i < u64_array_len(handles); i++) { u64_obj *ab = O(u64_array_get(handles, i)); if (u64_obj_alive(ab)) u64_clear_ability(asc, ab); }
  *u64_ivar(s, "AbilitySpecHandles") = u64_array_of(0, 0);
  return NONE_;
}
static u64_v w_on_dropped(u64_obj *s, int c, const u64_v *a)       // OnDropped_Implementation(NewLocation)
{
  w_set_owning_character(s, 0, 0);
  w_reset(s, 0, 0);
  if (A(c, a, 0).t == U64_VEC) *u64_ivar(s, "Location") = A(c, a, 0);
  *u64_ivar(s, "PickupEnabled") = u64_bool(1);                    // CollisionComp QueryOnly
  return NONE_;
}
static u64_v w_trace_target_actor(u64_obj *s, const char *slot, const char *cls)   // GetLineTraceTargetActor / GetSphereTraceTargetActor: lazily spawned, owned by the weapon
{
  u64_v *p = u64_ivar(s, slot);
  if (O(*p) && u64_obj_alive(O(*p))) return *p;
  u64_obj *ta = u64_spawn_actor(cls, u64_vec(0, 0, 0));
  if (ta) *u64_ivar(ta, "Owner") = u64_objv(s);
  *p = u64_objv(ta);
  return *p;
}
static u64_v w_line_trace_ta(u64_obj *s, int c, const u64_v *a) { (void)c; (void)a; return w_trace_target_actor(s, "LineTraceTargetActor", "GSGATA_LineTrace"); }
static u64_v w_sphere_trace_ta(u64_obj *s, int c, const u64_v *a) { (void)c; (void)a; return w_trace_target_actor(s, "SphereTraceTargetActor", "GSGATA_SphereTrace"); }
static u64_v w_begin_play(u64_obj *s, int c, const u64_v *a)       // AGSWeapon::BeginPlay
{
  (void)c; (void)a;
  w_reset(s, 0, 0);
  if (!O(*u64_ivar(s, "OwningCharacter")) && u64_true(*u64_ivar(s, "bSpawnWithCollision"))) *u64_ivar(s, "PickupEnabled") = u64_bool(1);
  return NONE_;
}
static u64_v w_end_play(u64_obj *s, int c, const u64_v *a)         // AGSWeapon::EndPlay: destroys the spawned trace actors
{
  (void)c; (void)a;
  static const char *const slots[] = {"LineTraceTargetActor", "SphereTraceTargetActor"};
  for (int i = 0; i < 2; i++) { u64_obj *t = O(*u64_ivar(s, slots[i])); if (t && u64_obj_alive(t)) u64_obj_destroy(t); }
  return NONE_;
}

// ---------------------------------------------------------------------------------------------------- AGSHeroCharacter
static void ctor_hero(u64_obj *o)                                  // AGSHeroCharacter::AGSHeroCharacter
{
  { u64_v cm = u64_comp(o, "CharacterMovement"); *u64_ivar(o, "CharacterMovement") = cm; *u64_ivar(cm.u.o, "MaxWalkSpeed") = u64_float(600.0f); }   // UCharacterMovementComponent default (the Blueprint sets none)
  *u64_ivar(o, "bStartInFirstPersonPerspective") = u64_bool(1);
  *u64_ivar(o, "bIsFirstPersonPerspective") = u64_bool(0);
  *u64_ivar(o, "Default1PFOV") = u64_float(90.0f); *u64_ivar(o, "Default3PFOV") = u64_float(80.0f);
  *u64_ivar(o, "NoWeaponTag") = u64_name("Weapon.Equipped.None");
  *u64_ivar(o, "WeaponChangingDelayReplicationTag") = u64_name("Ability.Weapon.IsChangingDelayReplication");
  *u64_ivar(o, "WeaponAmmoTypeNoneTag") = u64_name("Weapon.Ammo.None");
  *u64_ivar(o, "WeaponAbilityTag") = u64_name("Ability.Weapon");
  *u64_ivar(o, "CurrentWeaponTag") = u64_name("Weapon.Equipped.None");
  *u64_ivar(o, "ReviveDuration") = u64_float(4.0f);
  *u64_ivar(o, "KnockedDownTag") = u64_name("State.KnockedDown");
  *u64_ivar(o, "InteractingTag") = u64_name("State.Interacting");
  *u64_ivar(o, "DeadTag") = u64_name("State.Dead"); *u64_ivar(o, "EffectRemoveOnDeathTag") = u64_name("Effect.RemoveOnDeath");
  *u64_ivar(o, "Inventory_Weapons") = u64_array_of(0, 0);
}
static u64_v h_perspective(u64_obj *s, int c, const u64_v *a) { (void)c; (void)a; return u64_bool(u64_true(*u64_ivar(s, "bIsFirstPersonPerspective"))); }
static u64_v h_first_person_mesh(u64_obj *s, int c, const u64_v *a) { (void)c; (void)a; return u64_comp(s, "FirstPersonMesh"); }
static u64_v h_third_person_mesh(u64_obj *s, int c, const u64_v *a)
{ (void)c; (void)a; u64_v m = *u64_ivar(s, "Mesh"); return m.t == U64_OBJ && m.u.o ? m : u64_comp(s, "CharacterMesh0"); }
static u64_v h_current_weapon(u64_obj *s, int c, const u64_v *a) { (void)c; (void)a; return *u64_ivar(s, "CurrentWeapon"); }
static u64_v h_weapon_attach_point(u64_obj *s, int c, const u64_v *a) { (void)c; (void)a; return *u64_ivar(s, "WeaponAttachPoint"); }
static int inv_len(u64_obj *s) { return u64_array_len(*u64_ivar(s, "Inventory_Weapons")); }
static u64_obj *inv_at(u64_obj *s, int i) { return O(u64_array_get(*u64_ivar(s, "Inventory_Weapons"), i)); }
static int inv_find(u64_obj *s, u64_obj *w) { for (int i = 0; i < inv_len(s); i++) if (inv_at(s, i) == w) return i; return -1; }
static void inv_remove(u64_obj *s, u64_obj *w)
{
  u64_v *arr = u64_ivar(s, "Inventory_Weapons"); u64_v keep[64]; int n = 0;
  for (int i = 0; i < u64_array_len(*arr) && n < 64; i++) if (O(u64_array_get(*arr, i)) != w) keep[n++] = u64_array_get(*arr, i);
  *arr = u64_array_of(n, keep);
}
static u64_v h_does_weapon_exist(u64_obj *s, int c, const u64_v *a)      // DoesWeaponExistInInventory: same class
{
  u64_obj *in = O(A(c, a, 0));
  for (int i = 0; in && i < inv_len(s); i++) if (!strcmp(u64_obj_class(inv_at(s, i)), u64_obj_class(in))) return u64_bool(1);
  return u64_bool(0);
}
static u64_obj *hero_asc(u64_obj *s) { return u64_asc_of(s); }
static void add_ammo_effect(u64_obj *s, u64_obj *w)                // the transient "Ammo" GameplayEffect built in AddWeaponToInventory
{
  static u64_modinfo mods[8][2]; static u64_ge_def defs[8]; static char names[8][8]; static const char *const none[] = {0}; static int n;
  int k = n++ & 7, nm_ = 0;
  const char *pt = nm(*u64_ivar(w, "PrimaryAmmoType")), *st = nm(*u64_ivar(w, "SecondaryAmmoType"));
  if (strcmp(pt, "Weapon.Ammo.None") && ammo_attr(pt, 0))
    mods[k][nm_++] = (u64_modinfo){ammo_attr(pt, 0), U64_OP_ADD, U64_CALC_SCALABLE, (float)u64_as_int(*u64_ivar(w, "PrimaryClipAmmo")), "", 0, 1.0f, 0.0f, 0.0f, ""};
  if (strcmp(st, "Weapon.Ammo.None") && ammo_attr(st, 0))
    mods[k][nm_++] = (u64_modinfo){ammo_attr(st, 0), U64_OP_ADD, U64_CALC_SCALABLE, (float)u64_as_int(*u64_ivar(w, "SecondaryClipAmmo")), "", 0, 1.0f, 0.0f, 0.0f, ""};
  if (!nm_) return;
  snprintf(names[k], sizeof names[k], "Ammo%d", k);
  defs[k] = (u64_ge_def){.name = names[k], .dur = U64_DUR_INSTANT, .nmods = nm_, .mods = mods[k], .execs = none, .granted = none, .asset_tags = none,
                         .remove_tags = none, .app_require = none, .app_ignore = none, .ongoing_require = none, .ongoing_ignore = none};
  u64_ge_register(&defs[k]);
  u64_apply_ge(hero_asc(s), hero_asc(s), defs[k].name, 1.0f, 0);
}
static u64_v h_set_current_weapon(u64_obj *s, int c, const u64_v *a);
static u64_v h_unequip_current_weapon(u64_obj *s, int c, const u64_v *a);
static u64_v h_equip_weapon(u64_obj *s, int c, const u64_v *a)           // EquipWeapon (authority): SetCurrentWeapon(New, CurrentWeapon)
{
  u64_v args[2] = {A(c, a, 0), *u64_ivar(s, "CurrentWeapon")};
  return h_set_current_weapon(s, 2, args);
}
static u64_v h_add_weapon_to_inventory(u64_obj *s, int c, const u64_v *a)
{
  u64_obj *w = O(A(c, a, 0)); int equip = u64_true(A(c, a, 1));
  if (!w) return u64_bool(0);
  if (u64_true(h_does_weapon_exist(s, c, a))) {                    // already carrying one: take its ammo instead
    add_ammo_effect(s, w);
    u64_obj_destroy(w);
    return u64_bool(0);
  }
  u64_array_push(u64_ivar(s, "Inventory_Weapons"), u64_objv(w));
  u64_v own = u64_objv(s);
  u64_call(w, "SetOwningCharacter", 1, &own);
  u64_call(w, "AddAbilities", 0, 0);
  if (equip) h_equip_weapon(s, 1, (u64_v[]){u64_objv(w)});
  return u64_bool(1);
}
static u64_v h_remove_weapon_from_inventory(u64_obj *s, int c, const u64_v *a)
{
  u64_obj *w = O(A(c, a, 0));
  if (!w || !u64_true(h_does_weapon_exist(s, c, a))) return u64_bool(0);
  if (w == O(*u64_ivar(s, "CurrentWeapon"))) h_unequip_current_weapon(s, 0, 0);
  inv_remove(s, w);
  u64_call(w, "RemoveAbilities", 0, 0);
  u64_v none = u64_objv(0);
  u64_call(w, "SetOwningCharacter", 1, &none);
  u64_call(w, "ResetWeapon", 0, 0);
  return u64_bool(1);
}
static u64_v h_remove_all_weapons(u64_obj *s, int c, const u64_v *a)     // RemoveAllWeaponsFromInventory: drop them in a ring around the hero
{
  (void)c; (void)a;
  h_unequip_current_weapon(s, 0, 0);
  float radius = 50.0f, num = (float)inv_len(s);
  u64_v loc = *u64_ivar(s, "Location");
  for (int i = inv_len(s) - 1; i >= 0; i--) {
    u64_obj *w = inv_at(s, i);
    h_remove_weapon_from_inventory(s, 1, (u64_v[]){u64_objv(w)});
    float ox = radius * cosf((i / num) * 2.0f * 3.14159265f), oy = radius * sinf((i / num) * 2.0f * 3.14159265f);
    u64_v nl = u64_vec((loc.t == U64_VEC ? loc.u.v[0] : 0) + ox, (loc.t == U64_VEC ? loc.u.v[1] : 0) + oy, loc.t == U64_VEC ? loc.u.v[2] : 0);
    u64_call(w, "OnDropped", 1, &nl);
  }
  return NONE_;
}
static void unequip_weapon(u64_obj *s, u64_obj *w)                 // UnEquipWeapon(WeaponToUnEquip): drop the delegates, then UnEquip
{
  if (!w) return;
  u64_mc_remove(u64_ivar(w, "OnPrimaryClipAmmoChanged"), u64_delegate(u64_objv(s), "CurrentWeaponPrimaryClipAmmoChanged"));
  u64_mc_remove(u64_ivar(w, "OnSecondaryClipAmmoChanged"), u64_delegate(u64_objv(s), "CurrentWeaponSecondaryClipAmmoChanged"));
  u64_call(w, "UnEquip", 0, 0);
}
static void set_weapon_tag(u64_obj *s, const char *tag)            // Remove the old CurrentWeaponTag loose tag, store and add the new one
{
  u64_obj *asc = hero_asc(s);
  if (asc) u64_tag_add_loose(asc, nm(*u64_ivar(s, "CurrentWeaponTag")), -1);
  *u64_ivar(s, "CurrentWeaponTag") = u64_name(tag);
  if (asc) u64_tag_add_loose(asc, tag, 1);
}
static u64_v h_unequip_current_weapon(u64_obj *s, int c, const u64_v *a)
{
  (void)c; (void)a;
  set_weapon_tag(s, nm(*u64_ivar(s, "NoWeaponTag")));
  unequip_weapon(s, O(*u64_ivar(s, "CurrentWeapon")));
  *u64_ivar(s, "CurrentWeapon") = u64_objv(0);
  return NONE_;
}
static u64_v h_set_current_weapon(u64_obj *s, int c, const u64_v *a)
{
  u64_obj *nw = O(A(c, a, 0)), *last = O(A(c, a, 1));
  if (nw == last) return NONE_;
  u64_obj *asc = hero_asc(s);
  if (asc) u64_cancel_abilities(asc, nm(*u64_ivar(s, "WeaponAbilityTag")));        // CancelAbilities(Ability.Weapon)
  unequip_weapon(s, last);
  if (nw) {
    if (asc) u64_tag_add_loose(asc, nm(*u64_ivar(s, "CurrentWeaponTag")), -1);
    *u64_ivar(s, "CurrentWeapon") = u64_objv(nw);
    u64_v own = u64_objv(s);
    u64_call(nw, "SetOwningCharacter", 1, &own);
    u64_call(nw, "Equip", 0, 0);
    const char *wt = nm(*u64_ivar(nw, "WeaponTag"));
    *u64_ivar(s, "CurrentWeaponTag") = u64_name(wt);
    if (asc) u64_tag_add_loose(asc, wt, 1);
    u64_mc_add(u64_ivar(nw, "OnPrimaryClipAmmoChanged"), u64_delegate(u64_objv(s), "CurrentWeaponPrimaryClipAmmoChanged"));
    u64_mc_add(u64_ivar(nw, "OnSecondaryClipAmmoChanged"), u64_delegate(u64_objv(s), "CurrentWeaponSecondaryClipAmmoChanged"));
  } else h_unequip_current_weapon(s, 0, 0);
  return NONE_;
}
static u64_v h_next_weapon(u64_obj *s, int c, const u64_v *a)
{
  (void)c; (void)a;
  int n = inv_len(s);
  if (n < 2) return NONE_;
  int cur = inv_find(s, O(*u64_ivar(s, "CurrentWeapon")));
  h_unequip_current_weapon(s, 0, 0);
  h_equip_weapon(s, 1, (u64_v[]){u64_objv(inv_at(s, cur < 0 ? 0 : (cur + 1) % n))});
  return NONE_;
}
static u64_v h_previous_weapon(u64_obj *s, int c, const u64_v *a)
{
  (void)c; (void)a;
  int n = inv_len(s);
  if (n < 2) return NONE_;
  int cur = inv_find(s, O(*u64_ivar(s, "CurrentWeapon")));
  h_unequip_current_weapon(s, 0, 0);
  h_equip_weapon(s, 1, (u64_v[]){u64_objv(inv_at(s, cur < 0 ? 0 : abs(cur - 1 + n) % n))});
  return NONE_;
}
static u64_v h_num_weapons(u64_obj *s, int c, const u64_v *a) { (void)c; (void)a; return u64_int(inv_len(s)); }
static u64_v h_cur_weapon_int(u64_obj *s, const char *fn)
{
  u64_obj *w = O(*u64_ivar(s, "CurrentWeapon"));
  if (!w) return u64_int(0);
  return u64_call(w, fn, 0, 0);
}
static u64_v h_primary_clip(u64_obj *s, int c, const u64_v *a) { (void)c; (void)a; return h_cur_weapon_int(s, "GetPrimaryClipAmmo"); }
static u64_v h_max_primary_clip(u64_obj *s, int c, const u64_v *a) { (void)c; (void)a; return h_cur_weapon_int(s, "GetMaxPrimaryClipAmmo"); }
static u64_v h_secondary_clip(u64_obj *s, int c, const u64_v *a) { (void)c; (void)a; return h_cur_weapon_int(s, "GetSecondaryClipAmmo"); }
static u64_v h_max_secondary_clip(u64_obj *s, int c, const u64_v *a) { (void)c; (void)a; return h_cur_weapon_int(s, "GetMaxSecondaryClipAmmo"); }
static u64_v h_reserve(u64_obj *s, const char *slot)               // GetPrimaryReserveAmmo / GetSecondaryReserveAmmo
{
  u64_obj *w = O(*u64_ivar(s, "CurrentWeapon"));
  const char *attr = w ? ammo_attr(nm(*u64_ivar(w, slot)), 0) : 0;
  return u64_int(attr ? (int)u64_attr_get(hero_asc(s), attr) : 0);
}
static u64_v h_primary_reserve(u64_obj *s, int c, const u64_v *a) { (void)c; (void)a; return h_reserve(s, "PrimaryAmmoType"); }
static u64_v h_secondary_reserve(u64_obj *s, int c, const u64_v *a) { (void)c; (void)a; return h_reserve(s, "SecondaryAmmoType"); }
static u64_v h_spawn_default_inventory(u64_obj *s, int c, const u64_v *a)    // SpawnDefaultInventory (next tick after PostInitializeComponents)
{
  (void)c; (void)a;
  u64_v classes = *u64_ivar(s, "DefaultInventoryWeaponClasses");
  for (int i = 0; i < u64_array_len(classes); i++) {
    const char *cls = nm(u64_array_get(classes, i));
    if (!*cls) continue;
    u64_obj *w = u64_spawn_actor(cls, u64_vec(0, 0, 0));
    if (!w) continue;
    *u64_ivar(w, "Instigator") = u64_objv(s); *u64_ivar(w, "Owner") = u64_objv(s);
    *u64_ivar(w, "bSpawnWithCollision") = u64_bool(0);
    h_add_weapon_to_inventory(s, 2, (u64_v[]){u64_objv(w), u64_bool(i == 0)});
  }
  return NONE_;
}
static u64_v h_post_initialize_components(u64_obj *s, int c, const u64_v *a)
{
  (void)c; (void)a;
  u64_timer_call(0.0f, s, "SpawnDefaultInventory", NONE_);          // SetTimerForNextTick
  return NONE_;
}
static u64_v h_ammo_noop(u64_obj *s, int c, const u64_v *a) { (void)s; (void)c; (void)a; return NONE_; }   // HUD-only delegates (PlayerController widgets)

// knock down / dying
static void remove_death_effects(u64_obj *asc)
{
  u64_cancel_abilities(asc, 0);                                      // CancelAllAbilities
  u64_remove_ge_with_asset_tags(asc, "Effect.RemoveOnDeath");        // RemoveActiveEffectsWithTags(EffectRemoveOnDeathTag)
}
static u64_v h_knock_down(u64_obj *s, int c, const u64_v *a)       // AGSHeroCharacter::KnockDown
{
  (void)c; (void)a;
  u64_obj *asc = hero_asc(s);
  if (asc) {
    remove_death_effects(asc);
    u64_apply_ge(asc, asc, nm(*u64_ivar(s, "KnockDownEffect")), 1.0f, 0);
  }
  u64_attr_set_base(asc, "Health", u64_attr_get(asc, "MaxHealth"));  // SetHealth(GetMaxHealth()); SetShield(0)
  u64_attr_set_base(asc, "Shield", 0.0f);
  return NONE_;
}
static void game_mode_hero_died(u64_obj *hero);
static u64_v h_finish_dying(u64_obj *s, int c, const u64_v *a)     // AGSHeroCharacter::FinishDying, then AGSCharacterBase::FinishDying (Destroy)
{
  (void)c; (void)a;
  if (!u64_obj_alive(s)) return NONE_;
  h_remove_all_weapons(s, 0, 0);
  game_mode_hero_died(s);
  u64_obj *asc = hero_asc(s);
  if (asc) {
    u64_call(s, "RemoveCharacterAbilities", 0, 0);
    remove_death_effects(asc);
    u64_apply_ge(asc, asc, nm(*u64_ivar(s, "DeathEffect")), 1.0f, 0);
  }
  u64_v self = u64_objv(s);
  u64_mc_broadcast(u64_ivar(s, "OnCharacterDied"), 1, &self);
  u64_obj_destroy(s);
  return NONE_;
}
static u64_v cb_remove_character_abilities(u64_obj *s, int c, const u64_v *a)     // AGSCharacterBase::RemoveCharacterAbilities
{
  (void)c; (void)a;
  u64_obj *asc = u64_asc_of(s);
  u64_v given = *u64_ivar(s, "CharacterAbilities");
  u64_obj *list[64]; int n = u64_asc_abilities(asc, list, 64);
  for (int i = 0; i < n; i++) {
    if (O(*u64_ivar(list[i], "SourceObject")) != s) continue;
    for (int j = 0; j < u64_array_len(given); j++)
      if (!strncmp(nm(u64_array_get(given, j)), u64_obj_class(list[i]), strip_len(nm(u64_array_get(given, j))))) { u64_clear_ability(asc, list[i]); break; }
  }
  return NONE_;
}
static void gs_zero_health(u64_obj *owner)                          // AGSPlayerState::HealthChanged for heroes
{
  if (!is_class(owner, "GSHeroCharacter")) { u64_die(owner); return; }
  u64_obj *asc = u64_asc_of(owner);
  if (u64_tag_count(asc, "State.Dead")) return;
  if (!u64_tag_count(asc, "State.KnockedDown")) h_knock_down(owner, 0, 0); else h_finish_dying(owner, 0, 0);
}

// ---------------------------------------------------------------------------------------------------- AGASShooterGameModeBase
static u64_obj *gm_obj(void) { for (int i = 0; i < u64_obj_count(); i++) if (is_class(u64_obj_at(i), "GASShooterGameModeBase")) return u64_obj_at(i); return 0; }
static void ctor_gamemode(u64_obj *o)
{
  *u64_ivar(o, "RespawnDelay") = u64_float(5.0f);
  *u64_ivar(o, "HeroClass") = u64_cls("BP_HeroCharacter");
}
static void game_mode_hero_died(u64_obj *hero)                     // AGASShooterGameModeBase::HeroDied(Controller): respawn timer
{
  u64_obj *gm = gm_obj();
  if (!gm) return;
  if (u64_player_pawn() == hero) u64_set_player_pawn(0);                    // UnPossess (the spectator pawn is not modelled)
  u64_timer_call(fv(*u64_ivar(gm, "RespawnDelay")), gm, "RespawnHero", u64_bool(u64_true(*u64_ivar(hero, "IsPlayer"))));
}

static u64_v h_possessed_by(u64_obj *s, int c, const u64_v *a);
static u64_obj *find_named(const char *name) { for (int i = 0; i < u64_obj_count(); i++) { u64_obj *o = u64_obj_at(i); if (u64_obj_alive(o) && !strcmp(u64_obj_name(o), name)) return o; } return 0; }
static u64_obj *find_class_loc(const char *cls) { for (int i = 0; i < u64_obj_count(); i++) { u64_obj *o = u64_obj_at(i); if (u64_obj_alive(o) && !strcmp(u64_obj_class(o), cls)) return o; } return 0; }
static u64_v gm_respawn_hero(u64_obj *gm, int c, const u64_v *a)    // AGASShooterGameModeBase::RespawnHero(Controller): at the PlayerStart / EnemyHeroSpawn
{
  int is_player = u64_true(A(c, a, 0));
  u64_obj *start = is_player ? find_class_loc("PlayerStart") : find_named("EnemyHeroSpawn");
  if (!start) return NONE_;
  u64_v loc = *u64_ivar(start, "Location"), rot = *u64_ivar(start, "Rotation");
  u64_obj *hero = u64_spawn_actor(nm(*u64_ivar(gm, "HeroClass")), u64_xform_new(loc, rot));
  if (!hero) return NONE_;
  u64_actor_init(hero); u64_call(hero, "ReceiveBeginPlay", 0, 0);
  *u64_ivar(hero, "IsPlayer") = u64_bool(is_player);
  if (is_player) u64_set_player_pawn(hero);
  h_possessed_by(hero, 0, 0);
  if (u64_log) u64_log("  [respawn] %s", u64_obj_name(hero));
  return NONE_;
}

// ---------------------------------------------------------------------------------------------------- AGSGATA_Trace / LineTrace / SphereTrace
// The reusable trace target actor the weapons' abilities configure and hand to WaitTargetDataWithReusableActor.
extern void u64_target_data_ready(u64_obj *target_actor, u64_v handle);
extern u64_v u64_td_handle_from_hits(u64_v hits);
static void ctor_trace(u64_obj *o)                                  // AGSGATA_Trace::AGSGATA_Trace
{
  *u64_ivar(o, "MaxHitResultsPerTrace") = u64_int(1); *u64_ivar(o, "NumberOfTraces") = u64_int(1);
  *u64_ivar(o, "bIgnoreBlockingHits") = u64_bool(0); *u64_ivar(o, "bTraceAffectsAimPitch") = u64_bool(1);
  *u64_ivar(o, "bTraceFromPlayerViewPoint") = u64_bool(0); *u64_ivar(o, "MaxRange") = u64_float(999999.0f);
  *u64_ivar(o, "bUseAimingSpreadMod") = u64_bool(0);
  static const char *const zeros[] = {"BaseSpread", "AimingSpreadMod", "TargetingSpreadIncrement", "TargetingSpreadMax", "CurrentTargetingSpread"};
  for (int i = 0; i < 5; i++) *u64_ivar(o, zeros[i]) = u64_float(0.0f);
  *u64_ivar(o, "bUsePersistentHitResults") = u64_bool(0);
}
static void ctor_sphere_trace(u64_obj *o) { *u64_ivar(o, "TraceSphereRadius") = u64_float(100.0f); }
static u64_v tr_configure(u64_obj *s, int c, const u64_v *a)         // Configure(...): the line version has 21 parameters, the sphere version adds the radius after MaxRange
{
  static const char *const line_names[] = {"StartLocation", "AimingTag", "AimingRemovalTag", "TraceProfile", "Filter", "ReticleClass", "ReticleParams",
    "bIgnoreBlockingHits", "ShouldProduceTargetDataOnServer", "bUsePersistentHitResults", "bDebug", "bTraceAffectsAimPitch", "bTraceFromPlayerViewPoint",
    "bUseAimingSpreadMod", "MaxRange", "BaseSpread", "AimingSpreadMod", "TargetingSpreadIncrement", "TargetingSpreadMax", "MaxHitResultsPerTrace", "NumberOfTraces"};
  static const char *const sphere_names[] = {"StartLocation", "AimingTag", "AimingRemovalTag", "TraceProfile", "Filter", "ReticleClass", "ReticleParams",
    "bIgnoreBlockingHits", "ShouldProduceTargetDataOnServer", "bUsePersistentHitResults", "bDebug", "bTraceAffectsAimPitch", "bTraceFromPlayerViewPoint",
    "bUseAimingSpreadMod", "MaxRange", "TraceSphereRadius", "BaseSpread", "AimingSpreadMod", "TargetingSpreadIncrement", "TargetingSpreadMax", "MaxHitResultsPerTrace", "NumberOfTraces"};
  int sphere = is_class(s, "GSGATA_SphereTrace");
  const char *const *names = sphere ? sphere_names : line_names;
  int n = sphere ? 22 : 21;
  for (int i = 0; i < n && i < c; i++) *u64_ivar(s, names[i]) = a[i];
  if (u64_true(*u64_ivar(s, "bUsePersistentHitResults"))) *u64_ivar(s, "NumberOfTraces") = u64_int(1);
  return NONE_;
}
static u64_v tr_reset_spread(u64_obj *s, int c, const u64_v *a)
{
  (void)c; (void)a;
  *u64_ivar(s, "bUseAimingSpreadMod") = u64_bool(0);
  static const char *const zeros[] = {"BaseSpread", "AimingSpreadMod", "TargetingSpreadIncrement", "TargetingSpreadMax", "CurrentTargetingSpread"};
  for (int i = 0; i < 5; i++) *u64_ivar(s, zeros[i]) = u64_float(0.0f);
  return NONE_;
}
typedef struct { float x, y, z; } v3;
static v3 v3_of(u64_v v) { return v.t == U64_VEC ? (v3){v.u.v[0], v.u.v[1], v.u.v[2]} : (v3){0, 0, 0}; }
static v3 v3_add(v3 a, v3 b) { return (v3){a.x + b.x, a.y + b.y, a.z + b.z}; }
static v3 v3_sub(v3 a, v3 b) { return (v3){a.x - b.x, a.y - b.y, a.z - b.z}; }
static v3 v3_mul(v3 a, float k) { return (v3){a.x * k, a.y * k, a.z * k}; }
static float v3_dot(v3 a, v3 b) { return a.x * b.x + a.y * b.y + a.z * b.z; }
static v3 v3_norm(v3 a) { float l = sqrtf(v3_dot(a, a)); return l > 1e-6f ? v3_mul(a, 1.0f / l) : (v3){0, 0, 0}; }
static u64_v vv(v3 a) { return u64_vec(a.x, a.y, a.z); }
static v3 dir_of(v3 rot)                                             // FRotator::Vector
{
  float p = rot.x * 0.017453292f, y = rot.y * 0.017453292f;
  return (v3){cosf(p) * cosf(y), cosf(p) * sinf(y), sinf(p)};
}
static v3 rot_of_dir(v3 d)                                           // FVector::Rotation (pitch, yaw, 0)
{
  return (v3){atan2f(d.z, sqrtf(d.x * d.x + d.y * d.y)) * 57.29577951f, atan2f(d.y, d.x) * 57.29577951f, 0};
}
static v3 rotate_yaw(float yaw_deg, v3 v) { float y = yaw_deg * 0.017453292f, c = cosf(y), s = sinf(y); return (v3){v.x * c - v.y * s, v.x * s + v.y * c, v.z}; }
// AController::GetPlayerViewPoint for a hero: the active camera's world location and the control rotation. First person: the
// first-person camera (attached to the root, so actor location + its relative offset rotated by the actor's yaw); third person: the
// camera boom pivot pulled back along the view direction by TargetArmLength (no collision probe: there is no world geometry yet).
static void player_view(u64_obj *hero, v3 *loc, v3 *rot)
{
  v3 base = v3_of(*u64_ivar(hero, "Location")), ctrl = v3_of(*u64_ivar(hero, "ControlRotation")), arot = v3_of(*u64_ivar(hero, "Rotation"));
  *rot = ctrl;
  if (u64_true(*u64_ivar(hero, "bIsFirstPersonPerspective"))) {
    u64_obj *cam = O(u64_comp(hero, "FirstPersonCamera"));
    *loc = v3_add(base, rotate_yaw(arot.y, cam ? v3_of(*u64_ivar(cam, "RelativeLocation")) : (v3){0, 0, 0}));
  } else {
    u64_obj *boom = O(u64_comp(hero, "CameraBoom"));
    v3 pivot = v3_add(base, rotate_yaw(arot.y, boom ? v3_of(*u64_ivar(boom, "RelativeLocation")) : (v3){0, 0, 0}));
    float arm = boom && u64_ivar(boom, "TargetArmLength")->t == U64_FLOAT ? u64_ivar(boom, "TargetArmLength")->u.f : 300.0f;
    *loc = v3_sub(pivot, v3_mul(dir_of(ctrl), arm));
  }
}
// FRotator -> rotation matrix (columns: X, Y, Z axes), as FRotationMatrix
static void rot_mat(v3 rot, float m[9])
{
  float p = rot.x * 0.017453292f, y = rot.y * 0.017453292f, r = rot.z * 0.017453292f, cp = cosf(p), sp = sinf(p), cy = cosf(y), sy = sinf(y), cr = cosf(r), sr = sinf(r);
  float X[3] = {cp * cy, cp * sy, sp}, Y[3] = {sr * sp * cy - cr * sy, sr * sp * sy + cr * cy, -sr * cp}, Z[3] = {-(cr * sp * cy + sr * sy), cy * sr - cr * sp * sy, cr * cp};
  for (int i = 0; i < 3; i++) { m[i * 3] = X[i]; m[i * 3 + 1] = Y[i]; m[i * 3 + 2] = Z[i]; }
}
static v3 mat_mul_v(const float m[9], v3 v) { return (v3){m[0] * v.x + m[1] * v.y + m[2] * v.z, m[3] * v.x + m[4] * v.y + m[5] * v.z, m[6] * v.x + m[7] * v.y + m[8] * v.z}; }
static v3 quat_rot_v(const float q[4], v3 v)
{
  float tx = 2 * (q[1] * v.z - q[2] * v.y), ty = 2 * (q[2] * v.x - q[0] * v.z), tz = 2 * (q[0] * v.y - q[1] * v.x);
  return (v3){v.x + q[3] * tx + (q[1] * tz - q[2] * ty), v.y + q[3] * ty + (q[2] * tx - q[0] * tz), v.z + q[3] * tz + (q[0] * ty - q[1] * tx)};
}
// World location of a socket on a weapon's first-person mesh: the weapon is attached to the hero's FirstPersonMesh socket WeaponPoint (identity on b_RightWeapon) with
// WeaponMesh1PEquippedRelativeLocation; the FirstPersonMesh hangs off the FirstPersonCamera (control rotation, mesh offset / yaw -90); the bone comes from the animated pose.
extern u64_an_inst *u64_gs_anim_of(u64_obj *hero);
static int weapon_socket_world(u64_obj *wcomp, const char *socket, v3 *out)
{
  u64_obj *weapon = wcomp ? O(*u64_ivar(wcomp, "Owner")) : 0, *hero = weapon ? O(*u64_ivar(weapon, "OwningCharacter")) : 0;
  u64_an_inst *an = hero ? u64_gs_anim_of(hero) : 0; float bt[3], bq[4];
  if (!an || !u64_anim_bone_cs(an, "b_RightWeapon", bt, bq)) return 0;
  v3 local = v3_of(*u64_ivar(weapon, "WeaponMesh1PEquippedRelativeLocation"));
  const char *full = u64_obj_name(wcomp), *cn = strrchr(full, '.'); cn = cn ? cn + 1 : full;
  const char *mesh = 0; for (const u64_weapon_mesh *wm = u64_weapon_meshes; wm->cls; wm++) if (u64_class_is(u64_obj_class(weapon), wm->cls) && !strcmp(wm->comp, cn)) { mesh = wm->mesh; break; }
  for (const u64_mesh_socket *ms = u64_mesh_sockets; mesh && ms->mesh; ms++) if (!strcmp(ms->mesh, mesh) && !strcmp(ms->socket, socket)) { local = v3_add(local, (v3){ms->t[0], ms->t[1], ms->t[2]}); break; }
  v3 pm = v3_add((v3){bt[0], bt[1], bt[2]}, quat_rot_v(bq, local));                       // in the first-person mesh's space
  v3 base = v3_of(*u64_ivar(hero, "Location")), ctrl = v3_of(*u64_ivar(hero, "ControlRotation")), arot = v3_of(*u64_ivar(hero, "Rotation"));
  u64_obj *cam = O(u64_comp(hero, "FirstPersonCamera")), *fpm = O(u64_comp(hero, "FirstPersonMesh"));
  v3 cam_loc = v3_add(base, rotate_yaw(arot.y, cam ? v3_of(*u64_ivar(cam, "RelativeLocation")) : (v3){0, 0, 0}));
  v3 mrel = fpm && u64_ivar(fpm, "RelativeLocation")->t == U64_VEC ? v3_of(*u64_ivar(fpm, "RelativeLocation")) : (v3){0, 0, -149.5f};
  v3 mrot = fpm && u64_ivar(fpm, "RelativeRotation")->t == U64_VEC ? v3_of(*u64_ivar(fpm, "RelativeRotation")) : (v3){0, -90.0f, 0};
  float rc[9], rm[9], rw[9]; rot_mat(ctrl, rc); rot_mat(mrot, rm);
  for (int i = 0; i < 3; i++) for (int j = 0; j < 3; j++) rw[i * 3 + j] = rc[i * 3] * rm[j] + rc[i * 3 + 1] * rm[3 + j] + rc[i * 3 + 2] * rm[6 + j];   // R_ctrl * R_meshrel
  *out = v3_add(v3_add(cam_loc, mat_mul_v(rc, mrel)), mat_mul_v(rw, pm));
  return 1;
}
int u64_gs_muzzle_world(u64_obj *hero, float out[3])             // the current weapon's MuzzleFlashSocket (first-person mesh), for tests and the HUD
{
  u64_obj *w = O(u64_call(hero, "GetCurrentWeapon", 0, 0)); v3 p;
  if (!w || !weapon_socket_world(O(u64_comp(w, "WeaponMesh1P")), "MuzzleFlashSocket", &p)) return 0;
  out[0] = p.x; out[1] = p.y; out[2] = p.z; return 1;
}
// FGameplayAbilityTargetingLocationInfo::GetTargetingTransform().GetLocation(): literal transform, the source actor, or a socket on a component
static v3 targeting_location(u64_v info, u64_obj *source_actor)
{
  u64_obj *io = O(info);
  if (!io) return v3_of(source_actor ? *u64_ivar(source_actor, "Location") : NONE_);
  int type = u64_as_int(*u64_ivar(io, "LocationType"));
  if (type == 0) { u64_obj *lt = O(*u64_ivar(io, "LiteralTransform")); return lt ? v3_of(*u64_ivar(lt, "Location")) : (v3){0, 0, 0}; }
  if (type == 2) {
    u64_obj *comp = O(*u64_ivar(io, "SourceComponent")), *owner = comp ? O(*u64_ivar(comp, "Owner")) : 0;
    v3 w; if (comp && weapon_socket_world(comp, nm(*u64_ivar(io, "SourceSocketName")), &w)) return w;       // the animated socket
    u64_obj *wc = owner ? O(*u64_ivar(owner, "OwningCharacter")) : 0;           // fallback: a carried weapon is attached to its character
    if (wc) owner = wc;
    if (owner) return v3_of(*u64_ivar(owner, "Location"));
  }
  return v3_of(source_actor ? *u64_ivar(source_actor, "Location") : NONE_);
}
static uint32_t gs_rng = 0x1234567u;
static float frand(void) { gs_rng = gs_rng * 1664525u + 1013904223u; return (float)(gs_rng >> 8) / 16777216.0f; }
static v3 rand_cone(v3 dir, float half)                              // FRandomStream::VRandCone with equal half angles: uniform over the cone's solid angle
{
  if (half <= 0) return dir;
  float u = frand(), v = frand(), theta = 6.2831853f * u, cz = 1.0f - v * (1.0f - cosf(half)), sz = sqrtf(1.0f - cz * cz);
  v3 up = fabsf(dir.z) < 0.999f ? (v3){0, 0, 1} : (v3){1, 0, 0};
  v3 sx = v3_norm((v3){dir.y * up.z - dir.z * up.y, dir.z * up.x - dir.x * up.z, dir.x * up.y - dir.y * up.x}), sy = (v3){dir.y * sx.z - dir.z * sx.y, dir.z * sx.x - dir.x * sx.z, dir.x * sx.y - dir.y * sx.x};
  return v3_norm(v3_add(v3_mul(dir, cz), v3_add(v3_mul(sx, sz * cosf(theta)), v3_mul(sy, sz * sinf(theta)))));
}
static float current_spread(u64_obj *s)                              // AGSGATA_Trace::GetCurrentSpread
{
  float f = fv(*u64_ivar(s, "BaseSpread")) + fv(*u64_ivar(s, "CurrentTargetingSpread"));
  u64_obj *ab = O(*u64_ivar(s, "OwningAbility")), *asc = ab ? O(*u64_ivar(ab, "ASC")) : 0;
  const char *at = nm(*u64_ivar(s, "AimingTag")), *rt = nm(*u64_ivar(s, "AimingRemovalTag"));
  if (u64_true(*u64_ivar(s, "bUseAimingSpreadMod")) && *at && *rt && asc && u64_tag_count(asc, at) > u64_tag_count(rt ? asc : 0, rt)) f *= fv(*u64_ivar(s, "AimingSpreadMod"));
  return f;
}
// LineTraceWithFilter: LineTraceMultiByProfile with the actor's TraceProfile; HitResult objects (the filter handle only drops the source actor here)
static int gs_line_hits(u64_obj *s, const float st[3], const float en[3], u64_obj *source, u64_obj **hits, int max)
{
  u64_world_hit_t wh[16]; int n = u64_trace_profile(st, en, nm(*u64_ivar(s, "TraceProfile")), source, wh, max < 16 ? max : 16);
  for (int i = 0; i < n; i++) hits[i] = u64_hit_result_new(&wh[i], st, en);
  return n;
}
// One trace of AGSGATA_Trace::PerformTrace. Returns the number of hit results (objects in hits[]).
static int one_trace(u64_obj *s, u64_obj *source, v3 trace_start, u64_obj **hits, int max)
{
  float range = fv(*u64_ivar(s, "MaxRange"));
  v3 view_start = trace_start, view_rot = rot_of_dir((v3){1, 0, 0});
  u64_obj *hero = is_class(source, "GSHeroCharacter") ? source : 0;
  if (hero && u64_player_pawn() == hero) player_view(hero, &view_start, &view_rot);   // MasterPC->GetPlayerViewPoint
  else if (hero) view_rot = v3_of(*u64_ivar(hero, "ControlRotation"));
  v3 view_dir = dir_of(view_rot), view_end = v3_add(view_start, v3_mul(view_dir, range));
  { // ClipCameraRayToAbilityRange: when the camera is inside the ability's range sphere, end the view ray on the sphere
    v3 to_center = v3_sub(trace_start, view_start); float dot = v3_dot(to_center, view_dir);
    if (dot >= 0) { float d2 = v3_dot(to_center, to_center) - dot * dot, r2 = range * range;
      if (d2 <= r2) view_end = v3_add(view_start, v3_mul(view_dir, dot + sqrtf(r2 - d2))); }
  }
  u64_obj *probe[16]; float vs[3] = {view_start.x, view_start.y, view_start.z}, ve[3] = {view_end.x, view_end.y, view_end.z};
  int pn = gs_line_hits(s, vs, ve, source, probe, 16);
  float inc = fv(*u64_ivar(s, "TargetingSpreadIncrement")), mx = fv(*u64_ivar(s, "TargetingSpreadMax")), cur = fv(*u64_ivar(s, "CurrentTargetingSpread"));
  *u64_ivar(s, "CurrentTargetingSpread") = u64_float(cur + inc < mx ? cur + inc : mx);
  int use = 0; v3 adj_end = view_end;
  if (pn > 0) { v3 hl = v3_of(*u64_ivar(probe[0], "Location")); v3 dd = v3_sub(trace_start, hl); if (v3_dot(dd, dd) <= range * range) { use = 1; adj_end = hl; } }
  v3 aim = v3_norm(v3_sub(adj_end, trace_start));
  if (aim.x == 0 && aim.y == 0 && aim.z == 0) aim = view_dir;
  if (!u64_true(*u64_ivar(s, "bTraceAffectsAimPitch")) && use) {
    v3 orig = v3_norm(v3_sub(view_end, trace_start));
    if (orig.x != 0 || orig.y != 0 || orig.z != 0) { v3 ar = rot_of_dir(aim), orr = rot_of_dir(orig); ar.x = orr.x; aim = dir_of(ar); }
  }
  float half = current_spread(s) * 0.5f * 0.017453292f;
  v3 shoot = rand_cone(aim, half), end = v3_add(trace_start, v3_mul(shoot, range));
  *u64_ivar(s, "CurrentTraceEnd") = vv(end);
  float ts[3] = {trace_start.x, trace_start.y, trace_start.z}, te[3] = {end.x, end.y, end.z};
  float radius = is_class(s, "GSGATA_SphereTrace") ? fv(*u64_ivar(s, "TraceSphereRadius")) : 0.0f;
  int n = radius > 0 ? u64_trace_multi(ts, te, radius, source, hits, max)            // sphere sweeps: the characters' capsules
                     : gs_line_hits(s, ts, te, source, hits, max);                      // DoTrace + the filter: TraceStart / TraceEnd are recorded on each hit
  for (int i = 0; i < n; i++) { *u64_ivar(hits[i], "TraceStart") = vv(trace_start); *u64_ivar(hits[i], "TraceEnd") = vv(end); }
  return n;
}
static u64_v tr_perform_trace(u64_obj *s, int c, const u64_v *a)    // PerformTrace(SourceActor) -> hit results (array)
{
  u64_obj *source = O(A(c, a, 0)); if (!source) source = O(*u64_ivar(s, "SourceActor"));
  u64_v out = u64_array_of(0, 0);
  v3 start = targeting_location(*u64_ivar(s, "StartLocation"), source);
  u64_obj *hero = is_class(source, "GSHeroCharacter") ? source : 0;
  if (hero && u64_player_pawn() == hero && u64_true(*u64_ivar(s, "bTraceFromPlayerViewPoint"))) { v3 vl, vr; player_view(hero, &vl, &vr); start = vl; }
  int traces = u64_as_int(*u64_ivar(s, "NumberOfTraces")), per = u64_as_int(*u64_ivar(s, "MaxHitResultsPerTrace"));
  for (int t = 0; t < traces; t++) {
    u64_obj *hits[16];
    int n = one_trace(s, source, start, hits, 16);
    for (int j = 0; j < n; j++) { if (per >= 0 && j + 1 > per) continue; u64_array_push(&out, u64_objv(hits[j])); }     // MaxHitResultsPerTrace
  }
  return out;
}
static u64_v tr_start_targeting(u64_obj *s, int c, const u64_v *a)  // StartTargeting(Ability)
{
  u64_obj *ab = O(A(c, a, 0));
  *u64_ivar(s, "OwningAbility") = u64_objv(ab); *u64_ivar(s, "SourceActor") = u64_objv(ab ? O(*u64_ivar(ab, "Avatar")) : 0);
  return NONE_;
}
static u64_v tr_confirm(u64_obj *s, int c, const u64_v *a)           // ConfirmTargetingAndContinue: trace, MakeTargetData, TargetDataReadyDelegate
{
  (void)c; (void)a;
  u64_obj *source = O(*u64_ivar(s, "SourceActor"));
  if (!source) return NONE_;
  u64_v hits = tr_perform_trace(s, 0, 0), handle = u64_td_handle_from_hits(hits);
  u64_target_data_ready(s, handle);
  return NONE_;
}
static u64_v tr_stop_targeting(u64_obj *s, int c, const u64_v *a) { (void)c; (void)a; *u64_ivar(s, "WaitingTask") = u64_objv(0); return NONE_; }

// ---------------------------------------------------------------------------------------------------- UGSGameplayAbility
static void ctor_gs_jump(u64_obj *o)                                 // UGSGA_CharacterJump::UGSGA_CharacterJump
{
  *u64_ivar(o, "AbilityInputID") = u64_name("EGSAbilityInputID::Jump");
  tag_push(o, "AbilityTags", "Ability.Jump");
  u64_v *ow = u64_ivar(o, "ActivationOwnedTags"); u64_v keep = u64_array_of(0, 0);                 // ActivationOwnedTags.RemoveTag(Ability.BlocksInteraction)
  for (int i = 0; i < u64_array_len(*ow); i++) if (strcmp(nm(u64_array_get(*ow, i)), "Ability.BlocksInteraction")) u64_array_push(&keep, u64_array_get(*ow, i));
  *ow = keep;
}
static void ctor_gs_ability(u64_obj *o)                              // UGSGameplayAbility::UGSGameplayAbility
{
  *u64_ivar(o, "bActivateAbilityOnGranted") = u64_bool(0); *u64_ivar(o, "bActivateOnInput") = u64_bool(1);
  *u64_ivar(o, "bSourceObjectMustEqualCurrentWeaponToActivate") = u64_bool(0); *u64_ivar(o, "bCannotActivateWhileInteracting") = u64_bool(1);
  tag_push(o, "ActivationBlockedTags", "State.Dead"); tag_push(o, "ActivationBlockedTags", "State.KnockedDown");
  tag_push(o, "ActivationOwnedTags", "Ability.BlocksInteraction");
  *u64_ivar(o, "InteractingTag") = u64_name("State.Interacting"); *u64_ivar(o, "InteractingRemovalTag") = u64_name("State.InteractingRemoval");
}
static int gs_can_activate(u64_obj *ab)                              // UGSGameplayAbility::CanActivateAbility (before Super)
{
  if (!is_class(ab, "GSGameplayAbility")) return 1;
  u64_obj *asc = O(*u64_ivar(ab, "ASC"));
  if (u64_true(*u64_ivar(ab, "bCannotActivateWhileInteracting")) && u64_tag_count(asc, "State.Interacting") > u64_tag_count(asc, "State.InteractingRemoval")) return 0;
  if (u64_true(*u64_ivar(ab, "bSourceObjectMustEqualCurrentWeaponToActivate"))) {
    u64_obj *hero = O(*u64_ivar(ab, "Avatar"));
    u64_obj *w = is_class(hero, "GSHeroCharacter") ? O(*u64_ivar(hero, "CurrentWeapon")) : 0;
    return w && w == O(*u64_ivar(ab, "SourceObject"));
  }
  return 1;
}
static int gs_check_cost(u64_obj *ab)                                // CheckCost: Super && GSCheckCost (Blueprint event, default true)
{
  if (!is_class(ab, "GSGameplayAbility") || !u64_has_bp(ab, "GSCheckCost")) return 1;
  u64_v r = u64_call(ab, "GSCheckCost", 0, 0);
  return r.t == U64_NONE ? 1 : u64_true(r);
}
static void gs_apply_cost(u64_obj *ab)                               // ApplyCost: GSApplyCost first, then Super (the cost effect)
{
  if (is_class(ab, "GSGameplayAbility") && u64_has_bp(ab, "GSApplyCost")) u64_call(ab, "GSApplyCost", 0, 0);
}
static u64_v ga_make_container_spec(u64_obj *ab, int c, const u64_v *a)   // MakeEffectContainerSpec(ContainerTag, EventData, OverrideGameplayLevel)
{
  const char *tag = nm(A(c, a, 0));
  u64_obj *spec = u64_new("EffectContainerSpec", "GSGameplayEffectContainerSpec");
  *u64_ivar(spec, "TargetData") = u64_array_of(0, 0); *u64_ivar(spec, "TargetGameplayEffectSpecs") = u64_array_of(0, 0);
  u64_v map = *u64_ivar(ab, "EffectContainerMap"), container = NONE_;
  for (int i = 0; i + 1 < u64_array_len(map); i += 2) if (!strcmp(nm(u64_array_get(map, i)), tag)) { container = u64_array_get(map, i + 1); break; }
  if (container.t != U64_ARRAY) return u64_objv(spec);                  // not found: an empty spec
  u64_v target_type = NONE_, effects = NONE_;
  for (int i = 0; i + 1 < u64_array_len(container); i += 2) {
    const char *k = nm(u64_array_get(container, i));
    if (!strcmp(k, "TargetType")) target_type = u64_array_get(container, i + 1);
    else if (!strcmp(k, "TargetGameplayEffectClasses")) effects = u64_array_get(container, i + 1);
  }
  u64_obj *hero = O(*u64_ivar(ab, "Avatar"));
  if (strcmp(nm(target_type), "None") && *nm(target_type)) {                // TargetTypeCDO->GetTargets(...)
    u64_v hits = u64_array_of(0, 0), actors = u64_array_of(0, 0);
    const char *tt = nm(target_type);
    if (!strcmp(tt, "GSTargetType_UseOwner")) u64_array_push(&actors, u64_objv(hero));                       // OutActors.Add(TargetingCharacter)
    else if (!strcmp(tt, "GSTargetType_UseEventData")) {
      u64_obj *ev = O(A(c, a, 1)), *ctx = ev ? O(*u64_ivar(ev, "ContextHandle")) : 0, *hit = ctx ? O(*u64_ivar(ctx, "HitResult")) : 0;
      if (hit) u64_array_push(&hits, u64_objv(hit)); else if (ev && O(*u64_ivar(ev, "Target"))) u64_array_push(&actors, *u64_ivar(ev, "Target"));
    }
    u64_call(ab, "AddTargetsToEffectContainerSpec", 4, (u64_v[]){u64_objv(spec), u64_array_of(0, 0), hits, actors});
  }
  int level = A(c, a, 2).t == U64_NONE ? -1 : u64_as_int(A(c, a, 2));
  if (level == -1) level = u64_ivar(ab, "Level")->t == U64_NONE ? 1 : u64_as_int(*u64_ivar(ab, "Level"));
  u64_v *specs = u64_ivar(spec, "TargetGameplayEffectSpecs");
  for (int i = 0; i < u64_array_len(effects); i++) {
    u64_v s = u64_call(ab, "MakeOutgoingGameplayEffectSpec", 2, (u64_v[]){u64_array_get(effects, i), u64_int(level)});
    u64_array_push(specs, s);
  }
  return u64_objv(spec);
}
static u64_v ga_gs_check_cost_default(u64_obj *ab, int c, const u64_v *a) { (void)ab; (void)c; (void)a; return u64_bool(1); }   // GSCheckCost_Implementation
static u64_v ga_noop(u64_obj *ab, int c, const u64_v *a) { (void)ab; (void)c; (void)a; return NONE_; }                          // GSApplyCost_Implementation
static u64_v ga_on_avatar_set(u64_obj *ab, int c, const u64_v *a)      // UGSGameplayAbility::OnAvatarSet: passive abilities activate when granted
{
  (void)c; (void)a;
  if (u64_true(*u64_ivar(ab, "bActivateAbilityOnGranted"))) u64_ability_try_activate(ab);
  return NONE_;
}

// ---------------------------------------------------------------------------------------------------- controllers and possession
// A controller object per pawn: the player controller (PlayerController class from the game mode's BP) or an AI controller.
static u64_v h_possessed_by(u64_obj *s, int c, const u64_v *a)         // AGSHeroCharacter::PossessedBy (the parts that matter without a PlayerState)
{
  (void)c; (void)a;
  int player = u64_true(*u64_ivar(s, "IsPlayer"));
  u64_obj *pc = u64_new_copy(player ? "PlayerController" : "AIController", player ? "BP_PlayerController" : "GSHeroAIController");
  *u64_ivar(pc, "Pawn") = u64_objv(s); *u64_ivar(s, "Controller") = u64_objv(pc);
  u64_obj *asc = u64_asc_of(s);
  if (asc) u64_remove_ge_with_tags(asc, "State.Dead");                // RemoveActiveEffectsWithGrantedTags(DeadTag)
  if (player) *u64_ivar(s, "bIsFirstPersonPerspective") = *u64_ivar(s, "bStartInFirstPersonPerspective");     // SetupStartupPerspective
  return NONE_;
}

// ---------------------------------------------------------------------------------------------------- AGSPickup and weapons lying in the world
extern void u64_gc_roots_project_extra(void);
static void ctor_pickup(u64_obj *o)                                  // AGSPickup::AGSPickup
{
  *u64_ivar(o, "bIsActive") = u64_bool(1); *u64_ivar(o, "bCanRespawn") = u64_bool(1); *u64_ivar(o, "RespawnTime") = u64_float(5.0f);
  u64_obj *c = O(u64_comp(o, "CollisionComp"));                       // CollisionComp->InitCapsuleSize(40, 50)
  if (c) { *u64_ivar(c, "CapsuleRadius") = u64_float(40.0f); *u64_ivar(c, "CapsuleHalfHeight") = u64_float(50.0f); }
  tag_push(o, "RestrictedPickupTags", "State.Dead"); tag_push(o, "RestrictedPickupTags", "State.KnockedDown");
}
static int has_any_tag(u64_obj *asc, u64_v tags) { for (int i = 0; i < u64_array_len(tags); i++) if (u64_tag_count(asc, nm(u64_array_get(tags, i)))) return 1; return 0; }
static int can_be_picked_up(u64_obj *pk, u64_obj *ch)                // AGSPickup::CanBePickedUp
{
  if (!u64_true(*u64_ivar(pk, "bIsActive")) || !ch || !is_alive(ch) || !u64_obj_alive(pk)) return 0;
  if (has_any_tag(u64_asc_of(ch), *u64_ivar(pk, "RestrictedPickupTags"))) return 0;
  if (u64_has_bp(pk, "K2_CanBePickedUp")) { u64_v r = u64_call(pk, "K2_CanBePickedUp", 1, (u64_v[]){u64_objv(ch)}); return r.t == U64_NONE || u64_true(r); }
  return 1;
}
static void give_pickup_to(u64_obj *pk, u64_obj *ch)                 // AGSPickup::GivePickupTo: abilities (activated once), then effects at the character's level
{
  u64_obj *asc = u64_asc_of(ch);
  if (!asc) return;
  u64_v abil = *u64_ivar(pk, "AbilityClasses"), effects = *u64_ivar(pk, "EffectClasses");
  for (int i = 0; i < u64_array_len(abil); i++) {
    const char *cls = nm(u64_array_get(abil, i));
    if (!*cls) continue;
    u64_obj *def = class_default(cls);
    u64_obj *ab = u64_give_ability(asc, cls, 1, def ? nm(*u64_ivar(def, "AbilityInputID")) : "", pk);
    if (ab) { *u64_ivar(ab, "ClearOnEnd") = u64_bool(1); u64_ability_try_activate(ab); }
  }
  float level = u64_attr_get(asc, "CharacterLevel");
  for (int i = 0; i < u64_array_len(effects); i++) { const char *ge = nm(u64_array_get(effects, i)); if (*ge) u64_apply_ge(asc, asc, ge, level, 0); }
}
static u64_v pk_pickup_on_touch(u64_obj *pk, int c, const u64_v *a)  // AGSPickup::PickupOnTouch(Pawn)
{
  u64_obj *ch = O(A(c, a, 0));
  if (!can_be_picked_up(pk, ch)) return NONE_;
  give_pickup_to(pk, ch);
  *u64_ivar(pk, "PickedUpBy") = u64_objv(ch); *u64_ivar(pk, "bIsActive") = u64_bool(0);
  if (u64_has_bp(pk, "K2_OnPickedUp")) u64_call(pk, "K2_OnPickedUp", 0, 0);                    // OnPickedUp: the Blueprint hides the mesh; the sound is presentation
  if (u64_true(*u64_ivar(pk, "bCanRespawn")) && fv(*u64_ivar(pk, "RespawnTime")) > 0) u64_timer_call(fv(*u64_ivar(pk, "RespawnTime")), pk, "RespawnPickup", NONE_);
  else u64_obj_destroy(pk);
  return NONE_;
}
static u64_v pk_on_overlap(u64_obj *pk, int c, const u64_v *a)       // AGSPickup::NotifyActorBeginOverlap (the server)
{
  u64_obj *other = O(A(c, a, 0));
  if (other && other != pk) pk_pickup_on_touch(pk, 1, (u64_v[]){u64_objv(other)});
  return NONE_;
}
static int capsules_overlap(u64_obj *a, u64_obj *b)                  // two vertical capsules: segment distance <= radius sum (exact)
{
  float ax, ay, az, ar, ah, bx, by, bz, br, bh;
  if (!u64_actor_capsule(a, &ax, &ay, &az, &ar, &ah) || !u64_actor_capsule(b, &bx, &by, &bz, &br, &bh)) return 0;
  float sa = ah - ar, sb = bh - br; if (sa < 0) sa = 0; if (sb < 0) sb = 0;
  float gap = fmaxf(0.f, fmaxf((bz - sb) - (az + sa), (az - sa) - (bz + sb))), dh2 = (ax - bx) * (ax - bx) + (ay - by) * (ay - by), rr = ar + br;
  return dh2 + gap * gap <= rr * rr;
}
static u64_v pk_respawn(u64_obj *pk, int c, const u64_v *a)          // AGSPickup::RespawnPickup: active again; whoever is standing on it picks it up
{
  (void)c; (void)a;
  *u64_ivar(pk, "bIsActive") = u64_bool(1); *u64_ivar(pk, "PickedUpBy") = u64_objv(0);
  if (u64_has_bp(pk, "K2_OnRespawned")) u64_call(pk, "K2_OnRespawned", 0, 0);
  for (int i = 0; i < u64_obj_count(); i++) { u64_obj *p = u64_obj_at(i); if (u64_obj_is_actor(p) && u64_actor_is_pawn(p) && capsules_overlap(pk, p)) pk_pickup_on_touch(pk, 1, (u64_v[]){u64_objv(p)}); }
  return NONE_;
}
// AGSWeapon::NotifyActorBeginOverlap / PickUpOnTouch for a weapon lying in the world
static u64_v w_pickup_on_touch(u64_obj *w, int c, const u64_v *a)
{
  u64_obj *ch = O(A(c, a, 0));
  if (!ch || !is_class(ch, "GSHeroCharacter") || !is_alive(ch) || !u64_asc_of(ch) || has_any_tag(u64_asc_of(ch), *u64_ivar(w, "RestrictedPickupTags"))) return NONE_;
  u64_v r = u64_call(ch, "AddWeaponToInventory", 2, (u64_v[]){u64_objv(w), u64_bool(1)});
  (void)r;
  return NONE_;
}
// Begin-overlap notifications between pickup-mode actors and pawns, tracked per pair so each fires once on entering.
#define MAX_TOUCH 96
static struct { u64_obj *a, *b; } touching[MAX_TOUCH]; static int ntouch;
static int touch_find(u64_obj *a, u64_obj *b) { for (int i = 0; i < ntouch; i++) if (touching[i].a == a && touching[i].b == b) return i; return -1; }
// First-person hero animation: one ABP_Hero1P instance per hero, updated every tick (BlueprintUpdateAnimation + state machine + pose of the weapon socket chain).
static struct { u64_obj *hero; u64_an_inst *a; u64_obj *inst; } anims[8];
static void anim_tick(float dt)
{
  const u64_an_graph *g = u64_anim_find_graph("ABP_Hero1P");
  if (!g) return;
  for (int i = 0; i < 8; i++) if (anims[i].a && !u64_obj_alive(anims[i].hero)) { u64_anim_free(anims[i].a); memset(&anims[i], 0, sizeof anims[i]); }
  for (int j = 0; j < u64_obj_count(); j++) {
    u64_obj *h = u64_obj_at(j);
    if (!u64_obj_alive(h) || !u64_obj_is_actor(h) || !is_class(h, "GSHeroCharacter")) continue;
    int k = -1, free_ = -1;
    for (int i = 0; i < 8; i++) { if (anims[i].a && anims[i].hero == h) k = i; if (!anims[i].a && free_ < 0) free_ = i; }
    if (k < 0) {
      if (free_ < 0) continue;
      u64_v cm = u64_comp(h, "CharacterMovement"); *u64_ivar(h, "CharacterMovement") = cm;
      if (u64_ivar(cm.u.o, "MaxWalkSpeed")->t != U64_FLOAT) *u64_ivar(cm.u.o, "MaxWalkSpeed") = u64_float(600.0f);          // UCharacterMovementComponent default (the Blueprint sets none)
      u64_obj *inst = u64_new("ABP_Hero1P_inst", "ABP_Hero1P"); *u64_ivar(inst, "Pawn") = u64_objv(h); *u64_ivar(h, "AnimInstance1P") = u64_objv(inst);
      anims[free_].hero = h; anims[free_].inst = inst; anims[free_].a = u64_anim_new(g, inst); k = free_;
    }
    if (anims[k].a) u64_anim_update(anims[k].a, dt);
  }
}
void u64_cpp_montage_hook(u64_obj *actor, const char *name, float rate)       // PlayMontage on the first-person mesh: the AnimBP's slot blends it in
{
  for (int i = 0; i < 8; i++) if (anims[i].a && anims[i].hero == actor) u64_anim_montage_play(anims[i].a, name, rate);
}
u64_an_inst *u64_gs_anim_of(u64_obj *hero) { for (int i = 0; i < 8; i++) if (anims[i].a && anims[i].hero == hero) return anims[i].a; return 0; }
static void interact_tick(void);
static void gs_world_tick(float dt)
{
  anim_tick(dt);
  interact_tick();
  for (int i = 0; i < u64_obj_count(); i++) {
    u64_obj *o = u64_obj_at(i);
    if (!u64_obj_is_actor(o)) continue;
    int pickup = is_class(o, "GSPickup"), weapon = is_class(o, "GSWeapon") && !O(*u64_ivar(o, "OwningCharacter")) && u64_true(*u64_ivar(o, "PickupEnabled"));
    if (!pickup && !weapon) continue;
    for (int j = 0; j < u64_obj_count(); j++) {
      u64_obj *p = u64_obj_at(j);
      if (!u64_obj_is_actor(p) || !u64_actor_is_pawn(p)) continue;
      int now = capsules_overlap(o, p), idx = touch_find(o, p);
      if (now && idx < 0 && ntouch < MAX_TOUCH) { touching[ntouch].a = o; touching[ntouch++].b = p; if (pickup) pk_on_overlap(o, 1, (u64_v[]){u64_objv(p)}); else w_pickup_on_touch(o, 1, (u64_v[]){u64_objv(p)}); }
      else if (!now && idx >= 0) touching[idx] = touching[--ntouch];
      if (!u64_obj_alive(o)) break;
    }
  }
  for (int i = 0; i < ntouch; ) { if (!u64_obj_alive(touching[i].a) || !u64_obj_alive(touching[i].b)) touching[i] = touching[--ntouch]; else i++; }
}

// ---------------------------------------------------------------------------------------------------- IGSInteractable + UGSAT_WaitInteractableTarget
// Interface defaults (IGSInteractable): nothing is interactable until a class overrides it. Interacters are stored as pairs
// [component, actor, ...] in the "Interacters" variable.
static u64_v ia_false(u64_obj *s, int c, const u64_v *a) { (void)s; (void)c; (void)a; return u64_bool(0); }
static u64_v ia_zero(u64_obj *s, int c, const u64_v *a) { (void)s; (void)c; (void)a; return u64_float(0.0f); }
static u64_v ia_sync_type(u64_obj *s, int c, const u64_v *a)             // GetPreInteractSyncType / GetPostInteractSyncType(&bShouldSync, &Type, Component)
{ (void)s; u64_out(A(c, a, 0), u64_bool(0)); u64_out(A(c, a, 1), u64_int(0)); return NONE_; }                      // false, OnlyServerWait
static u64_v ia_register(u64_obj *s, int c, const u64_v *a)              // RegisterInteracter(Component, InteractingActor)
{
  u64_v *list = u64_ivar(s, "Interacters");
  for (int i = 0; i + 1 < u64_array_len(*list); i += 2) if (O(u64_array_get(*list, i)) == O(A(c, a, 0)) && O(u64_array_get(*list, i + 1)) == O(A(c, a, 1))) return NONE_;
  u64_array_push(list, A(c, a, 0)); u64_array_push(list, A(c, a, 1));
  return NONE_;
}
static u64_v ia_unregister(u64_obj *s, int c, const u64_v *a)
{
  u64_v *list = u64_ivar(s, "Interacters"), keep[128]; int n = 0;
  for (int i = 0; i + 1 < u64_array_len(*list) && n + 2 <= 128; i += 2) {
    if (O(u64_array_get(*list, i)) == O(A(c, a, 0)) && O(u64_array_get(*list, i + 1)) == O(A(c, a, 1))) continue;
    keep[n++] = u64_array_get(*list, i); keep[n++] = u64_array_get(*list, i + 1);
  }
  *list = u64_array_of(n, keep);
  return NONE_;
}
static u64_v ia_cancel(u64_obj *s, int c, const u64_v *a)                // InteractableCancelInteraction(Component): cancel Ability.Interaction on everyone interacting with it
{
  u64_v *list = u64_ivar(s, "Interacters"), keep[128]; int n = 0;
  for (int i = 0; i + 1 < u64_array_len(*list) && n + 2 <= 128; i += 2) {
    if (O(u64_array_get(*list, i)) != O(A(c, a, 0))) { keep[n++] = u64_array_get(*list, i); keep[n++] = u64_array_get(*list, i + 1); continue; }
    u64_obj *asc = u64_asc_of(O(u64_array_get(*list, i + 1)));
    if (asc) u64_cancel_abilities(asc, "Ability.Interaction");
  }
  *list = u64_array_of(n, keep);
  return NONE_;
}
// AGSHeroCharacter implements it for reviving a knocked down hero
static u64_v h_ia_available(u64_obj *s, int c, const u64_v *a)
{
  (void)c; (void)a; u64_obj *asc = u64_asc_of(s);
  return u64_bool(asc && u64_tag_count(asc, "State.KnockedDown") && !u64_tag_count(asc, "State.Interacting"));
}
static u64_v h_ia_duration(u64_obj *s, int c, const u64_v *a)
{ (void)c; (void)a; u64_obj *asc = u64_asc_of(s); return asc && u64_tag_count(asc, "State.KnockedDown") ? *u64_ivar(s, "ReviveDuration") : u64_float(0.0f); }
static u64_v h_ia_pre(u64_obj *s, int c, const u64_v *a)                 // PreInteract: start the Revive ability
{
  (void)c; (void)a; u64_obj *asc = u64_asc_of(s);
  if (asc && u64_tag_count(asc, "State.KnockedDown")) u64_call(asc, "TryActivateAbilitiesByTag", 1, (u64_v[]){u64_array_of(1, (u64_v[]){u64_name("Ability.Revive")})});
  return NONE_;
}
static u64_v h_ia_post(u64_obj *s, int c, const u64_v *a)                // PostInteract: the Revive effect
{
  (void)c; (void)a; u64_obj *asc = u64_asc_of(s);
  if (asc && u64_tag_count(asc, "State.KnockedDown")) u64_apply_ge(asc, asc, nm(*u64_ivar(s, "ReviveEffect")), 1.0f, 0);
  return NONE_;
}
static u64_v h_ia_sync_type(u64_obj *s, int c, const u64_v *a)
{
  u64_obj *asc = u64_asc_of(s);
  if (asc && u64_tag_count(asc, "State.KnockedDown")) { u64_out(A(c, a, 0), u64_bool(1)); u64_out(A(c, a, 1), u64_int(1)); return NONE_; }   // true, OnlyClientWait
  return ia_sync_type(s, c, a);
}
static u64_v h_ia_cancel_interaction(u64_obj *s, int c, const u64_v *a)  // CancelInteraction: cancel the Revive ability of a knocked down hero
{
  (void)c; (void)a; u64_obj *asc = u64_asc_of(s);
  if (asc && u64_tag_count(asc, "State.KnockedDown")) u64_cancel_abilities(asc, "Ability.Revive");
  return NONE_;
}

typedef struct { u64_obj *actor, *comp; float t, loc[3]; int blocking; } ihit_t;
// A trace by profile (UWorld::LineTraceMultiByProfile): the world query of u64_collision.c, hits sorted by distance up to the first blocking one.
static int profile_hits(const float s[3], const float e[3], const char *profile, u64_obj *ignore, ihit_t *out, int max)
{
  u64_world_hit_t wh[16]; int nw = u64_trace_profile(s, e, profile, ignore, wh, max < 16 ? max : 16);
  for (int i = 0; i < nw; i++) { out[i].actor = wh[i].actor; out[i].comp = wh[i].comp; out[i].t = wh[i].t; memcpy(out[i].loc, wh[i].loc, sizeof out[i].loc); out[i].blocking = wh[i].blocking; }
  return nw;
}
static int implements_interactable(u64_obj *actor) { return actor && (u64_has_bp(actor, "IsAvailableForInteraction") || is_class(actor, "GSHeroCharacter")); }
static u64_obj *hit_object(const ihit_t *h, v3 start, v3 end, int blocking)
{
  u64_obj *r = u64_new("HitResult", "HitResult");
  *u64_ivar(r, "HitActor") = u64_objv(h ? h->actor : 0); *u64_ivar(r, "HitComponent") = u64_objv(h ? h->comp : 0);
  *u64_ivar(r, "Location") = h ? u64_vec(h->loc[0], h->loc[1], h->loc[2]) : vv(end); *u64_ivar(r, "Time") = u64_float(h ? h->t : 1.0f);
  *u64_ivar(r, "bBlockingHit") = u64_bool(blocking); *u64_ivar(r, "TraceStart") = vv(start); *u64_ivar(r, "TraceEnd") = h && !blocking ? u64_vec(h->loc[0], h->loc[1], h->loc[2]) : vv(end);
  return r;
}
// UGSAT_WaitInteractableTarget::LineTrace(OutHitResult, Start, End, bLookForInteractableActor)
static u64_obj *it_line_trace(u64_obj *task, v3 start, v3 end, int look)
{
  u64_obj *ab = O(*u64_ivar(task, "Ability")), *avatar = ab ? O(*u64_ivar(ab, "Avatar")) : 0;
  float s3[3] = {start.x, start.y, start.z}, e3[3] = {end.x, end.y, end.z};
  ihit_t hits[16]; int n = profile_hits(s3, e3, nm(*u64_ivar(task, "TraceProfile")), avatar, hits, 16);
  u64_obj *res = hit_object(0, start, end, 0);
  for (int i = 0; i < n; i++) {
    if (hits[i].actor == avatar) continue;
    if (look && hits[i].actor) {
      if (hits[i].comp && u64_comp_response(hits[i].comp, "Interactable") == 1 && implements_interactable(hits[i].actor)) {
        u64_v ok = u64_call(hits[i].actor, "IsAvailableForInteraction", 1, (u64_v[]){u64_objv(hits[i].comp)});
        if (u64_true(ok)) return hit_object(&hits[i], start, end, 1);                          // an interactable component, treated as a blocking hit
      }
      return hit_object(0, start, (v3){hits[i].loc[0], hits[i].loc[1], hits[i].loc[2]}, 0);  // anything else ends the trace: empty result, TraceEnd at the hit
    }
    return hit_object(&hits[i], start, end, 1);
  }
  return res;
}
static u64_v interact_target_data(u64_obj *hit) { return u64_td_handle_from_hits(u64_array_of(1, (u64_v[]){u64_objv(hit)})); }
static u64_obj *td_hit_actor(u64_v td) { u64_v d = u64_array_get(td, 0); u64_obj *h = d.t == U64_OBJ ? O(*u64_ivar(d.u.o, "HitResult")) : 0; return h ? O(*u64_ivar(h, "HitActor")) : 0; }
static void it_perform(u64_obj *t)                                                              // UGSAT_WaitInteractableTarget::PerformTrace
{
  u64_obj *ab = O(*u64_ivar(t, "Ability")), *src = ab ? O(*u64_ivar(ab, "Avatar")) : 0;
  if (!src) return;
  v3 view_start, view_rot, start;
  player_view(src, &view_start, &view_rot);                                                   // the weapon point / head socket needs the animated pose: the eye stands in
  start = view_start;
  float range = fv(*u64_ivar(t, "MaxRange"));
  v3 view_dir = dir_of(view_rot), view_end = v3_add(view_start, v3_mul(view_dir, range));
  { v3 to_center = v3_sub(start, view_start); float dot = v3_dot(to_center, view_dir);        // ClipCameraRayToAbilityRange
    if (dot >= 0) { float d2 = v3_dot(to_center, to_center) - dot * dot, r2 = range * range; if (d2 <= r2) view_end = v3_add(view_start, v3_mul(view_dir, dot + sqrtf(r2 - d2))); } }
  u64_obj *probe = it_line_trace(t, view_start, view_end, 0);
  int use = u64_true(*u64_ivar(probe, "bBlockingHit")); v3 adj = view_end;
  if (use) { v3 hl = v3_of(*u64_ivar(probe, "Location")), dd = v3_sub(start, hl); if (v3_dot(dd, dd) <= range * range) adj = hl; else use = 0; }
  v3 aim = v3_norm(v3_sub(adj, start)); if (aim.x == 0 && aim.y == 0 && aim.z == 0) aim = view_dir;
  v3 end = v3_add(start, v3_mul(aim, range));
  u64_obj *ret = it_line_trace(t, start, end, 1);
  u64_v old = *u64_ivar(t, "TargetData");
  if (!u64_true(*u64_ivar(ret, "bBlockingHit"))) {
    *u64_ivar(ret, "Location") = vv(end);
    if (u64_array_len(old) > 0 && td_hit_actor(old)) u64_mc_broadcast(u64_ivar(t, "LostInteractableTarget"), 1, &old);
    *u64_ivar(t, "TargetData") = interact_target_data(ret);
  } else {
    int broadcast_new = 1;
    if (u64_array_len(old) > 0) {
      u64_obj *old_target = td_hit_actor(old);
      if (old_target == O(*u64_ivar(ret, "HitActor"))) broadcast_new = 0;
      else if (old_target) u64_mc_broadcast(u64_ivar(t, "LostInteractableTarget"), 1, &old);
    }
    if (broadcast_new) { u64_v nd = interact_target_data(ret); *u64_ivar(t, "TargetData") = nd; u64_mc_broadcast(u64_ivar(t, "FoundNewInteractableTarget"), 1, &nd); }
  }
}
static u64_v ga_wait_interactable(u64_obj *ab, int c, const u64_v *a)    // (Ability, Name, TraceProfile, MaxRange, TimerPeriod, bShowDebug)
{
  u64_obj *t = u64_task_new("GSAT_WaitInteractableTarget", O(A(c, a, 0)) ? O(A(c, a, 0)) : ab);
  *u64_ivar(t, "TraceProfile") = A(c, a, 2); *u64_ivar(t, "MaxRange") = A(c, a, 3); *u64_ivar(t, "TimerPeriod") = A(c, a, 4);
  *u64_ivar(t, "TargetData") = u64_array_of(0, 0); *u64_ivar(t, "NextTime") = u64_float(u64_world_time() + fv(A(c, a, 4)));
  return u64_objv(t);
}
static void interact_tick(void)
{
  u64_obj *snap[96]; int n = u64_tasks_snapshot(snap, 96); float now = u64_world_time();
  for (int i = 0; i < n; i++) {
    if (strcmp(u64_obj_class(snap[i]), "GSAT_WaitInteractableTarget") || now + 1e-5f < fv(*u64_ivar(snap[i], "NextTime"))) continue;
    *u64_ivar(snap[i], "NextTime") = u64_float(fv(*u64_ivar(snap[i], "NextTime")) + fv(*u64_ivar(snap[i], "TimerPeriod")));    // SetTimer(period, loop)
    it_perform(snap[i]);
  }
}

// ---------------------------------------------------------------------------------------------------- registration
static u64_v mv_start_sprinting(u64_obj *c, int n, const u64_v *a), mv_stop_sprinting(u64_obj *c, int n, const u64_v *a), mv_start_ads(u64_obj *c, int n, const u64_v *a), mv_stop_ads(u64_obj *c, int n, const u64_v *a);
typedef struct { const char *cls, *fn; u64_v (*f)(u64_obj *, int, const u64_v *); } m_t;
static const m_t methods[] = {
  {"GSCharacterBase", "IsAlive", cb_is_alive}, {"GSCharacterBase", "GetShield", cb_get_shield}, {"GSCharacterBase", "GetMaxShield", cb_get_max_shield},
  {"GSCharacterBase", "GetMoveSpeedBaseValue", cb_get_move_speed_base}, {"GSCharacterBase", "RemoveCharacterAbilities", cb_remove_character_abilities},
  {"GSWeapon", "GetPrimaryClipAmmo", w_get_primary_clip}, {"GSWeapon", "GetMaxPrimaryClipAmmo", w_get_max_primary_clip},
  {"GSWeapon", "GetSecondaryClipAmmo", w_get_secondary_clip}, {"GSWeapon", "GetMaxSecondaryClipAmmo", w_get_max_secondary_clip},
  {"GSWeapon", "SetPrimaryClipAmmo", w_set_primary_clip}, {"GSWeapon", "SetMaxPrimaryClipAmmo", w_set_max_primary_clip},
  {"GSWeapon", "SetSecondaryClipAmmo", w_set_secondary_clip}, {"GSWeapon", "SetMaxSecondaryClipAmmo", w_set_max_secondary_clip},
  {"GSWeapon", "HasInfiniteAmmo", w_has_infinite_ammo}, {"GSWeapon", "GetWeaponMesh1P", w_mesh1p}, {"GSWeapon", "GetWeaponMesh3P", w_mesh3p},
  {"GSWeapon", "ResetWeapon", w_reset}, {"GSWeapon", "SetOwningCharacter", w_set_owning_character}, {"GSWeapon", "Equip", w_equip},
  {"GSWeapon", "UnEquip", w_unequip}, {"GSWeapon", "AddAbilities", w_add_abilities}, {"GSWeapon", "RemoveAbilities", w_remove_abilities},
  {"GSWeapon", "OnDropped", w_on_dropped}, {"GSWeapon", "GetLineTraceTargetActor", w_line_trace_ta}, {"GSWeapon", "GetSphereTraceTargetActor", w_sphere_trace_ta},
  {"GSWeapon", "BeginPlay", w_begin_play}, {"GSWeapon", "EndPlay", w_end_play},
  {"GSHeroCharacter", "IsInFirstPersonPerspective", h_perspective}, {"GSHeroCharacter", "GetFirstPersonMesh", h_first_person_mesh},
  {"GSHeroCharacter", "GetThirdPersonMesh", h_third_person_mesh}, {"GSHeroCharacter", "GetCurrentWeapon", h_current_weapon},
  {"GSHeroCharacter", "GetWeaponAttachPoint", h_weapon_attach_point}, {"GSHeroCharacter", "DoesWeaponExistInInventory", h_does_weapon_exist},
  {"GSHeroCharacter", "AddWeaponToInventory", h_add_weapon_to_inventory}, {"GSHeroCharacter", "RemoveWeaponFromInventory", h_remove_weapon_from_inventory},
  {"GSHeroCharacter", "RemoveAllWeaponsFromInventory", h_remove_all_weapons}, {"GSHeroCharacter", "EquipWeapon", h_equip_weapon},
  {"GSHeroCharacter", "SetCurrentWeapon", h_set_current_weapon}, {"GSHeroCharacter", "UnEquipCurrentWeapon", h_unequip_current_weapon},
  {"GSHeroCharacter", "NextWeapon", h_next_weapon}, {"GSHeroCharacter", "PreviousWeapon", h_previous_weapon}, {"GSHeroCharacter", "GetNumWeapons", h_num_weapons},
  {"GSHeroCharacter", "GetPrimaryClipAmmo", h_primary_clip}, {"GSHeroCharacter", "GetMaxPrimaryClipAmmo", h_max_primary_clip},
  {"GSHeroCharacter", "GetSecondaryClipAmmo", h_secondary_clip}, {"GSHeroCharacter", "GetMaxSecondaryClipAmmo", h_max_secondary_clip},
  {"GSHeroCharacter", "GetPrimaryReserveAmmo", h_primary_reserve}, {"GSHeroCharacter", "GetSecondaryReserveAmmo", h_secondary_reserve},
  {"GSHeroCharacter", "SpawnDefaultInventory", h_spawn_default_inventory}, {"GSHeroCharacter", "PostInitializeComponents", h_post_initialize_components},
  {"GSHeroCharacter", "CurrentWeaponPrimaryClipAmmoChanged", h_ammo_noop}, {"GSHeroCharacter", "CurrentWeaponSecondaryClipAmmoChanged", h_ammo_noop},
  {"GSHeroCharacter", "KnockDown", h_knock_down}, {"GSHeroCharacter", "FinishDying", h_finish_dying},
  {"GASShooterGameModeBase", "RespawnHero", gm_respawn_hero},
  {"GSGATA_Trace", "Configure", tr_configure}, {"GSGATA_Trace", "ResetSpread", tr_reset_spread}, {"GSGATA_Trace", "PerformTrace", tr_perform_trace},
  {"GSGATA_Trace", "StartTargeting", tr_start_targeting}, {"GSGATA_Trace", "ConfirmTargetingAndContinue", tr_confirm}, {"GSGATA_Trace", "StopTargeting", tr_stop_targeting},
  {"GSGameplayAbility", "MakeEffectContainerSpec", ga_make_container_spec}, {"GSGameplayAbility", "OnAvatarSet", ga_on_avatar_set},
  {"GSHeroCharacter", "PossessedBy", h_possessed_by},
  {"GSCharacterBase", "StartSprinting", mv_start_sprinting}, {"GSCharacterBase", "StopSprinting", mv_stop_sprinting},
  {"GSCharacterBase", "StartAimDownSights", mv_start_ads}, {"GSCharacterBase", "StopAimDownSights", mv_stop_ads},
  {"GSPickup", "PickupOnTouch", pk_pickup_on_touch}, {"GSPickup", "RespawnPickup", pk_respawn}, {"GSWeapon", "PickUpOnTouch", w_pickup_on_touch},
  {"GSASCActorBase", "RegisterInteracter", ia_register}, {"GSASCActorBase", "UnregisterInteracter", ia_unregister}, {"GSASCActorBase", "InteractableCancelInteraction", ia_cancel},
  {"GSASCActorBase", "IsAvailableForInteraction", ia_false}, {"GSASCActorBase", "GetInteractionDuration", ia_zero},
  {"GSASCActorBase", "GetPreInteractSyncType", ia_sync_type}, {"GSASCActorBase", "GetPostInteractSyncType", ia_sync_type},
  {"GSHeroCharacter", "RegisterInteracter", ia_register}, {"GSHeroCharacter", "UnregisterInteracter", ia_unregister}, {"GSHeroCharacter", "InteractableCancelInteraction", ia_cancel},
  {"GSHeroCharacter", "IsAvailableForInteraction", h_ia_available}, {"GSHeroCharacter", "GetInteractionDuration", h_ia_duration},
  {"GSHeroCharacter", "PreInteract", h_ia_pre}, {"GSHeroCharacter", "PostInteract", h_ia_post},
  {"GSHeroCharacter", "GetPreInteractSyncType", h_ia_sync_type}, {"GSHeroCharacter", "CancelInteraction", h_ia_cancel_interaction},
  {"GSGameplayAbility", "WaitForInteractableTarget", ga_wait_interactable},
  {"GSGameplayAbility", "GSCheckCost", ga_gs_check_cost_default}, {"GSGameplayAbility", "GSApplyCost", ga_noop},
};
// UGSCharacterMovementComponent::GetMaxSpeed (SprintSpeedMultiplier 1.4, ADSSpeedMultiplier 0.8, KnockedDownSpeedMultiplier 0.4)
static float gs_max_speed(u64_obj *owner)
{
  if (!is_class(owner, "GSCharacterBase")) return -1.0f;
  u64_obj *asc = u64_asc_of(owner);
  if (!is_alive(owner)) return 0.0f;
  if (asc && u64_tag_count(asc, "State.Interacting") > u64_tag_count(asc, "State.InteractingRemoval")) return 0.0f;
  float ms = u64_attr_get(asc, "MoveSpeed");
  if (asc && u64_tag_count(asc, "State.KnockedDown")) return ms * 0.4f;
  if (u64_true(*u64_ivar(owner, "RequestToStartSprinting"))) return ms * 1.4f;
  if (u64_true(*u64_ivar(owner, "RequestToStartADS"))) return ms * 0.8f;
  return ms;
}
static u64_v mv_set_flag(u64_obj *comp, const char *flag, int v)       // Start/StopSprinting, Start/StopAimDownSights on the movement component
{
  u64_obj *owner = !strcmp(u64_obj_class(comp), "Component") ? O(*u64_ivar(comp, "Owner")) : comp;   // movement is modelled on the character itself
  if (owner) *u64_ivar(owner, flag) = u64_bool(v);
  return NONE_;
}
static u64_v mv_start_sprinting(u64_obj *c, int n, const u64_v *a) { (void)n; (void)a; return mv_set_flag(c, "RequestToStartSprinting", 1); }
static u64_v mv_stop_sprinting(u64_obj *c, int n, const u64_v *a) { (void)n; (void)a; return mv_set_flag(c, "RequestToStartSprinting", 0); }
static u64_v mv_start_ads(u64_obj *c, int n, const u64_v *a) { (void)n; (void)a; return mv_set_flag(c, "RequestToStartADS", 1); }
static u64_v mv_stop_ads(u64_obj *c, int n, const u64_v *a) { (void)n; (void)a; return mv_set_flag(c, "RequestToStartADS", 0); }
static const u64_gas_hooks gs_hooks = {gs_pre_attribute_change, gs_post_execute, gs_zero_health, gs_can_activate, gs_check_cost, gs_apply_cost, gs_max_speed, gs_world_tick};

__attribute__((constructor)) static void register_gasshooter(void)
{
  u64_cpp_register_class("GSCharacterBase", 0, 0);
  u64_cpp_register_class("GSHeroCharacter", "GSCharacterBase", ctor_hero);
  u64_cpp_register_class("GSWeapon", 0, ctor_weapon);
  u64_cpp_register_class("GSPickup", 0, ctor_pickup); u64_cpp_register_class("GSASCActorBase", 0, 0);
  u64_cpp_register_class("GSGameplayAbility", 0, ctor_gs_ability); u64_cpp_register_class("GSGA_CharacterJump", "GSGameplayAbility", ctor_gs_jump); u64_cpp_register_class("GSPlayerController", 0, 0);
  u64_cpp_register_class("GSGATA_Trace", 0, ctor_trace); u64_cpp_register_class("GSGATA_LineTrace", "GSGATA_Trace", 0);
  u64_cpp_register_class("GSGATA_SphereTrace", "GSGATA_Trace", ctor_sphere_trace);
  u64_cpp_register_class("GSProjectile", 0, 0);
  u64_cpp_register_class("GASShooterGameModeBase", 0, ctor_gamemode);
  for (unsigned i = 0; i < sizeof methods / sizeof methods[0]; i++) u64_cpp_register_method(methods[i].cls, methods[i].fn, methods[i].f);
  u64_gas_hooks_active = &gs_hooks;
  u64_game_mode_class = "BP_GameMode";
}

void u64_gc_roots_project(void) { for (int i = 0; i < cdo_n; i++) u64_gc_mark_obj(cdo_cache[i].o); for (int i = 0; i < ntouch; i++) { u64_gc_mark_obj(touching[i].a); u64_gc_mark_obj(touching[i].b); } }
