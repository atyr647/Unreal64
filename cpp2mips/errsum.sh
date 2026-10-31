#!/usr/bin/env bash
# aggregate first errors of every project file: errsum.sh <Source/Module> [N per file]
G="$1"; here="$(cd "$(dirname "$0")" && pwd)"
for f in $(find $G -name "*.cpp"); do g++ -std=c++20 -fsyntax-only -w -I$here/shim -I$here/build/gall -I$G/Public -I$G -I$G/.. $f 2>&1 | grep -E "error" | head -${2:-40} | sed "s|^|$(basename $f): |"; done | sed "s|$G/||g" | cut -c1-230
