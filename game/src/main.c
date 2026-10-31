// Unreal64 game ROM. Normal mode: joypad input (autoplay until touched). Comparison mode (U64_FREEZE_AT defined):
// deterministic fixed-step autoplay that freezes on one frame, for pixel comparison against the Linux renderer.
#include <libdragon.h>
#include <string.h>
#include <math.h>
#include "u64_gas_core.h"
#include "../../runtime/u64_game.h"
#include "render.h"
#if __has_include("freeze.h")
#include "freeze.h"
#endif

const u64_bpfn *u64_bp_registry(void);
int u64_bp_registry_count(void);

int main(void)
{
#ifdef U64_TRACE
  debug_init_isviewer();
#endif
  display_init(RESOLUTION_320x240, DEPTH_16_BPP, 2, GAMMA_NONE, FILTERS_RESAMPLE);
  joypad_init();
  u64_bp_table = u64_bp_registry(); u64_bp_count = u64_bp_registry_count();
  u64_level_load(); u64_game_start();

#ifdef U64_FREEZE_AT
  int frame = 0;
  for (;;) {
    if (frame < U64_FREEZE_AT) { u64_demo_step(frame); frame++;
#ifdef U64_TRACE
      { u64_obj *pw = u64_player_pawn(); if (pw) { u64_obj *a = u64_asc_of(pw); debugf("T %d hp=%.3f mp=%.3f st=%.3f x=%.3f y=%.3f n=%d\n", frame, (double)u64_attr_get(a, "Health"), (double)u64_attr_get(a, "Mana"), (double)u64_attr_get(a, "Stamina"), (double)u64_ivar(pw, "Location")->u.v[0], (double)u64_ivar(pw, "Location")->u.v[1], u64_obj_count()); } }
#endif
      if (frame < U64_FREEZE_AT) continue; }
    surface_t *d = display_get();
    u64_render_frame(d, frame / 30.0f, "AUTOPLAY");
    display_show(d);
  }
#else
  float t = 0; int turned = 0, manual = 0; uint64_t last = get_ticks();
  for (;;) {
    uint64_t now = get_ticks();
    float dt = (float)TICKS_DISTANCE(last, now) / TICKS_PER_SECOND; last = now;
    if (dt > 0.1f) dt = 0.1f;
    t += dt;
    joypad_poll();
    joypad_inputs_t in = joypad_get_inputs(JOYPAD_PORT_1);
    u64_pad pad; memset(&pad, 0, sizeof pad);
    pad.forward = in.stick_y / 80.0f; pad.right = in.stick_x / 80.0f;
    pad.turn = (in.btn.c_right ? 1.0f : 0.0f) - (in.btn.c_left ? 1.0f : 0.0f);
    pad.btn[U64_BTN_JUMP] = in.btn.a; pad.btn[U64_BTN_SPRINT] = in.btn.b; pad.btn[U64_BTN_DASH] = in.btn.z;
    pad.btn[U64_BTN_ADS] = in.btn.l; pad.btn[U64_BTN_FIRE] = in.btn.r;
    pad.btn[U64_BTN_METEOR] = in.btn.c_down; pad.btn[U64_BTN_ARMOR] = in.btn.c_up;
    int touched = fabsf(pad.forward) > 0.2f || fabsf(pad.right) > 0.2f || in.btn.a || in.btn.b || in.btn.z || in.btn.l || in.btn.r || in.btn.c_left || in.btn.c_right || in.btn.c_down || in.btn.c_up;
    if (touched) manual = 1;
    if (!manual) u64_autoplay(t, &pad, &turned);
    u64_game_update(dt, &pad);
    surface_t *d = display_get();
    u64_render_frame(d, t, manual ? "PLAYER" : "AUTOPLAY");
    display_show(d);
  }
#endif
}
