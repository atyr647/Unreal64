// Bridge to the project's own UGSAttributeSetBase (GASShooter, unmodified): the damage / shield / health / bounty rules and the max-change rules.
// Same scheme as bridge_ammo.cpp. The characters and controllers the rules walk (AGSCharacterBase, AController, AGSPlayerController) are runtime-backed
// proxies: IsAlive reads the runtime's Health; a "controller" is one per actor (the rules only compare them); the HUD calls are accepted and ignored.
#include "Characters/Abilities/AttributeSets/GSAttributeSetBase.h"
#include "Characters/GSCharacterBase.h"
#include "Player/GSPlayerController.h"
#include "Characters/Abilities/GSGA_CharacterJump.h"
extern "C" {
#include "u64_gas_core.h"
}


// ---- members of the project's character / controller classes this build does not compile (GSCharacterBase.cpp, GSPlayerController.cpp): runtime-backed
AGSCharacterBase::AGSCharacterBase(const FObjectInitializer &) {}
UAbilitySystemComponent *AGSCharacterBase::GetAbilitySystemComponent() const { return nullptr; }
bool AGSCharacterBase::IsAlive() const { return u64_obj_alive((u64_obj *)rt) && u64_attr_get(u64_asc_of((u64_obj *)rt), "Health") > 0; }
int32 AGSCharacterBase::GetAbilityLevel(EGSAbilityInputID) const { return 1; }
void AGSCharacterBase::RemoveCharacterAbilities() {}
void AGSCharacterBase::Die() {}
void AGSCharacterBase::FinishDying() {}
void AGSCharacterBase::AddDamageNumber(float, FGameplayTagContainer) {}
void AGSCharacterBase::BeginPlay() {}
void AGSCharacterBase::AddCharacterAbilities() {}
void AGSCharacterBase::InitializeAttributes() {}
void AGSCharacterBase::AddStartupEffects() {}
void AGSCharacterBase::ShowDamageNumber() {}
void AGSCharacterBase::SetHealth(float) {}
void AGSCharacterBase::SetMana(float) {}
void AGSCharacterBase::SetStamina(float) {}
void AGSCharacterBase::SetShield(float) {}
void AGSPlayerController::OnPossess(APawn *) {}
void AGSPlayerController::OnRep_PlayerState() {}
void AGSPlayerController::ShowDamageNumber(float, AGSCharacterBase *, FGameplayTagContainer) {}   // the Client RPC: HUD, ignored
void AGSPlayerController::ShowDamageNumber_Implementation(float, AGSCharacterBase *, FGameplayTagContainer) {}   // HUD: ignored

