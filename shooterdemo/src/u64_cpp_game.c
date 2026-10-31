// Ports of the project's own C++ gameplay classes (Source/GASDocumentation), hand-translated function by function:
// UGDGameplayAbility, UGDGA_FireGun, UGDGA_CharacterJump, AGDHeroCharacter (input), AGDProjectile and
// AGASDocumentationGameMode (HeroDied / RespawnHero). Blueprint classes inherit from these through u64_class_parents.
#include "u64_gas_core.h"
#include <math.h>
#include <stdio.h>

static const char *nm(u64_v v)
{
  if (v.t == U64_CLASS || v.t == U64_NAME || v.t == U64_STR) return v.u.s ? v.u.s : "";
  if (v.t == U64_OBJ && v.u.o) return u64_obj_name(v.u.o);
  return "";
}
static void tag_push(u64_obj *o, const char *prop, const char *tag) { u64_array_push(u64_ivar(o, prop), u64_name(tag)); }
static float fv(u64_v v) { return v.t == U64_FLOAT ? v.u.f : (v.t == U64_INT || v.t == U64_BOOL) ? (float)v.u.i : 0; }

// ---------------------------------------------------------------------------------------------------- constructors
static void ctor_ability(u64_obj *o)                 // UGDGameplayAbility::UGDGameplayAbility
{
  tag_push(o, "ActivationBlockedTags", "State.Dead");
  tag_push(o, "ActivationBlockedTags", "State.Debuff.Stun");
}
static void ctor_firegun(u64_obj *o)                 // UGDGA_FireGun::UGDGA_FireGun
{
  tag_push(o, "AbilityTags", "Ability.Skill.Ability1");
  tag_push(o, "ActivationOwnedTags", "Ability.Skill.Ability1");
  tag_push(o, "ActivationBlockedTags", "Ability.Skill");
  *u64_ivar(o, "Range") = u64_float(1000.0f);
  *u64_ivar(o, "Damage") = u64_float(12.0f);
}
static void ctor_jump(u64_obj *o)                    // UGDGA_CharacterJump::UGDGA_CharacterJump
{
  *u64_ivar(o, "AbilityInputID") = u64_name("EGDAbilityInputID::Jump");
  tag_push(o, "AbilityTags", "Ability.Jump");
}
static void ctor_hero(u64_obj *o)                    // AGDHeroCharacter::AGDHeroCharacter
{
  *u64_ivar(o, "BaseTurnRate") = u64_float(45.0f);
  *u64_ivar(o, "BaseLookUpRate") = u64_float(45.0f);
}
static void ctor_gamemode(u64_obj *o)                // AGASDocumentationGameMode::AGASDocumentationGameMode
{
  *u64_ivar(o, "RespawnDelay") = u64_float(5.0f);
  *u64_ivar(o, "HeroClass") = u64_cls("BP_HeroCharacter");
}
typedef struct { const char *cls, *parent; void (*ctor)(u64_obj *); } cpp_class_t;
static const cpp_class_t cpp_classes[] = {
  {"GDGameplayAbility", 0, ctor_ability}, {"GDGA_FireGun", "GDGameplayAbility", ctor_firegun},
  {"GDGA_CharacterJump", "GDGameplayAbility", ctor_jump}, {"GDCharacterBase", 0, 0},
  {"GDHeroCharacter", "GDCharacterBase", ctor_hero}, {"GDMinionCharacter", "GDCharacterBase", 0},
  {"GDProjectile", 0, 0}, {"GASDocumentationGameMode", 0, ctor_gamemode},
};
// Other projects register their C++ classes and members here (u64_cpp_gasshooter.c) instead of editing the tables above.
#define MAX_REG 512
static struct { const char *cls, *parent; void (*ctor)(u64_obj *); } reg_cls[MAX_REG]; static int n_reg_cls;
typedef u64_v (*cpp_fn)(u64_obj *self, int argc, const u64_v *argv);
static struct { const char *cls, *fn; cpp_fn f; } reg_fn[MAX_REG]; static int n_reg_fn;
void u64_cpp_register_class(const char *cls, const char *parent, void (*ctor)(u64_obj *))
{
  if (n_reg_cls < MAX_REG) reg_cls[n_reg_cls++] = (typeof(reg_cls[0])){cls, parent, ctor};
}
void u64_cpp_register_method(const char *cls, const char *fn, u64_v (*f)(u64_obj *, int, const u64_v *))
{
  if (n_reg_fn < MAX_REG) reg_fn[n_reg_fn++] = (typeof(reg_fn[0])){cls, fn, f};
}
int u64_cpp_class_registered(const char *cls)
{
  for (int i = 0; i < n_reg_cls; i++) if (!strcmp(reg_cls[i].cls, cls)) return 1;
  for (unsigned i = 0; i < sizeof cpp_classes / sizeof cpp_classes[0]; i++) if (!strcmp(cpp_classes[i].cls, cls)) return 1;
  return 0;
}
const char *u64_parent_of(const char *cls)
{
  for (int i = 0; i < n_reg_cls; i++) if (!strcmp(reg_cls[i].cls, cls)) return reg_cls[i].parent;
  for (const u64_class_parent_def *p = u64_class_parents; p->cls; p++) if (!strcmp(p->cls, cls)) return p->parent;
  for (unsigned i = 0; i < sizeof cpp_classes / sizeof cpp_classes[0]; i++) if (!strcmp(cpp_classes[i].cls, cls)) return cpp_classes[i].parent;
  return 0;
}
int u64_class_is(const char *cls, const char *anc) { for (; cls; cls = u64_parent_of(cls)) if (!strcmp(cls, anc)) return 1; return 0; }
void u64_cpp_construct(u64_obj *o, const char *cls)
{
  for (int i = 0; i < n_reg_cls; i++) if (!strcmp(reg_cls[i].cls, cls) && reg_cls[i].ctor) reg_cls[i].ctor(o);
  for (unsigned i = 0; i < sizeof cpp_classes / sizeof cpp_classes[0]; i++)
    if (!strcmp(cpp_classes[i].cls, cls) && cpp_classes[i].ctor) cpp_classes[i].ctor(o);
}

