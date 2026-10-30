# Unreal64 — plan

Goal: take an **unmodified Unreal Engine project** and get it onto a real N64 with as much preserved as possible.

## The hard limit, stated once

N64: 93.75 MHz CPU, 4 MB RAM (8 MB with Expansion Pak), a fixed-function RDP with 4 KB of texture memory (TMEM).
Unreal 5 needs gigabytes of RAM and a programmable GPU. The engine itself cannot execute on an N64, and nothing in
Lumen, Nanite, hardware RT or path tracing has an N64 equivalent that *runs*. What can be done is to **run those
features offline, on a PC, and bake their results into something the N64 can draw.** That is how this plan treats
"preserve as much as possible": the project is never modified, the PC does the expensive work once, the N64 plays the result.

## Pipeline: unmodified project -> N64 ROM

```
Unreal project --(UnrealEditor-Cmd + tools/ue/export_level.py, project untouched)--> level.glb  (+KHR lights)
level.glb --(tools/n64fit)--> assets/*.glb + *.png + formats.mk + level_index.h
assets --(Tiny3D gltf_to_t3d + libdragon mksprite)--> .t3dm models + .sprite textures --> viewer/ ROM
```

What each Unreal feature becomes:

| Unreal feature | N64 treatment | Implemented |
|---|---|---|
| Static meshes / Nanite | Triangle-budget decimation (meshoptimizer quadric), then re-tessellation of flat areas so baked light has vertices to live on. Hard cap on total triangles. | Yes (tested) |
| Lumen GI | CPU bake into vertex colours: direct light, ambient occlusion, one diffuse bounce | Yes (tested, brute-force rays) |
| Hardware RT shadows | Ray-traced shadows in the same bake | Yes (tested) |
| Path tracing | Not yet. The bake is the hook: swap in Embree/Cycles for production-quality converged results | No |
| Materials | Base colour only: textured -> `TEX0 × SHADE` combiner, untextured -> `SHADE`; cutout and translucent modes map to RDP render modes. Normal/roughness/metallic are dropped | Yes (schema checked against a real Fast64 export) |
| Textures | Fitted to TMEM: best of RGBA16 / CI8 / CI4 at power-of-two sizes by error vs a reference | Yes (tested) |
| Reflections | Planned: sphere-map env texture on glossy materials (Tiny3D supports UV generation) | No |
| Streaming/World Partition | Grid cells, culled by distance at runtime | Yes (cells; runtime culling in source) |
| LODs | Planned: second coarser set per cell | No |
| Lights/Blueprints/animation/audio/UI | Not converted. Needs a Blueprint bytecode interpreter and asset converters | No |

Honest quality expectation: you get a *recognisable, pre-lit low-poly rendition of the level* you can walk around,
not the game. 5-bit colour, 4 KB textures, a few thousand triangles on screen.

## Status — what is and isn't verified

| Item | State |
|---|---|
| `tools/n64fit` converter | 14 unit tests pass on a synthetic Unreal-style level (room, pillar, high-poly sphere, sun + point light). Baked output inspected visually: lamp falloff, pillar and sphere shadows, corner occlusion, bounce light all present |
| Material JSON vs Tiny3D importer | Checked by reading the importer source and a real Fast64 file, **not** by running the importer |
| `viewer/src/main.c` | Pass a syntax/type check (`gcc -fsyntax-only -DN64`) against libdragon `preview` + Tiny3D headers. The check was confirmed able to fail. **Never compiled for MIPS, never run** |
| MIPS toolchain | Downloaded libdragon's prebuilt toolchain and unpacked it; **running it was blocked by the session's permission policy**, so nothing has been built |
| ares / any emulator run | **Not done** (blocked for the same reason; ares has no prebuilt Linux release asset at the URLs I tried, so it would also need a source build) |
| `tools/ue/export_level.py` | **Untested.** No Unreal install here; API names from memory |
| `tools/run_ares.sh` | **Untested** |
| Anything on real hardware | **Not done** |

## Next steps, in order

1. Allow building/running the downloaded toolchain (see below), build libdragon `preview` + Tiny3D, build `viewer`, boot in ares, screenshot. This is the first real proof and will probably expose bugs.
2. Run the importer on `n64fit` output; fix whatever it rejects.
3. Export a real level from a real Unreal project with `export_level.py`; fix the script and the loader against real exporter output.
4. Reflections (sphere map), LODs, collision + gravity so you can walk a real level.
5. Production bake backend (Embree or Cycles) for path-traced quality; the interface is `bake.bake()`.
6. Gameplay: a Blueprint bytecode interpreter is the only route to preserving game logic natively. Large.