namespace {
struct Proxy;
struct RtASC : UAbilitySystemComponent {
  u64_obj *asc = nullptr; UGSAttributeSetBase *set = nullptr; FGameplayAttributeData *find(FName n) { return set->FindAttributeData(n); }
  void SetNumericAttributeBase(const FGameplayAttribute &a, float v) override
  {
    u64_attr_set_base(asc, a.Property->Name.ToString(), v);
    if (FGameplayAttributeData *d = find(a.Property->Name)) { d->SetBaseValue(v); d->SetCurrentValue(v); }
  }
  void ApplyModToAttributeUnsafe(const FGameplayAttribute &a, EGameplayModOp op, float m) override
  {
    if (op != EGameplayModOp::Additive) return;
    const char *n = a.Property->Name.ToString(); u64_attr_add_base_unsafe(asc, n, m);
    if (FGameplayAttributeData *d = find(a.Property->Name)) { d->SetBaseValue(d->BaseValue + m); d->SetCurrentValue(u64_attr_get(asc, n)); }
  }
  FGameplayEffectContextHandle MakeEffectContext() override { return FGameplayEffectContextHandle(); }
  void ApplyGameplayEffectToSelf(const UGameplayEffect *ge, float, FGameplayEffectContextHandle) override;
};
struct Actor { Actor() : ch(FObjectInitializer()) {} u64_obj *obj; AGSCharacterBase ch; AController ctl; AGSPlayerController pc; bool player; };
struct Slot { u64_obj *asc; UGSAttributeSetBase set; RtASC rt; FGameplayAbilityActorInfo info; };
Slot slots[16]; int nslots; Actor actors[16]; int nactors;
Actor *actor_for(u64_obj *o)
{
  for (int i = 0; i < nactors; i++) if (actors[i].obj == o) return &actors[i];
  if (nactors >= 16) return nullptr;
  Actor *a = &actors[nactors++]; a->obj = o; a->ch.rt = o; a->player = o == u64_player_pawn();
  a->ctl.Pawn = &a->ch; a->pc.Pawn = &a->ch; a->ch.Controller = a->player ? (AController *)&a->pc : &a->ctl;
  return a;
}
Slot *slot_for(u64_obj *asc)            // slots and proxies are cached by runtime object; the runtime reuses object slots, so the ownership is re-read on every call
{
  Slot *s = nullptr;
  for (int i = 0; i < nslots; i++) if (slots[i].asc == asc) s = &slots[i];
  if (!s) { if (nslots >= 16) return nullptr; s = &slots[nslots++]; s->asc = asc; s->rt.asc = asc; s->rt.set = &s->set; s->set.Owner = &s->rt; }
  Actor *a = actor_for(u64_asc_owner(asc));
  if (a) {
    a->player = a->obj == u64_player_pawn(); a->ch.Controller = a->player ? (AController *)&a->pc : &a->ctl;
    s->info.AvatarActor.p = &a->ch; s->info.PlayerController.p = a->player ? (AController *)&a->pc : nullptr;
  }
  s->rt.AbilityActorInfo.p = &s->info;
  return s;
}
void sync_in(Slot *s) { for (const char *const *n = UGSAttributeSetBase::AttributeNames(); *n; n++) { FGameplayAttributeData *d = s->set.FindAttributeData(FName(*n)); float v = u64_attr_get(s->asc, *n); d->SetBaseValue(v); d->SetCurrentValue(v); } }
bool mine(const char *name) { for (const char *const *n = UGSAttributeSetBase::AttributeNames(); *n; n++) if (!strcmp(*n, name)) return true; return false; }

void RtASC::ApplyGameplayEffectToSelf(const UGameplayEffect *ge, float level, FGameplayEffectContextHandle)   // the bounty effect: instant, additive modifiers
{
  static u64_modinfo mods[2][8]; static u64_ge_def defs[2]; static const char *const none[] = {0}; static int n;
  int k = n++ & 1, nm = ge->Modifiers.Num() < 8 ? ge->Modifiers.Num() : 8;                       // two alternating slots: the effect is applied immediately
  for (int i = 0; i < nm; i++) { const FGameplayModifierInfo &m = ge->Modifiers[i]; mods[k][i] = (u64_modinfo){m.Attribute.Property->Name.ToString(), U64_OP_ADD, U64_CALC_SCALABLE, m.ModifierMagnitude.Value, "", 0, 1.0f, 0.0f, 0.0f, ""}; }
  defs[k] = (u64_ge_def){.name = k ? "CxxBounty1" : "CxxBounty0", .dur = U64_DUR_INSTANT, .nmods = nm, .mods = mods[k], .execs = none, .granted = none,
                         .asset_tags = none, .remove_tags = none, .app_require = none, .app_ignore = none, .ongoing_require = none, .ongoing_ignore = none};
  u64_ge_register(&defs[k]);
  u64_apply_ge(asc, asc, defs[k].name, level, 0);
}
}

extern "C" int u64_cxx_base_calls;
int u64_cxx_base_calls;
// UGSAttributeSetBase::PostGameplayEffectExecute for attribute `name` of `tgt`, caused by the ability system `src` (may be null). 0 if it is not one of its attributes.
extern "C" int u64_cxx_base_post_execute(u64_obj *tgt, const char *name, u64_obj *src)
{
  Slot *s = mine(name) ? slot_for(tgt) : nullptr; if (!s) return 0;
  Slot *so = src ? slot_for(src) : nullptr;
  sync_in(s); if (so) sync_in(so);
  FGameplayEffectModCallbackData data(s->rt);
  data.EffectSpec.Context.d = nullptr;
  static FGameplayEffectContext ctx; ctx.Instigator = so ? &so->rt : nullptr; ctx.EffectCauser = so ? so->info.AvatarActor.p : nullptr;
  if (so) data.EffectSpec.Context.d = &ctx;
  data.EvaluatedData.Attribute = FGameplayAttribute(FindFieldChecked<FProperty>(nullptr, FName(name)));
  s->set.PostGameplayEffectExecute(data);
  u64_cxx_base_calls++;
  return 1;
}
// PreAttributeChange (max-change adjustment, MoveSpeed clamp)
extern "C" int u64_cxx_base_pre_change(u64_obj *asc, const char *name, float *nv)
{
  Slot *s = mine(name) ? slot_for(asc) : nullptr; if (!s) return 0;
  sync_in(s);
  s->set.PreAttributeChange(FGameplayAttribute(FindFieldChecked<FProperty>(nullptr, FName(name))), *nv);
  return 1;
}

