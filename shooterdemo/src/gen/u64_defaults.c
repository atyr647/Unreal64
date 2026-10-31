#include "u64_gas_core.h"
void u64_apply_defaults(u64_obj *o, const char *cls) {
  if (!strcmp(cls, "ABP_Hero1P")) {
    *u64_ivar(o, "IsRifle") = u64_bool(1);
  }
  if (!strcmp(cls, "AnimNotifyState_AddLooseGameplayTags")) {
    *u64_ivar(o, "GameplayTags") = u64_array_const(0, 0, (u64_v[]){u64_int(0)} + 1);
  }
  if (!strcmp(cls, "AnimNotify_GenericEventByTag")) {
    *u64_ivar(o, "EventTag") = u64_name("None");
  }
  if (!strcmp(cls, "BP_Chest")) {
    *u64_ivar(o, "OpenInteractionDuration") = u64_float(3.0f);
    *u64_ivar(o, "ChestTopAnimationDuration") = u64_float(0.20000000298023224f);
    *u64_ivar(o, "LockStartingRelativeLocation") = u64_vec(0.0f, 0.0f, 0.0f);
    *u64_ivar(o, "AvailabilityTags") = u64_array_const(1, 2, (u64_v[]){u64_int(0), u64_name("State.Interacting"), u64_name("Object.Chest.State.Open")} + 1);
    *u64_ivar(o, "ListenTags") = u64_array_const(2, 1, (u64_v[]){u64_int(0), u64_name("Object.Chest.State.Open")} + 1);
    *u64_ivar(o, "OpenTag") = u64_name("Object.Chest.State.Open");
    *u64_ivar(o, "WeaponClasses") = u64_array_const(3, 3, (u64_v[]){u64_int(0), u64_cls("BP_Rifle"), u64_cls("BP_RocketLauncher"), u64_cls("BP_Shotgun")} + 1);
    *u64_ivar(o, "ItemClasses") = u64_array_const(4, 6, (u64_v[]){u64_int(0), u64_cls("BP_Pickup_AmmoRifle"), u64_cls("BP_Pickup_AmmoRocket"), u64_cls("BP_Pickup_AmmoShotgun"), u64_cls("BP_Pickup_Health"), u64_cls("BP_Pickup_Mana"), u64_cls("BP_Pickup_Shield")} + 1);
    *u64_ivar(o, "AbilitySystemComponent") = u64_comp(o, "AbilitySystemComponent");
  }
  if (!strcmp(cls, "BP_DamageVolume")) {
    *u64_ivar(o, "Damage") = u64_float(25.0f);
    *u64_ivar(o, "DamageGameplayEffectClass") = u64_cls("GE_DamageVolume");
    *u64_ivar(o, "bReplicates") = u64_bool(1);
  }
  if (!strcmp(cls, "BP_Door")) {
    *u64_ivar(o, "ListenTags") = u64_array_const(5, 1, (u64_v[]){u64_int(0), u64_name("Object.Door.State.Open")} + 1);
    *u64_ivar(o, "MaxDoorMovementDistance") = u64_float(140.0f);
    *u64_ivar(o, "TimeToOpenCloseDoor") = u64_float(0.5f);
    *u64_ivar(o, "AbilitySystemComponent") = u64_comp(o, "AbilitySystemComponent");
    *u64_ivar(o, "RemoteRole") = u64_name("ROLE_None");
  }
  if (!strcmp(cls, "BP_GameMode")) {
    *u64_ivar(o, "PlayerControllerClass") = u64_cls("BP_PlayerController");
    *u64_ivar(o, "PlayerStateClass") = u64_cls("GSPlayerState");
    *u64_ivar(o, "DefaultPawnClass") = u64_cls("BP_HeroCharacter");
    *u64_ivar(o, "SpectatorClass") = u64_cls("BP_SpectatorPawn");
  }
  if (!strcmp(cls, "BP_HealthManaStaminaShieldRegenVolume")) {
    *u64_ivar(o, "BuffGameplayEffectClass") = u64_cls("GE_HealthManaStaminaShieldRegenVolume");
    *u64_ivar(o, "bReplicates") = u64_bool(1);
  }
  if (!strcmp(cls, "BP_HealthManaStaminaShieldVolume")) {
    *u64_ivar(o, "bReplicates") = u64_bool(1);
  }
  if (!strcmp(cls, "BP_HeroCharacter")) {
    *u64_ivar(o, "WeaponAttachPoint") = u64_name("WeaponPoint");
    *u64_ivar(o, "ThirdPersonCameraBoom") = u64_comp(o, "CameraBoom");
    *u64_ivar(o, "ThirdPersonCamera") = u64_comp(o, "FollowCamera");
    *u64_ivar(o, "FirstPersonCamera") = u64_comp(o, "FirstPersonCamera");
    *u64_ivar(o, "FirstPersonMesh") = u64_comp(o, "FirstPersonMesh");
    *u64_ivar(o, "UIFloatingStatusBarComponent") = u64_comp(o, "UIFloatingStatusBarComponent");
    *u64_ivar(o, "DefaultInventoryWeaponClasses") = u64_array_const(6, 1, (u64_v[]){u64_int(0), u64_cls("BP_Rifle")} + 1);
    *u64_ivar(o, "KnockDownEffect") = u64_cls("GE_KnockedDown");
    *u64_ivar(o, "ReviveEffect") = u64_cls("GE_Revive");
    *u64_ivar(o, "DeathEffect") = u64_cls("GE_Dead");
    *u64_ivar(o, "DeathMontage") = u64_cls("Death_Montage");
    *u64_ivar(o, "DeathSound") = u64_cls("Stinger_PlayerDeath_Stereo_Cue");
    *u64_ivar(o, "CharacterAbilities") = u64_array_const(7, 7, (u64_v[]){u64_int(0), u64_cls("GSGA_CharacterJump"), u64_cls("GA_Sprint"), u64_cls("GA_NextWeapon"), u64_cls("GA_PreviousWeapon"), u64_cls("GA_Revive"), u64_cls("GA_InteractPassive"), u64_cls("GA_InteractActive")} + 1);
    *u64_ivar(o, "DefaultAttributes") = u64_cls("GE_HeroAttributes");
    *u64_ivar(o, "StartupEffects") = u64_array_const(8, 4, (u64_v[]){u64_int(0), u64_cls("GE_HealthRegen"), u64_cls("GE_ManaRegen"), u64_cls("GE_StaminaRegen"), u64_cls("GE_ShieldRegen")} + 1);
    *u64_ivar(o, "Mesh") = u64_comp(o, "CharacterMesh0");
    *u64_ivar(o, "CharacterMovement") = u64_comp(o, "CharMoveComp");
    *u64_ivar(o, "CapsuleComponent") = u64_comp(o, "CollisionCylinder");
    *u64_ivar(o, "ArrowComponent") = u64_comp(o, "Arrow");
    *u64_ivar(o, "RootComponent") = u64_comp(o, "CollisionCylinder");
    { u64_obj *c = u64_asobj(u64_comp(o, "FirstPersonCamera"));
      *u64_ivar(c, "RelativeLocation") = u64_vec(0.0f, 0.0f, 70.0f);
    }
    { u64_obj *c = u64_asobj(u64_comp(o, "FirstPersonMesh"));
      *u64_ivar(c, "AnimClass") = u64_cls("ABP_Hero1P");
      *u64_ivar(c, "SkeletalMesh") = u64_cls("HeroFPP");
      *u64_ivar(c, "RelativeLocation") = u64_vec(0.0f, 0.0f, -149.5f);
      *u64_ivar(c, "RelativeRotation") = u64_vec(0.0f, -90.0f, 0.0f);
    }
    { u64_obj *c = u64_asobj(u64_comp(o, "CharacterMesh0"));
      *u64_ivar(c, "AnimClass") = u64_cls("ABP_Hero3P");
      *u64_ivar(c, "SkeletalMesh") = u64_cls("HeroTPP");
      *u64_ivar(c, "bGenerateOverlapEvents") = u64_bool(1);
      *u64_ivar(c, "RelativeLocation") = u64_vec(0.0f, 0.0f, -87.0f);
      *u64_ivar(c, "RelativeRotation") = u64_vec(0.0f, -90.0f, 0.0f);
    }
    { u64_obj *c = u64_asobj(u64_comp(o, "CollisionCylinder"));
      *u64_ivar(c, "CapsuleHalfHeight") = u64_float(86.0f);
    }
  }
  if (!strcmp(cls, "BP_Pickup_AmmoRifle")) {
    *u64_ivar(o, "CollisionComp") = u64_comp(o, "CollisionComp");
    *u64_ivar(o, "PickupSound") = u64_cls("AssaultRifle_AmmoPickup_Cue");
    *u64_ivar(o, "EffectClasses") = u64_array_const(9, 1, (u64_v[]){u64_int(0), u64_cls("GE_Pickup_AmmoRifle")} + 1);
    *u64_ivar(o, "RootComponent") = u64_comp(o, "CollisionComp");
  }
  if (!strcmp(cls, "BP_Pickup_AmmoRocket")) {
    *u64_ivar(o, "CollisionComp") = u64_comp(o, "CollisionComp");
    *u64_ivar(o, "PickupSound") = u64_cls("AssaultRifle_AmmoPickup_Cue");
    *u64_ivar(o, "EffectClasses") = u64_array_const(10, 1, (u64_v[]){u64_int(0), u64_cls("GE_Pickup_AmmoRocket")} + 1);
    *u64_ivar(o, "RootComponent") = u64_comp(o, "CollisionComp");
  }
  if (!strcmp(cls, "BP_Pickup_AmmoShotgun")) {
    *u64_ivar(o, "CollisionComp") = u64_comp(o, "CollisionComp");
    *u64_ivar(o, "PickupSound") = u64_cls("AssaultRifle_AmmoPickup_Cue");
    *u64_ivar(o, "EffectClasses") = u64_array_const(11, 1, (u64_v[]){u64_int(0), u64_cls("GE_Pickup_AmmoShotgun")} + 1);
    *u64_ivar(o, "RootComponent") = u64_comp(o, "CollisionComp");
  }
  if (!strcmp(cls, "BP_Pickup_Health")) {
    *u64_ivar(o, "CollisionComp") = u64_comp(o, "CollisionComp");
    *u64_ivar(o, "PickupSound") = u64_cls("StingerPickup_Health_Cue");
    *u64_ivar(o, "EffectClasses") = u64_array_const(12, 1, (u64_v[]){u64_int(0), u64_cls("GE_Pickup_Health")} + 1);
    *u64_ivar(o, "RootComponent") = u64_comp(o, "CollisionComp");
  }
  if (!strcmp(cls, "BP_Pickup_Mana")) {
    *u64_ivar(o, "CollisionComp") = u64_comp(o, "CollisionComp");
    *u64_ivar(o, "PickupSound") = u64_cls("StingerPickup_Health_Cue");
    *u64_ivar(o, "EffectClasses") = u64_array_const(13, 1, (u64_v[]){u64_int(0), u64_cls("GE_Pickup_Mana")} + 1);
    *u64_ivar(o, "RootComponent") = u64_comp(o, "CollisionComp");
  }
  if (!strcmp(cls, "BP_Pickup_Shield")) {
    *u64_ivar(o, "CollisionComp") = u64_comp(o, "CollisionComp");
    *u64_ivar(o, "PickupSound") = u64_cls("StingerPickup_Health_Cue");
    *u64_ivar(o, "EffectClasses") = u64_array_const(14, 1, (u64_v[]){u64_int(0), u64_cls("GE_Pickup_Shield")} + 1);
    *u64_ivar(o, "RootComponent") = u64_comp(o, "CollisionComp");
  }
  if (!strcmp(cls, "BP_PlayerController")) {
    *u64_ivar(o, "UIHUDWidgetClass") = u64_cls("UI_HUD");
    *u64_ivar(o, "TransformComponent") = u64_comp(o, "TransformComponent0");
    *u64_ivar(o, "RootComponent") = u64_comp(o, "TransformComponent0");
    *u64_ivar(o, "bHiddenEdTemporary") = u64_bool(1);
  }
  if (!strcmp(cls, "BP_Rifle")) {
    *u64_ivar(o, "WeaponTag") = u64_name("Weapon.Equipped.Rifle");
    *u64_ivar(o, "PrimaryIcon") = u64_cls("Rifle_PrimaryIcon");
    *u64_ivar(o, "SecondaryIcon") = u64_cls("Rifle_SecondaryIcon");
    *u64_ivar(o, "PrimaryClipIcon") = u64_cls("Rifle_PrimaryClipIcon");
    *u64_ivar(o, "SecondaryClipIcon") = u64_cls("Rifle_SecondaryClipIcon");
    *u64_ivar(o, "FireMode") = u64_name("Weapon.Rifle.FireMode.FullAuto");
    *u64_ivar(o, "PrimaryAmmoType") = u64_name("Weapon.Ammo.Rifle");
    *u64_ivar(o, "PrimaryClipAmmo") = u64_int(30);
    *u64_ivar(o, "MaxPrimaryClipAmmo") = u64_int(30);
    *u64_ivar(o, "PrimaryHUDReticleClass") = u64_cls("HUDReticle_Dot");
    *u64_ivar(o, "CollisionComp") = u64_comp(o, "CollisionComponent");
    *u64_ivar(o, "WeaponMesh1P") = u64_comp(o, "WeaponMesh1P");
    *u64_ivar(o, "WeaponMesh3P") = u64_comp(o, "WeaponMesh3P");
    *u64_ivar(o, "Abilities") = u64_array_const(15, 5, (u64_v[]){u64_int(0), u64_cls("GA_RiflePrimary"), u64_cls("GA_RiflePrimaryInstant"), u64_cls("GA_RifleAlternate"), u64_cls("GA_RifleSecondary"), u64_cls("GA_RifleReload")} + 1);
    *u64_ivar(o, "DefaultFireMode") = u64_name("Weapon.Rifle.FireMode.FullAuto");
    *u64_ivar(o, "Equip1PMontage") = u64_cls("HeroFPP_RifleEquip_Montage");
    *u64_ivar(o, "Equip3PMontage") = u64_cls("HeroTPP_Equip");
    *u64_ivar(o, "PickupSound") = u64_cls("AssaultRifle_AmmoPickup_Cue");
    *u64_ivar(o, "RootComponent") = u64_comp(o, "CollisionComponent");
    { u64_obj *c = u64_asobj(u64_comp(o, "WeaponMesh1P"));
      *u64_ivar(c, "SkeletalMesh") = u64_cls("Rifle");
    }
    { u64_obj *c = u64_asobj(u64_comp(o, "WeaponMesh3P"));
      *u64_ivar(c, "SkeletalMesh") = u64_cls("Rifle");
    }
  }
  if (!strcmp(cls, "BP_Rifle_FireCameraShake")) {
    *u64_ivar(o, "OscillationDuration") = u64_float(0.25f);
  }
  if (!strcmp(cls, "BP_RocketLauncher")) {
    *u64_ivar(o, "WeaponTag") = u64_name("Weapon.Equipped.RocketLauncher");
    *u64_ivar(o, "PrimaryIcon") = u64_cls("RocketLauncher_PrimaryIcon");
    *u64_ivar(o, "SecondaryIcon") = u64_cls("RocketLauncher_SecondaryIcon");
    *u64_ivar(o, "PrimaryClipIcon") = u64_cls("RocketLauncher_PrimaryClipIcon");
    *u64_ivar(o, "SecondaryClipIcon") = u64_cls("RocketLauncher_SecondaryClip");
    *u64_ivar(o, "PrimaryAmmoType") = u64_name("Weapon.Ammo.Rocket");
    *u64_ivar(o, "PrimaryClipAmmo") = u64_int(3);
    *u64_ivar(o, "MaxPrimaryClipAmmo") = u64_int(3);
    *u64_ivar(o, "PrimaryHUDReticleClass") = u64_cls("HUDReticle_Dot");
    *u64_ivar(o, "CollisionComp") = u64_comp(o, "CollisionComponent");
    *u64_ivar(o, "WeaponMesh1P") = u64_comp(o, "WeaponMesh1P");
    *u64_ivar(o, "WeaponMesh3P") = u64_comp(o, "WeaponMesh3P");
    *u64_ivar(o, "Abilities") = u64_array_const(16, 4, (u64_v[]){u64_int(0), u64_cls("GA_RocketLauncherPrimary"), u64_cls("GA_RocketLauncherPrimaryInstant"), u64_cls("GA_RocketLauncherSecondary"), u64_cls("GA_RocketLauncherReload")} + 1);
    *u64_ivar(o, "DefaultFireMode") = u64_name("Weapon.FireMode.None");
    *u64_ivar(o, "Equip1PMontage") = u64_cls("HeroFPP_LauncherEquip_Montage");
    *u64_ivar(o, "Equip3PMontage") = u64_cls("HeroTPP_Equip");
    *u64_ivar(o, "PickupSound") = u64_cls("AssaultRifle_AmmoPickup_Cue");
    *u64_ivar(o, "RootComponent") = u64_comp(o, "CollisionComponent");
    { u64_obj *c = u64_asobj(u64_comp(o, "WeaponMesh1P"));
      *u64_ivar(c, "SkeletalMesh") = u64_cls("Launcher");
    }
    { u64_obj *c = u64_asobj(u64_comp(o, "WeaponMesh3P"));
      *u64_ivar(c, "SkeletalMesh") = u64_cls("Launcher");
    }
  }
  if (!strcmp(cls, "BP_RocketLauncherProjectile")) {
    *u64_ivar(o, "ExplosionRadius") = u64_float(200.0f);
    *u64_ivar(o, "ProjectileMovement") = u64_comp(o, "ProjectileMovement");
    *u64_ivar(o, "InitialLifeSpan") = u64_float(5.0f);
    { u64_obj *c = u64_asobj(u64_comp(o, "ProjectileMovement"));
      *u64_ivar(c, "InitialSpeed") = u64_float(3000.0f);
    }
  }
  if (!strcmp(cls, "BP_RocketLauncher_FireCameraShake")) {
    *u64_ivar(o, "OscillationDuration") = u64_float(0.6000000238418579f);
  }
  if (!strcmp(cls, "BP_Shotgun")) {
    *u64_ivar(o, "WeaponTag") = u64_name("Weapon.Equipped.Shotgun");
    *u64_ivar(o, "PrimaryAmmoType") = u64_name("Weapon.Ammo.Shotgun");
    *u64_ivar(o, "PrimaryClipAmmo") = u64_int(6);
    *u64_ivar(o, "MaxPrimaryClipAmmo") = u64_int(6);
    *u64_ivar(o, "PrimaryHUDReticleClass") = u64_cls("HUDReticle_Spread");
    *u64_ivar(o, "CollisionComp") = u64_comp(o, "CollisionComponent");
    *u64_ivar(o, "WeaponMesh1P") = u64_comp(o, "WeaponMesh1P");
    *u64_ivar(o, "WeaponMesh3P") = u64_comp(o, "WeaponMesh3P");
    *u64_ivar(o, "WeaponMesh3PickupRelativeLocation") = u64_vec(0.0f, -15.0f, 0.0f);
    *u64_ivar(o, "WeaponMesh1PEquippedRelativeLocation") = u64_vec(0.0f, 0.0f, 12.0f);
    *u64_ivar(o, "WeaponMesh3PEquippedRelativeLocation") = u64_vec(0.0f, 0.0f, 12.0f);
    *u64_ivar(o, "Abilities") = u64_array_const(17, 5, (u64_v[]){u64_int(0), u64_cls("GA_ShotgunPrimary"), u64_cls("GA_ShotgunPrimaryInstant"), u64_cls("GA_ShotgunAlternate"), u64_cls("GA_ShotgunSecondary"), u64_cls("GA_ShotgunReload")} + 1);
    *u64_ivar(o, "DefaultFireMode") = u64_name("Weapon.Shotgun.FireMode.SemiAuto");
    *u64_ivar(o, "Equip1PMontage") = u64_cls("HeroFPP_RifleEquip_Montage");
    *u64_ivar(o, "Equip3PMontage") = u64_cls("HeroTPP_Equip");
    *u64_ivar(o, "PickupSound") = u64_cls("AssaultRifle_AmmoPickup_Cue");
    *u64_ivar(o, "RootComponent") = u64_comp(o, "CollisionComponent");
    { u64_obj *c = u64_asobj(u64_comp(o, "WeaponMesh1P"));
      *u64_ivar(c, "SkeletalMesh") = u64_cls("SK_FPGun");
    }
    { u64_obj *c = u64_asobj(u64_comp(o, "WeaponMesh3P"));
      *u64_ivar(c, "SkeletalMesh") = u64_cls("SK_FPGun");
      *u64_ivar(c, "RelativeLocation") = u64_vec(0.0f, -15.0f, 0.0f);
    }
  }
  if (!strcmp(cls, "BP_SingleTargetReticle")) {
    *u64_ivar(o, "bSnapToTargetedActor") = u64_bool(1);
  }
  if (!strcmp(cls, "BP_SpectatorPawn")) {
    *u64_ivar(o, "MovementComponent") = u64_comp(o, "MovementComponent0");
    *u64_ivar(o, "CollisionComponent") = u64_comp(o, "CollisionComponent0");
    *u64_ivar(o, "RootComponent") = u64_comp(o, "CollisionComponent0");
    { u64_obj *c = u64_asobj(u64_comp(o, "CollisionComponent0"));
      *u64_ivar(c, "AreaClass") = u64_cls("NavArea_Obstacle");
    }
  }
  if (!strcmp(cls, "GA_InteractActive")) {
    *u64_ivar(o, "MaxDistanceToTarget") = u64_float(350.0f);
    *u64_ivar(o, "AbilityInputID") = u64_name("EGSAbilityInputID::Interact");
    *u64_ivar(o, "AbilityID") = u64_name("EGSAbilityInputID::Interact");
    *u64_ivar(o, "bActivateOnInput") = u64_bool(0);
    *u64_ivar(o, "bCannotActivateWhileInteracting") = u64_bool(0);
    *u64_ivar(o, "AbilityTags") = u64_array_const(18, 1, (u64_v[]){u64_int(0), u64_name("Ability.Interaction")} + 1);
    *u64_ivar(o, "bServerRespectsRemoteAbilityCancellation") = u64_bool(0);
    *u64_ivar(o, "NetSecurityPolicy") = u64_name("EGameplayAbilityNetSecurityPolicy::ServerOnlyTermination");
    *u64_ivar(o, "AbilityTriggers") = u64_array_const(20, 1, (u64_v[]){u64_int(0), u64_array_const(19, 2, (u64_v[]){u64_int(0), u64_name("Ability.Interaction"), u64_name("EGameplayAbilityTriggerSource::GameplayEvent")} + 1)} + 1);
    *u64_ivar(o, "ActivationOwnedTags") = u64_array_const(21, 0, (u64_v[]){u64_int(0)} + 1);
    *u64_ivar(o, "ActivationBlockedTags") = u64_array_const(22, 3, (u64_v[]){u64_int(0), u64_name("State.Dead"), u64_name("State.KnockedDown"), u64_name("Ability.BlocksInteraction")} + 1);
  }
  if (!strcmp(cls, "GA_InteractPassive")) {
    *u64_ivar(o, "ActiveBlockedTag") = u64_name("Ability.BlocksInteraction");
    *u64_ivar(o, "MaxTraceRange") = u64_float(999999.0f);
    *u64_ivar(o, "AbilityInputID") = u64_name("EGSAbilityInputID::Interact");
    *u64_ivar(o, "AbilityID") = u64_name("EGSAbilityInputID::Interact");
    *u64_ivar(o, "bActivateAbilityOnGranted") = u64_bool(1);
    *u64_ivar(o, "bCannotActivateWhileInteracting") = u64_bool(0);
    *u64_ivar(o, "NetExecutionPolicy") = u64_name("EGameplayAbilityNetExecutionPolicy::LocalOnly");
    *u64_ivar(o, "ActivationOwnedTags") = u64_array_const(23, 0, (u64_v[]){u64_int(0)} + 1);
  }
  if (!strcmp(cls, "GA_NextWeapon")) {
    *u64_ivar(o, "AbilityInputID") = u64_name("EGSAbilityInputID::NextWeapon");
    *u64_ivar(o, "AbilityID") = u64_name("EGSAbilityInputID::NextWeapon");
    *u64_ivar(o, "AbilityTags") = u64_array_const(24, 1, (u64_v[]){u64_int(0), u64_name("Ability.Weapon.IsChanging")} + 1);
    *u64_ivar(o, "bServerRespectsRemoteAbilityCancellation") = u64_bool(0);
    *u64_ivar(o, "BlockAbilitiesWithTag") = u64_array_const(25, 1, (u64_v[]){u64_int(0), u64_name("Ability.Weapon")} + 1);
    *u64_ivar(o, "ActivationBlockedTags") = u64_array_const(26, 3, (u64_v[]){u64_int(0), u64_name("Ability.Weapon.IsChanging"), u64_name("State.Dead"), u64_name("State.KnockedDown")} + 1);
  }
  if (!strcmp(cls, "GA_PreviousWeapon")) {
    *u64_ivar(o, "AbilityInputID") = u64_name("EGSAbilityInputID::PrevWeapon");
    *u64_ivar(o, "AbilityID") = u64_name("EGSAbilityInputID::PrevWeapon");
    *u64_ivar(o, "AbilityTags") = u64_array_const(27, 1, (u64_v[]){u64_int(0), u64_name("Ability.Weapon.IsChanging")} + 1);
    *u64_ivar(o, "bServerRespectsRemoteAbilityCancellation") = u64_bool(0);
    *u64_ivar(o, "BlockAbilitiesWithTag") = u64_array_const(28, 1, (u64_v[]){u64_int(0), u64_name("Ability.Weapon")} + 1);
    *u64_ivar(o, "ActivationBlockedTags") = u64_array_const(29, 3, (u64_v[]){u64_int(0), u64_name("Ability.Weapon.IsChanging"), u64_name("State.Dead"), u64_name("State.KnockedDown")} + 1);
  }
  if (!strcmp(cls, "GA_Revive")) {
    *u64_ivar(o, "Montage") = u64_cls("Revive_Montage");
    *u64_ivar(o, "bCannotActivateWhileInteracting") = u64_bool(0);
    *u64_ivar(o, "AbilityTags") = u64_array_const(30, 1, (u64_v[]){u64_int(0), u64_name("Ability.Revive")} + 1);
    *u64_ivar(o, "NetExecutionPolicy") = u64_name("EGameplayAbilityNetExecutionPolicy::ServerInitiated");
    *u64_ivar(o, "NetSecurityPolicy") = u64_name("EGameplayAbilityNetSecurityPolicy::ServerOnly");
    *u64_ivar(o, "ActivationRequiredTags") = u64_array_const(31, 1, (u64_v[]){u64_int(0), u64_name("State.KnockedDown")} + 1);
    *u64_ivar(o, "ActivationBlockedTags") = u64_array_const(32, 1, (u64_v[]){u64_int(0), u64_name("State.Dead")} + 1);
  }
  if (!strcmp(cls, "GA_RifleAlternate")) {
    *u64_ivar(o, "AbilityInputID") = u64_name("EGSAbilityInputID::AlternateFire");
    *u64_ivar(o, "AbilityID") = u64_name("EGSAbilityInputID::AlternateFire");
    *u64_ivar(o, "bSourceObjectMustEqualCurrentWeaponToActivate") = u64_bool(1);
    *u64_ivar(o, "AbilityTags") = u64_array_const(33, 1, (u64_v[]){u64_int(0), u64_name("Ability.Weapon.Alternate")} + 1);
    *u64_ivar(o, "NetExecutionPolicy") = u64_name("EGameplayAbilityNetExecutionPolicy::LocalOnly");
    *u64_ivar(o, "ActivationBlockedTags") = u64_array_const(34, 3, (u64_v[]){u64_int(0), u64_name("Ability.Weapon.IsChanging"), u64_name("State.Dead"), u64_name("State.KnockedDown")} + 1);
  }
  if (!strcmp(cls, "GA_RiflePrimary")) {
    *u64_ivar(o, "TimeBetweenShots") = u64_float(0.10000000149011612f);
    *u64_ivar(o, "AmmoCost") = u64_int(1);
    *u64_ivar(o, "AbilityInputID") = u64_name("EGSAbilityInputID::PrimaryFire");
    *u64_ivar(o, "AbilityID") = u64_name("EGSAbilityInputID::PrimaryFire");
    *u64_ivar(o, "bSourceObjectMustEqualCurrentWeaponToActivate") = u64_bool(1);
    *u64_ivar(o, "AbilityTags") = u64_array_const(35, 1, (u64_v[]){u64_int(0), u64_name("Ability.Weapon.Primary")} + 1);
    *u64_ivar(o, "NetExecutionPolicy") = u64_name("EGameplayAbilityNetExecutionPolicy::LocalOnly");
    *u64_ivar(o, "CancelAbilitiesWithTag") = u64_array_const(36, 1, (u64_v[]){u64_int(0), u64_name("Ability.Weapon.Reload")} + 1);
    *u64_ivar(o, "BlockAbilitiesWithTag") = u64_array_const(37, 2, (u64_v[]){u64_int(0), u64_name("Ability.Sprint"), u64_name("Ability.Weapon.IsChanging")} + 1);
    *u64_ivar(o, "ActivationOwnedTags") = u64_array_const(38, 2, (u64_v[]){u64_int(0), u64_name("Ability.CancelsSprint"), u64_name("Ability.BlocksInteraction")} + 1);
    *u64_ivar(o, "ActivationBlockedTags") = u64_array_const(39, 3, (u64_v[]){u64_int(0), u64_name("Ability.Weapon.IsChanging"), u64_name("State.Dead"), u64_name("State.KnockedDown")} + 1);
  }
  if (!strcmp(cls, "GA_RiflePrimaryInstant")) {
    *u64_ivar(o, "AmmoCost") = u64_int(1);
    *u64_ivar(o, "AimingSpreadMod") = u64_float(0.15000000596046448f);
    *u64_ivar(o, "BulletDamage") = u64_float(10.0f);
    *u64_ivar(o, "FiringSpreadIncrement") = u64_float(1.2000000476837158f);
    *u64_ivar(o, "FiringSpreadMax") = u64_float(10.0f);
    *u64_ivar(o, "TimeOfLastShot") = u64_float(-9999999.0f);
    *u64_ivar(o, "WeaponSpread") = u64_float(1.0f);
    *u64_ivar(o, "TraceFromPlayerViewPoint") = u64_bool(1);
    *u64_ivar(o, "AimingTag") = u64_name("Weapon.Rifle.Aiming");
    *u64_ivar(o, "AimingRemovalTag") = u64_name("Weapon.Rifle.AimingRemoval");
    *u64_ivar(o, "AbilityID") = u64_name("EGSAbilityInputID::PrimaryFire");
    *u64_ivar(o, "bSourceObjectMustEqualCurrentWeaponToActivate") = u64_bool(1);
    *u64_ivar(o, "EffectContainerMap") = u64_array_const(42, 2, (u64_v[]){u64_int(0), u64_name("Ability"), u64_array_const(41, 4, (u64_v[]){u64_int(0), u64_name("TargetType"), u64_cls("None"), u64_name("TargetGameplayEffectClasses"), u64_array_const(40, 1, (u64_v[]){u64_int(0), u64_cls("GE_RifleDamage")} + 1)} + 1)} + 1);
    *u64_ivar(o, "AbilityTags") = u64_array_const(43, 1, (u64_v[]){u64_int(0), u64_name("Ability.Weapon.Primary.Instant")} + 1);
    *u64_ivar(o, "ActivationOwnedTags") = u64_array_const(44, 2, (u64_v[]){u64_int(0), u64_name("Weapon.IsFiring"), u64_name("Ability.BlocksInteraction")} + 1);
    *u64_ivar(o, "ActivationBlockedTags") = u64_array_const(45, 3, (u64_v[]){u64_int(0), u64_name("Ability.Weapon.IsChanging"), u64_name("State.Dead"), u64_name("State.KnockedDown")} + 1);
  }
  if (!strcmp(cls, "GA_RifleReload")) {
    *u64_ivar(o, "AbilityInputID") = u64_name("EGSAbilityInputID::Reload");
    *u64_ivar(o, "AbilityID") = u64_name("EGSAbilityInputID::Reload");
    *u64_ivar(o, "bSourceObjectMustEqualCurrentWeaponToActivate") = u64_bool(1);
    *u64_ivar(o, "EffectContainerMap") = u64_array_const(48, 2, (u64_v[]){u64_int(0), u64_name("Ability"), u64_array_const(47, 4, (u64_v[]){u64_int(0), u64_name("TargetType"), u64_cls("GSTargetType_UseOwner"), u64_name("TargetGameplayEffectClasses"), u64_array_const(46, 1, (u64_v[]){u64_int(0), u64_cls("GE_RifleReload")} + 1)} + 1)} + 1);
    *u64_ivar(o, "AbilityTags") = u64_array_const(49, 1, (u64_v[]){u64_int(0), u64_name("Ability.Weapon.Reload")} + 1);
    *u64_ivar(o, "CancelAbilitiesWithTag") = u64_array_const(50, 1, (u64_v[]){u64_int(0), u64_name("Ability.Weapon.Primary")} + 1);
    *u64_ivar(o, "BlockAbilitiesWithTag") = u64_array_const(51, 1, (u64_v[]){u64_int(0), u64_name("Ability.Weapon.IsChanging")} + 1);
    *u64_ivar(o, "ActivationOwnedTags") = u64_array_const(52, 2, (u64_v[]){u64_int(0), u64_name("Ability.Weapon.Reload"), u64_name("Ability.BlocksInteraction")} + 1);
    *u64_ivar(o, "ActivationBlockedTags") = u64_array_const(53, 3, (u64_v[]){u64_int(0), u64_name("Ability.Weapon.IsChanging"), u64_name("State.Dead"), u64_name("State.KnockedDown")} + 1);
  }
  if (!strcmp(cls, "GA_RifleSecondary")) {
    *u64_ivar(o, "Targeting1PFOV") = u64_float(60.0f);
    *u64_ivar(o, "GE_Aiming_Class") = u64_cls("GE_RifleAiming");
    *u64_ivar(o, "GE_AimingRemoval_Class") = u64_cls("GE_RifleAimingRemoval");
    *u64_ivar(o, "AbilityInputID") = u64_name("EGSAbilityInputID::SecondaryFire");
    *u64_ivar(o, "AbilityID") = u64_name("EGSAbilityInputID::SecondaryFire");
    *u64_ivar(o, "bSourceObjectMustEqualCurrentWeaponToActivate") = u64_bool(1);
    *u64_ivar(o, "AbilityTags") = u64_array_const(54, 1, (u64_v[]){u64_int(0), u64_name("Ability.Weapon.Secondary")} + 1);
    *u64_ivar(o, "BlockAbilitiesWithTag") = u64_array_const(55, 2, (u64_v[]){u64_int(0), u64_name("Ability.Sprint"), u64_name("Ability.Weapon.IsChanging")} + 1);
    *u64_ivar(o, "ActivationOwnedTags") = u64_array_const(56, 2, (u64_v[]){u64_int(0), u64_name("Ability.CancelsSprint"), u64_name("Ability.BlocksInteraction")} + 1);
    *u64_ivar(o, "ActivationBlockedTags") = u64_array_const(57, 3, (u64_v[]){u64_int(0), u64_name("Ability.Weapon.IsChanging"), u64_name("State.KnockedDown"), u64_name("State.Dead")} + 1);
  }
  if (!strcmp(cls, "GA_RocketLauncherPrimary")) {
    *u64_ivar(o, "TimeBetweenShots") = u64_float(0.4000000059604645f);
    *u64_ivar(o, "AmmoCost") = u64_int(1);
    *u64_ivar(o, "AbilityInputID") = u64_name("EGSAbilityInputID::PrimaryFire");
    *u64_ivar(o, "AbilityID") = u64_name("EGSAbilityInputID::PrimaryFire");
    *u64_ivar(o, "bSourceObjectMustEqualCurrentWeaponToActivate") = u64_bool(1);
    *u64_ivar(o, "AbilityTags") = u64_array_const(58, 1, (u64_v[]){u64_int(0), u64_name("Ability.Weapon.Primary")} + 1);
    *u64_ivar(o, "NetExecutionPolicy") = u64_name("EGameplayAbilityNetExecutionPolicy::LocalOnly");
    *u64_ivar(o, "CancelAbilitiesWithTag") = u64_array_const(59, 1, (u64_v[]){u64_int(0), u64_name("Ability.Weapon.Reload")} + 1);
    *u64_ivar(o, "BlockAbilitiesWithTag") = u64_array_const(60, 2, (u64_v[]){u64_int(0), u64_name("Ability.Sprint"), u64_name("Ability.Weapon.IsChanging")} + 1);
    *u64_ivar(o, "ActivationOwnedTags") = u64_array_const(61, 2, (u64_v[]){u64_int(0), u64_name("Ability.CancelsSprint"), u64_name("Ability.BlocksInteraction")} + 1);
    *u64_ivar(o, "ActivationBlockedTags") = u64_array_const(62, 3, (u64_v[]){u64_int(0), u64_name("Ability.Weapon.IsChanging"), u64_name("State.Dead"), u64_name("State.KnockedDown")} + 1);
  }
  if (!strcmp(cls, "GA_RocketLauncherPrimaryInstant")) {
    *u64_ivar(o, "RocketDamage") = u64_float(60.0f);
    *u64_ivar(o, "AmmoCost") = u64_int(1);
    *u64_ivar(o, "AbilityID") = u64_name("EGSAbilityInputID::PrimaryFire");
    *u64_ivar(o, "bSourceObjectMustEqualCurrentWeaponToActivate") = u64_bool(1);
    *u64_ivar(o, "EffectContainerMap") = u64_array_const(65, 2, (u64_v[]){u64_int(0), u64_name("Ability"), u64_array_const(64, 4, (u64_v[]){u64_int(0), u64_name("TargetType"), u64_cls("None"), u64_name("TargetGameplayEffectClasses"), u64_array_const(63, 1, (u64_v[]){u64_int(0), u64_cls("GE_RocketLauncherDamage")} + 1)} + 1)} + 1);
    *u64_ivar(o, "ActivationBlockedTags") = u64_array_const(66, 3, (u64_v[]){u64_int(0), u64_name("Ability.Weapon.IsChanging"), u64_name("State.Dead"), u64_name("State.KnockedDown")} + 1);
  }
  if (!strcmp(cls, "GA_RocketLauncherReload")) {
    *u64_ivar(o, "AbilityInputID") = u64_name("EGSAbilityInputID::Reload");
    *u64_ivar(o, "AbilityID") = u64_name("EGSAbilityInputID::Reload");
    *u64_ivar(o, "bSourceObjectMustEqualCurrentWeaponToActivate") = u64_bool(1);
    *u64_ivar(o, "EffectContainerMap") = u64_array_const(69, 2, (u64_v[]){u64_int(0), u64_name("Ability"), u64_array_const(68, 4, (u64_v[]){u64_int(0), u64_name("TargetType"), u64_cls("GSTargetType_UseOwner"), u64_name("TargetGameplayEffectClasses"), u64_array_const(67, 1, (u64_v[]){u64_int(0), u64_cls("GE_RocketLauncherReload")} + 1)} + 1)} + 1);
    *u64_ivar(o, "AbilityTags") = u64_array_const(70, 1, (u64_v[]){u64_int(0), u64_name("Ability.Weapon.Reload")} + 1);
    *u64_ivar(o, "CancelAbilitiesWithTag") = u64_array_const(71, 1, (u64_v[]){u64_int(0), u64_name("Ability.Weapon.Primary")} + 1);
    *u64_ivar(o, "BlockAbilitiesWithTag") = u64_array_const(72, 1, (u64_v[]){u64_int(0), u64_name("Ability.Weapon.IsChanging")} + 1);
    *u64_ivar(o, "ActivationOwnedTags") = u64_array_const(73, 2, (u64_v[]){u64_int(0), u64_name("Ability.Weapon.Reload"), u64_name("Ability.BlocksInteraction")} + 1);
    *u64_ivar(o, "ActivationBlockedTags") = u64_array_const(74, 3, (u64_v[]){u64_int(0), u64_name("Ability.Weapon.IsChanging"), u64_name("State.Dead"), u64_name("State.KnockedDown")} + 1);
  }
  if (!strcmp(cls, "GA_RocketLauncherSecondary")) {
    *u64_ivar(o, "Targeting1PFOV") = u64_float(60.0f);
    *u64_ivar(o, "GE_Aiming_Class") = u64_cls("GE_RocketLauncherAiming");
    *u64_ivar(o, "GE_AimingRemoval_Class") = u64_cls("GE_RocketLauncherAimingRemoval");
    *u64_ivar(o, "AimingHUDReticle") = u64_cls("HUDReticle_Spread");
    *u64_ivar(o, "MaxRange") = u64_float(3000.0f);
    *u64_ivar(o, "RocketDamage") = u64_float(60.0f);
    *u64_ivar(o, "TimeToStartTargetingAgain") = u64_float(0.4000000059604645f);
    *u64_ivar(o, "TimeBetweenFastRockets") = u64_float(0.20000000298023224f);
    *u64_ivar(o, "TimeBetweenShots") = u64_float(0.4000000059604645f);
    *u64_ivar(o, "AmmoCost") = u64_int(1);
    *u64_ivar(o, "MaxTargets") = u64_int(3);
    *u64_ivar(o, "AbilityInputID") = u64_name("EGSAbilityInputID::SecondaryFire");
    *u64_ivar(o, "AbilityID") = u64_name("EGSAbilityInputID::SecondaryFire");
    *u64_ivar(o, "bSourceObjectMustEqualCurrentWeaponToActivate") = u64_bool(1);
    *u64_ivar(o, "EffectContainerMap") = u64_array_const(77, 2, (u64_v[]){u64_int(0), u64_name("Ability"), u64_array_const(76, 4, (u64_v[]){u64_int(0), u64_name("TargetType"), u64_cls("None"), u64_name("TargetGameplayEffectClasses"), u64_array_const(75, 1, (u64_v[]){u64_int(0), u64_cls("GE_RocketLauncherDamage")} + 1)} + 1)} + 1);
    *u64_ivar(o, "AbilityTags") = u64_array_const(78, 1, (u64_v[]){u64_int(0), u64_name("Ability.Weapon.Secondary")} + 1);
    *u64_ivar(o, "bServerRespectsRemoteAbilityCancellation") = u64_bool(0);
    *u64_ivar(o, "CancelAbilitiesWithTag") = u64_array_const(79, 1, (u64_v[]){u64_int(0), u64_name("Ability.Weapon.Reload")} + 1);
    *u64_ivar(o, "BlockAbilitiesWithTag") = u64_array_const(80, 4, (u64_v[]){u64_int(0), u64_name("Ability.Sprint"), u64_name("Ability.Weapon.Primary"), u64_name("Ability.Weapon.IsChanging"), u64_name("Ability.Weapon.Reload")} + 1);
    *u64_ivar(o, "ActivationOwnedTags") = u64_array_const(81, 2, (u64_v[]){u64_int(0), u64_name("Ability.CancelsSprint"), u64_name("Ability.BlocksInteraction")} + 1);
    *u64_ivar(o, "ActivationBlockedTags") = u64_array_const(82, 3, (u64_v[]){u64_int(0), u64_name("Ability.Weapon.IsChanging"), u64_name("State.Dead"), u64_name("State.KnockedDown")} + 1);
  }
  if (!strcmp(cls, "GA_ShotgunAlternate")) {
    *u64_ivar(o, "AbilityInputID") = u64_name("EGSAbilityInputID::AlternateFire");
    *u64_ivar(o, "AbilityID") = u64_name("EGSAbilityInputID::AlternateFire");
    *u64_ivar(o, "bSourceObjectMustEqualCurrentWeaponToActivate") = u64_bool(1);
    *u64_ivar(o, "AbilityTags") = u64_array_const(83, 1, (u64_v[]){u64_int(0), u64_name("Ability.Weapon.Alternate")} + 1);
    *u64_ivar(o, "NetExecutionPolicy") = u64_name("EGameplayAbilityNetExecutionPolicy::LocalOnly");
    *u64_ivar(o, "ActivationBlockedTags") = u64_array_const(84, 3, (u64_v[]){u64_int(0), u64_name("Ability.Weapon.IsChanging"), u64_name("State.Dead"), u64_name("State.KnockedDown")} + 1);
  }
  if (!strcmp(cls, "GA_ShotgunPrimary")) {
    *u64_ivar(o, "TimeBetweenShots") = u64_float(0.4000000059604645f);
    *u64_ivar(o, "AmmoCost") = u64_int(1);
    *u64_ivar(o, "AbilityInputID") = u64_name("EGSAbilityInputID::PrimaryFire");
    *u64_ivar(o, "AbilityID") = u64_name("EGSAbilityInputID::PrimaryFire");
    *u64_ivar(o, "bSourceObjectMustEqualCurrentWeaponToActivate") = u64_bool(1);
    *u64_ivar(o, "AbilityTags") = u64_array_const(85, 1, (u64_v[]){u64_int(0), u64_name("Ability.Weapon.Primary")} + 1);
    *u64_ivar(o, "NetExecutionPolicy") = u64_name("EGameplayAbilityNetExecutionPolicy::LocalOnly");
    *u64_ivar(o, "CancelAbilitiesWithTag") = u64_array_const(86, 1, (u64_v[]){u64_int(0), u64_name("Ability.Weapon.Reload")} + 1);
    *u64_ivar(o, "BlockAbilitiesWithTag") = u64_array_const(87, 2, (u64_v[]){u64_int(0), u64_name("Ability.Sprint"), u64_name("Ability.Weapon.IsChanging")} + 1);
    *u64_ivar(o, "ActivationOwnedTags") = u64_array_const(88, 2, (u64_v[]){u64_int(0), u64_name("Ability.CancelsSprint"), u64_name("Ability.BlocksInteraction")} + 1);
    *u64_ivar(o, "ActivationBlockedTags") = u64_array_const(89, 3, (u64_v[]){u64_int(0), u64_name("Ability.Weapon.IsChanging"), u64_name("State.Dead"), u64_name("State.KnockedDown")} + 1);
  }
  if (!strcmp(cls, "GA_ShotgunPrimaryInstant")) {
    *u64_ivar(o, "AmmoCost") = u64_int(1);
    *u64_ivar(o, "AimingSpreadMod") = u64_float(0.30000001192092896f);
    *u64_ivar(o, "PelletDamage") = u64_float(6.0f);
    *u64_ivar(o, "TimeOfLastShot") = u64_float(-9999999.0f);
    *u64_ivar(o, "WeaponSpread") = u64_float(12.0f);
    *u64_ivar(o, "TraceFromPlayerViewPoint") = u64_bool(1);
    *u64_ivar(o, "NumberOfPellets") = u64_int(8);
    *u64_ivar(o, "AimingTag") = u64_name("Weapon.Shotgun.Aiming");
    *u64_ivar(o, "AimingRemovalTag") = u64_name("Weapon.Shotgun.AimingRemoval");
    *u64_ivar(o, "AbilityID") = u64_name("EGSAbilityInputID::PrimaryFire");
    *u64_ivar(o, "bSourceObjectMustEqualCurrentWeaponToActivate") = u64_bool(1);
    *u64_ivar(o, "EffectContainerMap") = u64_array_const(92, 2, (u64_v[]){u64_int(0), u64_name("Ability"), u64_array_const(91, 4, (u64_v[]){u64_int(0), u64_name("TargetType"), u64_cls("None"), u64_name("TargetGameplayEffectClasses"), u64_array_const(90, 1, (u64_v[]){u64_int(0), u64_cls("GE_RifleDamage")} + 1)} + 1)} + 1);
    *u64_ivar(o, "AbilityTags") = u64_array_const(93, 1, (u64_v[]){u64_int(0), u64_name("Ability.Weapon.Primary.Instant")} + 1);
    *u64_ivar(o, "ActivationOwnedTags") = u64_array_const(94, 2, (u64_v[]){u64_int(0), u64_name("Weapon.IsFiring"), u64_name("Ability.BlocksInteraction")} + 1);
    *u64_ivar(o, "ActivationBlockedTags") = u64_array_const(95, 3, (u64_v[]){u64_int(0), u64_name("Ability.Weapon.IsChanging"), u64_name("State.Dead"), u64_name("State.KnockedDown")} + 1);
  }
  if (!strcmp(cls, "GA_ShotgunReload")) {
    *u64_ivar(o, "AbilityInputID") = u64_name("EGSAbilityInputID::Reload");
    *u64_ivar(o, "AbilityID") = u64_name("EGSAbilityInputID::Reload");
    *u64_ivar(o, "bSourceObjectMustEqualCurrentWeaponToActivate") = u64_bool(1);
    *u64_ivar(o, "EffectContainerMap") = u64_array_const(98, 2, (u64_v[]){u64_int(0), u64_name("Ability"), u64_array_const(97, 4, (u64_v[]){u64_int(0), u64_name("TargetType"), u64_cls("GSTargetType_UseOwner"), u64_name("TargetGameplayEffectClasses"), u64_array_const(96, 1, (u64_v[]){u64_int(0), u64_cls("GE_ShotgunReload")} + 1)} + 1)} + 1);
    *u64_ivar(o, "AbilityTags") = u64_array_const(99, 1, (u64_v[]){u64_int(0), u64_name("Ability.Weapon.Reload")} + 1);
    *u64_ivar(o, "CancelAbilitiesWithTag") = u64_array_const(100, 1, (u64_v[]){u64_int(0), u64_name("Ability.Weapon.Primary")} + 1);
    *u64_ivar(o, "BlockAbilitiesWithTag") = u64_array_const(101, 1, (u64_v[]){u64_int(0), u64_name("Ability.Weapon.IsChanging")} + 1);
    *u64_ivar(o, "ActivationOwnedTags") = u64_array_const(102, 2, (u64_v[]){u64_int(0), u64_name("Ability.Weapon.Reload"), u64_name("Ability.BlocksInteraction")} + 1);
    *u64_ivar(o, "ActivationBlockedTags") = u64_array_const(103, 3, (u64_v[]){u64_int(0), u64_name("Ability.Weapon.IsChanging"), u64_name("State.Dead"), u64_name("State.KnockedDown")} + 1);
  }
  if (!strcmp(cls, "GA_ShotgunSecondary")) {
    *u64_ivar(o, "Targeting1PFOV") = u64_float(60.0f);
    *u64_ivar(o, "GE_Aiming_Class") = u64_cls("GE_ShotgunAiming");
    *u64_ivar(o, "GE_AimingRemoval_Class") = u64_cls("GE_ShotgunAimingRemoval");
    *u64_ivar(o, "AbilityInputID") = u64_name("EGSAbilityInputID::SecondaryFire");
    *u64_ivar(o, "AbilityID") = u64_name("EGSAbilityInputID::SecondaryFire");
    *u64_ivar(o, "bSourceObjectMustEqualCurrentWeaponToActivate") = u64_bool(1);
    *u64_ivar(o, "AbilityTags") = u64_array_const(104, 1, (u64_v[]){u64_int(0), u64_name("Ability.Weapon.Secondary")} + 1);
    *u64_ivar(o, "BlockAbilitiesWithTag") = u64_array_const(105, 2, (u64_v[]){u64_int(0), u64_name("Ability.Sprint"), u64_name("Ability.Weapon.IsChanging")} + 1);
    *u64_ivar(o, "ActivationOwnedTags") = u64_array_const(106, 2, (u64_v[]){u64_int(0), u64_name("Ability.CancelsSprint"), u64_name("Ability.BlocksInteraction")} + 1);
    *u64_ivar(o, "ActivationBlockedTags") = u64_array_const(107, 3, (u64_v[]){u64_int(0), u64_name("Ability.Weapon.IsChanging"), u64_name("State.Dead"), u64_name("State.KnockedDown")} + 1);
  }
  if (!strcmp(cls, "GA_Sprint")) {
    *u64_ivar(o, "AbilityInputID") = u64_name("EGSAbilityInputID::Sprint");
    *u64_ivar(o, "AbilityID") = u64_name("EGSAbilityInputID::Sprint");
    *u64_ivar(o, "AbilityTags") = u64_array_const(108, 1, (u64_v[]){u64_int(0), u64_name("Ability.Sprint")} + 1);
    *u64_ivar(o, "CostGameplayEffectClass") = u64_cls("GE_SprintCost");
    *u64_ivar(o, "ActivationOwnedTags") = u64_array_const(109, 0, (u64_v[]){u64_int(0)} + 1);
  }
  if (!strcmp(cls, "GC_KnockedDown")) {
    *u64_ivar(o, "KnockedDownSound") = u64_cls("Stinger_PlayerDeath_Stereo_Cue");
    *u64_ivar(o, "GameplayCueTag") = u64_name("GameplayCue.Hero.KnockedDown");
    *u64_ivar(o, "GameplayCueName") = u64_name("GameplayCue.Hero.KnockedDown");
  }
  if (!strcmp(cls, "GC_Revived")) {
    *u64_ivar(o, "RevivedSound") = u64_cls("StingerPickup_Health_Cue");
    *u64_ivar(o, "GameplayCueTag") = u64_name("GameplayCue.Hero.Revived");
    *u64_ivar(o, "GameplayCueName") = u64_name("GameplayCue.Hero.Revived");
  }
  if (!strcmp(cls, "GC_RifleFire")) {
    *u64_ivar(o, "GameplayCueTag") = u64_name("GameplayCue.Weapon.Rifle.Fire");
    *u64_ivar(o, "GameplayCueName") = u64_name("GameplayCue.Weapon.Rifle.Fire");
  }
  if (!strcmp(cls, "GC_RocketLauncherFire")) {
    *u64_ivar(o, "GameplayCueTag") = u64_name("GameplayCue.Weapon.RocketLauncher.Fire");
    *u64_ivar(o, "GameplayCueName") = u64_name("GameplayCue.Weapon.RocketLauncher.Fire");
  }
  if (!strcmp(cls, "GC_RocketLauncherImpact")) {
    *u64_ivar(o, "GameplayCueTag") = u64_name("GameplayCue.Weapon.RocketLauncher.Impact");
    *u64_ivar(o, "GameplayCueName") = u64_name("GameplayCue.Weapon.RocketLauncher.Impact");
  }
  if (!strcmp(cls, "GC_ShotgunFire")) {
    *u64_ivar(o, "GameplayCueTag") = u64_name("GameplayCue.Weapon.Shotgun.Fire");
    *u64_ivar(o, "GameplayCueName") = u64_name("GameplayCue.Weapon.Shotgun.Fire");
  }
  if (!strcmp(cls, "GC_Sprinting")) {
    *u64_ivar(o, "RunSound") = u64_cls("Run_Loop_Cue");
    *u64_ivar(o, "RunStopSound") = u64_cls("RunStop_Cue");
    *u64_ivar(o, "bAutoDestroyOnRemove") = u64_bool(1);
    *u64_ivar(o, "GameplayCueTag") = u64_name("GameplayCue.Ability.Sprinting");
    *u64_ivar(o, "GameplayCueName") = u64_name("GameplayCue.Ability.Sprinting");
  }
  if (!strcmp(cls, "GE_BleedingOut")) {
    *u64_ivar(o, "DurationPolicy") = u64_name("EGameplayEffectDurationType::Infinite");
    *u64_ivar(o, "PeriodicInhibitionPolicy") = u64_name("EGameplayEffectPeriodInhibitionRemovedPolicy::ResetPeriod");
  }
  if (!strcmp(cls, "GE_ChestOpen")) {
    *u64_ivar(o, "DurationPolicy") = u64_name("EGameplayEffectDurationType::HasDuration");
  }
  if (!strcmp(cls, "GE_Dead")) {
    *u64_ivar(o, "DurationPolicy") = u64_name("EGameplayEffectDurationType::Infinite");
  }
  if (!strcmp(cls, "GE_DoorOpen")) {
    *u64_ivar(o, "DurationPolicy") = u64_name("EGameplayEffectDurationType::HasDuration");
  }
  if (!strcmp(cls, "GE_HealthManaStaminaShieldRegenVolume")) {
    *u64_ivar(o, "DurationPolicy") = u64_name("EGameplayEffectDurationType::Infinite");
  }
  if (!strcmp(cls, "GE_HealthRegen")) {
    *u64_ivar(o, "DurationPolicy") = u64_name("EGameplayEffectDurationType::Infinite");
  }
  if (!strcmp(cls, "GE_Interacting")) {
    *u64_ivar(o, "DurationPolicy") = u64_name("EGameplayEffectDurationType::Infinite");
  }
  if (!strcmp(cls, "GE_InteractingRemoval")) {
    *u64_ivar(o, "DurationPolicy") = u64_name("EGameplayEffectDurationType::Infinite");
  }
  if (!strcmp(cls, "GE_KnockedDown")) {
    *u64_ivar(o, "DurationPolicy") = u64_name("EGameplayEffectDurationType::Infinite");
  }
  if (!strcmp(cls, "GE_ManaRegen")) {
    *u64_ivar(o, "DurationPolicy") = u64_name("EGameplayEffectDurationType::Infinite");
  }
  if (!strcmp(cls, "GE_Reviving")) {
    *u64_ivar(o, "DurationPolicy") = u64_name("EGameplayEffectDurationType::Infinite");
  }
  if (!strcmp(cls, "GE_RifleAiming")) {
    *u64_ivar(o, "DurationPolicy") = u64_name("EGameplayEffectDurationType::Infinite");
  }
  if (!strcmp(cls, "GE_RifleAimingRemoval")) {
    *u64_ivar(o, "DurationPolicy") = u64_name("EGameplayEffectDurationType::Infinite");
  }
  if (!strcmp(cls, "GE_RocketLauncherAiming")) {
    *u64_ivar(o, "DurationPolicy") = u64_name("EGameplayEffectDurationType::Infinite");
  }
  if (!strcmp(cls, "GE_RocketLauncherAimingRemoval")) {
    *u64_ivar(o, "DurationPolicy") = u64_name("EGameplayEffectDurationType::Infinite");
  }
  if (!strcmp(cls, "GE_ShieldRegen")) {
    *u64_ivar(o, "DurationPolicy") = u64_name("EGameplayEffectDurationType::Infinite");
  }
  if (!strcmp(cls, "GE_ShotgunAiming")) {
    *u64_ivar(o, "DurationPolicy") = u64_name("EGameplayEffectDurationType::Infinite");
  }
  if (!strcmp(cls, "GE_ShotgunAimingRemoval")) {
    *u64_ivar(o, "DurationPolicy") = u64_name("EGameplayEffectDurationType::Infinite");
  }
  if (!strcmp(cls, "GE_Sprinting")) {
    *u64_ivar(o, "DurationPolicy") = u64_name("EGameplayEffectDurationType::Infinite");
  }
  if (!strcmp(cls, "GE_SprintingRemoval")) {
    *u64_ivar(o, "DurationPolicy") = u64_name("EGameplayEffectDurationType::Infinite");
  }
  if (!strcmp(cls, "GE_StaminaRegen")) {
    *u64_ivar(o, "DurationPolicy") = u64_name("EGameplayEffectDurationType::Infinite");
  }
  if (!strcmp(cls, "GE_WeaponIsChangingDelayReplicationTag")) {
    *u64_ivar(o, "DurationPolicy") = u64_name("EGameplayEffectDurationType::HasDuration");
    *u64_ivar(o, "StackingType") = u64_name("EGameplayEffectStackingType::AggregateBySource");
    *u64_ivar(o, "StackLimitCount") = u64_int(1);
  }
  if (!strcmp(cls, "HUDReticle_Dot")) {
    *u64_ivar(o, "bHasScriptImplementedTick") = u64_bool(0);
    *u64_ivar(o, "bHasScriptImplementedPaint") = u64_bool(0);
  }
  if (!strcmp(cls, "HUDReticle_Spread")) {
    *u64_ivar(o, "bHasScriptImplementedTick") = u64_bool(0);
    *u64_ivar(o, "bHasScriptImplementedPaint") = u64_bool(0);
  }
  if (!strcmp(cls, "UI_DamageNumber")) {
    *u64_ivar(o, "HeadShotFontSize") = u64_int(56);
    *u64_ivar(o, "NormalFontSize") = u64_int(42);
    *u64_ivar(o, "DesignSizeMode") = u64_name("EDesignPreviewSizeMode::Custom");
    *u64_ivar(o, "bHasScriptImplementedTick") = u64_bool(0);
    *u64_ivar(o, "bHasScriptImplementedPaint") = u64_bool(0);
  }
  if (!strcmp(cls, "UI_FloatingStatusBar_Hero")) {
    *u64_ivar(o, "bHasScriptImplementedTick") = u64_bool(0);
    *u64_ivar(o, "bHasScriptImplementedPaint") = u64_bool(0);
  }
  if (!strcmp(cls, "UI_HUD")) {
    *u64_ivar(o, "InteractionTick") = u64_float(0.05000000074505806f);
    *u64_ivar(o, "bHasScriptImplementedTick") = u64_bool(0);
    *u64_ivar(o, "bHasScriptImplementedPaint") = u64_bool(0);
  }
  if (!strcmp(cls, "UI_TargetReticle")) {
    *u64_ivar(o, "bHasScriptImplementedTick") = u64_bool(0);
    *u64_ivar(o, "bHasScriptImplementedPaint") = u64_bool(0);
  }
  if (!strcmp(cls, "WC_DamageText")) {
    *u64_ivar(o, "Space") = u64_name("EWidgetSpace::Screen");
    *u64_ivar(o, "WidgetClass") = u64_cls("UI_DamageNumber");
  }
}
