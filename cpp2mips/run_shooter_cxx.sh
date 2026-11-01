#!/usr/bin/env bash
# The GASShooter scenario suite with the ammo attribute rules executed by the project's own C++ (GSAmmoAttributeSet.cpp, unmodified, built against the UE shim).
#   cpp2mips/run_shooter_cxx.sh <GASShooter>/Content <GASShooter>/Source/GASShooter
set -euo pipefail
content="${1:?path to the project Content directory}"; gs="${2:?path to the project Source/<Module> directory}"
here="$(cd "$(dirname "$0")/.." && pwd)"; out="$(mktemp -d)"; [ -n "${KEEP:-}" ] || trap 'rm -rf "$out"' EXIT; echo "build dir: $out" >&2
python3 "$here/tools/ue64/bpc.py" "$content" "$out/gen" | tail -1
python3 "$here/cpp2mips/uht_lite.py" "$out/ugen" $(find "$gs/Public" -name "*.h") >/dev/null
mkdir -p "$out/stubs"
for c in "AGSHeroCharacter Characters/Heroes/GSHeroCharacter.h hero" "AGSWeapon Weapons/GSWeapon.h weapon" "UGSAbilitySystemComponent Characters/Abilities/GSAbilitySystemComponent.h asc"; do set -- $c; python3 "$here/cpp2mips/stubgen.py" --skip IsInFirstPersonPerspective $1 "$gs/Public/$2" > "$out/stubs/stubs_$3.inc"; done
objs=()
for f in u64_rt u64_natives u64_gas u64_gas_core u64_gas_extra u64_collision u64_anim u64_math u64_movement u64_cpp_game u64_cpp_gasshooter; do
  gcc -g ${SAN:+-fsanitize=address,undefined} -w -DU64_CXX_AMMO -DU64_CXX_BASE -DU64_CXX_JUMP -DU64_CXX_ABILITY -DU64_CXX_EXEC -DU64_CXX_TRACE -DU64_CXX_TASKS -I"$here/runtime" -c -o "$out/$f.o" "$here/runtime/$f.c"; objs+=("$out/$f.o"); done
gcc -g ${SAN:+-fsanitize=address,undefined} -w -DU64_CXX_AMMO -DU64_CXX_BASE -DU64_CXX_JUMP -DU64_CXX_ABILITY -DU64_CXX_EXEC -DU64_CXX_TRACE -DU64_CXX_TASKS -I"$here/runtime" -c -o "$out/run_shooter.o" "$here/tests/bp/run_shooter.c"; objs+=("$out/run_shooter.o")
for f in "$out"/gen/*.c; do gcc -g ${SAN:+-fsanitize=address,undefined} -w -I"$here/runtime" -c -o "$f.o" "$f"; objs+=("$f.o"); done
cxx=(-g ${SAN:+-fsanitize=address,undefined} -std=c++17 -fno-exceptions -fno-threadsafe-statics -w -I"$here/cpp2mips/shim" -I"$out/ugen" -I"$out/stubs" -I"$gs/Public" -I"$gs/.." -I"$here/runtime")
g++ "${cxx[@]}" -c -o "$out/bridge.o" "$here/cpp2mips/bridge_ammo.cpp"
g++ "${cxx[@]}" -std=c++20 -c -o "$out/bridge_base.o" "$here/cpp2mips/bridge_base.cpp"
g++ "${cxx[@]}" -c -o "$out/baseset.o" "$gs/Private/Characters/Abilities/AttributeSets/GSAttributeSetBase.cpp"
g++ "${cxx[@]}" -c -o "$out/jumpcpp.o" "$gs/Private/Characters/Abilities/GSGA_CharacterJump.cpp"
for f in GSGameplayAbility GSTargetType GSAbilityTypes GSInteractable GSGATA_Trace GSGATA_LineTrace GSGATA_SphereTrace; do g++ "${cxx[@]}" -std=c++20 -c -o "$out/$f.o" "$gs/Private/Characters/Abilities/$f.cpp"; done
g++ "${cxx[@]}" -std=c++20 -c -o "$out/GSAT_WaitInteractableTarget.o" "$gs/Private/Characters/Abilities/AbilityTasks/GSAT_WaitInteractableTarget.cpp"
g++ "${cxx[@]}" -std=c++20 -c -o "$out/bridge_exec.o" "$here/cpp2mips/bridge_exec.cpp"
g++ "${cxx[@]}" -std=c++20 -c -o "$out/GSDamageExecutionCalc.o" "$gs/Private/Characters/Abilities/GSDamageExecutionCalc.cpp"
g++ "${cxx[@]}" -c -o "$out/fname.o" "$here/cpp2mips/fname.cpp"
g++ "${cxx[@]}" -c -o "$out/ammoset.o" "$gs/Private/Characters/Abilities/AttributeSets/GSAmmoAttributeSet.cpp"
g++ ${SAN:+-fsanitize=address,undefined} -o "$out/shooter" "${objs[@]}" "$out/bridge.o" "$out/fname.o" "$out/ammoset.o" "$out/bridge_base.o" "$out/baseset.o" "$out/jumpcpp.o" "$out/GSGameplayAbility.o" "$out/GSTargetType.o" "$out/GSAbilityTypes.o" "$out/GSInteractable.o" "$out/GSGATA_Trace.o" "$out/GSGATA_LineTrace.o" "$out/GSGATA_SphereTrace.o" "$out/bridge_exec.o" "$out/GSAT_WaitInteractableTarget.o" "$out/GSDamageExecutionCalc.o" -lm
{ if [ -n "${SAN:-}" ]; then "$out/shooter" 2>&1; else stdbuf -o0 "$out/shooter"; fi; } | grep -E "FAIL|shooter:|project.s C\+\+|runtime error|ERROR: Addr" || true
