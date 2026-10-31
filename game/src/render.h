#pragma once
#include <libdragon.h>
#include "../../runtime/u64_game.h"
void u64_autoplay(float t, u64_pad *pad, int *turned);
void u64_demo_step(int frame);                                  // one deterministic 1/30 s step of the autoplay game
void u64_render_frame(surface_t *d, float t, const char *mode);
