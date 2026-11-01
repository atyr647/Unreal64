// Draws the HUD's draw list (runtime/u64_widgets.c) on the N64 at 640 x 480: the 1280 x 720 design viewport scaled by 0.45 to 576 x 324, bottom-aligned inside a safe margin (the bars are 11 pixels
// high, so the 8 x 8 font fits in them). Rectangles keep their colours and
// alpha; text uses the built-in 8 x 8 font (the asset's fonts are not available), placed by the widget's rectangle and justification. Images that are textures are drawn as frames.
#include <libdragon.h>
#include <string.h>
#include <stdio.h>
#include "u64_widgets.h"

static uint32_t col(const float *c) { return graphics_make_color((int)(c[0] * 255.0f + 0.5f), (int)(c[1] * 255.0f + 0.5f), (int)(c[2] * 255.0f + 0.5f), (int)(c[3] * 255.0f + 0.5f)); }
void u64_hud_render_loop(const u64_draw_cmd *cmds, int n, const char *const *lines, int nlines)
{
  console_close();
  display_init(RESOLUTION_640x480, DEPTH_16_BPP, 2, GAMMA_NONE, FILTERS_RESAMPLE);
  const float sc = 0.45f; const int ox = 32, oy = 132, vw = 576, vh = 324;      // 1280 x 720 -> 576 x 324
  for (;;) {
    surface_t *d = display_get();
    graphics_fill_screen(d, graphics_make_color(24, 28, 36, 255));
    graphics_draw_box(d, ox, oy, vw, vh, graphics_make_color(40, 52, 64, 255));                      // the viewport (no 3D scene here)
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
