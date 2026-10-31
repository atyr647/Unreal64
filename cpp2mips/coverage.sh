#!/usr/bin/env bash
# Which of the project's .cpp files compile (syntax check) against the shim, and what stops the rest.   cpp2mips/coverage.sh <Source/Module> [file-substring]
gs="${1:?Source/<Module> dir}"; here="$(cd "$(dirname "$0")" && pwd)"; out="$here/build/gall"; rm -rf "$out"
python3 "$here/uht_lite.py" "$out" $(find "$gs" -name "*.h") >/dev/null 2>&1
ok=0; n=0
for f in $(find "$gs" -name "*.cpp" | sort | grep "${2:-.}"); do
  n=$((n+1))
  if g++ -std=c++20 -fsyntax-only -w -fno-exceptions -fno-rtti -I"$here/shim" -I"$out" -I"$gs/Public" -I"$gs" -I"$gs/.." "$f" >/dev/null 2>"$out/err.txt"; then ok=$((ok+1)); echo "OK   $(basename "$f")"
  else echo "ERR  $(basename "$f")  $(grep -c 'error' "$out/err.txt") errors; first: $(grep -m1 -o 'fatal error: .*\|error: .*' "$out/err.txt" | cut -c1-110)"; fi
done
echo "$ok / $n compile"
