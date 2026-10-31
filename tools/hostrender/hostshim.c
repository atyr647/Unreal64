// The hardware calls libdragon's graphics.c makes, as plain C.
#include <stdint.h>
#include <string.h>
int vi_get_bpp(void) { return 16; }
void sys_hw_memset16(void *p, uint16_t v, size_t bytes) { uint16_t *q = p; for (size_t i = 0; i < bytes / 2; i++) q[i] = v; }
void sys_hw_memset32(void *p, uint32_t v, size_t bytes) { uint32_t *q = p; for (size_t i = 0; i < bytes / 4; i++) q[i] = v; }
void *__sprite_upgrade(void *s) { return s; }   // sprite drawing is not used by the game
