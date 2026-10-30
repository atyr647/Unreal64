#!/usr/bin/env bash
# Boot a ROM in ares under a virtual display and save a screenshot. STATUS: UNTESTED (ares not installed here).
# usage: tools/run_ares.sh viewer/unreal64_viewer.z64 shot.png [seconds]
set -euo pipefail
rom="${1:?rom}"; shot="${2:-shot.png}"; wait_s="${3:-8}"
command -v ares >/dev/null || { echo "ares not found on PATH"; exit 2; }
xvfb-run -a -s "-screen 0 1280x960x24" bash -c '
  ares --no-file-prompt "$0" & pid=$!
  sleep "$1"
  import -window root "$2" 2>/dev/null || xwd -root -silent | convert xwd:- "$2"
  kill $pid 2>/dev/null || true
' "$rom" "$wait_s" "$shot"
echo "wrote $shot"
