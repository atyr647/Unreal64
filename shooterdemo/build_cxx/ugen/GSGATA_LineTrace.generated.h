#pragma once
#undef CURRENT_FILE_ID
#define CURRENT_FILE_ID GSGATA_LineTrace
class AGSGATA_LineTrace;
#define GSGATA_LineTrace_21_GENERATED_BODY public:  typedef AGSGATA_Trace Super;  typedef AGSGATA_LineTrace ThisClass; static UClass *StaticClass() { static UClass c = {"AGSGATA_LineTrace", &ShimMake<AGSGATA_LineTrace>}; return &c; } bool InstanceOf(const UClass *c) const override { return c == StaticClass() || Super::InstanceOf(c); } 
