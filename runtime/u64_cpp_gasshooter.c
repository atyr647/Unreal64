// Ports of GASShooter's C++ gameplay classes (Source/GASShooter), hand-translated function by function from the
// project's own sources: AGSCharacterBase, AGSHeroCharacter (inventory / weapons / knock down / dying), AGSWeapon,
// UGSAttributeSetBase + UGSAmmoAttributeSet (as attribute rules) and AGASShooterGameModeBase.
// Compiled only into builds of this project; it registers itself with the runtime's class / method tables.
// Differences that come from the runtime, not the game: the hero owns its ability system component (the project keeps it on
// the PlayerState, which only matters across respawns), and authority / replication code paths are always the server's.
#include "u64_gas_core.h"
#include <math.h>
#include <stdio.h>

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
  if (!strcmp(name, "MaxHealth")) u64_adjust_for_max_change(asc, "Health", "MaxHealth", *nv);
  else if (!strcmp(name, "MaxMana")) u64_adjust_for_max_change(asc, "Mana", "MaxMana", *nv);
  else if (!strcmp(name, "MaxStamina")) u64_adjust_for_max_change(asc, "Stamina", "MaxStamina", *nv);
  else if (!strcmp(name, "MoveSpeed")) *nv = clampf(*nv, 150, 1000);
}
static int is_alive(u64_obj *ch) { return u64_obj_alive(ch) && u64_attr_get(u64_asc_of(ch), "Health") > 0; }   // GetHealth() > 0
static int gs_post_execute(u64_obj *tgt, const char *name, u64_obj *src)            // UGSAttributeSetBase / UGSAmmoAttributeSet::PostGameplayEffectExecute
{
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
  if (u64_log) u64_log("  [respawn] %s", u64_obj_name(hero));
  return NONE_;
}

// ---------------------------------------------------------------------------------------------------- registration
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
};
static const u64_gas_hooks gs_hooks = {gs_pre_attribute_change, gs_post_execute, gs_zero_health};

__attribute__((constructor)) static void register_gasshooter(void)
{
  u64_cpp_register_class("GSCharacterBase", 0, 0);
  u64_cpp_register_class("GSHeroCharacter", "GSCharacterBase", ctor_hero);
  u64_cpp_register_class("GSWeapon", 0, ctor_weapon);
  u64_cpp_register_class("GSPickup", 0, 0); u64_cpp_register_class("GSASCActorBase", 0, 0);
  u64_cpp_register_class("GSGameplayAbility", 0, 0); u64_cpp_register_class("GSPlayerController", 0, 0);
  u64_cpp_register_class("GSProjectile", 0, 0);
  u64_cpp_register_class("GASShooterGameModeBase", 0, ctor_gamemode);
  for (unsigned i = 0; i < sizeof methods / sizeof methods[0]; i++) u64_cpp_register_method(methods[i].cls, methods[i].fn, methods[i].f);
  u64_gas_hooks_active = &gs_hooks;
  u64_game_mode_class = "BP_GameMode";
}

void u64_gc_roots_project(void) { for (int i = 0; i < cdo_n; i++) u64_gc_mark_obj(cdo_cache[i].o); }
