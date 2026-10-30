// Host harness: run the compiled BP_MinionSpawner BeginPlay and print the trace of native calls it makes.
#include "u64_rt.h"
#include <stdio.h>
#include <stdarg.h>
const u64_bpfn *u64_bp_registry(void); int u64_bp_registry_count(void);
static void logf_(const char *f, ...) { va_list a; va_start(a, f); vprintf(f, a); va_end(a); putchar('\n'); }
static u64_v native(u64_obj *self, const char *fn, int argc, const u64_v *argv)
{
  (void)self; (void)argv;
  printf("  [native] %s/%d\n", fn, argc);
  if (!strcmp(fn, "HasAuthority")) return u64_bool(1);
  if (!strcmp(fn, "BeginDeferredActorSpawnFromClass") || !strcmp(fn, "FinishSpawningActor")) return u64_objv(u64_new("Minion", "BP_Minion"));
  return (u64_v){U64_NONE};
}
int main(void)
{
  u64_bp_table = u64_bp_registry(); u64_bp_count = u64_bp_registry_count();
  u64_native_hook = native; u64_log = logf_;
  u64_obj *spawner = u64_new("Spawner0", "BP_MinionSpawner");
  *u64_ivar(spawner, "RespawnDelay") = u64_float(3.0f);
  u64_call(spawner, "ReceiveBeginPlay", 0, 0);
  return 0;
}