// ---------------------------------------------------------------------------------------------------- geometry helpers
static u64_v vec_of(u64_obj *o, const char *n) { u64_v *v = u64_ivar(o, n); return v->t == U64_VEC ? *v : u64_vec(0, 0, 0); }
static void rot_yaw(float yaw_deg, const float in[3], float out[3])
{
  float y = yaw_deg * 0.0174533f, c = cosf(y), s = sinf(y);
  out[0] = in[0] * c - in[1] * s; out[1] = in[0] * s + in[1] * c; out[2] = in[2];
}
static u64_v comp_location(u64_obj *actor, const char *comp)      // component world location = actor + R(yaw) * RelativeLocation
{
  u64_v base = vec_of(actor, "Location"), rot = vec_of(actor, "Rotation");
  u64_obj *c = u64_asobj(u64_comp(actor, comp));
  u64_v rel = c ? vec_of(c, "RelativeLocation") : u64_vec(0, 0, 0);
  float r[3]; rot_yaw(rot.u.v[1], rel.u.v, r);
  return u64_vec(base.u.v[0] + r[0], base.u.v[1] + r[1], base.u.v[2] + r[2]);
}
static u64_v look_at_rotation(u64_v a, u64_v b)                   // UKismetMathLibrary::FindLookAtRotation -> (pitch, yaw, 0)
{
  float dx = b.u.v[0] - a.u.v[0], dy = b.u.v[1] - a.u.v[1], dz = b.u.v[2] - a.u.v[2];
  return u64_vec(atan2f(dz, sqrtf(dx * dx + dy * dy)) * 57.29578f, atan2f(dy, dx) * 57.29578f, 0);
}
static u64_v forward_of(u64_v rot)
{
  float p = rot.u.v[0] * 0.0174533f, y = rot.u.v[1] * 0.0174533f;
  return u64_vec(cosf(p) * cosf(y), cosf(p) * sinf(y), sinf(p));
}

