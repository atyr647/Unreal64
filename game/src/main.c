// Unreal64 game: the converted Unreal level running on the N64. Top-down view of the live world state, HUD from the real
// attributes, controller -> the ported hero input. With no controller input for the first seconds, an autoplay script
// drives the hero (walk to the red spawner and shoot), which is how it runs unattended in an emulator.
#include <libdragon.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include "u64_gas_core.h"
#include "../../runtime/u64_game.h"

const u64_bpfn *u64_bp_registry(void);
int u64_bp_registry_count(void);

#define SCALE 0.16f                       // pixels per Unreal unit (cm)
static color_t C(int r, int g, int b) { return RGBA32(r, g, b, 255); }

static void box(surface_t *d, int x, int y, int w, int h, uint32_t col) { if (w > 0 && h > 0) graphics_draw_box(d, x, y, w, h, col); }
static void bar(surface_t *d, int x, int y, int w, int h, float frac, uint32_t fg)
{
  if (frac < 0) frac = 0; if (frac > 1) frac = 1;
  box(d, x, y, w, h, graphics_make_color(30, 30, 36, 255));
  box(d, x, y, (int)(w * frac), h, fg);
}
static float attr(u64_obj *a, const char *n) { return u64_attr_get(a, n); }

// Autoplay: the same script as the host test (tests/bp/run_game.c).
static void autoplay(float t, u64_pad *pad, int *turned)
{
  memset(pad, 0, sizeof *pad);
  if (!*turned) { pad->turn_delta = 6.8f; *turned = 1; }
  if (t > 0.2f && t < 1.0f) pad->forward = 1;
  if (t > 2.4f) pad->btn[U64_BTN_FIRE] = ((int)(t * 4)) % 3 == 0;
}

