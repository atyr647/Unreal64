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

**Everything the project's Blueprints and C++ call is ported: 140/140 engine functions, 153/153 Blueprint functions decoded,
and the project's C++ gameplay classes.** Run live, the whole chain works on the emulated N64 (docs/img/ares_game*.png):
controller -> C++ `UGDGA_FireGun` -> montage notifies -> gameplay events -> projectile (swept, 7000 u/s) -> the Blueprint's
overlap event -> trace -> damage spec -> `GDDamageExecCalculation` -> attribute set -> health bar.

What is ported, and from what:
- **Blueprints:** all 153 functions decode with every jump and the in-memory size validated, across the UE 4.0-5.0 bytecode
  dialects (incl. the pre-4.1x `Let`/`Context` form). Parameters bind in declaration order (taken from the event stub's pin
  order, since export order differs), out-parameters write back, return values return.
- **C++ (hand-translated from Source/):** `UGDGameplayAbility`, `UGDGA_FireGun`, `UGDGA_CharacterJump`, `AGDHeroCharacter`
  (input axes, `FinishDying`), `AGDProjectile`, `AGASDocumentationGameMode` (HeroDied/RespawnHero at the PlayerStart /
  EnemyHeroSpawn), `GDAttributeSetBase`, `GDDamageExecCalculation`, `GDCharacterMovementComponent`, the async tasks.
  Blueprint classes inherit through the real parent-class table (constructor defaults, then BP defaults, root first).
- **Gameplay Ability System** (`u64_gas_core.c`): effects read from all 21 GameplayEffect assets (modifiers, ops, magnitudes,
  duration, period, stacking, tags, requirements, executions, **conditional effects**, attribute **snapshots**), aggregation,
  Pre/PostGameplayEffectExecute, abilities (cost, cooldown, blocked/required/cancel tags, input), tasks, tag events.
- **Character movement** (`u64_movement.c`): `CalcVelocity`, `ApplyVelocityBraking`, `PhysFalling` (air control, average-
  velocity integration), jump/landing, rotation, with each character's tuning values read from its CharacterMovementComponent.
- **Collision:** exact vertical-capsule vs yawed-box/sphere overlap, line traces with full hit results, swept spheres,
  `OnComponentBeginOverlap/EndOverlap` events dispatched to the handlers named by the graph's `K2Node_ComponentBoundEvent`s.
- **Animation notifies:** read from each montage with their real times; the `Die` notify (1.526 s) is what destroys a dead
  character; `GenericEventByTag` notifies send gameplay events that reach the montage task.
- **Game:** `runtime/u64_game_loop.c` + `game/` ROM: top-down view of the live world with real collision-shape sizes, health
  bars, HUD from the real attributes, joypad input (stick = move, C-left/right = turn, A jump, B sprint, Z dash, L aim,
  R fire, C-down meteor, C-up armor), and an autoplay script so it runs unattended.

Verification: 24 Python tests; 60+ exact-value scenario checks in `tests/bp/run_gas.c` (derived from the assets and the
C++, e.g. jump apex 420^2/(2*980), `100/(100+Armor)`, regen +1/s, stack limit 4, Dash cost -15, trace hit at x=166, rotated-box
overlap, the whole shooting chain, respawn after 5 s); they pass on the host (also under AddressSanitizer) **and on the
emulated N64** (docs/img/ares_suite.png). The runtime counts storage exhaustion and the tests fail if any pool overflows.

**Still not ported (reported, not guessed):**
- **Skeletal data:** `tools/ue64/skeleton.py` now reads reference skeletons (bone hierarchy + local transforms, validated
  scan; 6-bone gun and 68-bone mannequin both parse) and `SkeletalMeshSocket`s (e.g. gun `Muzzle` = Grip_Bone + (52.96, 0, 11.34)).
  Not wired into the fire-gun muzzle: the gun is attached to the mannequin's `GunSocket` on `hand_r`, and where `hand_r` is
  depends on the animation (the reference pose has the arm out sideways), so a correct muzzle needs animation evaluation —
  anim sequences (compressed tracks) and the AnimBP state machine — which is not parsed. Until then the muzzle is the gun
  component's position. Skinning is likewise not done.
- **World geometry:** the data has no level collision meshes, so the floor is the height an actor starts at and traces/
  overlaps only see characters, projectiles and trigger volumes. (The editor export on a Mac mini provides geometry.)
