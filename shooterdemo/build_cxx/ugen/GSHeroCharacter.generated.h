#pragma once
#undef CURRENT_FILE_ID
#define CURRENT_FILE_ID GSHeroCharacter
struct FGSHeroInventory;
class AGSHeroCharacter;
#define GSHeroCharacter_30_GENERATED_BODY public:  typedef FGSHeroInventory ThisClass; static UClass *StaticClass() { static UClass c = {"FGSHeroInventory", &ShimMake<FGSHeroInventory>}; return &c; } static UScriptStruct *StaticStruct() { static UScriptStruct s; return &s; } 
#define GSHeroCharacter_50_GENERATED_BODY public: typedef AGSCharacterBase Super;  typedef AGSHeroCharacter ThisClass; static UClass *StaticClass() { static UClass c = {"AGSHeroCharacter", &ShimMake<AGSHeroCharacter>}; return &c; } bool InstanceOf(const UClass *c) const override { return c == StaticClass() || Super::InstanceOf(c); } 
