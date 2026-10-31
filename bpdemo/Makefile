# N64 ROM that runs compiled Unreal Blueprints. Regenerate gen/ with:
#   python3 tools/ue64/bpc.py <Project>/Content bpdemo/src/gen
BUILD_DIR=build
SOURCE_DIR=src
include $(N64_INST)/include/n64.mk
N64_CFLAGS += -std=gnu2x -DU64_NO_MAIN -I../runtime -Wno-unused-label -Wno-unused-variable -Wno-unused-but-set-variable -Wno-error
N64_LDFLAGS += -lm

src = src/main.c src/u64_rt.c src/u64_natives.c src/u64_gas.c src/u64_gas_core.c src/u64_gas_extra.c src/u64_collision.c src/u64_anim.c src/u64_math.c src/u64_movement.c src/u64_cpp_game.c src/u64_game_loop.c src/scenarios.c $(wildcard src/gen/*.c)

all: bpdemo.z64
$(BUILD_DIR)/bpdemo.elf: $(patsubst $(SOURCE_DIR)/%.c,$(BUILD_DIR)/%.o,$(src))
bpdemo.z64: N64_ROM_TITLE="Unreal64 BP"

clean:
	rm -rf $(BUILD_DIR) *.z64
-include $(wildcard $(BUILD_DIR)/*.d)
-include $(wildcard $(BUILD_DIR)/gen/*.d)
.PHONY: all clean
