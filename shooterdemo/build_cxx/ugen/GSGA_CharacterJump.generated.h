#pragma once
#undef CURRENT_FILE_ID
#define CURRENT_FILE_ID GSGA_CharacterJump
class UGSGA_CharacterJump;
#define GSGA_CharacterJump_15_GENERATED_BODY public:  typedef UGSGameplayAbility Super;  typedef UGSGA_CharacterJump ThisClass; static UClass *StaticClass() { static UClass c = {"UGSGA_CharacterJump", &ShimMake<UGSGA_CharacterJump>}; return &c; } bool InstanceOf(const UClass *c) const override { return c == StaticClass() || Super::InstanceOf(c); } 
