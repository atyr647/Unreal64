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
} u64_ge_def;
typedef struct { const char *name; float length, blend_out; } u64_montage_def;
typedef struct { const char *cls; int kind; float a, b, c; } u64_shape_def;   // kind 1: box half extents; 2: capsule (radius, half height)

extern const u64_ge_def u64_ge_defs[]; extern const int u64_ge_count;      // generated: u64_gas_data.c
extern const u64_montage_def u64_montages[];                               // terminated by a null name
extern const u64_shape_def u64_shapes[];

// ---- arrays ----
u64_v u64_array_of(int n, const u64_v *items);
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
