# Linux vs ares comparison

`render_host` runs the converted game on Linux with the same fixed-step autoplay as the ROM and writes the frames the ROM
would draw (libdragon's own `graphics.c` for text/rects, via a tiny shim). Build it with the ROM's float flags, otherwise
`frame / 30.0f` rounds differently (libdragon builds with `-ffast-math`) and timing thresholds land one frame apart:

    gcc -w -O1 -ffast-math -ftrapping-math -fno-associative-math -DN64 -DLIBDRAGON_PREVIEW=2 -Itools/hostrender/shim \
        -I$L/include -I$L/src -Iruntime -o render_host tools/hostrender/render_host.c tools/hostrender/hostshim.c \
        game/src/render.c $L/src/graphics.c runtime/*.c <generated bp C> -lm

* `render_host <outdir> <frame...>` writes `linux_<frame>.ppm`; `render_host --trace N` prints per-frame state.
* ROM side: put `#define U64_FREEZE_AT <frame>` in `game/src/freeze.h` (gitignored) and build; the ROM steps N frames then
  holds that frame. Add `#define U64_TRACE 1` to print the same per-frame state through ISViewer (ares echoes it to stdout).
* `mk_cmp.py <dir> <out.png>` crops the ares screenshot, and builds Linux | ares | |diff| sheets with MAE.

Result: the per-frame state trace (HP/mana/stamina/position/object count, 549 frames) is identical on Linux and ares;
frames differ only by the N64 VI resampling blur (MAE ~3/255).
