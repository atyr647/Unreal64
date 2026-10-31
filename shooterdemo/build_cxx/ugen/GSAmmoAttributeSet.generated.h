#pragma once
#undef CURRENT_FILE_ID
#define CURRENT_FILE_ID GSAmmoAttributeSet
#define GSAmmoAttributeSet_23_GENERATED_BODY public: typedef UAttributeSet Super; typedef UGSAmmoAttributeSet ThisClass; static UClass *StaticClass() { static UClass c = {"UGSAmmoAttributeSet"}; return &c; } FGameplayAttributeData *FindAttributeData(FName n) override { if (n == FName("RifleReserveAmmo")) return &RifleReserveAmmo; if (n == FName("MaxRifleReserveAmmo")) return &MaxRifleReserveAmmo; if (n == FName("RocketReserveAmmo")) return &RocketReserveAmmo; if (n == FName("MaxRocketReserveAmmo")) return &MaxRocketReserveAmmo; if (n == FName("ShotgunReserveAmmo")) return &ShotgunReserveAmmo; if (n == FName("MaxShotgunReserveAmmo")) return &MaxShotgunReserveAmmo; return Super::FindAttributeData(n); }
