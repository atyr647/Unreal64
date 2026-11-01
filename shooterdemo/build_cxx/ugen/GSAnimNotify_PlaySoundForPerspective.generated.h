#pragma once
#undef CURRENT_FILE_ID
#define CURRENT_FILE_ID GSAnimNotify_PlaySoundForPerspective
class UGSAnimNotify_PlaySoundForPerspective;
#define GSAnimNotify_PlaySoundForPerspective_15_GENERATED_BODY public: typedef UAnimNotify_PlaySound Super;  typedef UGSAnimNotify_PlaySoundForPerspective ThisClass; static UClass *StaticClass() { static UClass c = {"UGSAnimNotify_PlaySoundForPerspective", &ShimMake<UGSAnimNotify_PlaySoundForPerspective>}; return &c; } bool InstanceOf(const UClass *c) const override { return c == StaticClass() || Super::InstanceOf(c); } 
