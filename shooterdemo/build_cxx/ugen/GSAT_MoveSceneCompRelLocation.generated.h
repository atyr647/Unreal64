#pragma once
#undef CURRENT_FILE_ID
#define CURRENT_FILE_ID GSAT_MoveSceneCompRelLocation
class UGSAT_MoveSceneCompRelLocation;
#define GSAT_MoveSceneCompRelLocation_21_GENERATED_BODY public:  typedef UAbilityTask Super;  UGSAT_MoveSceneCompRelLocation(const FObjectInitializer &i = FObjectInitializer()); typedef UGSAT_MoveSceneCompRelLocation ThisClass; static UClass *StaticClass() { static UClass c = {"UGSAT_MoveSceneCompRelLocation", &ShimMake<UGSAT_MoveSceneCompRelLocation>}; return &c; } bool InstanceOf(const UClass *c) const override { return c == StaticClass() || Super::InstanceOf(c); } 