// ---- UGSGA_CharacterJump (non-instanced: one object serves every activation; the runtime ability object is passed through `rt`)
namespace {
UGSGA_CharacterJump jump;
bool commit_hook(void *ab) { return u64_ability_commit((u64_obj *)ab, 1, 1); }
void end_hook(void *ab, bool cancelled) { u64_ability_end((u64_obj *)ab, cancelled); }
void char_jump_hook(void *actor, int start) { u64_call((u64_obj *)actor, start ? "Jump" : "StopJumping", 0, 0); }
bool char_can_jump_hook(void *actor) { return u64_true(u64_call((u64_obj *)actor, "CanJump", 0, 0)); }
Slot *jump_slot(u64_obj *ab)
{
  AbilityCommitHook = commit_hook; AbilityEndHook = end_hook; CharJumpHook = char_jump_hook; CharCanJumpHook = char_can_jump_hook;
  u64_obj *asc = (u64_obj *)u64_ivar(ab, "ASC")->u.o; Slot *s = slot_for(asc);
  jump.rt = ab; jump.CurrentActorInfo = s ? &s->info : nullptr;
  return s;
}
}
extern "C" int u64_cxx_jump_calls;
int u64_cxx_jump_calls;
extern "C" void u64_cxx_jump_activate(u64_obj *ab)
{
  u64_cxx_jump_calls++;
  Slot *s = jump_slot(ab); if (!s) return;
  FGameplayAbilitySpecHandle h; FGameplayAbilityActivationInfo ai;
  jump.ActivateAbility(h, &s->info, ai, nullptr);
}
extern "C" int u64_cxx_jump_can_activate(u64_obj *ab)
{
  Slot *s = jump_slot(ab); if (!s) return 0;
  s->info.AvatarActor.p->rt = u64_asc_owner((u64_obj *)u64_ivar(ab, "ASC")->u.o);
  FGameplayAbilitySpecHandle h; return jump.CanActivateAbility(h, &s->info);
}
extern "C" void u64_cxx_jump_released(u64_obj *ab)
{
  Slot *s = jump_slot(ab); if (!s) return;
  FGameplayAbilitySpecHandle h; FGameplayAbilityActivationInfo ai;
  jump.InputReleased(h, &s->info, ai);
}

// ---- UGSGameplayAbility (GSGameplayAbility.cpp is not compiled in this build: it needs prediction keys / replicated target data / effect spec creation).
// The parts the jump ability reaches through its base class are runtime glue: the runtime has already run the GS activation rules (interacting tag,
// weapon match, cost, cooldown) before the C++ ability is asked, so the base checks answer "yes" here.
UGSGameplayAbility::UGSGameplayAbility() {}
void UGSGameplayAbility::OnAvatarSet(const FGameplayAbilityActorInfo *, const FGameplayAbilitySpec &) {}
bool UGSGameplayAbility::CanActivateAbility(const FGameplayAbilitySpecHandle, const FGameplayAbilityActorInfo *, const FGameplayTagContainer *, const FGameplayTagContainer *, FGameplayTagContainer *) const { return true; }
bool UGSGameplayAbility::CheckCost(const FGameplayAbilitySpecHandle, const FGameplayAbilityActorInfo *, FGameplayTagContainer *) const { return true; }
void UGSGameplayAbility::ApplyCost(const FGameplayAbilitySpecHandle, const FGameplayAbilityActorInfo *, const FGameplayAbilityActivationInfo) const {}
FGSGameplayEffectContainerSpec UGSGameplayAbility::MakeEffectContainerSpecFromContainer(const FGSGameplayEffectContainer &, const FGameplayEventData &, int32) { return FGSGameplayEffectContainerSpec(); }
FGSGameplayEffectContainerSpec UGSGameplayAbility::MakeEffectContainerSpec(FGameplayTag, const FGameplayEventData &, int32) { return FGSGameplayEffectContainerSpec(); }
TArray<FActiveGameplayEffectHandle> UGSGameplayAbility::ApplyEffectContainerSpec(const FGSGameplayEffectContainerSpec &) { return TArray<FActiveGameplayEffectHandle>(); }
bool UGSGameplayAbility::BatchRPCTryActivateAbility(FGameplayAbilitySpecHandle, bool) { return false; }
void UGSGameplayAbility::ExternalEndAbility() {}
FString UGSGameplayAbility::GetCurrentPredictionKeyStatus() { return FString(); }
bool UGSGameplayAbility::IsPredictionKeyValidForMorePrediction() const { return false; }
bool UGSGameplayAbility::GSCheckCost_Implementation(const FGameplayAbilitySpecHandle, const FGameplayAbilityActorInfo &) const { return true; }
void UGSGameplayAbility::SetHUDReticle(TSubclassOf<UGSHUDReticle>) {}
void UGSGameplayAbility::ResetHUDReticle() {}
void UGSGameplayAbility::SendTargetDataToServer(const FGameplayAbilityTargetDataHandle &) {}
bool UGSGameplayAbility::IsInputPressed() const { return false; }
void UGSGameplayAbility::SetCurrentMontageForMesh(USkeletalMeshComponent *, UAnimMontage *) {}