- `ScalableFloat` curve tables, custom magnitude calculation classes and effect `Components`: the extractor flags them as
  unsupported (this project uses none). 
- UE5 editor-saved Blueprints contain only graph nodes, not bytecode (they need a cook or a graph compiler); networking;
  UI drawing beyond the HUD bars; audio; hit-react/damage-number visuals; AI behaviour (minions stand still).
- Lumen, Nanite and the rest of the rendering pipeline (see the pipeline section; geometry/lighting conversion is
  separate from gameplay).

Bugs found by this work: argument expressions of a `Context` call were evaluated against the callee instead of the caller
(`Self` as an argument pointed at the wrong object) — fixed in `codegen.py`.

Known gaps: out-parameters are passed by value; struct members use a placeholder accessor; no typing (all values are
`u64_v`); latent actions/timers/delegates are stubs; C++ gameplay is not handled at all.

## Status — what is and isn't verified

| Item | State |
|---|---|
| `tools/n64fit` converter | 14 unit tests pass on a synthetic Unreal-style level (room, pillar, high-poly sphere, sun + point light). Baked output inspected visually: lamp falloff, pillar and sphere shadows, corner occlusion, bounce light all present |
| Material JSON vs Tiny3D importer | Checked by reading the importer source and a real Fast64 file, **not** by running the importer |
| `game/` ROM | The converted level running live on the N64 (ares): world, shooting, health bars, HUD, input, autoplay |
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


## Second project: GASShooter (tranek/GASShooter, UE 4.26)

Chosen because it has what the first project lacks: animations and AnimBPs (the AnimGraph nodes are readable data), skeletal meshes,
a real map, 91 Blueprints and ~8k lines of C++. Run its tests with `tools/run_shooter_tests.sh <GASShooter>/Content`.

**Verified end to end on the host (exact values from the assets and the C++, GC after every tick, ASan/UBSan):**

* 385/385 Blueprint functions decode; 242/248 engine calls implemented (22 of those are audio / UI / cue visuals, accepted and ignored).
* Hero spawn and attributes, default inventory, weapons (add / equip / next / previous / drop ring / duplicate pickup ammo),
  shield-then-health damage, knock down -> finish dying -> respawn (and picking its dropped weapons up again).
* Firing: rifle (aim from the camera, 1 deg spread, 10 damage x 100/120 armor), shotgun (8 pellets x 6), rocket launcher (projectile,
  sphere explosion radius 200, 60 damage), reload (clip + reserve conserved), aim down sights (FOV task 90 -> 60 -> 90, aiming tags),
  sprint / ADS movement speed (x1.4 / x0.8), ammo / health pickups with respawn.
* Interaction: the chest. `GA_InteractPassive` traces with the `InteractTrace` profile (overlaps everything, so the first component hit
  decides), finds the lock (the only component overlapping "Interactable"), Interact starts `GA_InteractActive` through its GameplayEvent
  trigger, the 3 s hold ends in `PostInteract` (GE_ChestOpen, 5 s, + loot). Collision comes from the assets: convex hulls in `BodySetup`
  (each element carries its own transform: the chest meshes are rotated 90 degrees), the component hierarchy of the Blueprint (SCS) and
  per-component responses.
