#pragma once
#undef CURRENT_FILE_ID
#define CURRENT_FILE_ID GSCharacterBase
struct FGSDamageNumber;
class AGSCharacterBase;
#define GSCharacterBase_18_GENERATED_BODY public:  typedef FGSDamageNumber ThisClass; static UClass *StaticClass() { static UClass c = {"FGSDamageNumber"}; return &c; } static UScriptStruct *StaticStruct() { static UScriptStruct s; return &s; } 
#define GSCharacterBase_40_GENERATED_BODY public: typedef ACharacter Super;  typedef AGSCharacterBase ThisClass; static UClass *StaticClass() { static UClass c = {"AGSCharacterBase"}; return &c; } bool InstanceOf(const UClass *c) const override { return c == StaticClass() || Super::InstanceOf(c); } 
