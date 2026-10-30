// Runs compiled Unreal Blueprints on the N64 and prints what they do. Natives are stubbed with a call log.
#include <libdragon.h>
#include <stdarg.h>
#include <stdio.h>
#include "u64_rt.h"

const u64_bpfn *u64_bp_registry(void);
int u64_bp_registry_count(void);

static void logf_(const char *f, ...)
{
  va_list a; va_start(a, f); vprintf(f, a); va_end(a); putchar('\n');
}

static u64_v native(u64_obj *self, const char *fn, int argc, const u64_v *argv)
{
  (void)self; (void)argv;
  printf("  [native] %s/%d\n", fn, argc);
  if(!strcmp(fn, "HasAuthority")) return u64_bool(1);
  return (u64_v){U64_NONE};
}

int main(void)
{
  console_init();
  console_set_debug(false);
  u64_bp_table = u64_bp_registry();
  u64_bp_count = u64_bp_registry_count();
  u64_native_hook = native;
  u64_log = logf_;

  printf("Unreal64: Blueprint on N64\n%d compiled functions\n\n", u64_bp_count);
  u64_obj *spawner = u64_new("Spawner0", "BP_MinionSpawner");
  *u64_ivar(spawner, "RespawnDelay") = u64_float(3.0f);
  printf("BP_MinionSpawner.BeginPlay:\n");
  u64_call(spawner, "ReceiveBeginPlay", 0, 0);
  printf("\ndone\n");
  for(;;) { console_render(); }
}
