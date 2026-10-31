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

int u64_run_scenarios(int verbose);

int main(void)
{
  console_init();
  console_set_debug(false);
  u64_bp_table = u64_bp_registry();
  u64_bp_count = u64_bp_registry_count();
  u64_log = logf_;

  printf("Unreal64: Blueprints on N64\n%d compiled functions\n", u64_bp_count);
  u64_level_load();
  int placed = 0; for(int i = 0; i < u64_obj_count(); i++) placed += u64_obj_is_actor(u64_obj_at(i));
  printf("level: %d actors\n\n", placed);

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
  int alive = 0; for(int i = 0; i < u64_obj_count(); i++) alive += u64_obj_is_actor(u64_obj_at(i));
  printf("\nt=12.0 done, %d actors alive\n\n", alive);
  u64_run_scenarios(0);   // GAS / ability / task / out-param scenarios against the compiled Blueprints
  for(;;) { console_render(); }
}
