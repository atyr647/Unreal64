# Unreal64 game ROM: the converted level running live (top-down view + HUD), driven by the controller.
#   python3 ../tools/ue64/bpc.py <Project>/Content src/gen ; ./sync.sh ; make     (needs N64_INST with libdragon)
# Requires the Expansion Pak (8 MB): the Blueprint runtime's pools are sized for it.
BUILD_DIR=build
SOURCE_DIR=src
include $(N64_INST)/include/n64.mk
N64_CFLAGS += -std=gnu2x -I../runtime -Wno-unused-label -Wno-unused-variable -Wno-unused-but-set-variable -Wno-error
N64_LDFLAGS += -lm

src = src/main.c src/render.c src/u64_rt.c src/u64_natives.c src/u64_gas.c src/u64_gas_core.c src/u64_math.c src/u64_movement.c src/u64_cpp_game.c src/u64_game_loop.c $(wildcard src/gen/*.c)

all: unreal64_game.z64
$(BUILD_DIR)/unreal64_game.elf: $(patsubst $(SOURCE_DIR)/%.c,$(BUILD_DIR)/%.o,$(src))
unreal64_game.z64: N64_ROM_TITLE="Unreal64 Game"

clean:
	rm -rf $(BUILD_DIR) *.z64
-include $(wildcard $(BUILD_DIR)/*.d)
-include $(wildcard $(BUILD_DIR)/gen/*.d)
.PHONY: all clean
