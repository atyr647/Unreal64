#!/usr/bin/env bash
# Copy the runtime into src/ (the n64.mk build compiles from SOURCE_DIR).
cd "$(dirname "$0")" && for f in u64_rt.c u64_natives.c u64_gas.c u64_gas_core.c u64_movement.c u64_cpp_game.c u64_game_loop.c; do cp ../runtime/$f src/; done
