#!/usr/bin/env bash
# Compile GASShooter's Blueprints to C and run the host scenario tests (exact values from its assets and C++ sources).
#   tools/run_shooter_tests.sh <GASShooter>/Content     (needs gcc)
set -euo pipefail
content="${1:?path to the project Content directory}"
here="$(cd "$(dirname "$0")/.." && pwd)"
out="$(mktemp -d)"
trap 'rm -rf "$out"' EXIT
rt="$here/runtime/u64_rt.c $here/runtime/u64_natives.c $here/runtime/u64_gas.c $here/runtime/u64_gas_core.c $here/runtime/u64_gas_extra.c $here/runtime/u64_collision.c $here/runtime/u64_anim.c $here/runtime/u64_math.c $here/runtime/u64_movement.c $here/runtime/u64_cpp_game.c $here/runtime/u64_cpp_gasshooter.c"
python3 "$here/tools/ue64/bpc.py" "$content" "$out/gen" | tail -1
grep "native coverage" "$out/gen/compat_report.txt"
gcc -w -fsanitize=address,undefined -I"$here/runtime" -o "$out/shooter" "$here/tests/bp/run_shooter.c" $rt "$out"/gen/*.c -lm
"$out/shooter" | grep -E "FAIL|shooter:"
