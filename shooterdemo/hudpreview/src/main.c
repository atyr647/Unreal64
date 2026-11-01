#include <libdragon.h>
#include "u64_widgets.h"
extern const u64_draw_cmd hud_dump[]; extern const int hud_dump_n;
void u64_hud_render_loop(const u64_draw_cmd *cmds, int n, const char *const *lines, int nlines);
int main(void)
{
  console_init();
  static const char *lines[3] = {"Unreal64 HUD preview", "draw list from the scenarios", "1280x720 -> 576x324"};
  u64_hud_render_loop(hud_dump, hud_dump_n, lines, 3);
  return 0;
}
