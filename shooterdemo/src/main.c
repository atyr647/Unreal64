// Runs GASShooter's scenario suite on the N64 and prints the result (see tests/bp/run_shooter.c).
#include <libdragon.h>
#include <stdio.h>
#include "u64_gas_core.h"

const u64_bpfn *u64_bp_registry(void);
int u64_bp_registry_count(void);
int u64_run_shooter_scenarios(int verbose);

int main(void)
{
  console_init();
  console_set_debug(false);
  u64_bp_table = u64_bp_registry();
  u64_bp_count = u64_bp_registry_count();
  printf("Unreal64: GASShooter scenarios on N64\n%d compiled functions\n\n", u64_bp_count);
  int failures = u64_run_shooter_scenarios(0);
  printf("\n%s\n", failures ? "SOME CHECKS FAILED" : "ALL CHECKS PASSED");
  for(;;) { console_render(); }
}
