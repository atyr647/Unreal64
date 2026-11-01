# N64 ROM that runs GASShooter's compiled Blueprints and the project's C++ ports through the scenario suite. Regenerate gen/ with:
#   python3 tools/ue64/bpc.py <GASShooter>/Content shooterdemo/src/gen ; shooterdemo/sync.sh ; make
# CXX_AMMO=1 GASSHOOTER_SRC=<GASShooter>/Source/GASShooter ROM=shooterdemo_cxx BUILD_DIR=build_cxx make
#   runs the ammo attribute rules in the project's own, unmodified UGSAmmoAttributeSet.cpp (mips64-elf-g++, see ../cpp2mips)
BUILD_DIR ?= build
SOURCE_DIR=src
ROM ?= shooterdemo
.DEFAULT_GOAL := all
include $(N64_INST)/include/n64.mk
N64_CFLAGS += -std=gnu2x -DU64_NO_MAIN -I../runtime -Wno-unused-label -Wno-unused-variable -Wno-unused-but-set-variable -Wno-error
N64_LDFLAGS += -lm

src = src/main.c src/u64_rt.c src/u64_natives.c src/u64_gas.c src/u64_gas_core.c src/u64_gas_extra.c src/u64_collision.c src/u64_anim.c src/u64_math.c src/u64_movement.c src/u64_cpp_game.c src/u64_cpp_gasshooter.c src/u64_game_loop.c src/scenarios.c $(wildcard src/gen/*.c)
objs = $(patsubst $(SOURCE_DIR)/%.c,$(BUILD_DIR)/%.o,$(src))

ifdef CXX_AMMO
GS = $(GASSHOOTER_SRC)
N64_CFLAGS += -DU64_CXX_AMMO -DU64_CXX_BASE -DU64_CXX_JUMP -DU64_CXX_ABILITY -DU64_CXX_EXEC
N64_CXXFLAGS += -std=gnu++20 -fno-exceptions -fno-threadsafe-statics -w -I../cpp2mips/shim -I$(BUILD_DIR)/ugen -I$(BUILD_DIR)/stubs -I$(GS)/Public -I$(GS)/.. -I../runtime
objs += $(BUILD_DIR)/cxx/bridge_ammo.o $(BUILD_DIR)/cxx/fname.o $(BUILD_DIR)/cxx/GSAmmoAttributeSet.o $(BUILD_DIR)/cxx/bridge_base.o $(BUILD_DIR)/cxx/GSAttributeSetBase.o $(BUILD_DIR)/cxx/GSGA_CharacterJump.o $(BUILD_DIR)/cxx/GSGameplayAbility.o $(BUILD_DIR)/cxx/GSTargetType.o $(BUILD_DIR)/cxx/GSAbilityTypes.o $(BUILD_DIR)/cxx/GSInteractable.o $(BUILD_DIR)/cxx/GSDamageExecutionCalc.o $(BUILD_DIR)/cxx/bridge_exec.o
UHT_HEADERS = $(shell find $(GS)/Public -name "*.h")
$(BUILD_DIR)/ugen/GSAmmoAttributeSet.generated.h: $(UHT_HEADERS)
	python3 ../cpp2mips/uht_lite.py $(BUILD_DIR)/ugen $(UHT_HEADERS) > /dev/null
	@mkdir -p $(BUILD_DIR)/stubs
	python3 ../cpp2mips/stubgen.py AGSHeroCharacter $(GS)/Public/Characters/Heroes/GSHeroCharacter.h > $(BUILD_DIR)/stubs/stubs_hero.inc
	python3 ../cpp2mips/stubgen.py AGSWeapon $(GS)/Public/Weapons/GSWeapon.h > $(BUILD_DIR)/stubs/stubs_weapon.inc
	python3 ../cpp2mips/stubgen.py UGSAbilitySystemComponent $(GS)/Public/Characters/Abilities/GSAbilitySystemComponent.h > $(BUILD_DIR)/stubs/stubs_asc.inc
$(BUILD_DIR)/cxx/bridge_ammo.o: ../cpp2mips/bridge_ammo.cpp $(BUILD_DIR)/ugen/GSAmmoAttributeSet.generated.h
	@mkdir -p $(dir $@); echo "    [CXX] $<"; $(N64_CXX) $(N64_CXXFLAGS) -c -o $@ $<
$(BUILD_DIR)/cxx/fname.o: ../cpp2mips/fname.cpp
	@mkdir -p $(dir $@); echo "    [CXX] $<"; $(N64_CXX) $(N64_CXXFLAGS) -c -o $@ $<
$(BUILD_DIR)/cxx/GSAmmoAttributeSet.o: $(GS)/Private/Characters/Abilities/AttributeSets/GSAmmoAttributeSet.cpp $(BUILD_DIR)/ugen/GSAmmoAttributeSet.generated.h
	@mkdir -p $(dir $@); echo "    [CXX] $<"; $(N64_CXX) $(N64_CXXFLAGS) -c -o $@ $<
$(BUILD_DIR)/cxx/bridge_base.o: ../cpp2mips/bridge_base.cpp $(BUILD_DIR)/ugen/GSAmmoAttributeSet.generated.h
	@mkdir -p $(dir $@); echo "    [CXX] $<"; $(N64_CXX) $(N64_CXXFLAGS) -c -o $@ $<
$(BUILD_DIR)/cxx/GSAttributeSetBase.o: $(GS)/Private/Characters/Abilities/AttributeSets/GSAttributeSetBase.cpp $(BUILD_DIR)/ugen/GSAmmoAttributeSet.generated.h
	@mkdir -p $(dir $@); echo "    [CXX] $<"; $(N64_CXX) $(N64_CXXFLAGS) -c -o $@ $<
$(BUILD_DIR)/cxx/GSGA_CharacterJump.o: $(GS)/Private/Characters/Abilities/GSGA_CharacterJump.cpp $(BUILD_DIR)/ugen/GSAmmoAttributeSet.generated.h
	@mkdir -p $(dir $@); echo "    [CXX] $<"; $(N64_CXX) $(N64_CXXFLAGS) -c -o $@ $<
$(BUILD_DIR)/cxx/bridge_exec.o: ../cpp2mips/bridge_exec.cpp $(BUILD_DIR)/ugen/GSAmmoAttributeSet.generated.h
	@mkdir -p $(dir $@); echo "    [CXX] $<"; $(N64_CXX) $(N64_CXXFLAGS) -c -o $@ $<
$(BUILD_DIR)/cxx/%.o: $(GS)/Private/Characters/Abilities/%.cpp $(BUILD_DIR)/ugen/GSAmmoAttributeSet.generated.h
	@mkdir -p $(dir $@); echo "    [CXX] $<"; $(N64_CXX) $(N64_CXXFLAGS) -c -o $@ $<
endif

all: $(ROM).z64
$(BUILD_DIR)/$(ROM).elf: $(objs)
$(ROM).z64: N64_ROM_TITLE="Unreal64 GASShooter"

clean:
	rm -rf $(BUILD_DIR) *.z64
-include $(wildcard $(BUILD_DIR)/*.d)
-include $(wildcard $(BUILD_DIR)/gen/*.d)
.PHONY: all clean
