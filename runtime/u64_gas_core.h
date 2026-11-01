// Gameplay Ability System core: data structures emitted from the project's GameplayEffect assets, plus the API
// used by natives. Behaviour follows Unreal's GAS and the project's own C++ (GDAttributeSetBase, GDCharacterBase).
#pragma once
#include "u64_rt.h"

enum { U64_OP_ADD, U64_OP_MUL, U64_OP_DIV, U64_OP_OVERRIDE };
enum { U64_CALC_SCALABLE, U64_CALC_ATTR, U64_CALC_SETBYCALLER };
enum { U64_DUR_INSTANT, U64_DUR_INFINITE, U64_DUR_HAS_DURATION };

typedef struct { const char *attr; int op, calc; float value; const char *backing; int backing_target; float coef, pre, post; const char *sbc; } u64_modinfo;
typedef struct {
  const char *name; int dur; float dur_value, period; int nmods; const u64_modinfo *mods;
  const char *const *execs, *const *granted, *const *asset_tags, *const *remove_tags;
  const char *const *app_require, *const *app_ignore, *const *ongoing_require, *const *ongoing_ignore;
  int stack_type, stack_limit;
  const char *const *cond_effects, *const *cond_required;      // ConditionalGameplayEffects: class + '|'-joined required source tags
} u64_ge_def;
typedef struct { float time; const char *cls, *tag; } u64_notify_def;
typedef struct { const char *name; float length, blend_out; int nnotifies; const u64_notify_def *notifies; } u64_montage_def;
typedef struct { const char *cls, *comp; int kind; float a, b, c, rx, ry, rz, ryaw; } u64_shape_def;   // kind 1: box half extents; 2: capsule (radius, half height); 3: sphere (radius)
typedef struct { const char *cls, *fn, *comp; int end; } u64_bound_event_def;   // from K2Node_ComponentBoundEvent: handler function -> component and begin/end
extern const u64_bound_event_def u64_bound_events[];

typedef struct { const char *cls; float max_accel, braking, friction, jump_z, gravity_scale, air_control, orient_to_movement, rotation_rate; } u64_movement_def;
extern const u64_movement_def u64_movements[];
typedef struct { const char *cls; float initial_speed, max_speed, gravity_scale; } u64_projectile_def;
extern const u64_projectile_def u64_projectiles[];
typedef struct { float time, value, arrive, leave; int interp; } u64_curve_key;           // interp: 0 linear, 1 constant, 2 cubic
typedef struct { const char *prop; int nkeys; const u64_curve_key *keys; } u64_tl_track;
typedef struct { const char *cls, *var; float length; int loop, autoplay; const char *update, *finished, *direction; int ntracks; const u64_tl_track *tracks; } u64_timeline_def;
extern const u64_timeline_def u64_timelines[];                             // Timeline nodes: terminated by a null class
float u64_curve_eval(const u64_curve_key *keys, int n, float t);
typedef struct { int n; const float *planes; } u64_hull;                                  // convex hull: n planes (nx, ny, nz, d), inside n.x <= d
typedef struct { const char *name; int nhulls; const u64_hull *hulls; } u64_mesh_def;      // a static mesh's simple collision (mesh space)
typedef struct { const char *chan; int resp; } u64_resp;                                    // 0 ignore, 1 overlap, 2 block
typedef struct { const char *cls, *comp, *mesh, *parent; float loc[3], rot[3], scale[3]; int enabled, nresp; const u64_resp *resp; } u64_comp_def;
// Skeletal meshes: the physics asset's simple shapes in mesh space (reference pose). kind 0 box (half extents a, b, c), 1 sphere (radius a), 2 sphyl
// (radius a, half length of the cylinder b; axis = local z). The character Mesh component places them in the actor.
typedef struct { const char *bone; int kind; float t[3], r[9], a, b, c, rt[3], rr[9]; } u64_phys_body;   // t / r: bone * element in the reference pose; rt / rr: the element relative to its bone (for an animated pose)
typedef struct { const char *name; int n; const u64_phys_body *bodies; } u64_phys_mesh;
typedef struct { const char *cls, *comp, *mesh; float loc[3], rot[3], scale[3]; int enabled, nresp; const u64_resp *resp; } u64_skel_comp_def;
typedef struct { const char *mesh; float loc[3], rot[3], scale[3]; } u64_world_mesh;   // a static mesh actor placed in the map (default collision: blocks everything)
extern const u64_world_mesh u64_world_meshes[];
extern const u64_phys_mesh u64_phys_meshes[];
extern const u64_skel_comp_def u64_skel_comps[];
extern const u64_mesh_def u64_meshes[];                                                     // null-name terminated
extern const u64_comp_def u64_comp_defs[];
typedef struct { const char *cls, *parent; } u64_class_parent_def;
extern const u64_class_parent_def u64_class_parents[];
extern const u64_ge_def u64_ge_defs[]; extern const int u64_ge_count;      // generated: u64_gas_data.c
extern const u64_montage_def u64_montages[];                               // terminated by a null name
extern const u64_shape_def u64_shapes[];

