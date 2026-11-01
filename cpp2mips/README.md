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

## Status of the remaining files (measured)

15 of 45 compile. The rest total about 700 first-pass errors, 217 of them in `GSAbilitySystemComponent.cpp` (animation montage replication: that file belongs to
the animation work, not to the GAS core) and ~120 in `GSHeroCharacter.cpp`. The shim now has `ShimAny` / `SHIM_FN` stubs (`shim/ue_stubs.h`) for engine members that
have no runtime backing; they answer a default and are listed once on stderr when `UE_SHIM_REPORT=1`, so anything that compiles but is not backed is visible, not silent.
Compiling is not running: a class that runs needs each stub it reaches replaced by a runtime-backed implementation (as done for the attribute sets and the jump ability).
`cpp2mips/errsum.sh <Source/Module>` prints the first errors of every file.

## Update: the ability base class runs in the project's C++ (GSGameplayAbility.cpp)

`GSGameplayAbility.cpp` (the base of the project's Blueprint abilities), `GSTargetType.cpp`, `GSAbilityTypes.cpp` and `GSInteractable.cpp` are compiled unmodified and run inside
the scenario suite (`bridge_base.cpp`, section "UGSGameplayAbility"; 170 calls in the suite). What runs as the project wrote it: the constructor (default flags, blocked / owned
tags, interacting tags), `OnAvatarSet` (passive abilities activate when granted, through the ASC's `TryActivateAbility`), `CanActivateAbility` (the interacting-tag rule and
the "source object must be the current weapon" rule, which casts the avatar to `AGSHeroCharacter` and compares `GetCurrentWeapon()`), `CheckCost` / `ApplyCost` (they dispatch to
the `GSCheckCost` / `GSApplyCost` Blueprint events, as UHT's generated thunks do), and the effect containers: `MakeEffectContainerSpec` walks the container map, builds the target
type's class default object (`TSubclassOf::GetDefaultObject`, `UGSTargetType_UseOwner::GetTargets_Implementation`, ...), collects `FGameplayAbilityTargetData` handles and makes
the outgoing effect specs; the bridge converts the resulting `FGSGameplayEffectContainerSpec` back into the runtime's spec object. The scenario suite found one bridge bug
(class values from the runtime are `U64_CLASS`, not names: the target type came back empty and the reload effect was never applied).

What the bridge needed: `UClass` now carries a factory (`ShimMake`, emitted by `uht_lite`) so class default objects exist; `FGameplayAbilityActorInfo::AbilitySystemComponent` is a weak
pointer as in the engine; the ASC proxy answers `GetTagCount` / `TryActivateAbility` from the runtime; `AGSHeroCharacter`, `AGSWeapon` and `UGSAbilitySystemComponent` exist as
runtime-backed proxies, with their virtual members generated empty by `stubgen.py` (their vtables need definitions; the behaviour is the runtime's). RTTI is now on:
`GSAbilitySystemGlobals.h` uses `dynamic_cast` (ROM +33 KB with the new classes). Run with `SAN=1 cpp2mips/run_shooter_cxx.sh ...` for ASan + UBSan (clean).
The engine's own base-class checks (`Super::CanActivateAbility`: required / blocked tags, cooldown; `Super::ApplyCost`: the cost effect) stay in the runtime, which already implements them.
Not backed: replicated target data, prediction keys (`FPredictionKey` is always invalid), `UGSAbilitySystemComponent`'s montage functions (reported by `UE_SHIM_REPORT=1`).

### The damage formula too (GSDamageExecutionCalc.cpp)

`UGSDamageExecutionCalc` (set-by-caller + captured Damage, the x1.5 headshot multiplier when the effect has `Effect.Damage.CanHeadShot` and the context's hit bone is `b_head`,
mitigation `100 / (100 + Armor)`, the `Effect.Damage.HeadShot` dynamic tag) runs as written (`bridge_exec.cpp`, `-DU64_CXX_EXEC`, 11 executions in the suite, the headshot value
12.5 included). The shim has the engine types it uses (`FGameplayEffectAttributeCaptureDefinition`, `DECLARE/DEFINE_ATTRIBUTE_CAPTUREDEF`, `FGameplayEffectCustomExecutionParameters`
and output, `FAggregatorEvaluateParameters`); the runtime resolves the captures (`u64_exec_in`) and applies the output modifier. One behaviour change: the runtime used to notify
`WaitReceiveDamage` tasks from this calculation; the project's code does not, so on this path it no longer does (GASDocumentation's calculation, a different class, still does).

### Weapon targeting (GSGATA_Trace / LineTrace / SphereTrace)

The reusable trace target actor the weapons use runs as written (`bridge_base.cpp`, `-DU64_CXX_TRACE`): `PerformTrace` (trace start from the player view point, `AimWithPlayerController`,
`ClipCameraRayToAbilityRange`, the spread cone through `FRandomStream::VRandCone`, `LineTraceWithFilter` / `SphereTraceWithFilter`, trimming to `MaxHitResultsPerTrace`, the default hit at the end of a
trace that hit nothing, persistent hit results), `GetCurrentSpread`, `ResetSpread`, `StartTargeting`, `ConfirmTargetingAndContinue` (-> `MakeTargetData` -> `TargetDataReadyDelegate`),
`StopTargeting`. The runtime object keeps the Blueprint-visible properties; they are synced into one C++ actor per runtime actor on each call, and `CurrentTargetingSpread` / `CurrentTraceEnd` are
synced back. World queries are hooks into `u64_collision.c` (`LineTraceMultiByProfile` = `u64_trace_profile`; `SweepMultiByProfile` = the characters' capsules, **not** profile-filtered, as before).
New shim math (`ue_math.h`): `FRotator`, `FVector::Rotation / RotateAngleAxis`, `FRandomStream` (seed mutation `Seed*196314165+907633515`, `GetFraction` bit trick, the elliptical `VRandCone`);
`FMath::Rand` is a fixed LCG (the same sequence on host and N64). `tests/test_random_stream.py` checks these against an independent Python implementation of the same definitions (it checks the port, not
the engine: the engine's source is not available here, so the formulas are from my knowledge of it and unverified against a build).
Behaviour changes found by running the project's code: a trace that hits nothing now returns the default end-of-trace hit (as the project does; the hand port returned an empty array), and spread cones
use the engine's sampling (not uniform in solid angle) instead of the hand-written one. Approximations that remain: the start location's rotation is the hero's control rotation (the real one is
the weapon socket's rotation), reticle actors are not spawned (HUD), debug drawing is off.

### The interaction trace task (GSAT_WaitInteractableTarget.cpp)

`UGSAT_WaitInteractableTarget` (the ability task that looks for something to interact with) runs as written (`-DU64_CXX_TASKS`): the project's own factory `WaitForInteractableTarget` builds the task,
`Activate` registers its `SetTimer` (the shim's timer list, fired from the runtime's frame with the runtime's world time), and every period `PerformTrace` aims from the view point, clips the ray to
range, traces by the `InteractTrace` profile, applies the interactable-component rule (the hit component must *overlap* the Interactable channel and the actor must `Implements<UGSInteractable>()`
and be available through `IGSInteractable::Execute_IsAvailableForInteraction`), and broadcasts `FoundNewInteractableTarget` / `LostInteractableTarget`, which reach the Blueprint's handlers.
The chest, door and revive scenarios now go through it. New shim pieces: `TimerManager::SetTimer` over member-function pointers, socket-type targeting location info, `FHitResult::Component`,
`UPrimitiveComponent::GetCollisionResponseToChannel` (runtime hook), `AActor::Implements<>` (runtime hook), one-receiver delegate hooks for `DECLARE_..._OneParam`, and `Execute_<Event>` declarations
emitted by `uht_lite.py` for interface events. Bug found on the way: target data kept between ticks must not hold pointers to runtime objects (they are collected); a rebuilt hit lost its component until the
C++ hit carried it. Approximation kept from the hand port: the start location (weapon point / head socket) is the camera's eye position, since those sockets need the animated meshes.

15 -> 23 of 45 files compile.
