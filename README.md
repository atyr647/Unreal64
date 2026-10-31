# Unreal64

Put an **unmodified Unreal Engine project** on a real N64 — as much of it as physically survives.

The engine can't execute on an N64 (4–8 MB RAM, 4 KB texture memory, no shaders), so this repo does the expensive
work on a PC and gives the N64 the result. Read [docs/PLAN.md](docs/PLAN.md): it states what each Unreal feature
becomes and, importantly, what is verified versus only written.

- **Convert** (`tools/n64fit`, `viewer/`): Unreal level -> glTF -> decimated, TMEM-fitted, **light-baked** (shadows, AO, one GI bounce into vertex colours) Tiny3D assets -> standalone ROM that walks the level.

## Status

Host-side tooling is tested. **No ROM has been built or run yet** (no emulator or hardware run of any kind); see the plan.

## Try the converter (no N64 tools needed)

```
pip install -r requirements.txt
python3 -m unittest discover -s tests
python3 tests/scene_gen.py level.gltf      # synthetic test level; or use your own glTF/GLB with KHR_lights_punctual
cd tools && python3 -m n64fit ../level.gltf ../viewer --tris 1500
```
Output goes to `viewer/assets/` and `viewer/level_index.h`, which the viewer's Makefile consumes
(needs libdragon `preview`, Tiny3D, and the MIPS toolchain; none of which have been run in CI here).

## Gameplay (the converted project running)

`tools/ue64/bpc.py <Project>/Content out/` compiles the project's Blueprints to C and extracts effects, montages, collision
shapes, movement tuning and class defaults; `runtime/` is the N64-side Unreal runtime (GAS, movement, collision, notifies,
ports of the project's C++); `game/` is a playable ROM (see docs/PLAN.md and docs/img/ares_game*.png). Run the checks with
`tools/run_bp_tests.sh <Project>/Content`.

## Linux vs ares comparison

`tools/hostrender` renders the same deterministic autoplay on Linux and in ares (ROM frozen on a chosen frame). The
per-frame game state (HP/mana/stamina/position/object count, 549 frames) is identical on both; screenshots differ only by
the N64 VI resampling blur (MAE ≈ 3/255). See `docs/img/compare_ares_linux_1.png` / `_2.png` and `tools/hostrender/README.md`.

## GASShooter on the N64 (ares)

`shooterdemo/` builds a ROM that runs GASShooter's converted Blueprints, the C++ ports and the scenario suite (`tests/bp/run_shooter.c`)
on the console: `docs/img/ares_shooter_suite.png` shows it passing (same floating-point results as the host run). Rebuild with
`python3 tools/ue64/bpc.py <GASShooter>/Content shooterdemo/src/gen && shooterdemo/sync.sh && make -C shooterdemo` (N64_INST set).