// ---- arrays ----
u64_v u64_array_of(int n, const u64_v *items);
u64_v u64_array_const(int id, int n, const u64_v *items);   // shared constant (class defaults); copy-on-write
int u64_array_len(u64_v a);
u64_v u64_array_get(u64_v a, int i);
void u64_array_push(u64_v *a, u64_v item);

// ---- core API ----
void u64_gas_tick(float dt);
void u64_actor_init(u64_obj *actor);                                       // InitAbilityActorInfo + startup flow
u64_obj *u64_asc_of(u64_obj *actor);
float u64_attr_get(u64_obj *asc, const char *name);
int u64_tag_has(u64_obj *asc, const char *query);                          // hierarchical match
int u64_apply_ge(u64_obj *target_asc, u64_obj *source_asc, const char *ge_name, float level, u64_obj *spec);
void u64_remove_ge(int handle);
void u64_die(u64_obj *actor);
int u64_ability_try_activate(u64_obj *ability);
int u64_ability_commit(u64_obj *ability, int cost, int cooldown);
void u64_ability_end(u64_obj *ability, int cancelled);
int u64_input_press(u64_obj *actor, const char *input_id);                 // e.g. "EGDAbilityInputID::Sprint"
void u64_input_release_id(u64_obj *actor, const char *input_id);
int u64_overlapping(u64_obj *self, u64_obj **out, int max);
float u64_montage_length(const char *name, float *blend_out);
float u64_cooldown_remaining(u64_obj *asc, const char *tag, float *duration);
int u64_effect_stack_count(u64_obj *asc, const char *tag);
void u64_remove_ge_class(u64_obj *asc, const char *ge_name);
void u64_remove_ge_with_tags(u64_obj *asc, const char *tag);
float u64_max_speed(u64_obj *actor);
int u64_asc_ability_count(u64_obj *asc);
void u64_movement_tick(float dt);
// notifies / montages / events / collision
void u64_montage_play(u64_obj *actor, const char *montage, float rate);
void u64_send_gameplay_event(u64_obj *actor, const char *tag, u64_v payload);
int u64_line_trace(const float start[3], const float end[3], u64_obj *ignore, u64_obj **hits, int max);   // fills HitResult objects

// Game flow ported from AGASDocumentationGameMode / AGDHeroCharacter.
void u64_game_start(void);                                  // spawns the player's hero at the PlayerStart
u64_obj *u64_player_pawn(void);
void u64_input_axis(u64_obj *pawn, const char *axis, float value);    // MoveForward, MoveRight, Turn, TurnRate, LookUp, LookUpRate
u64_v u64_xform_new(u64_v loc, u64_v rot);                          // transforms keep their rotation
int u64_cpp_ability_activate(u64_obj *ability);             // 1 if a C++ override handled activation
int u64_cpp_ability_can_activate(u64_obj *ability);
void u64_cpp_ability_input_released(u64_obj *ability);
int u64_ge_register(const u64_ge_def *def);                    // effects created at run time (e.g. the bounty effect, tests)
void u64_attr_add_base_unsafe(u64_obj *asc, const char *name, float delta);
void u64_attr_set_base(u64_obj *asc, const char *name, float v);   // the attribute setter (SetHealth etc.)
void u64_overlap_events_tick(void);
int u64_sweep_sphere(const float start[3], const float end[3], float radius, u64_obj *ignore, float *t_out);

