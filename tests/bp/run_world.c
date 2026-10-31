// Host harness: load the converted level, simulate 12 s, kill the first minion at t=5 and watch the Blueprints react.
#include "u64_rt.h"
#include <stdio.h>
#include <stdarg.h>
const u64_bpfn *u64_bp_registry(void); int u64_bp_registry_count(void);
static void logf_(const char *f, ...) { va_list a; va_start(a, f); vprintf(f, a); va_end(a); putchar('\n'); }
int main(int argc, char **argv)
{
  int verbose = argc > 1;
  u64_bp_table = u64_bp_registry(); u64_bp_count = u64_bp_registry_count();
  u64_log = logf_;
  u64_level_load();
  int actors0 = 0; for (int i = 0; i < u64_obj_count(); i++) actors0 += u64_obj_is_actor(u64_obj_at(i));
  printf("actors in level: %d\n", actors0);
  int killed = 0;
  for (int step = 0; step < 120; step++) {
    u64_world_tick(0.1f);
    if (!killed && u64_world_time() >= 5.0f) {
      for (int i = 0; i < u64_obj_count(); i++) {
        u64_obj *o = u64_obj_at(i);
        if (u64_obj_alive(o) && !strncmp(u64_obj_name(o), "BP_RedMinion", 12)) { printf("-- t=5.0: killing %s\n", u64_obj_name(o)); u64_kill(o); killed = 1; break; }
      }
    }
  }
  int alive = 0; for (int i = 0; i < u64_obj_count(); i++) if (u64_obj_is_actor(u64_obj_at(i))) alive++;
  printf("t=%.1f actors alive: %d\n", u64_world_time(), alive);
  if (u64_rt_overflows) { printf("RUNTIME STORAGE EXHAUSTED (%d)\n", u64_rt_overflows); return 1; }
  (void)verbose; return 0;
}