// ---------------------------------------------------------------------------------------------------- transforms with rotation
static u64_obj *ring[16];                                         // recycled transform objects
u64_v u64_xform_new(u64_v loc, u64_v rot)
{
  static int next;
  int i = next++ & 15;
  if (!ring[i]) ring[i] = u64_new(u64_intern("Transform"), "Transform");
  *u64_ivar(ring[i], "Location") = loc.t == U64_VEC ? loc : u64_vec(0, 0, 0);
  *u64_ivar(ring[i], "Rotation") = rot.t == U64_VEC ? rot : u64_vec(0, 0, 0);
  u64_obj_set_alive(ring[i], 0);                                  // not an actor
  return u64_objv(ring[i]);
}

// ---------------------------------------------------------------------------------------------------- abilities
static u64_obj *avatar(u64_obj *ab) { u64_v *a = u64_ivar(ab, "Avatar"); return a->t == U64_OBJ ? a->u.o : 0; }
static u64_v firegun_activate(u64_obj *ab)                         // UGDGA_FireGun::ActivateAbility
{
  if (!u64_ability_commit(ab, 1, 1)) { u64_ability_end(ab, 1); return (u64_v){U64_NONE}; }
  u64_obj *asc = u64_asc_of(avatar(ab));
  u64_v montage = *u64_ivar(ab, "FireHipMontage");
  if (u64_tag_has(asc, "State.AimDownSights") && !u64_tag_has(asc, "State.AimDownSights.Removal"))
    montage = *u64_ivar(ab, "FireIronsightsMontage");
  u64_v targs[8] = {u64_objv(ab), u64_name("None"), montage, u64_array_of(0, 0), u64_float(1.0f), u64_name("None"), u64_bool(0), u64_float(1.0f)};
  u64_v task = u64_call(ab, "PlayMontageAndWaitForEvent", 8, targs);
  u64_obj *t = u64_asobj(task);
  if (!t) return (u64_v){U64_NONE};
  u64_mc_add(u64_ivar(t, "OnBlendOut"), u64_delegate(u64_objv(ab), "OnCompleted"));
  u64_mc_add(u64_ivar(t, "OnCompleted"), u64_delegate(u64_objv(ab), "OnCompleted"));
  u64_mc_add(u64_ivar(t, "OnInterrupted"), u64_delegate(u64_objv(ab), "OnCancelled"));
  u64_mc_add(u64_ivar(t, "OnCancelled"), u64_delegate(u64_objv(ab), "OnCancelled"));
  u64_mc_add(u64_ivar(t, "EventReceived"), u64_delegate(u64_objv(ab), "EventReceived"));
  u64_call(t, "ReadyForActivation", 0, 0);
  return (u64_v){U64_NONE};
}
static u64_v firegun_cancelled(u64_obj *ab, int argc, const u64_v *a) { (void)argc; (void)a; u64_ability_end(ab, 1); return (u64_v){U64_NONE}; }
static u64_v firegun_completed(u64_obj *ab, int argc, const u64_v *a) { (void)argc; (void)a; u64_ability_end(ab, 0); return (u64_v){U64_NONE}; }
static u64_v firegun_event(u64_obj *ab, int argc, const u64_v *a)  // UGDGA_FireGun::EventReceived
{
  const char *tag = argc > 0 ? nm(a[0]) : "";
  if (!strcmp(tag, "Event.Montage.EndAbility")) { u64_ability_end(ab, 0); return (u64_v){U64_NONE}; }
  if (strcmp(tag, "Event.Montage.SpawnProjectile")) return (u64_v){U64_NONE};
  u64_obj *hero = avatar(ab);
  if (!hero) { u64_ability_end(ab, 1); return (u64_v){U64_NONE}; }
  // Start = gun component's Muzzle socket. Sockets need the skeleton pose (not in the data yet): the gun component's
  // world location is used instead. End = camera boom location + camera forward * Range.
  u64_v start = comp_location(hero, "GunComponent"), boom = comp_location(hero, "CameraBoom");
  u64_v ctrl = vec_of(hero, "ControlRotation"), fwd = forward_of(ctrl);
  float range = fv(*u64_ivar(ab, "Range"));
  u64_v end = u64_vec(boom.u.v[0] + fwd.u.v[0] * range, boom.u.v[1] + fwd.u.v[1] * range, boom.u.v[2] + fwd.u.v[2] * range);
  u64_v rot = look_at_rotation(start, end);
  u64_v sargs[2] = {*u64_ivar(ab, "DamageGameplayEffect"), u64_float(1.0f)};     // MakeOutgoingGameplayEffectSpec(DamageGameplayEffect, level)
  u64_v spec = u64_call(ab, "MakeOutgoingGameplayEffectSpec", 2, sargs);
  u64_v aargs[3] = {spec, u64_name("Data.Damage"), *u64_ivar(ab, "Damage")};     // SetSetByCallerMagnitude(Data.Damage, Damage)
  u64_call(ab, "AssignTagSetByCallerMagnitude", 3, aargs);
  u64_obj *proj = u64_spawn_actor(nm(*u64_ivar(ab, "ProjectileClass")), u64_xform_new(start, rot));   // SpawnActorDeferred
  if (proj) {
    *u64_ivar(proj, "DamageEffectSpecHandle") = spec; *u64_ivar(proj, "Range") = u64_float(range);
    *u64_ivar(proj, "Instigator") = u64_objv(hero); *u64_ivar(proj, "Owner") = u64_objv(hero);
    u64_actor_init(proj); u64_call(proj, "ReceiveBeginPlay", 0, 0);              // FinishSpawning
  }
  return (u64_v){U64_NONE};
}
static u64_v jump_input_released(u64_obj *ab)                      // UGDGA_CharacterJump::InputReleased -> CancelAbility -> StopJumping
{
  u64_obj *c = avatar(ab);
  if (c) { u64_ability_end(ab, 1); u64_call(c, "StopJumping", 0, 0); }
  return (u64_v){U64_NONE};
}

