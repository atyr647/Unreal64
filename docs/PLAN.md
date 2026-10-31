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
| Blueprint gameplay | Kismet bytecode decoded and lowered to C (`tools/ue64/kismet.py`, `codegen.py`), linked against `runtime/u64_rt.h`. Dynamic-typed values; natives are by-name calls the N64 runtime must implement (`compat_report.txt` lists them) | **Yes for the saved-class path** (see below) |
| Animation/audio/UI/physics/natives | Not converted | No |

Honest quality expectation: you get a *recognisable, pre-lit low-poly rendition of the level* you can walk around,
not the game. 5-bit colour, 4 KB textures, a few thousand triangles on screen.

## Blueprints

Blueprint logic exists on disk as compiled Kismet bytecode only in packages that saved their generated class
(UE4-era assets, and any **cooked** build). Editor-saved UE5 Blueprints store just the graph nodes
(`K2Node_*`), so for those the route is the editor cook on the Mac mini, or compiling the graph directly (not done).

On `tranek/GASDocumentation` (UE 5.3 project, 259 packages incl. UE4.0-era assets):
- 153 `Function` exports found; **150 decode and validate** (every jump lands on a token boundary and the decoded
  in-memory size equals the header's size field). The 3 failures are a UE 4.8-era bytecode dialect.
- All 150 lower to C that compiles with gcc, and **link into one N64 ROM that runs in ares** (docs/img/ares_blueprint.png:
  `BP_MinionSpawner.BeginPlay` -> ubergraph -> `HasAuthority` -> `K2_SetTimerDelegate`, identical to the host run).
- 159 distinct native functions are called by name; none are implemented yet beyond a logging stub. That list
  (`bpc.py` writes `compat_report.txt`) is the real work queue: it is the engine API surface the runtime must provide.

**World runtime** (`runtime/u64_natives.c`): timers, actor spawning, multicast delegates, kill/respawn, ~50 engine natives
(math, vectors, comparisons, `IsValid`, `HasAuthority`, `SetTimerDelegate`, `BeginDeferredActorSpawnFromClass`, ...).
Class default values come from each Blueprint's default object (tagged properties) and placed actors from the map
(`leveldata.py`). On the real `Map_Startup` the converted Blueprints reproduce the minion-spawner behaviour:
both spawners spawn at t=2.0 (the Blue one with its per-instance `MinionClass` override), a killed minion triggers
`MinionDied`, which re-arms the timer and respawns at t+2.0 — identical on host and in ares (docs/img/ares_world.png).
The compiler reports exactly which engine functions remain unimplemented (`compat_report.txt`).

**Engine function coverage: 127/129** of the functions this project's Blueprints call (the other two are Blueprint
ubergraphs in old-dialect assets that cannot be decoded).

**The Gameplay Ability System is ported from real data and the project's C++, not approximated**
(`runtime/u64_gas_core.c`, `tools/ue64/gasdata.py`, `props.py`):
- All 21 GameplayEffect assets are read (modifiers, op, magnitude type, duration, period, stacking, granted/asset/remove
  tags, application and ongoing tag requirements, executions) and compiled to tables.
- Attribute values are `aggregate(base, active modifiers)` (Additive/Multiplicative/Division/Override, stack-scaled);
  instant and periodic effects change the base. Periodic effects execute on application and every period; Duration effects
  expire; ongoing tag requirements inhibit modifiers *and* granted tags (so `GE_HealthRegen` stops while `State.Dead`).
- `GDAttributeSetBase` is ported: `PreAttributeChange` (MoveSpeed clamp 150-1000; `AdjustAttributeForMaxChange`, which is
  what takes Health 0 -> 100 when `GE_HeroAttributes` overrides MaxHealth), `PostGameplayEffectExecute` (Damage meta
  attribute, Health/Mana/Stamina clamps, bounty to the source).
