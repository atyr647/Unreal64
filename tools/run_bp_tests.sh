#!/usr/bin/env bash
# Compile a project's Blueprints to C and run the host scenario tests against them.
#   tools/run_bp_tests.sh <Project>/Content          (needs gcc; the scenarios target the GASDocumentation project)
set -euo pipefail
content="${1:?path to the project's Content directory}"
here="$(cd "$(dirname "$0")/.." && pwd)"
out="$(mktemp -d)"; trap 'rm -rf "$out"' EXIT
python3 "$here/tools/ue64/bpc.py" "$content" "$out/gen" | tail -1
sed -n '/native coverage/p' "$out/gen/compat_report.txt"
gcc -w -I"$here/runtime" -o "$out/gas" "$here/tests/bp/run_gas.c" "$here/runtime/u64_rt.c" "$here/runtime/u64_natives.c" "$here/runtime/u64_gas.c" "$out"/gen/*.c -lm
"$out/gas" | tail -1
gcc -w -I"$here/runtime" -o "$out/world" "$here/tests/bp/run_world.c" "$here/runtime/u64_rt.c" "$here/runtime/u64_natives.c" "$here/runtime/u64_gas.c" "$out"/gen/*.c -lm
"$out/world" | grep -v '^\[bp\]'
