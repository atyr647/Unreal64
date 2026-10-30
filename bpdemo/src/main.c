// Runs the Unreal level's compiled Blueprints on the N64 and prints what happens.
#include <libdragon.h>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include "u64_rt.h"

const u64_bpfn *u64_bp_registry(void);
int u64_bp_registry_count(void);

static void logf_(const char *f, ...)
{
  if(strncmp(f, "  [", 3)) return;  // only world events: spawns, kills
  va_list a; va_start(a, f); vprintf(f, a); va_end(a); putchar('\n');
}

int main(void)
{
  console_init();
  console_set_debug(false);
  u64_bp_table = u64_bp_registry();
  u64_bp_count = u64_bp_registry_count();
  u64_log = logf_;

  printf("Unreal64: Blueprints on N64\n%d compiled functions\n", u64_bp_count);
  u64_level_load();
  printf("level: %d actors\n\n", u64_obj_count());

  int killed = 0;
  for(int step = 0; step < 120; step++) {
    u64_world_tick(0.1f);
    if(!killed && u64_world_time() >= 5.0f) {
      for(int i = 0; i < u64_obj_count(); i++) {
        u64_obj *o = u64_obj_at(i);
        if(u64_obj_alive(o) && !strncmp(u64_obj_name(o), "BP_RedMinion", 12)) {
          printf("-- t=5.0 killing %s\n", u64_obj_name(o));
          u64_kill(o); killed = 1; break;
        }
      }
    }
  }
  printf("\nt=12.0 done, %d actors total\n", u64_obj_count());
  for(;;) { console_render(); }
}
