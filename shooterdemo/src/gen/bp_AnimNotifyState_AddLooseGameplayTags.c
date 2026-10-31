#include "u64_rt.h"

void bp_AnimNotifyState_AddLooseGameplayTags__Received_NotifyBegin(u64_obj *self, const u64_v *args, int argc) {
  (void)args; (void)argc;
  struct { u64_v Animation; u64_v CallFunc_GetAbilitySystemComponent_ReturnValue; u64_v CallFunc_GetOwner_ReturnValue; u64_v K2Node_DynamicCast_AsGSAbility_System_Component; u64_v K2Node_DynamicCast_AsGSCharacter_Base; u64_v K2Node_DynamicCast_bSuccess; u64_v K2Node_DynamicCast_bSuccess_1; u64_v MeshComp; u64_v ReturnValue; u64_v TotalDuration; u64_v _unsupported; } L; memset(&L, 0, sizeof L); (void)L;
  U64_FLOW_DECL; (void)flow_n;
  if (argc > 0) L.MeshComp = args[0];
  if (argc > 1) L.TotalDuration = args[1];
  if (argc > 2) L.Animation = args[2];
  L_0:; 
  L_1:; 
  L_2:; L.CallFunc_GetOwner_ReturnValue = ({ u64_v _o = L.MeshComp; U64_CALL(u64_asobj(_o), "GetOwner"); });
  L_44:; 
  L_45:; L.K2Node_DynamicCast_AsGSCharacter_Base = u64_cast(L.CallFunc_GetOwner_ReturnValue, "GSCharacterBase");
  L_81:; L.K2Node_DynamicCast_bSuccess = L.K2Node_DynamicCast_AsGSCharacter_Base;
  L_110:; 
  L_111:; if (!u64_true(L.K2Node_DynamicCast_bSuccess)) goto L_298;
  L_125:; 
  L_126:; L.CallFunc_GetAbilitySystemComponent_ReturnValue = U64_CALL(self, "GetAbilitySystemComponent", L.K2Node_DynamicCast_AsGSCharacter_Base);
  L_155:; 
  L_156:; L.K2Node_DynamicCast_AsGSAbility_System_Component = u64_cast(L.CallFunc_GetAbilitySystemComponent_ReturnValue, "GSAbilitySystemComponent");
  L_192:; L.K2Node_DynamicCast_bSuccess_1 = L.K2Node_DynamicCast_AsGSAbility_System_Component;
  L_221:; 
  L_222:; if (!u64_true(L.K2Node_DynamicCast_bSuccess_1)) goto L_298;
  L_236:; 
  L_237:; 
  L_238:; ({ u64_v _o = L.K2Node_DynamicCast_AsGSAbility_System_Component; U64_CALL(u64_asobj(_o), "K2_AddLooseGameplayTags", (*u64_ivar(self, "GameplayTags")), u64_int(1)); });
  L_284:; 
  L_285:; 
  L_286:; L.ReturnValue = u64_bool(1);
  L_297:; 
  L_298:; L.ReturnValue = L.ReturnValue; goto u64_ret;
  L_308:; 
  u64_ret:; u64_set_return(L.ReturnValue);

}

void bp_AnimNotifyState_AddLooseGameplayTags__Received_NotifyEnd(u64_obj *self, const u64_v *args, int argc) {
  (void)args; (void)argc;
  struct { u64_v Animation; u64_v CallFunc_GetAbilitySystemComponent_ReturnValue; u64_v CallFunc_GetOwner_ReturnValue; u64_v K2Node_DynamicCast_AsGSAbility_System_Component; u64_v K2Node_DynamicCast_AsGSCharacter_Base; u64_v K2Node_DynamicCast_bSuccess; u64_v K2Node_DynamicCast_bSuccess_1; u64_v MeshComp; u64_v ReturnValue; u64_v _unsupported; } L; memset(&L, 0, sizeof L); (void)L;
  U64_FLOW_DECL; (void)flow_n;
  if (argc > 0) L.MeshComp = args[0];
  if (argc > 1) L.Animation = args[1];
  L_0:; 
  L_1:; 
  L_2:; L.CallFunc_GetOwner_ReturnValue = ({ u64_v _o = L.MeshComp; U64_CALL(u64_asobj(_o), "GetOwner"); });
  L_44:; 
  L_45:; L.K2Node_DynamicCast_AsGSCharacter_Base = u64_cast(L.CallFunc_GetOwner_ReturnValue, "GSCharacterBase");
  L_81:; L.K2Node_DynamicCast_bSuccess = L.K2Node_DynamicCast_AsGSCharacter_Base;
  L_110:; 
  L_111:; if (!u64_true(L.K2Node_DynamicCast_bSuccess)) goto L_298;
  L_125:; 
  L_126:; L.CallFunc_GetAbilitySystemComponent_ReturnValue = U64_CALL(self, "GetAbilitySystemComponent", L.K2Node_DynamicCast_AsGSCharacter_Base);
  L_155:; 
  L_156:; L.K2Node_DynamicCast_AsGSAbility_System_Component = u64_cast(L.CallFunc_GetAbilitySystemComponent_ReturnValue, "GSAbilitySystemComponent");
  L_192:; L.K2Node_DynamicCast_bSuccess_1 = L.K2Node_DynamicCast_AsGSAbility_System_Component;
  L_221:; 
  L_222:; if (!u64_true(L.K2Node_DynamicCast_bSuccess_1)) goto L_298;
  L_236:; 
  L_237:; 
  L_238:; ({ u64_v _o = L.K2Node_DynamicCast_AsGSAbility_System_Component; U64_CALL(u64_asobj(_o), "K2_RemoveLooseGameplayTags", (*u64_ivar(self, "GameplayTags")), u64_int(1)); });
  L_284:; 
  L_285:; 
  L_286:; L.ReturnValue = u64_bool(1);
  L_297:; 
  L_298:; L.ReturnValue = L.ReturnValue; goto u64_ret;
  L_308:; 
  u64_ret:; u64_set_return(L.ReturnValue);

}
