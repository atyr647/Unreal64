// Linux renderer: runs the converted game with the same deterministic autoplay as the ROM and writes the frames the ROM
// would draw (libdragon's own graphics code) as PPM files. usage: render_host <outdir> <frame> [frame...]
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "u64_gas_core.h"
#include "../../game/src/render.h"

const u64_bpfn *u64_bp_registry(void); int u64_bp_registry_count(void);

static void write_ppm(const char *path, surface_t *s)
{
  FILE *f = fopen(path, "wb"); fprintf(f, "P6\n%d %d\n255\n", s->width, s->height);
  for (int y = 0; y < s->height; y++) for (int x = 0; x < s->width; x++) {
    uint16_t p = ((uint16_t *)((char *)s->buffer + y * s->stride))[x];       // RGBA5551
    int r = (p >> 11) & 31, g = (p >> 6) & 31, b = (p >> 1) & 31;
    unsigned char c[3] = {(unsigned char)((r << 3) | (r >> 2)), (unsigned char)((g << 3) | (g >> 2)), (unsigned char)((b << 3) | (b >> 2))};
    fwrite(c, 1, 3, f);
  }
  fclose(f);
}
int main(int argc, char **argv)
{
  u64_bp_table = u64_bp_registry(); u64_bp_count = u64_bp_registry_count();
  u64_level_load(); u64_game_start();
  if (argc > 1 && !strcmp(argv[1], "--scan")) {                      // list frames that have a projectile in flight
    for (int f = 0; f < 700; f++) {
      u64_demo_step(f);
      for (int i = 0; i < u64_obj_count(); i++) if (u64_obj_is_actor(u64_obj_at(i)) && strstr(u64_obj_class(u64_obj_at(i)), "GunProjectile")) { printf("%d ", f + 1); break; }
    }
    printf("\n"); return 0;
  }
  if (argc > 2 && !strcmp(argv[1], "--trace")) {
    for (int f = 0; f < atoi(argv[2]); f++) { u64_demo_step(f); u64_obj *pw = u64_player_pawn(); if (pw) { u64_obj *a = u64_asc_of(pw); int frame = f + 1; printf("T %d hp=%.3f mp=%.3f st=%.3f x=%.3f y=%.3f n=%d\n", frame, (double)u64_attr_get(a, "Health"), (double)u64_attr_get(a, "Mana"), (double)u64_attr_get(a, "Stamina"), (double)u64_ivar(pw, "Location")->u.v[0], (double)u64_ivar(pw, "Location")->u.v[1], u64_obj_count()); } }
    return 0;
  }
  static uint16_t pix[320 * 240];
  surface_t surf = surface_make(pix, FMT_RGBA16, 320, 240, 640);
  int next = 2, frame = 0;
  for (; next < argc; next++) {
    int target = atoi(argv[next]);
    while (frame < target) u64_demo_step(frame++);
    u64_render_frame(&surf, frame / 30.0f, "AUTOPLAY");
    char path[512]; snprintf(path, sizeof path, "%s/linux_%d.ppm", argv[1], target);
    write_ppm(path, &surf);
  }
  return u64_rt_overflows != 0;
}
