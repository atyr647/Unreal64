// Character movement, ported from UCharacterMovementComponent (CalcVelocity, ApplyVelocityBraking, PhysFalling lateral
// control, jump/landing) with the tuning values read from each character's CharacterMovementComponent.
// There is no world collision geometry in the data yet, so the floor is the height the actor starts at.
#include "u64_gas_core.h"
#include <math.h>

static const u64_movement_def DEFAULT_MOVE = {"", 2048.0f, 2048.0f, 8.0f, 420.0f, 1.0f, 0.05f, 0.0f, 360.0f};
static const u64_movement_def *move_def(u64_obj *o, int *is_character)
{
  const char *cls = u64_obj_class(o);
  for (const u64_movement_def *m = u64_movements; m->cls; m++) if (!strcmp(m->cls, cls)) { *is_character = 1; return m; }
  *is_character = 0;
  return &DEFAULT_MOVE;
}
typedef struct { float x, y, z; } v3;
static v3 getv(u64_obj *o, const char *n) { u64_v *v = u64_ivar(o, n); return v->t == U64_VEC ? (v3){v->u.v[0], v->u.v[1], v->u.v[2]} : (v3){0, 0, 0}; }
static void setv(u64_obj *o, const char *n, v3 a) { *u64_ivar(o, n) = u64_vec(a.x, a.y, a.z); }
static float len2(v3 a) { return sqrtf(a.x * a.x + a.y * a.y); }
static float size(v3 a) { return sqrtf(a.x * a.x + a.y * a.y + a.z * a.z); }
static v3 clamp_size(v3 a, float m) { float s = size(a); return s > m && s > 0 ? (v3){a.x * m / s, a.y * m / s, a.z * m / s} : a; }

// UCharacterMovementComponent::ApplyVelocityBraking
static v3 apply_braking(v3 vel, float dt, float friction, float braking, float friction_factor)
{
  if (size(vel) == 0 || dt < 1e-6f) return vel;
  friction = fmaxf(0.f, friction * fmaxf(0.f, friction_factor));
  braking = fmaxf(0.f, braking);
  int zero_friction = friction == 0.f, zero_braking = braking == 0.f;
  if (zero_friction && zero_braking) return vel;
  v3 old = vel;
  float remaining = dt; const float max_step = 1.0f / 33.0f;
  float s = size(vel);
  v3 rev = zero_braking || s == 0 ? (v3){0, 0, 0} : (v3){-braking * vel.x / s, -braking * vel.y / s, -braking * vel.z / s};
  while (remaining >= 1e-6f) {
    float step = remaining > max_step ? fminf(max_step, remaining * 0.5f) : remaining;
    remaining -= step;
    vel.x += (-friction * vel.x + rev.x) * step; vel.y += (-friction * vel.y + rev.y) * step; vel.z += (-friction * vel.z + rev.z) * step;
    if (vel.x * old.x + vel.y * old.y + vel.z * old.z <= 0.f) return (v3){0, 0, 0};
  }
  float sq = vel.x * vel.x + vel.y * vel.y + vel.z * vel.z;
  if (sq <= 1.f || (!zero_braking && sq <= 100.f)) return (v3){0, 0, 0};    // BRAKE_TO_STOP_VELOCITY = 10
  return vel;
}
// UCharacterMovementComponent::CalcVelocity
static v3 calc_velocity(v3 vel, v3 accel, float max_speed, float analog, float dt, float friction, float braking)
{
  int zero_accel = size(accel) < 1e-4f;
  float max_input = fmaxf(max_speed * analog, 0.f);
  int over_max = size(vel) > max_speed * 1.01f && max_speed >= 0;
  if (zero_accel || over_max) {
    v3 old = vel;
    vel = apply_braking(vel, dt, friction, braking, 2.0f);                      // BrakingFrictionFactor = 2
    if (over_max && size(vel) < max_speed && !zero_accel) { float s = size(old); if (s > 0) vel = (v3){old.x / s * max_speed, old.y / s * max_speed, old.z / s * max_speed}; }
  } else {
    float s = size(accel), vs = size(vel);
    v3 dir = {accel.x / s, accel.y / s, accel.z / s};
    float k = fminf(dt * friction, 1.f);
    vel.x -= (vel.x - dir.x * vs) * k; vel.y -= (vel.y - dir.y * vs) * k; vel.z -= (vel.z - dir.z * vs) * k;
  }
  if (!zero_accel) {
    float new_max = size(vel) > max_input * 1.01f ? size(vel) : max_input;
    vel.x += accel.x * dt; vel.y += accel.y * dt; vel.z += accel.z * dt;
    vel = clamp_size(vel, new_max);
  }
  return vel;
}

