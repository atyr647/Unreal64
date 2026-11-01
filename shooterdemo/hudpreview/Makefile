# A tiny ROM that draws a HUD draw list dumped from the shooter scenarios (HUD_DUMP=hudpreview/src/hud_dump.c tools/run_shooter_tests.sh <Content>): a quick look at the renderer in ares.
BUILD_DIR ?= build
SOURCE_DIR = src
.DEFAULT_GOAL := all
include $(N64_INST)/include/n64.mk
N64_CFLAGS += -std=gnu2x -I../../runtime
src = src/main.c src/hud_render.c src/hud_v0.c src/hud_v1.c
objs = $(patsubst $(SOURCE_DIR)/%.c,$(BUILD_DIR)/%.o,$(src))
all: hudpreview.z64
$(BUILD_DIR)/hudpreview.elf: $(objs)
hudpreview.z64: N64_ROM_TITLE="Unreal64 HUD"
src/hud_render.c: ../src/hud_render.c
	cp $< $@
clean:
	rm -rf $(BUILD_DIR) *.z64
-include $(wildcard $(BUILD_DIR)/*.d)
.PHONY: all clean
