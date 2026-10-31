// N64 host for the C++ rules test: the same test_ammo.cpp, printed on the libdragon console.
#include <libdragon.h>
#define main ammo_main
#include "../test_ammo.cpp"
#undef main
int main(void)
{
  console_init();
  console_set_render_mode(RENDER_MANUAL);
  debug_init_isviewer();
  printf("C++ on N64: GSAmmoAttributeSet.cpp\n(unmodified, built with mips64-elf-g++)\n\n");
  int f = ammo_main();
  printf("\nresult: %s\n", f ? "FAILED" : "ALL PASSED");
  console_render();
  for (;;) {}
}
