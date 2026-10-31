# The project's gameplay C++ (GASShooter, unmodified) built for the N64 against the UE shim.
#   GASSHOOTER_SRC=<GASShooter>/Source/GASShooter  make
BUILD_DIR=build
SOURCE_DIR=.
include $(N64_INST)/include/n64.mk
GS ?= $(GASSHOOTER_SRC)
N64_CXXFLAGS += -std=gnu++17 -fno-exceptions -fno-rtti -fno-threadsafe-statics -I../shim -I../build/gen -I$(GS)/Public -Wno-unused -Wno-invalid-offsetof
N64_LDFLAGS += -lm

src = main_n64.cpp ../fname.cpp $(GS)/Private/Characters/Abilities/AttributeSets/GSAmmoAttributeSet.cpp
objs = $(BUILD_DIR)/main_n64.o $(BUILD_DIR)/fname.o $(BUILD_DIR)/GSAmmoAttributeSet.o

all: cppammo.z64
$(BUILD_DIR)/GSAmmoAttributeSet.o: $(GS)/Private/Characters/Abilities/AttributeSets/GSAmmoAttributeSet.cpp
	@mkdir -p $(dir $@)
	@echo "    [CXX] $<"
	$(N64_CXX) $(N64_CXXFLAGS) -c -o $@ $<
$(BUILD_DIR)/fname.o: ../fname.cpp
	@mkdir -p $(dir $@)
	@echo "    [CXX] $<"
	$(N64_CXX) $(N64_CXXFLAGS) -c -o $@ $<
$(BUILD_DIR)/cppammo.elf: $(objs)
cppammo.z64: N64_ROM_TITLE="Unreal64 C++ test"
clean:
	rm -rf $(BUILD_DIR) *.z64
-include $(wildcard $(BUILD_DIR)/*.d)
.PHONY: all clean
