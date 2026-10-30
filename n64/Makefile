BUILD_DIR=build
SOURCE_DIR=src
include $(N64_INST)/include/n64.mk

all: unreal64.z64

OBJS = $(BUILD_DIR)/main.o

unreal64.z64: N64_ROM_TITLE="Unreal64"

$(BUILD_DIR)/unreal64.elf: $(OBJS)

clean:
	rm -rf $(BUILD_DIR) unreal64.z64

-include $(wildcard $(BUILD_DIR)/*.d)

.PHONY: all clean