- `GDDamageExecCalculation` is ported exactly (target Armor, source Damage, SetByCaller `Data.Damage`, `100/(100+Armor)`).
- Startup follows `GDHeroCharacter::PossessedBy` / `GDMinionCharacter::BeginPlay`: DefaultAttributes -> max-out ->
  StartupEffects -> CharacterAbilities. Death follows `Die`/`FinishDying` (OnCharacterDied, cancel abilities, remove
  `Effect.RemoveOnDeath` effects, add `State.Dead`, destroy after the death montage's real length).
- Abilities: cost check (`CanApplyAttributeModifiers`), cooldown (the cooldown effect's granted tags), activation
  blocked/required tags, `CancelAbilitiesWithTag`, owned tags, `BlockAbilitiesWithTag`, Commit applies cost + cooldown,
  stun cancels abilities (`StunTagChanged`), input press/release by `AbilityInputID`.
- Tasks mirror the C++ async tasks (attribute changed, cooldown begin/end, effect stack change) and the montage task
  fires BlendOut then Completed at the montage's **real** length and blend-out time read from the AnimMontage asset.
- Movement speed follows `GetMaxSpeed` (0 when dead/stunned, x1.4 sprinting, x0.5 aiming). Overlaps use the real
  collision shapes (BoxExtent x RelativeScale3D; capsule radii/half heights, engine defaults where not overridden).
- Components are real objects owned by their actor; arrays are real; out-parameters write through references.

Verified by scenarios that check exact values derived from the assets (e.g. 25 dmg vs Armor 15 = 21.739; regen +1 per
second; stack limit 4 -> Armor 55; Dash cost -15 applied by the Blueprint's own CommitAbility; Dash montage cancelled on
end; sprint speed 840) plus the project's own compiled Sprint/Dash abilities. Same checks pass on host (also under
AddressSanitizer) and in ares (docs/img/ares_gas.png).

**Not ported (reported, not guessed):**
- Collision queries that need geometry: `LineTraceMultiForObjects` and hit results (need collision meshes from the editor
  export). Overlaps use actor-level shapes: component relative transforms and rotation are ignored.
- Animation: montage notifies (`EventReceived` from AnimNotifies) are not read; no skeletal pose / anim graph evaluation.
- `ScalableFloat` curve tables, custom magnitude calculation classes, conditional/overflow effects: the data extractor
  flags them as unsupported (none are used by this project). Attribute capture snapshotting is not modelled.
- No replication/networking; UI widgets only keep state (Text/Percent/Visibility), nothing is drawn.
- Hit-react and damage-number effects are Blueprint/C++ events that are called when they exist but have no visuals.

Bugs found by this work: argument expressions of a `Context` call were evaluated against the callee instead of the caller
(`Self` as an argument pointed at the wrong object) — fixed in `codegen.py`.

Known gaps: out-parameters are passed by value; struct members use a placeholder accessor; no typing (all values are
`u64_v`); latent actions/timers/delegates are stubs; C++ gameplay is not handled at all.

## Status — what is and isn't verified

| Item | State |
|---|---|
| `tools/n64fit` converter | 14 unit tests pass on a synthetic Unreal-style level (room, pillar, high-poly sphere, sun + point light). Baked output inspected visually: lamp falloff, pillar and sphere shadows, corner occlusion, bounce light all present |
| Material JSON vs Tiny3D importer | Checked by reading the importer source and a real Fast64 file, **not** by running the importer |
| `viewer/` ROM | **Builds** (real MIPS build against Pak-pinned libdragon + Tiny3D) and **runs in ares**: textured floor, baked-lit walls (docs/img/ares_viewer.png). Camera/input/culling untested; test-scene walls show a winding issue |
| `tools/ue64/uasset.py`, `kismet.py`, `codegen.py` | All 259 packages' import/export tables parse (gated per package version, with a bounds oracle that rejects misparses); 150/153 functions decode and validate; generated C compiles on host and MIPS; runs in ares |
| `viewer/src/main.c` (older note) | Pass a syntax/type check (`gcc -fsyntax-only -DN64`) against libdragon `preview` + Tiny3D headers. The check was confirmed able to fail. **Never compiled for MIPS, never run** |
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

Additional bug found while porting: in UE 4.25-style packages a field path is stored innermost-first, so taking the last
name collapsed every variable of `UI_HUD` into one (fixed; regression test in tests/test_props.py). Another: argument
expressions of a `Context` call must run in the caller's frame (fixed earlier).