int u64_cpp_ability_activate(u64_obj *ab)
{
  const char *cls = u64_obj_class(ab);
  if (u64_class_is(cls, "GDGA_FireGun")) { firegun_activate(ab); return 1; }
  #ifdef U64_CXX_JUMP                                                // the project's own UGSGA_CharacterJump (unmodified C++): see cpp2mips/
  if (u64_class_is(cls, "GSGA_CharacterJump")) { extern void u64_cxx_jump_activate(u64_obj *); u64_cxx_jump_activate(ab); return 1; }
#endif
  if (u64_class_is(cls, "GDGA_CharacterJump") || u64_class_is(cls, "GSGA_CharacterJump")) {   // UGDGA_CharacterJump / UGSGA_CharacterJump::ActivateAbility
    if (!u64_ability_commit(ab, 1, 1)) { u64_ability_end(ab, 1); return 1; }
    u64_obj *c = avatar(ab); if (c) u64_call(c, "Jump", 0, 0);
    return 1;
  }
  return 0;
}
int u64_cpp_ability_can_activate(u64_obj *ab)
{
#ifdef U64_CXX_JUMP
  if (u64_class_is(u64_obj_class(ab), "GSGA_CharacterJump")) { extern int u64_cxx_jump_can_activate(u64_obj *); return u64_cxx_jump_can_activate(ab); }
#endif
  if (u64_class_is(u64_obj_class(ab), "GDGA_CharacterJump") || u64_class_is(u64_obj_class(ab), "GSGA_CharacterJump")) {     // Character && Character->CanJump()
    u64_obj *c = avatar(ab);
    return c && u64_true(u64_call(c, "CanJump", 0, 0));
  }
  return 1;
}
void u64_cpp_ability_input_released(u64_obj *ab)
{
#ifdef U64_CXX_JUMP
  if (u64_class_is(u64_obj_class(ab), "GSGA_CharacterJump")) { extern void u64_cxx_jump_released(u64_obj *); u64_cxx_jump_released(ab); return; }
#endif
  if (u64_class_is(u64_obj_class(ab), "GDGA_CharacterJump") || u64_class_is(u64_obj_class(ab), "GSGA_CharacterJump")) jump_input_released(ab);
}

