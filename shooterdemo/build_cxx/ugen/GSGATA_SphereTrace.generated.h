#pragma once
#undef CURRENT_FILE_ID
#define CURRENT_FILE_ID GSGATA_SphereTrace
class AGSGATA_SphereTrace;
#define GSGATA_SphereTrace_21_GENERATED_BODY public:  typedef AGSGATA_Trace Super;  typedef AGSGATA_SphereTrace ThisClass; static UClass *StaticClass() { static UClass c = {"AGSGATA_SphereTrace", &ShimMake<AGSGATA_SphereTrace>}; return &c; } bool InstanceOf(const UClass *c) const override { return c == StaticClass() || Super::InstanceOf(c); } 
