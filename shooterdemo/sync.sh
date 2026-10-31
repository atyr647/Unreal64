#!/usr/bin/env bash
cd "$(dirname "$0")" && for f in u64_rt.c u64_natives.c u64_gas.c u64_gas_core.c u64_gas_extra.c u64_collision.c u64_math.c u64_movement.c u64_cpp_game.c u64_cpp_gasshooter.c u64_game_loop.c; do cp ../runtime/$f src/; done; cp ../tests/bp/run_shooter.c src/scenarios.c