* Collision as the engine does it: `LineTraceMultiByProfile` (`u64_trace_profile`): channel = the profile's object type, a component answers
  min(profile response to the component's object type, component response to the channel), hits sorted, ending at the first block. Static
  meshes (convex hulls), the character Mesh (the PhysicsAsset's box / sphere / sphyl bodies in the reference pose, with the bone name in the hit)
  and the capsule (which in GASShooter ignores Ability / Projectile, so bullets hit the mesh). Headshots (`b_head`, x1.5) and the revive
  interaction (the Mesh overlaps Interactable) follow from that; the door timeline moves the Door component.
* Engine layer written for this: target data handles, effect contexts with hit results, effect containers, per-frame ability tasks
  (FOV, move component, repeat, delay-one-frame, wait input / event / tag / target data), RegisterGameplayTagEvent listeners,
  timelines (curve keys parsed from the assets), parent-function calls, dynamic casts, 4.26 function parameters, a garbage collector.

**Not done (honest list):**

* Interaction: doors (the timeline moves a component; `u64_comp_world` honours moved components, not tested) and reviving are untested
  scenarios. Profile traces use the shape's response to the trace channel and the project's channel defaults; the *profile's* own
  response to the component's object type is not modelled (the data has the responses; only needed for profiles that ignore a type).
* World geometry: the map's four arena walls are `Cube` static mesh actors and the floor is `/Engine/Maps/Templates/SM_Template_Map_Floor`
  (engine content: not in the project, so it comes with the editor export); BSP brushes (`Model` / `Polys`) are not parsed. The floor is
  still the spawn height. Placed meshes with project data (the four wall cubes) take part in trace queries (`u64_world_meshes`); the floor is skipped until its mesh is available.
* Animation, done: AnimSequence raw tracks (`tools/ue64/animation.py`), the compiled first-person AnimBP (`animgraph.py` -> `runtime/u64_anim.c`: sequence players,
  BlendListByBool / ByEnum, the Locomotion state machine with its crossfades, rules from the compiled transition functions and property-access copies,
  layered blend without filters), the weapon muzzle through camera -> mesh -> animated `b_RightWeapon` -> equipped offset -> mesh socket. The idle pose and the muzzle
  position are checked against an independent Python evaluation.
  Montages play through the AnimBP slots (blend in / out, plain segments; the rifle fire recoil is checked against Python).
  Third person: `ABP_Hero3P` is evaluated too (BlendSpacePlayer 2D grid, cached poses, layered blends with branch filters, pass-through for Fabrik / aim offset /
  space conversions); the heroes' physics bodies follow the animated pose (headshot test aims at the posed head; idle bodies checked against Python).
  Notify states of montages run (`AnimNotifyState_*`: begin at the notify time, end after its Duration, through the Blueprint's `ReceivedNotifyBegin` / `End`): the equip montages add
  `Ability.Weapon.IsChanging` for 0.3539 s, which blocks the weapon abilities; equipping now plays the weapon's 1P and 3P equip montages (the hand port had skipped them). Checked with
  exact times. Not interrupted when a montage is stopped early (the end still fires at its nominal time). The sequence-level notifies in the project's assets are all `PlaySound` /
  `GSAnimNotify_PlaySoundForPerspective` (audio: not played); the reload / equip montages' `GenericEventByTag` notifies already worked.
  Animation, not done: aim offset (`RotationOffsetBlendSpace`: mesh-space rotation-additive samples against the Idle frame) and Fabrik IK are passed through unchanged. I held back on the aim
  offset because its exact composition order and the additive blend-space accumulation can't be checked against an engine build here, and for characters that never pitch (every placed hero) it
  is close to the identity; montages on the 3P skeleton beyond the equip one.
* HUD (the player's `UI_HUD`, a UMG widget blueprint): exported from the asset (`tools/ue64/widgets.py`: canvas panels, switcher, text blocks, progress bars, images; slots with anchors / offsets / alignment;
  the engine's default slot offsets 0, 0, 100, 30), laid out by `runtime/u64_widgets.c` with UMG's canvas rules (checked against an independent Python layout on the real asset at two viewports, and by hand-made
  cases), and driven by the project's own flow: `CreateHUD` when the player's hero is possessed, the compiled widget functions format and set the widgets (`100 / 100`, `+ 1.0` with the engine's number
  formatting options, named `Format` arguments), ammo / weapon updates, the attribute listener (health 75 -> text `75 / 100`, bar 435 of 580 wide), the interaction prompt (`Press 'E'` / `Hold 'E'` with its
  duration bar, gone after the interaction). Rectangles, texts and bar widths are asserted against numbers computed by hand from the asset's values (status panel 9.13, 599.55 in a 1280 x 720 viewport, ...).
  On the N64 (`shooterdemo/src/hud_render.c`, 640 x 480): the layout space scaled by 0.3, 8 x 8 built-in font, rectangles with alpha; the ares screenshot is `docs/img/ares_shooter_hud.png`.
  Layout space: the project has no UI-scale override, so the engine's default rule applies (shortest side, curve 480 -> 4/9, 720 -> 2/3, 1080 -> 1, 8640 -> 8: a 16:9 viewport of 720 or 1080 lines lays out as
  1920 x 1080, the space the HUD was designed in; `u64_ui_scale`, checked against Python; the curve's key values are from my memory of the engine's defaults, unverified). An earlier version of this step
  laid the HUD out in 1280 x 720 and so drew it too large; fixed.
  Floating status bars (`UI_FloatingStatusBar_Hero`, `AGSHeroCharacter::InitializeFloatingStatusBar`): one per hero except the locally controlled player; the widget component is 120 above the hero, screen space,
  draw size 500 x 500, centred on the point's projection through the player's camera (UE axes, horizontal FOV, 16:9; far ones first). The projection is checked against the formula written out in the test (four bars,
  exact rectangles of the background and the health bar), and the enemy's shield bar follows damage through the widget's own attribute listener (91.67 / 100 -> 149.84 of 163.46 wide). Every hero now has a
  player-state stand-in, since the widget reads its ability system through it.
  Damage numbers (`AGSCharacterBase::AddDamageNumber` / `ShowDamageNumber`, `UI_DamageNumber` + `WC_DamageText`): the project's own `PostGameplayEffectExecute` calls `PC->ShowDamageNumber` for the local
  player's hits (headshot tag from the effect spec); the runtime queues them (one per 0.1 s), spawns the component (screen space, 200 x 100, pivot 0.5 / 2.0 at the target's origin) and plays the widget's
  `FadeAnimation` (a MovieScene: opacity and Y translation channels, Hermite keys with per-second tangents, speed 2, assuming 24000 ticks / s), destroyed after 1 s. Tests: text `8` (8.333, half to even)
  and `12` (12.5 headshot), colours (1, 1, 0.498) / (1, 0, 0), font 42 / 56, position from an independent projection with the camera's actual rotation, animated offset and opacity from the keys.
  On the N64 the number is drawn in the enemy view (`docs/img/ares_shooter_damage_number.png`: yellow `8` above the floating bars; the full suite passes there). This work also fixed a real bug: an ability's `MakeOutgoingGameplayEffectSpec` used a phantom ASC created for the ability object as the spec's source instead of the hero's ASC.
  HUD, not done: textures (weapon icon / background drawn as frames), the asset's fonts and text measuring (auto-sized slots use a fixed-pitch estimate; the 8 x 8 font overlaps small labels at 576 pixels wide),
  the character name text (the hero's name is empty), occlusion of the floating bars by geometry, the material reticles,
  the respawn countdown wiring. The player state is a stand-in (the ability Blueprints cast their owning actor to it; the cast is answered with the hero's).
* Audio, networking, AI behaviour of the placed heroes.
* Unverified engine assumption: the first execution of a periodic effect happens on the next tick (see below).

## Toward an Unreal Engine export target

What the two projects taught about the shape of the thing:

1. **Three layers.** (a) *Engine layer*, project independent: `u64_rt` (objects, GC), `u64_natives`, `u64_math`, `u64_gas*`
   (ability system, target data, tasks, timelines), movement and collision. (b) *Generated data and code* from the project's assets
   (`bpc.py`): Blueprint functions as C, class defaults, level actors, effect / montage / timeline / shape tables. (c) *Project
   ports* of the project's own C++ (`u64_cpp_*.c`), registered through `u64_cpp_register_class / method` and the `u64_gas_hooks`.
2. **The C++ is the unscalable part.** Both ports were translated by hand (about 2k lines for 1.2k lines of GASDocumentation C++, and a
   comparable effort per GASShooter subsystem). An editor export target has the project's C++ and its reflection data available, so
   the realistic route is a C++ -> C translation step for gameplay classes (UCLASS / UFUNCTION surface, TArray / FString / delegates /
   GAS calls mapped onto the engine layer) with the hand ports as the reference implementation and the scenario tests as the oracle.
3. **Assets come from the editor, not from parsing.** The package parser here exists because no editor is available; an exporter
   plugin would write the same tables (components with collision responses, static-mesh collision, skeleton poses, animation tracks,
   level placement, curves, tags) directly from live objects and avoid version-specific serialisation (4.24 vs 4.26 vs 5.x dialects,
   field paths, tagged-property layouts all cost real time here).
4. **Runtime limits that bit:** pools sized for one demo (strings, attributes, tags, abilities, timers, effects) silently corrupted
   state; they are now larger and loud, objects / arrays / variable slots are collected between ticks. A target needs per-project
   pool sizing in the exported data and the overflow counter as a build-time and test-time gate.
5. **Testing discipline that paid off:** exact-value scenarios derived from assets and source, host vs ares state traces (found a
   fast-math divergence), GC after every tick, ASan/UBSan. The export target should generate scenario skeletons from the project's
   own gameplay assets (abilities, effects, pickups) the same way.
