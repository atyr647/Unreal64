// Unreal64 level viewer: walks through a level converted by tools/n64fit (baked light in vertex colours).
// STATUS: syntax/type-checked against libdragon + Tiny3D headers only; not built into a ROM, not run.
#include <libdragon.h>
#include <debug.h>
#include <t3d/t3d.h>
#include <t3d/t3dmodel.h>
#include "level_index.h"

#define FB_COUNT 3
#define EYE_HEIGHT_PAD 0.0f
#define MOVE_SPEED 3.0f   // metres per second at full stick
#define TURN_SPEED 2.2f   // radians per second at full stick
#define DRAW_DISTANCE 80.0f

static bool cell_visible(const u64_cell_t *c, fm_vec3_t eye)
{
  // Distance from the eye to the cell's AABB; whole cells beyond the draw distance are skipped.
  float d2 = 0;
  for(int i = 0; i < 3; i++) {
    float v = eye.v[i];
    float lo = c->aabb_min[i], hi = c->aabb_max[i];
    float dd = v < lo ? lo - v : (v > hi ? v - hi : 0.0f);
    d2 += dd * dd;
  }
  return d2 < DRAW_DISTANCE * DRAW_DISTANCE;
}

int main(void)
{
  debug_init(DEBUG_FEATURE_ALL);
  asset_init_compression(2);
  dfs_init(DFS_DEFAULT_LOCATION);
  display_init(RESOLUTION_320x240, DEPTH_16_BPP, FB_COUNT, GAMMA_NONE, FILTERS_RESAMPLE_ANTIALIAS);
  rdpq_init();
  joypad_init();
  t3d_init((T3DInitParams){});

  T3DViewport viewport = t3d_viewport_create_buffered(FB_COUNT);
  T3DModel *models[U64_CELL_COUNT];
  T3DMat4FP *mats = malloc_uncached(sizeof(T3DMat4FP) * U64_CELL_COUNT * FB_COUNT);
  for(int i = 0; i < U64_CELL_COUNT; i++) {
    models[i] = t3d_model_load(U64_CELLS[i].file);
  }

  // Light is baked into vertex colours, so runtime lighting is neutral: white ambient, no lights.
  uint8_t ambient[4] = {0xFF, 0xFF, 0xFF, 0xFF};

  fm_vec3_t eye = {{U64_START[0], U64_START[1] + EYE_HEIGHT_PAD, U64_START[2]}};
  float yaw = 0.0f;
  int frame = 0;
  uint64_t last = get_ticks();

  for(;;) {
    uint64_t now = get_ticks();
    float dt = (float)TICKS_DISTANCE(last, now) / TICKS_PER_SECOND;
    last = now;
    frame = (frame + 1) % FB_COUNT;

    joypad_poll();
    joypad_inputs_t in = joypad_get_inputs(JOYPAD_PORT_1);
    yaw -= (in.stick_x / 80.0f) * TURN_SPEED * dt;
    float fwd = (in.stick_y / 80.0f) * MOVE_SPEED * dt;
    eye.v[0] += -fm_sinf(yaw) * fwd;
    eye.v[2] += -fm_cosf(yaw) * fwd;
    fm_vec3_t target = {{eye.v[0] - fm_sinf(yaw), eye.v[1], eye.v[2] - fm_cosf(yaw)}};

    t3d_viewport_set_projection(&viewport, T3D_DEG_TO_RAD(80.0f), 0.2f, DRAW_DISTANCE);
    t3d_viewport_look_at(&viewport, &eye, &target, &(fm_vec3_t){{0, 1, 0}});

    for(int i = 0; i < U64_CELL_COUNT; i++) {
      const u64_cell_t *c = &U64_CELLS[i];
      float s = c->scale;  // model units -> metres
      t3d_mat4fp_from_srt_euler(&mats[i * FB_COUNT + frame], (float[3]){s, s, s}, (float[3]){0, 0, 0}, (float[3]){c->origin[0], c->origin[1], c->origin[2]});
    }

    rdpq_attach(display_get(), display_get_zbuf());
    t3d_frame_start();
    t3d_viewport_attach(&viewport);
    t3d_screen_clear_color(RGBA32(24, 28, 40, 0xFF));
    t3d_screen_clear_depth();
    t3d_light_set_ambient(ambient);
    t3d_light_set_count(0);

    for(int i = 0; i < U64_CELL_COUNT; i++) {
      if(!cell_visible(&U64_CELLS[i], eye)) continue;
      t3d_matrix_push(&mats[i * FB_COUNT + frame]);
      t3d_model_draw(models[i]);
      t3d_matrix_pop(1);
    }
    rdpq_detach_show();
  }
}