// Per-project attribute-set rules (the project's UAttributeSet subclasses). Without hooks the GASDocumentation rules apply.
typedef struct {
  void (*pre_attribute_change)(u64_obj *asc, const char *name, float *new_value);       // UAttributeSet::PreAttributeChange
  int (*post_execute)(u64_obj *target_asc, const char *name, u64_obj *source_asc);      // PostGameplayEffectExecute; 1 = handled
  void (*zero_health)(u64_obj *owner);                                                  // the owner's reaction to Health reaching 0
  int (*can_activate)(u64_obj *ability);                                                // UGameplayAbility::CanActivateAbility additions; 0 blocks
  int (*check_cost)(u64_obj *ability);                                                  // CheckCost additions (GSCheckCost)
  void (*apply_cost)(u64_obj *ability);                                                 // ApplyCost additions (GSApplyCost), run before the cost effect
  float (*max_speed)(u64_obj *actor);                                                   // the project's GetMaxSpeed; negative = use the default
  void (*world_tick)(float dt);                                                         // per-frame project code (actor overlap notifications, ...)
} u64_gas_hooks;
extern const u64_gas_hooks *u64_gas_hooks_active;
// The inputs of a damage execution calculation (UGSDamageExecutionCalc), already resolved by the runtime: captured Armor (target, current) and Damage (source, snapshot),
// the spec's set-by-caller magnitudes and asset tags, the context's hit bone. The project's own C++ (U64_CXX_EXEC) computes the output.
typedef struct { float armor, damage; int nsbc; const char *sbc_tag[4]; float sbc_val[4]; const char *const *asset_tags; int has_hit; const char *bone; } u64_exec_in;
typedef struct { float mitigated; int headshot; } u64_exec_out;
int u64_cxx_exec_damage(const u64_exec_in *in, u64_exec_out *out);
void u64_adjust_for_max_change(u64_obj *asc, const char *affected, const char *maxattr, float new_max);   // AdjustAttributeForMaxChange
u64_obj *u64_give_ability(u64_obj *asc, const char *cls, int level, const char *input_id, u64_obj *source);
void u64_clear_ability(u64_obj *asc, u64_obj *ability);
void u64_ability_triggers(u64_obj *asc, const char *tag, u64_v payload);
int u64_is_asc(u64_obj *o);
int u64_asc_abilities(u64_obj *asc, u64_obj **out, int max);
void u64_tag_add_loose(u64_obj *asc, const char *tag, int delta);
int u64_tag_count(u64_obj *asc, const char *tag);
int u64_remove_ge_with_asset_tags(u64_obj *asc, const char *tag);
void u64_cancel_abilities(u64_obj *asc, const char *tag);
u64_obj *u64_asc_owner(u64_obj *asc);
extern const char *u64_game_mode_class;
void u64_set_player_pawn(u64_obj *pawn);
void u64_notify_tag_change(u64_obj *asc);                      // u64_gas_extra.c: RegisterGameplayTagEvent listeners
void u64_ability_input_pressed(u64_obj *ability);              // an input press for an already running ability
u64_obj *u64_task_new(const char *cls, u64_obj *ability);
void u64_task_track(u64_obj *t);
void u64_task_untrack(u64_obj *t);
int u64_tasks_snapshot(u64_obj **out, int max);
int u64_ability_check_cost(u64_obj *ability);
int u64_effect_handle_stack(int handle);
u64_obj *u64_effect_handle_source(int handle);
int u64_trace_multi(const float start[3], const float end[3], float radius, u64_obj *ignore, u64_obj **hits, int max);
int u64_actor_capsule(u64_obj *o, float *cx, float *cy, float *cz, float *r, float *hh);
int u64_actor_is_pawn(u64_obj *o);
typedef struct { u64_obj *actor, *comp; float t, loc[3], normal[3]; int blocking; const char *bone; } u64_world_hit_t;
int u64_trace_profile(const float start[3], const float end[3], const char *profile, u64_obj *ignore_actor, u64_world_hit_t *out, int max);   // LineTraceMultiByProfile (u64_collision.c)
u64_obj *u64_hit_result_new(const u64_world_hit_t *h, const float start[3], const float end[3]);
int u64_comp_response(u64_obj *comp, const char *channel);                    // 0 ignore, 1 overlap, 2 block
void u64_comp_world(u64_obj *actor, const char *comp, float loc_out[3]);
extern const u64_resp u64_channel_defaults[];
typedef struct { const char *name, *object_type; int nresp; const u64_resp *resp; } u64_profile_def;
extern const u64_profile_def u64_profiles[];                                  // collision profiles of the project: name -> object type (trace channel)
