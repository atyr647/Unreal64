// Draws the HUD's draw lists (runtime/u64_widgets.c) on the N64 at 640 x 480: the 1920 x 1080 layout space scaled by 0.3 to 576 x 324, bottom-aligned inside a safe margin (a 25-unit bar is 7.5 pixels
// high: the 8 x 8 font just fits). Rectangles keep their colours and
// alpha; text uses the built-in 8 x 8 font (the asset's fonts are not available), placed by the widget's rectangle and justification. Images that are textures are drawn as frames.
#include <libdragon.h>
#include <string.h>
#include <stdio.h>
#include "u64_widgets.h"

// The 3D world behind each view as lines (the scenarios' wireframe of the level's collision, projected through the player's camera): clipped to the 1920 x 1080 viewport (Liang-Barsky) first.
extern u64_draw_cmd u64_world_snap[2][400] __attribute__((weak)); extern int u64_world_snap_n[2] __attribute__((weak));
static int clip_line(float *x0, float *y0, float *x1, float *y1, float w, float h)
{
  float dx = *x1 - *x0, dy = *y1 - *y0, t0 = 0, t1 = 1, p[4] = {-dx, dx, -dy, dy}, q[4] = {*x0, w - *x0, *y0, h - *y0};
  for (int i = 0; i < 4; i++) {
    if (p[i] == 0) { if (q[i] < 0) return 0; continue; }
    float r = q[i] / p[i];
    if (p[i] < 0) { if (r > t1) return 0; if (r > t0) t0 = r; } else { if (r < t0) return 0; if (r < t1) t1 = r; }
  }
  float ax = *x0 + t0 * dx, ay = *y0 + t0 * dy, bx = *x0 + t1 * dx, by = *y0 + t1 * dy; *x0 = ax; *y0 = ay; *x1 = bx; *y1 = by;
  return 1;
}
static uint32_t col(const float *c) { return graphics_make_color((int)(c[0] * 255.0f + 0.5f), (int)(c[1] * 255.0f + 0.5f), (int)(c[2] * 255.0f + 0.5f), (int)(c[3] * 255.0f + 0.5f)); }
void u64_hud_render_loop(const u64_draw_cmd *const *views, const int *counts, int nviews, const char *const *lines, int nlines)
{
  console_close();
  display_init(RESOLUTION_640x480, DEPTH_16_BPP, 2, GAMMA_NONE, FILTERS_RESAMPLE);
  const float sc = 0.3f; const int ox = 32, oy = 132, vw = 576, vh = 324;      // 1920 x 1080 -> 576 x 324
  for (int frame = 0;; frame++) {
    const u64_draw_cmd *cmds = views[(frame / 240) % nviews]; int n = counts[(frame / 240) % nviews];                // each view for about four seconds
    surface_t *d = display_get();
    graphics_fill_screen(d, graphics_make_color(24, 28, 36, 255));
    graphics_draw_box(d, ox, oy, vw, vh, graphics_make_color(40, 52, 64, 255));                      // the viewport (the wireframe world is drawn on it)
    if (&u64_world_snap[0][0] && &u64_world_snap_n[0]) {
      int v = (frame / 240) % nviews;
      for (int i = 0; i < u64_world_snap_n[v]; i++) {
        const u64_draw_cmd *c = &u64_world_snap[v][i]; float x0 = c->x, y0 = c->y, x1 = c->w, y1 = c->h;
        if (c->kind == 3 && clip_line(&x0, &y0, &x1, &y1, 1920.0f, 1080.0f))
          graphics_draw_line(d, ox + (int)(x0 * sc), oy + (int)(y0 * sc), ox + (int)(x1 * sc), oy + (int)(y1 * sc), col(c->rgba));
      }
    }
    for (int i = 0; i < n; i++) {
      const u64_draw_cmd *c = &cmds[i]; int x = ox + (int)(c->x * sc + 0.5f), y = oy + (int)(c->y * sc + 0.5f), w = (int)(c->w * sc + 0.5f), h = (int)(c->h * sc + 0.5f);
      if (c->kind == 0) { if (w > 0 && h > 0) { if (c->rgba[3] >= 0.999f) graphics_draw_box(d, x, y, w, h, col(c->rgba)); else graphics_draw_box_trans(d, x, y, w, h, col(c->rgba)); } }
      else if (c->kind == 2) { uint32_t f = graphics_make_color(160, 160, 160, 255); graphics_draw_box(d, x, y, w, 1, f); graphics_draw_box(d, x, y + h - 1, w, 1, f); graphics_draw_box(d, x, y, 1, h, f); graphics_draw_box(d, x + w - 1, y, 1, h, f); }
      else if (c->kind == 1 && c->text) {
        int tw = (int)strlen(c->text) * 8, tx = c->justify == 1 ? x + (w - tw) / 2 : c->justify == 2 ? x + w - tw : x, ty = y + (h - 8) / 2;
        graphics_set_color(col(c->rgba), 0);
        graphics_draw_text(d, tx, ty, c->text);
      }
    }
    graphics_set_color(graphics_make_color(255, 255, 255, 255), 0);
    for (int i = 0; i < nlines; i++) graphics_draw_text(d, 32, 16 + i * 12, lines[i]);
    display_show(d);
  }
}