int main(void)
{
  display_init(RESOLUTION_320x240, DEPTH_16_BPP, 2, GAMMA_NONE, FILTERS_RESAMPLE);
  joypad_init();
  u64_bp_table = u64_bp_registry(); u64_bp_count = u64_bp_registry_count();
  u64_level_load(); u64_game_start();

  uint32_t col_bg = graphics_make_color(14, 16, 24, 255), col_white = graphics_make_color(235, 235, 235, 255);
  float t = 0, cx = 0, cy = 0; int turned = 0, manual = 0; uint64_t last = get_ticks();
  float last_pos[2] = {-970, -346};

  for (;;) {
    uint64_t now = get_ticks();
    float dt = (float)TICKS_DISTANCE(last, now) / TICKS_PER_SECOND; last = now;
    if (dt > 0.1f) dt = 0.1f;
    t += dt;

    // ---- input
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
    if (!manual) autoplay(t, &pad, &turned);
    u64_game_update(dt, &pad);

    // ---- camera follows the player's pawn (or where it last was)
    u64_obj *pawn = u64_player_pawn();
    if (pawn) { u64_v *l = u64_ivar(pawn, "Location"); last_pos[0] = l->u.v[0]; last_pos[1] = l->u.v[1]; }
    cx = last_pos[0]; cy = last_pos[1];

    // ---- draw
    surface_t *d = display_get();
    graphics_fill_screen(d, col_bg);
    for (int pass = 0; pass < 2; pass++)                         // volumes first, then characters and projectiles on top
    for (int i = 0; i < u64_obj_count(); i++) {
      u64_obj *o = u64_obj_at(i);
      if (!u64_obj_is_actor(o)) continue;
      const char *cls = u64_obj_class(o);
      u64_v *loc = u64_ivar(o, "Location");
      int sx = 160 + (int)((loc->u.v[0] - cx) * SCALE), sy = 120 - (int)((loc->u.v[1] - cy) * SCALE);
      const u64_shape_def *sh = 0;
      for (const u64_shape_def *s = u64_shapes; s->cls; s++) if (!strcmp(s->cls, cls)) { sh = s; break; }
      if (sx < -40 || sx > 360 || sy < -40 || sy > 280) continue;
      if (pass == 0 && sh && sh->kind == 1) {                    // trigger volumes: dim filled boxes in their effect's colour
        int w = (int)(sh->a * SCALE), h = (int)(sh->b * SCALE);
        uint32_t col = strstr(cls, "Damage") ? graphics_make_color(90, 28, 28, 255) : strstr(cls, "Regen") ? graphics_make_color(24, 84, 52, 255) : graphics_make_color(28, 52, 100, 255);
        box(d, sx - w, sy - h, 2 * w, 2 * h, col);
      } else if (pass == 1 && sh && sh->kind == 2) {             // characters: the capsule's real radius
        int r = (int)(sh->a * SCALE); if (r < 3) r = 3;
        int is_pl = (o == pawn), hero = strstr(cls, "Hero") != 0;
        uint32_t col = is_pl ? graphics_make_color(90, 230, 120, 255) : hero ? graphics_make_color(220, 90, 220, 255) : strstr(cls, "Red") ? graphics_make_color(230, 70, 60, 255) : graphics_make_color(70, 130, 235, 255);
        box(d, sx - r, sy - r, 2 * r, 2 * r, col);
        u64_obj *asc = u64_asc_of(o);
        float hp = attr(asc, "Health"), mx = attr(asc, "MaxHealth");
        if (mx > 0) bar(d, sx - r - 2, sy - r - 5, 2 * r + 4, 3, hp / mx, hp / mx > 0.5f ? graphics_make_color(80, 220, 90, 255) : graphics_make_color(230, 90, 60, 255));
        if (is_pl) {                                             // facing indicator along the control rotation
          u64_v *cr = u64_ivar(o, "ControlRotation"); float yaw = (cr->t == U64_VEC ? cr->u.v[1] : 0) * 0.0174533f;
          for (int k = 4; k < 22; k += 2) box(d, sx + (int)(cosf(yaw) * k), sy - (int)(sinf(yaw) * k), 2, 2, col_white);
        }
      } else if (pass == 1 && strstr(cls, "GunProjectile")) {
        box(d, sx - 1, sy - 1, 3, 3, col_white);
      } else if (pass == 1 && strstr(cls, "Spawner")) {
        box(d, sx - 2, sy - 2, 5, 5, graphics_make_color(150, 150, 160, 255));
      }
    }

    // ---- HUD (real attributes of the player's pawn)
    char buf[64];
    snprintf(buf, sizeof buf, "t=%d.%d  %s", (int)t, (int)(t * 10) % 10, manual ? "PLAYER" : "AUTOPLAY");
    graphics_set_color(graphics_make_color(200, 200, 210, 255), 0); graphics_draw_text(d, 6, 6, buf);
    if (pawn) {
      u64_obj *asc = u64_asc_of(pawn);
      float hp = attr(asc, "Health"), mhp = attr(asc, "MaxHealth"), mp = attr(asc, "Mana"), mmp = attr(asc, "MaxMana"), st = attr(asc, "Stamina"), mst = attr(asc, "MaxStamina");
      bar(d, 6, 200, 120, 6, mhp > 0 ? hp / mhp : 0, graphics_make_color(220, 60, 60, 255));
      bar(d, 6, 210, 120, 6, mmp > 0 ? mp / mmp : 0, graphics_make_color(70, 120, 230, 255));
      bar(d, 6, 220, 120, 6, mst > 0 ? st / mst : 0, graphics_make_color(230, 200, 60, 255));
      snprintf(buf, sizeof buf, "HP %d/%d", (int)(hp + 0.5f), (int)(mhp + 0.5f)); graphics_draw_text(d, 132, 199, buf);
      snprintf(buf, sizeof buf, "MP %d  ST %d", (int)(mp + 0.5f), (int)(st + 0.5f)); graphics_draw_text(d, 132, 214, buf);
      if (u64_tag_has(asc, "State.Sprinting")) graphics_draw_text(d, 230, 6, "SPRINT");
      if (u64_tag_has(asc, "State.AimDownSights")) graphics_draw_text(d, 230, 16, "ADS");
    } else {
      graphics_draw_text(d, 104, 112, "YOU DIED - RESPAWNING");
    }
    display_show(d);
  }
}
