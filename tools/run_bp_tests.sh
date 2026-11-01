#!/usr/bin/env bash
# Compile a project's Blueprints to C and run the host scenario tests against them.
#   tools/run_bp_tests.sh <Project>/Content     (needs gcc; the scenarios target the GASDocumentation project)
set -euo pipefail
content="${1:?path to the project Content directory}"
here="$(cd "$(dirname "$0")/.." && pwd)"
out="$(mktemp -d)"
trap 'rm -rf "$out"' EXIT
rt="$here/runtime/u64_rt.c $here/runtime/u64_natives.c $here/runtime/u64_gas.c $here/runtime/u64_gas_core.c $here/runtime/u64_gas_extra.c $here/runtime/u64_collision.c $here/runtime/u64_anim.c $here/runtime/u64_widgets.c $here/runtime/u64_math.c $here/runtime/u64_movement.c $here/runtime/u64_cpp_game.c"
python3 "$here/tools/ue64/bpc.py" "$content" "$out/gen" | tail -1
grep "native coverage" "$out/gen/compat_report.txt"
gcc -w -I"$here/runtime" -o "$out/gas" "$here/tests/bp/run_gas.c" $rt "$out"/gen/*.c -lm
"$out/gas" | tail -1
gcc -w -I"$here/runtime" -o "$out/world" "$here/tests/bp/run_world.c" $rt "$out"/gen/*.c -lm
"$out/world" | grep -v "bp\]" || true
