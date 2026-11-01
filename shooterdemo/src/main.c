// Runs GASShooter's scenario suite on the N64 and prints the result (see tests/bp/run_shooter.c).
#include <libdragon.h>
#include <stdio.h>
#include "u64_gas_core.h"

const u64_bpfn *u64_bp_registry(void);
int u64_bp_registry_count(void);
int u64_run_shooter_scenarios(int verbose);
#include "u64_widgets.h"
extern u64_draw_cmd u64_hud_snap[2][96]; extern int u64_hud_snap_n[2];
void u64_hud_render_loop(const u64_draw_cmd *const *views, const int *counts, int nviews, const char *const *lines, int nlines);

int main(void)
{
  console_init();
  console_set_debug(false);
  u64_bp_table = u64_bp_registry();
  u64_bp_count = u64_bp_registry_count();
  printf("Unreal64: GASShooter scenarios on N64\n%d compiled functions\n\n", u64_bp_count);
  int failures = u64_run_shooter_scenarios(0);
  printf("\n%s\n", failures ? "SOME CHECKS FAILED" : "ALL CHECKS PASSED");
  static char l1[40], l2[56];
  snprintf(l1, sizeof l1, "GASShooter: %s", failures ? "SOME CHECKS FAILED" : "ALL CHECKS PASSED");
  snprintf(l2, sizeof l2, "HUD: chest hold %d, enemy view %d draws", u64_hud_snap_n[0], u64_hud_snap_n[1]);
  static const char *lines[3]; lines[0] = "Unreal64 on N64"; lines[1] = l1; lines[2] = l2;
  static const u64_draw_cmd *views[2]; views[0] = u64_hud_snap[0]; views[1] = u64_hud_snap[1];
  u64_hud_render_loop(views, u64_hud_snap_n, 2, lines, 3);
}
