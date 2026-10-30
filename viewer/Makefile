# Unreal64 level viewer. Usage:
#   python -m n64fit <level.glb> viewer/        (run from tools/, writes viewer/assets/ and viewer/level_index.h)
#   make T3D_INST=/path/to/tiny3d               (needs libdragon toolchain, Tiny3D built, N64_INST set)
BUILD_DIR=build
T3D_INST ?= $(shell realpath ../third_party/tiny3d)

include $(N64_INST)/include/n64.mk
include $(T3D_INST)/t3d.mk
-include assets/formats.mk

N64_CFLAGS += -std=gnu2x -I.

src = src/main.c
assets_png = $(wildcard assets/*.png)
assets_glb = $(wildcard assets/*.glb)
assets_conv = $(addprefix filesystem/,$(notdir $(assets_png:%.png=%.sprite))) \
              $(addprefix filesystem/,$(notdir $(assets_glb:%.glb=%.t3dm)))

all: unreal64_viewer.z64

filesystem/%.sprite: assets/%.png
	@mkdir -p $(dir $@)
	@echo "    [SPRITE] $@ ($(SPRITE_FMT_$*))"
	$(N64_MKSPRITE) $(MKSPRITE_FLAGS) -f $(SPRITE_FMT_$*) -o filesystem "$<"

filesystem/%.t3dm: assets/%.glb
	@mkdir -p $(dir $@)
	@echo "    [T3D-MODEL] $@"
	$(T3D_GLTF_TO_3D) "$<" $@ --bvh --base-scale=$(BASE_SCALE_$*)
	$(N64_BINDIR)/mkasset -c 2 -o filesystem $@

$(BUILD_DIR)/unreal64_viewer.dfs: $(assets_conv)
$(BUILD_DIR)/unreal64_viewer.elf: $(src:%.c=$(BUILD_DIR)/%.o)

unreal64_viewer.z64: N64_ROM_TITLE="Unreal64"
unreal64_viewer.z64: $(BUILD_DIR)/unreal64_viewer.dfs

clean:
	rm -rf $(BUILD_DIR) *.z64 filesystem

-include $(wildcard $(BUILD_DIR)/*.d)
.PHONY: all clean