static void tick_actor(u64_obj *o, const u64_movement_def *d, float dt)
{
  v3 loc = getv(o, "Location"), vel = getv(o, "Velocity"), in = getv(o, "InputVec");
  *u64_ivar(o, "InputVec") = u64_vec(0, 0, 0);                                    // input is per-frame (ConsumeInputVector)
  u64_v *gz = u64_ivar(o, "GroundZ"); if (gz->t != U64_FLOAT) *gz = u64_float(loc.z);
  int falling = u64_true(*u64_ivar(o, "bFalling"));
  float max_speed = u64_max_speed(o);
  float in_mag = fminf(len2(in), 1.0f);
  v3 accel = {0, 0, 0};
  if (in_mag > 0) { float s = len2(in); accel = (v3){in.x / s * in_mag * d->max_accel, in.y / s * in_mag * d->max_accel, 0}; }
  // ACharacter::CheckJumpInput
  if (u64_true(*u64_ivar(o, "bPressedJump")) && !falling && u64_ivar(o, "JumpCount")->u.i < 1) {
    vel.z = d->jump_z; falling = 1; u64_ivar(o, "JumpCount")->t = U64_INT; u64_ivar(o, "JumpCount")->u.i++;
  }
  v3 old_vel = vel;
  if (!falling) {
    v3 lat = calc_velocity((v3){vel.x, vel.y, 0}, accel, max_speed, in_mag, dt, d->friction, d->braking);
    vel = (v3){lat.x, lat.y, 0};
  } else {
    // PhysFalling: air control scales the lateral acceleration (boosted when slow), then CalcVelocity with no friction/braking
    v3 fall = {accel.x, accel.y, 0};
    if (len2(fall) > 0) {
      float tick_air = d->air_control;
      if (2.0f > 0 && vel.x * vel.x + vel.y * vel.y < 25.0f * 25.0f) tick_air = fminf(1.f, 2.0f * tick_air);   // AirControlBoostMultiplier/Threshold
      fall = clamp_size((v3){fall.x * tick_air, fall.y * tick_air, 0}, d->max_accel);
    }
    v3 lat = calc_velocity((v3){vel.x, vel.y, 0}, fall, max_speed, in_mag, dt, 0.f, 0.f);
    vel.x = lat.x; vel.y = lat.y;
    vel.z += -980.0f * d->gravity_scale * dt;                                      // GravityZ
  }
  if (falling) { loc.x += 0.5f * (old_vel.x + vel.x) * dt; loc.y += 0.5f * (old_vel.y + vel.y) * dt; loc.z += 0.5f * (old_vel.z + vel.z) * dt; }   // PhysFalling integrates the average velocity
  else { loc.x += vel.x * dt; loc.y += vel.y * dt; loc.z += vel.z * dt; }
  if (falling && loc.z <= gz->u.f && vel.z <= 0) {                                 // landing (OnLanded -> ResetJumpState)
    loc.z = gz->u.f; vel.z = 0; falling = 0;
    *u64_ivar(o, "JumpCount") = u64_int(0); *u64_ivar(o, "bPressedJump") = u64_bool(0);
  }
  if (d->orient_to_movement > 0 && len2(vel) > 1.0f) {                             // bOrientRotationToMovement
    v3 rot = getv(o, "Rotation"); float target = atan2f(vel.y, vel.x) * 57.29578f;
    float delta = target - rot.y; while (delta > 180) delta -= 360; while (delta < -180) delta += 360;
    float step = d->rotation_rate * dt; if (delta > step) delta = step; if (delta < -step) delta = -step;
    rot.y += delta; setv(o, "Rotation", rot);
  }
  setv(o, "Location", loc); setv(o, "Velocity", vel);
  *u64_ivar(o, "bFalling") = u64_bool(falling);
  *u64_ivar(o, "Acceleration") = u64_vec(accel.x, accel.y, accel.z);
}

static void movement_step(float dt);
void u64_movement_tick(float dt)
{
  while (dt > 0.0501f) { movement_step(0.05f); dt -= 0.05f; }          // MaxSimulationTimeStep = 0.05
  movement_step(dt);
}
static void movement_step(float dt)
{
  for (int i = 0; i < u64_obj_count(); i++) {
    u64_obj *o = u64_obj_at(i);
    if (!u64_obj_is_actor(o)) continue;
    int is_char; const u64_movement_def *d = move_def(o, &is_char);
    if (is_char) tick_actor(o, d, dt);
  }
}