// ---------------------------------------------------------------------------------------------------- game mode + player
static u64_obj *game_mode, *player_pawn_;
u64_obj *u64_player_pawn(void) { return player_pawn_; }
static u64_obj *find_class(const char *cls) { for (int i = 0; i < u64_obj_count(); i++) { u64_obj *o = u64_obj_at(i); if (!strcmp(u64_obj_class(o), cls) && u64_ivar(o, "Location")->t == U64_VEC) return o; } return 0; }   // placed actors of a class (PlayerStart is not alive: it is not a pawn)
static u64_obj *find_named(const char *name) { for (int i = 0; i < u64_obj_count(); i++) { u64_obj *o = u64_obj_at(i); if (u64_obj_alive(o) && !strcmp(u64_obj_name(o), name)) return o; } return 0; }
const char *u64_game_mode_class = "GASDocumentationGameMode";       // a project port selects its own (e.g. GASShooter's BP_GameMode)
void u64_set_player_pawn(u64_obj *p) { player_pawn_ = p; }
static u64_obj *gm(void) { if (!game_mode) game_mode = u64_new("GameMode", u64_game_mode_class); return game_mode; }
static u64_obj *spawn_hero_at(u64_obj *start)
{
  u64_v loc = start ? vec_of(start, "Location") : u64_vec(0, 0, 0), rot = start ? vec_of(start, "Rotation") : u64_vec(0, 0, 0);
  u64_obj *hero = u64_spawn_actor(nm(*u64_ivar(gm(), "HeroClass")), u64_xform_new(loc, rot));
  if (hero) { u64_actor_init(hero); u64_call(hero, "ReceiveBeginPlay", 0, 0); }
  return hero;
}
void u64_game_start(void)                                          // GameMode::RestartPlayer: the hero starts at the PlayerStart
{
  u64_obj *hero = spawn_hero_at(find_class("PlayerStart"));
  if (hero) { *u64_ivar(hero, "IsPlayer") = u64_bool(1); player_pawn_ = hero; u64_v r; u64_cpp_dispatch(hero, "PossessedBy", 0, 0, &r); }
}
static u64_v respawn_hero(u64_obj *self, int argc, const u64_v *a)  // AGASDocumentationGameMode::RespawnHero(Controller)
{
  (void)self;
  int is_player = argc > 0 && u64_true(a[0]);
  u64_obj *start = is_player ? find_class("PlayerStart") : find_named("EnemyHeroSpawn");
  u64_obj *hero = spawn_hero_at(start);
  if (hero) { *u64_ivar(hero, "IsPlayer") = u64_bool(is_player); if (is_player) player_pawn_ = hero; if (u64_log) u64_log("  [respawn] %s", u64_obj_name(hero)); }
  return (u64_v){U64_NONE};
}
static void hero_died(u64_obj *hero)                                // AGASDocumentationGameMode::HeroDied(Controller)
{
  int is_player = u64_true(*u64_ivar(hero, "IsPlayer"));
  if (hero == player_pawn_) player_pawn_ = 0;                       // UnPossess; the spectator pawn is not modelled
  float delay = fv(*u64_ivar(gm(), "RespawnDelay"));
  if (u64_log) u64_log("  [gamemode] hero died, respawn in %d s", (int)delay);
  u64_timer_call(delay, gm(), "RespawnHero", u64_bool(is_player));
}

