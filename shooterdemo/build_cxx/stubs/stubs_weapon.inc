USkeletalMeshComponent* AGSWeapon::GetWeaponMesh1P() const { return nullptr; }
USkeletalMeshComponent* AGSWeapon::GetWeaponMesh3P() const { return nullptr; }
bool AGSWeapon::HasInfiniteAmmo() const { return bool(); }
bool AGSWeapon::OnDropped_Validate(FVector NewLocation)  { return bool(); }
class UAbilitySystemComponent* AGSWeapon::GetAbilitySystemComponent() const { return nullptr; }
int32 AGSWeapon::GetAbilityLevel(EGSAbilityInputID AbilityID)  { return int32(); }
int32 AGSWeapon::GetMaxPrimaryClipAmmo() const { return int32(); }
int32 AGSWeapon::GetMaxSecondaryClipAmmo() const { return int32(); }
int32 AGSWeapon::GetPrimaryClipAmmo() const { return int32(); }
int32 AGSWeapon::GetSecondaryClipAmmo() const { return int32(); }
void AGSWeapon::AddAbilities()  { }
void AGSWeapon::BeginPlay()  { }
void AGSWeapon::EndPlay(EEndPlayReason::Type EndPlayReason)  { }
void AGSWeapon::Equip()  { }
void AGSWeapon::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const { }
void AGSWeapon::NotifyActorBeginOverlap(class AActor* Other)  { }
void AGSWeapon::OnDropped_Implementation(FVector NewLocation)  { }
void AGSWeapon::OnRep_MaxPrimaryClipAmmo(int32 OldMaxPrimaryClipAmmo)  { }
void AGSWeapon::OnRep_MaxSecondaryClipAmmo(int32 OldMaxSecondaryClipAmmo)  { }
void AGSWeapon::OnRep_PrimaryClipAmmo(int32 OldPrimaryClipAmmo)  { }
void AGSWeapon::OnRep_SecondaryClipAmmo(int32 OldSecondaryClipAmmo)  { }
void AGSWeapon::PickUpOnTouch(AGSHeroCharacter* InCharacter)  { }
void AGSWeapon::PreReplication(IRepChangedPropertyTracker& ChangedPropertyTracker)  { }
void AGSWeapon::RemoveAbilities()  { }
void AGSWeapon::ResetWeapon()  { }
void AGSWeapon::SetMaxPrimaryClipAmmo(int32 NewMaxPrimaryClipAmmo)  { }
void AGSWeapon::SetMaxSecondaryClipAmmo(int32 NewMaxSecondaryClipAmmo)  { }
void AGSWeapon::SetPrimaryClipAmmo(int32 NewPrimaryClipAmmo)  { }
void AGSWeapon::SetSecondaryClipAmmo(int32 NewSecondaryClipAmmo)  { }
void AGSWeapon::UnEquip()  { }
