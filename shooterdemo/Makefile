# N64 ROM that runs GASShooter's compiled Blueprints and the project's C++ ports through the scenario suite. Regenerate gen/ with:
#   python3 tools/ue64/bpc.py <GASShooter>/Content shooterdemo/src/gen ; shooterdemo/sync.sh ; make
BUILD_DIR=build
SOURCE_DIR=src
include $(N64_INST)/include/n64.mk
N64_CFLAGS += -std=gnu2x -DU64_NO_MAIN -I../runtime -Wno-unused-label -Wno-unused-variable -Wno-unused-but-set-variable -Wno-error
N64_LDFLAGS += -lm

src = src/main.c src/u64_rt.c src/u64_natives.c src/u64_gas.c src/u64_gas_core.c src/u64_gas_extra.c src/u64_collision.c src/u64_math.c src/u64_movement.c src/u64_cpp_game.c src/u64_cpp_gasshooter.c src/u64_game_loop.c src/scenarios.c $(wildcard src/gen/*.c)

all: shooterdemo.z64
$(BUILD_DIR)/shooterdemo.elf: $(patsubst $(SOURCE_DIR)/%.c,$(BUILD_DIR)/%.o,$(src))
shooterdemo.z64: N64_ROM_TITLE="Unreal64 GASShooter"

clean:
	rm -rf $(BUILD_DIR) *.z64
-include $(wildcard $(BUILD_DIR)/*.d)
-include $(wildcard $(BUILD_DIR)/gen/*.d)
.PHONY: all clean
