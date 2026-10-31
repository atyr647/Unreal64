#!/usr/bin/env bash
# Build the project's gameplay C++ against the UE shim and run it on the host (g++) and, with libdragon installed, for the N64.
#   cpp2mips/run_tests.sh <GASShooter>/Source/GASShooter [--n64]
set -euo pipefail
gs="${1:?path to the project Source/<Module> directory}"
here="$(cd "$(dirname "$0")" && pwd)"
out="$here/build"; mkdir -p "$out/gen"
python3 "$here/uht_lite.py" "$out/gen" "$gs/Public/Characters/Abilities/AttributeSets/GSAmmoAttributeSet.h" >/dev/null
g++ -std=c++17 -fno-exceptions -fno-rtti -fno-threadsafe-statics -Wall -Wno-unused -Wno-invalid-offsetof -I"$here/shim" -I"$out/gen" -I"$gs/Public" \
  -o "$out/test_ammo_host" "$here/test_ammo.cpp" "$here/fname.cpp" "$gs/Private/Characters/Abilities/AttributeSets/GSAmmoAttributeSet.cpp"
"$out/test_ammo_host" | grep -E "FAIL|ammo:"
if [ "${2:-}" = "--n64" ]; then
  (cd "$here/n64" && GASSHOOTER_SRC="$gs" make >/dev/null && echo "n64 ROM: $here/n64/cppammo.z64")
fi
