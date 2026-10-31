# C++ to MIPS: compiling a project's own gameplay C++ for the N64

Until now the project C++ (GASShooter's `Source/`) was ported to C by hand (`runtime/u64_cpp_gasshooter.c`). This directory tests the
alternative: compile the **unmodified** `.cpp` files with `mips64-elf-g++` against a small implementation of the Unreal API (the shim) and
let the Blueprint runtime call into them.

## What works (verified)

`GSAmmoAttributeSet.cpp` (GASShooter, unmodified) builds with the libdragon toolchain's `mips64-elf-g++` (GCC 16.2, `-fno-exceptions
-fno-rtti -fno-threadsafe-statics`) and passes its checks on the host *and* in ares on the N64 (ROM 230 KB):

* `shim/` implements the types the file touches (`FName`, `FGameplayTag`, `FGameplayAttribute(Data)`, `UAttributeSet`, `FMath`, `TArray`,
  the `GAMEPLAYATTRIBUTE_*` macros, the reflection macros as no-ops).
* `uht_lite.py` plays the role of Unreal Header Tool: it writes `<Header>.generated.h` with the `GENERATED_BODY()` expansion
  (`Super`, `ThisClass`, `StaticClass()`) and a name -> member lookup for every `FGameplayAttributeData` property. Property lookup by
  name is what `FindFieldChecked` and attribute replication/serialization need.
* `test_ammo.cpp` checks exact values from the C++ (clamp of the changed reserve attribute to `[0, Max]`, `Weapon.Ammo.*` tag -> attribute).

Run: `cpp2mips/run_tests.sh <GASShooter>/Source/GASShooter [--n64]` (the N64 build needs libdragon; the ROM is `n64/cppammo.z64`).

## How big is the rest? (measured on GASShooter)

* 45 `.cpp` files, 7.9k lines. They reference **126 distinct engine types**; 48 of them are used by three or more files
  (`FGameplayTag(Container)`, `FName`, `TArray`, `FString`, `UGameplayAbility`, `FVector`, `FHitResult`, `TSubclassOf`, `FMath`,
  `FGameplayAbilityTargetDataHandle`, `UWorld`, ...). About 60 engine headers are included.
* 11 files need next to nothing (empty widgets / controllers, 5-30 line classes). The heavy ones are `GSHeroCharacter` (43 types, 1180
  lines), `GSAbilitySystemComponent` (30), `GSGATA_Trace` (24), `GSGATA_SphereTrace` (20).
* The engine side that has to exist is mostly **GAS** (ability system component, abilities, tasks, target data, effect specs/contexts) and
  `AActor`/`ACharacter`/`APlayerController`. The Unreal64 runtime already implements GAS semantics for the Blueprint side, so the shim
  is a thin C++ face over `runtime/u64_gas_core.c` rather than a second implementation.

## What the shim still needs before whole classes (not single attribute sets) can run

1. **Object model bridge.** C++ objects need to appear to Blueprints as `u64_obj`s: `uht_lite` must also emit, per class, the property
   table (name, type, offset) and a trampoline per `UFUNCTION` for `u64_cpp_register_method`. `Cast<T>` / `IsA` without RTTI come from the
   generated class ids (`StaticClass()` chain), which `uht_lite` already prepares.
2. **Lifetime.** `NewObject` / `SpawnActor` memory has to be owned by the u64 GC (or the C++ objects pinned by it); today objects live in the
   runtime's pools, C++ instances would be placement-new'ed into a pool.
3. **Delegates and timers** (`DECLARE_DYNAMIC_MULTICAST_DELEGATE`, `FTimerManager`): map to `u64_mc_*` and the existing timer list.
4. **Dynamic containers** (`TArray`, `TMap`, `FString`): the shim `TArray` is fixed-capacity; real ones need an allocator on the 8 MB heap.
5. **Virtual overrides across the language line** (a Blueprint subclass overriding a C++ virtual): the runtime already resolves the
   class-parent chain BP -> C++ -> native; the C++ side needs a vtable thunk that calls `u64_call` for overridden `BlueprintNativeEvent`s.

## Assessment

It is feasible, and it scales better than hand porting: the surface is bounded (~130 types for this project, shared across projects
because most of it is the GAS plugin and `AActor`), the compiler is the same GCC, code size is small (the ammo set is ~190 KB including
the C++ runtime and libdragon). The risk is in the reflection bridge (item 1) and lifetime (item 2), not in the code generation.
Recommended order: (a) shim `UAbilitySystemComponent` over the runtime ASC and run `GSAttributeSetBase` (damage/shield/health rules, the
most exact-value-checkable class) through `PostGameplayEffectExecute`, replacing `rules_*` in `u64_cpp_gasshooter.c`, keeping the shooter
scenarios (`tools/run_shooter_tests.sh`) as the regression; (b) property/function tables from `uht_lite` so Blueprints call C++ methods
without registry glue; (c) the heavy classes, one at a time, each swapped in only when the scenarios still pass.

## Update: the damage rules run in the project's C++ too

`GSAttributeSetBase.cpp` (damage -> shield -> health, clamps, bounty effect, max-change scaling, MoveSpeed clamp) also compiles unmodified against the
shim and runs inside the scenario suite (`cpp2mips/bridge_base.cpp`; 1270 calls, all scenarios pass on the host, `run_shooter_cxx.sh`). What it took:
the character / controller types as runtime-backed proxies (`IsAlive` reads the runtime's Health; one "controller" per actor), `Cast<>` without RTTI
(uht_lite emits `InstanceOf`), `FGameplayEffectSpec` / context / `NewObject<UGameplayEffect>` stand-ins, and the project's `GSCharacterBase` /
`GSPlayerController` members that this build does not compile defined as runtime glue (`bridge_base.cpp`). Two bugs the suite caught in my own shim:
a property table that recycled entries (attribute identity broke after 256 lookups) and proxies cached by runtime object slot (slots are reused).
Not mirrored: the spec's dynamic asset tags (damage-number UI only).

## Compile coverage (measured: `cpp2mips/coverage.sh <Source/Module>`, `g++ -fsyntax-only` against the shim)

13 of the 45 `.cpp` files compile unmodified (the two attribute sets, effect types, target types, UI / cue-manager classes and more; `coverage.sh` lists them). Started
the GAS core in the shim (`shim/ue_gas.h`: ability specs / handles / target data / events / effect contexts, `UGameplayAbility`, `UAbilitySystemGlobals`,
the ASC surface in `AttributeSet.h`; `shim/ue_engine.h`: collision query params, world, components, tasks, widgets, movement stubs). The remaining 35 each stop at
a handful of missing members or types; the loop that was used (compile, read the most common first error, add the type, repeat) took the count from 2 to 10
in about 40 rounds of small additions. What is left is dominated by: the ability task base and `AbilityTask_WaitTargetData` / `GameplayAbilityTargetActor`
members (GATA traces, `GSAT_*`), `GSAbilitySystemComponent` (animation montage replication, which is animation work), movement saved-move classes,
`GSInteractable` (UInterface macros), widget / HUD types. These are declarations, not behaviour: none of them is run yet. The two compiled classes that ARE run
(ammo, damage rules) pass the full suite on the host and on the N64.
