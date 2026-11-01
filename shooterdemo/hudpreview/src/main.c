#include <libdragon.h>
#include "u64_widgets.h"
extern const u64_draw_cmd hud_dump0[], hud_dump1[]; extern const int hud_dump0_n, hud_dump1_n;
void u64_hud_render_loop(const u64_draw_cmd *const *views, const int *counts, int nviews, const char *const *lines, int nlines);
int main(void)
{
  console_init();
  static const char *lines[3] = {"Unreal64 HUD preview", "draw lists from the scenarios", "1920x1080 layout -> 576x324"};
  static const u64_draw_cmd *views[2]; static int counts[2];
  views[0] = hud_dump0; views[1] = hud_dump1; counts[0] = hud_dump0_n; counts[1] = hud_dump1_n;
  u64_hud_render_loop(views, counts, 2, lines, 3);
  return 0;
}
