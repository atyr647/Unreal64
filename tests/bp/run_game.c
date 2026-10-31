// Host autoplay: a scripted player walks to the red minion spawner and shoots; prints what happens each second.
#include "u64_gas_core.h"
#include "u64_game.h"
#include <stdio.h>
#include <stdarg.h>
#include <math.h>
const u64_bpfn *u64_bp_registry(void); int u64_bp_registry_count(void);
static int verbose;
static void lg(const char *f, ...) { if (!verbose) return; va_list a; va_start(a, f); vprintf(f, a); va_end(a); putchar('\n'); }
int main(int argc, char **argv)
{
  verbose = argc > 1;
  u64_bp_table = u64_bp_registry(); u64_bp_count = u64_bp_registry_count(); u64_log = lg;
  u64_level_load(); u64_game_start();
  float t = 0; int turned = 0;
  for (int step = 0; step < 600; step++, t += 0.05f) {          // 30 s
    u64_pad pad = {0};
    if (!turned) { pad.turn_delta = 6.8f; turned = 1; }          // 6.8 * 2.5 = 17 degrees toward the red spawner
    if (t > 0.2f && t < 1.0f) pad.forward = 1;
    if (t > 2.4f) pad.btn[U64_BTN_FIRE] = ((int)(t * 4)) % 3 == 0;      // pulse the fire button
    u64_game_update(0.05f, &pad);
    if (step % 20 == 19) {
      u64_obj *p = u64_player_pawn(); u64_v *pl = p ? u64_ivar(p, "Location") : 0;
      printf("t=%4.1f hero %s", t + 0.05f, p ? "" : "(none) ");
      if (p) printf("(%5.0f,%5.0f) hp=%.0f st=%.0f", pl->u.v[0], pl->u.v[1], u64_attr_get(u64_asc_of(p), "Health"), u64_attr_get(u64_asc_of(p), "Stamina"));
      for (int i = 0; i < u64_obj_count(); i++) { u64_obj *o = u64_obj_at(i);
        if (u64_obj_is_actor(o) && strstr(u64_obj_class(o), "Minion")) printf("  %s hp=%.0f", u64_obj_name(o), u64_attr_get(u64_asc_of(o), "Health")); }
      putchar('\n');
    }
  }
  return u64_rt_overflows != 0;
}
