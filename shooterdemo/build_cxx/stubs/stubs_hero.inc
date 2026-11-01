FSimpleMulticastDelegate* AGSHeroCharacter::GetTargetCancelInteractionDelegate(UPrimitiveComponent* InteractionComponent)  { return nullptr; }
bool AGSHeroCharacter::IsAvailableForInteraction_Implementation(UPrimitiveComponent* InteractionComponent) const { return bool(); }
float AGSHeroCharacter::GetInteractionDuration_Implementation(UPrimitiveComponent* InteractionComponent) const { return float(); }
void AGSHeroCharacter::BeginPlay()  { }
void AGSHeroCharacter::CancelInteraction_Implementation(UPrimitiveComponent* InteractionComponent)  { }
void AGSHeroCharacter::CurrentWeaponPrimaryClipAmmoChanged(int32 OldPrimaryClipAmmo, int32 NewPrimaryClipAmmo)  { }
void AGSHeroCharacter::CurrentWeaponPrimaryReserveAmmoChanged(const FOnAttributeChangeData& Data)  { }
void AGSHeroCharacter::CurrentWeaponSecondaryClipAmmoChanged(int32 OldSecondaryClipAmmo, int32 NewSecondaryClipAmmo)  { }
void AGSHeroCharacter::CurrentWeaponSecondaryReserveAmmoChanged(const FOnAttributeChangeData& Data)  { }
void AGSHeroCharacter::EndPlay(const EEndPlayReason::Type EndPlayReason)  { }
void AGSHeroCharacter::FinishDying()  { }
void AGSHeroCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const { }
void AGSHeroCharacter::GetPreInteractSyncType_Implementation(bool& bShouldSync, EAbilityTaskNetSyncType& Type, UPrimitiveComponent* InteractionComponent) const { }
void AGSHeroCharacter::KnockDown()  { }
void AGSHeroCharacter::NextWeapon()  { }
void AGSHeroCharacter::OnRep_Controller()  { }
void AGSHeroCharacter::OnRep_PlayerState()  { }
void AGSHeroCharacter::PlayKnockDownEffects()  { }
void AGSHeroCharacter::PlayReviveEffects()  { }
void AGSHeroCharacter::PossessedBy(AController* NewController)  { }
void AGSHeroCharacter::PostInitializeComponents()  { }
void AGSHeroCharacter::PostInteract_Implementation(AActor* InteractingActor, UPrimitiveComponent* InteractionComponent)  { }
void AGSHeroCharacter::PreInteract_Implementation(AActor* InteractingActor, UPrimitiveComponent* InteractionComponent)  { }
void AGSHeroCharacter::PreviousWeapon()  { }
void AGSHeroCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)  { }
void AGSHeroCharacter::WeaponChangingDelayReplicationTagChanged(const FGameplayTag CallbackTag, int32 NewCount)  { }
