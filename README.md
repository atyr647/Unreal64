# Unreal64

Put an **unmodified Unreal Engine project** on a real N64 — as much of it as physically survives.

The engine can't execute on an N64 (4–8 MB RAM, 4 KB texture memory, no shaders), so this repo does the expensive
work on a PC and gives the N64 the result. Read [docs/PLAN.md](docs/PLAN.md): it states what each Unreal feature
becomes and, importantly, what is verified versus only written.

- **Mode A, convert** (`tools/n64fit`, `viewer/`): Unreal level -> glTF -> decimated, TMEM-fitted, **light-baked** (shadows, AO, one GI bounce into vertex colours) Tiny3D assets -> standalone ROM that walks the level.
- **Mode B, stream** (`host/`, `n64/`): the real game runs on the PC at full quality; the N64 shows it and sends controller input.

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
