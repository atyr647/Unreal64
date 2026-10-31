#!/usr/bin/env bash
# The GASShooter scenario suite with the ammo attribute rules executed by the project's own C++ (GSAmmoAttributeSet.cpp, unmodified, built against the UE shim).
#   cpp2mips/run_shooter_cxx.sh <GASShooter>/Content <GASShooter>/Source/GASShooter
set -euo pipefail
content="${1:?path to the project Content directory}"; gs="${2:?path to the project Source/<Module> directory}"
here="$(cd "$(dirname "$0")/.." && pwd)"; out="$(mktemp -d)"; [ -n "${KEEP:-}" ] || trap 'rm -rf "$out"' EXIT; echo "build dir: $out" >&2
python3 "$here/tools/ue64/bpc.py" "$content" "$out/gen" | tail -1
python3 "$here/cpp2mips/uht_lite.py" "$out/ugen" "$gs/Public/Characters/Abilities/AttributeSets/GSAmmoAttributeSet.h" "$gs/Public/Characters/Abilities/AttributeSets/GSAttributeSetBase.h" "$gs/Public/Characters/GSCharacterBase.h" "$gs/Public/Player/GSPlayerController.h" >/dev/null
objs=()
for f in u64_rt u64_natives u64_gas u64_gas_core u64_gas_extra u64_collision u64_math u64_movement u64_cpp_game u64_cpp_gasshooter; do
  gcc -g -w -DU64_CXX_AMMO -DU64_CXX_BASE -I"$here/runtime" -c -o "$out/$f.o" "$here/runtime/$f.c"; objs+=("$out/$f.o"); done
gcc -g -w -DU64_CXX_AMMO -DU64_CXX_BASE -I"$here/runtime" -c -o "$out/run_shooter.o" "$here/tests/bp/run_shooter.c"; objs+=("$out/run_shooter.o")
for f in "$out"/gen/*.c; do gcc -g -w -I"$here/runtime" -c -o "$f.o" "$f"; objs+=("$f.o"); done
cxx=(-g -std=c++17 -fno-exceptions -fno-rtti -fno-threadsafe-statics -w -I"$here/cpp2mips/shim" -I"$out/ugen" -I"$gs/Public" -I"$gs/.." -I"$here/runtime")
g++ "${cxx[@]}" -c -o "$out/bridge.o" "$here/cpp2mips/bridge_ammo.cpp"
g++ "${cxx[@]}" -std=c++20 -c -o "$out/bridge_base.o" "$here/cpp2mips/bridge_base.cpp"
g++ "${cxx[@]}" -c -o "$out/baseset.o" "$gs/Private/Characters/Abilities/AttributeSets/GSAttributeSetBase.cpp"
g++ "${cxx[@]}" -c -o "$out/fname.o" "$here/cpp2mips/fname.cpp"
g++ "${cxx[@]}" -c -o "$out/ammoset.o" "$gs/Private/Characters/Abilities/AttributeSets/GSAmmoAttributeSet.cpp"
g++ -o "$out/shooter" "${objs[@]}" "$out/bridge.o" "$out/fname.o" "$out/ammoset.o" "$out/bridge_base.o" "$out/baseset.o" -lm
stdbuf -o0 "$out/shooter" | grep -E "FAIL|shooter:|project.s C\+\+" || true