// ---------------------------------------------------------------------------------------------------- hero input axes
void u64_input_axis(u64_obj *pawn, const char *axis, float v)
{
  if (!pawn) return;
  u64_v ctrl = vec_of(pawn, "ControlRotation");
  int alive = u64_true(u64_call(pawn, "IsAlive", 0, 0));
  float yaw_only[3] = {0, ctrl.u.v[1], 0};
  if (!strcmp(axis, "MoveForward") || !strcmp(axis, "MoveRight")) {
    u64_v r[1] = {u64_vec(yaw_only[0], yaw_only[1], yaw_only[2])};
    u64_v dir = u64_call(pawn, !strcmp(axis, "MoveForward") ? "GetForwardVector" : "GetRightVector", 1, r);
    u64_v a2[2] = {dir, u64_float(v)};
    u64_call(pawn, "AddMovementInput", 2, a2);
  } else if (!strcmp(axis, "Turn")) { if (alive) { u64_v a1 = u64_float(v); u64_call(pawn, "AddControllerYawInput", 1, &a1); } }
  else if (!strcmp(axis, "LookUp")) { if (alive) { u64_v a1 = u64_float(v); u64_call(pawn, "AddControllerPitchInput", 1, &a1); } }
  else if (!strcmp(axis, "TurnRate")) { if (alive) { u64_v a1 = u64_float(v * fv(*u64_ivar(pawn, "BaseTurnRate")) * u64_world_dt()); u64_call(pawn, "AddControllerYawInput", 1, &a1); } }
  else if (!strcmp(axis, "LookUpRate")) { if (alive) { u64_v a1 = u64_float(v * fv(*u64_ivar(pawn, "BaseLookUpRate")) * u64_world_dt()); u64_call(pawn, "AddControllerPitchInput", 1, &a1); } }
}

// ---------------------------------------------------------------------------------------------------- member-function dispatch
static u64_v fn_finish_dying(u64_obj *self, int argc, const u64_v *a)   // AGDHeroCharacter::FinishDying -> GameMode::HeroDied; then Super (Destroy)
{
  (void)argc; (void)a;
  if (u64_class_is(u64_obj_class(self), "GDHeroCharacter") && u64_obj_alive(self)) hero_died(self);
  u64_obj_destroy(self);
  return (u64_v){U64_NONE};
}
static struct { const char *cls, *fn; cpp_fn f; } methods[] = {
  {"GDGA_FireGun", "OnCompleted", (cpp_fn)firegun_completed}, {"GDGA_FireGun", "OnCancelled", (cpp_fn)firegun_cancelled},
  {"GDGA_FireGun", "EventReceived", (cpp_fn)firegun_event},
  {"GDHeroCharacter", "FinishDying", fn_finish_dying},
  {"GASDocumentationGameMode", "RespawnHero", respawn_hero},
};
int u64_cpp_dispatch_from(const char *start, u64_obj *self, const char *fn, int argc, const u64_v *argv, u64_v *ret)
{
  for (const char *c = start; c; c = u64_parent_of(c)) {
    for (int i = 0; i < n_reg_fn; i++)
      if (!strcmp(reg_fn[i].cls, c) && !strcmp(reg_fn[i].fn, fn)) { *ret = reg_fn[i].f(self, argc, argv); return 1; }
    for (unsigned i = 0; i < sizeof methods / sizeof methods[0]; i++)
      if (!strcmp(methods[i].cls, c) && !strcmp(methods[i].fn, fn)) { *ret = methods[i].f(self, argc, argv); return 1; }
  }
  return 0;
}

void u64_gc_roots_cpp(void)
{
  for (int i = 0; i < 16; i++) u64_gc_mark_obj(ring[i]);
  u64_gc_mark_obj(game_mode); u64_gc_mark_obj(player_pawn_);
}
int u64_cpp_dispatch(u64_obj *self, const char *fn, int argc, const u64_v *argv, u64_v *ret) { return u64_cpp_dispatch_from(u64_obj_class(self), self, fn, argc, argv, ret); }
