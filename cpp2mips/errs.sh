#!/usr/bin/env bash
# errs.sh <Source/Module> <file-substring>  : full error list for one file against the shim
gs="$1"; here="$(cd "$(dirname "$0")" && pwd)"; out="$here/build/gall"
[ -d "$out" ] || python3 "$here/uht_lite.py" "$out" $(find "$gs" -name "*.h") >/dev/null 2>&1
f=$(find "$gs" -name "*.cpp" | grep "$2" | head -1)
g++ -std=c++20 -fsyntax-only -w -fno-exceptions -I"$here/shim" -I"$out" -I"$gs/Public" -I"$gs" -I"$gs/.." "$f" 2>&1 | grep -E "error" | cut -c1-260
