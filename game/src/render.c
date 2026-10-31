#if __has_include("freeze.h")
#include "freeze.h"
#endif
// Shared by the N64 ROM and the Linux comparison renderer: the autoplay script, one deterministic game step, and the
// frame drawing. Drawing uses libdragon's graphics_* API so both sides run the same drawing code.
#include <libdragon.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include "u64_gas_core.h"
#include "../../runtime/u64_game.h"
#include "render.h"

#define SCALE 0.16f                       // pixels per Unreal unit (cm)
static void box(surface_t *d, int x, int y, int w, int h, uint32_t col) { if (w > 0 && h > 0) graphics_draw_box(d, x, y, w, h, col); }
static void bar(surface_t *d, int x, int y, int w, int h, float frac, uint32_t fg)
{
  if (frac < 0) frac = 0; if (frac > 1) frac = 1;
  box(d, x, y, w, h, graphics_make_color(30, 30, 36, 255));
  box(d, x, y, (int)(w * frac), h, fg);
}
static float attr(u64_obj *a, const char *n) { return u64_attr_get(a, n); }

// The demo player: walks to the red spawner sprinting, shoots (aiming down sights for a while), dashes, and is finally
// killed by a damage effect to show the death/respawn flow. t is game time in seconds.
void u64_autoplay(float t, u64_pad *pad, int *turned)
{
  memset(pad, 0, sizeof *pad);
  if (!*turned) { pad->turn_delta = 6.8f; *turned = 1; }
  if (t > 0.2f && t < 1.0f) { pad->forward = 1; pad->btn[U64_BTN_SPRINT] = 1; }
  if (t > 2.4f && t < 14.0f) pad->btn[U64_BTN_FIRE] = ((int)(t * 4)) % 3 == 0;
  if (t > 5.0f && t < 7.0f) pad->btn[U64_BTN_ADS] = 1;
  if (t > 8.0f && t < 8.2f) pad->btn[U64_BTN_DASH] = 1;
}

static int turned_, killed_;
static float last_pos[2] = {-970, -346};                       // camera follows the pawn; holds its last position while dead
static void track_camera(void) { u64_obj *pw = u64_player_pawn(); if (pw) { u64_v *l = u64_ivar(pw, "Location"); last_pos[0] = l->u.v[0]; last_pos[1] = l->u.v[1]; } }
void u64_demo_step(int frame)
{
  float t = frame / 30.0f; u64_pad pad;
  u64_autoplay(t, &pad, &turned_);
  if (!killed_ && t >= 14.0f && u64_player_pawn()) {           // lethal damage through the real effect pipeline
    u64_obj *pl = u64_player_pawn(), *asc = u64_asc_of(pl);
    u64_v spec = u64_call(asc, "MakeOutgoingSpec", 2, (u64_v[]){u64_cls("GE_DamageVolume_C"), u64_float(1)});
    u64_call(asc, "AssignTagSetByCallerMagnitude", 3, (u64_v[]){spec, u64_name("Data.Damage"), u64_float(5000)});
    u64_call(asc, "BP_ApplyGameplayEffectSpecToSelf", 1, &spec);
    killed_ = 1;
  }
  u64_game_update(1.0f / 30.0f, &pad);
  track_camera();
}

void u64_render_frame(surface_t *d, float t, const char *mode)
{
  uint32_t col_bg = graphics_make_color(14, 16, 24, 255), col_white = graphics_make_color(235, 235, 235, 255);
  u64_obj *pawn = u64_player_pawn();
  track_camera();
  float cx = last_pos[0], cy = last_pos[1];

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

  char buf[64];
  snprintf(buf, sizeof buf, "t=%d.%d  %s", (int)t, (int)(t * 10) % 10, mode);
  graphics_set_color(graphics_make_color(200, 200, 210, 255), 0); graphics_draw_text(d, 6, 6, buf);
  if (pawn) {
    u64_obj *asc = u64_asc_of(pawn);
    float hp = attr(asc, "Health"), mhp = attr(asc, "MaxHealth"), mp = attr(asc, "Mana"), mmp = attr(asc, "MaxMana"), st = attr(asc, "Stamina"), mst = attr(asc, "MaxStamina");
    bar(d, 6, 200, 120, 6, mhp > 0 ? hp / mhp : 0, graphics_make_color(220, 60, 60, 255));
    bar(d, 6, 210, 120, 6, mmp > 0 ? mp / mmp : 0, graphics_make_color(70, 120, 230, 255));
    bar(d, 6, 220, 120, 6, mst > 0 ? st / mst : 0, graphics_make_color(230, 200, 60, 255));
    snprintf(buf, sizeof buf, "HP %d/%d", (int)(hp + 0.5f), (int)(mhp + 0.5f)); graphics_draw_text(d, 132, 199, buf);
    snprintf(buf, sizeof buf, "MP %d  ST %d", (int)(mp + 0.5f), (int)(st + 0.5f)); graphics_draw_text(d, 132, 214, buf);
#ifdef U64_DBG
    snprintf(buf, sizeof buf, "%.4f", (double)st); graphics_draw_text(d, 132, 226, buf);
#endif
    if (u64_tag_has(asc, "State.Sprinting")) graphics_draw_text(d, 230, 6, "SPRINT");
    if (u64_tag_has(asc, "State.AimDownSights")) graphics_draw_text(d, 230, 16, "ADS");
  } else {
    graphics_draw_text(d, 104, 112, "YOU DIED - RESPAWNING");
  }
}
