#include "u64_rt.h"

void bp_BP_HealthManaStaminaShieldRegenVolume__ExecuteUbergraph_BP_HealthManaStaminaShieldRegenVolume(u64_obj *self, const u64_v *args, int argc) {
  (void)args; (void)argc;
  struct { u64_v ReturnValue; u64_v _unsupported; } L; memset(&L, 0, sizeof L); (void)L;
  U64_FLOW_DECL; (void)flow_n;
  if (argc > 0) (*u64_pframe(self, "EntryPoint")) = args[0];
  L_0:; u64_dispatch: switch (u64_as_int((*u64_pframe(self, "EntryPoint")))) { case 0: goto L_0; case 10: goto L_10; case 11: goto L_11; case 12: goto L_12; case 13: goto L_13; case 49: goto L_49; case 78: goto L_78; case 79: goto L_79; case 93: goto L_93; case 94: goto L_94; case 123: goto L_123; case 152: goto L_152; case 198: goto L_198; case 236: goto L_236; case 237: goto L_237; case 238: goto L_238; case 252: goto L_252; case 253: goto L_253; case 282: goto L_282; case 336: goto L_336; case 337: goto L_337; case 410: goto L_410; case 411: goto L_411; case 416: goto L_416; case 417: goto L_417; case 418: goto L_418; case 419: goto L_419; case 455: goto L_455; case 484: goto L_484; case 485: goto L_485; case 499: goto L_499; case 500: goto L_500; case 529: goto L_529; case 558: goto L_558; case 604: goto L_604; case 642: goto L_642; case 643: goto L_643; case 644: goto L_644; case 658: goto L_658; case 659: goto L_659; case 688: goto L_688; case 689: goto L_689; case 740: goto L_740; case 741: goto L_741; case 743: goto L_743; }
  L_10:; 
  L_11:; 
  L_12:; 
  L_13:; (*u64_pframe(self, "K2Node_DynamicCast_AsGSCharacter_Base")) = u64_cast((*u64_pframe(self, "K2Node_ComponentBoundEvent_OtherActor_1")), "GSCharacterBase");
  L_49:; (*u64_pframe(self, "K2Node_DynamicCast_bSuccess")) = (*u64_pframe(self, "K2Node_DynamicCast_AsGSCharacter_Base"));
  L_78:; 
  L_79:; if (!u64_true((*u64_pframe(self, "K2Node_DynamicCast_bSuccess")))) goto L_741;
  L_93:; 
  L_94:; (*u64_pframe(self, "CallFunc_GetAbilitySystemComponent_ReturnValue")) = U64_CALL(self, "GetAbilitySystemComponent", (*u64_pframe(self, "K2Node_DynamicCast_AsGSCharacter_Base")));
  L_123:; (*u64_pframe(self, "CallFunc_IsValid_ReturnValue")) = U64_CALL(self, "IsValid", (*u64_pframe(self, "CallFunc_GetAbilitySystemComponent_ReturnValue")));
  L_152:; (*u64_pframe(self, "CallFunc_IsAlive_ReturnValue_1")) = ({ u64_v _o = (*u64_pframe(self, "K2Node_DynamicCast_AsGSCharacter_Base")); U64_CALL(u64_asobj(_o), "IsAlive"); });
  L_198:; (*u64_pframe(self, "CallFunc_BooleanAND_ReturnValue_1")) = U64_CALL(self, "BooleanAND", (*u64_pframe(self, "CallFunc_IsAlive_ReturnValue_1")), (*u64_pframe(self, "CallFunc_IsValid_ReturnValue")));
  L_236:; 
  L_237:; 
  L_238:; if (!u64_true((*u64_pframe(self, "CallFunc_BooleanAND_ReturnValue_1")))) goto L_741;
  L_252:; 
  L_253:; (*u64_pframe(self, "CallFunc_GetAbilitySystemComponent_ReturnValue")) = U64_CALL(self, "GetAbilitySystemComponent", (*u64_pframe(self, "K2Node_DynamicCast_AsGSCharacter_Base")));
  L_282:; (*u64_pframe(self, "CallFunc_MakeEffectContext_ReturnValue")) = ({ u64_v _o = (*u64_pframe(self, "CallFunc_GetAbilitySystemComponent_ReturnValue")); U64_CALL(u64_asobj(_o), "MakeEffectContext"); });
  L_336:; 
  L_337:; (*u64_pframe(self, "CallFunc_BP_ApplyGameplayEffectToSelf_ReturnValue")) = ({ u64_v _o = (*u64_pframe(self, "CallFunc_GetAbilitySystemComponent_ReturnValue")); U64_CALL(u64_asobj(_o), "BP_ApplyGameplayEffectToSelf", (*u64_ivar(self, "BuffGameplayEffectClass")), u64_float(1.0f), (*u64_pframe(self, "CallFunc_MakeEffectContext_ReturnValue"))); });
  L_410:; 
  L_411:; goto L_741;
  L_416:; 
  L_417:; 
  L_418:; 
  L_419:; (*u64_pframe(self, "K2Node_DynamicCast_AsGSCharacter_Base_1")) = u64_cast((*u64_pframe(self, "K2Node_ComponentBoundEvent_OtherActor")), "GSCharacterBase");
  L_455:; (*u64_pframe(self, "K2Node_DynamicCast_bSuccess_1")) = (*u64_pframe(self, "K2Node_DynamicCast_AsGSCharacter_Base_1"));
  L_484:; 
  L_485:; if (!u64_true((*u64_pframe(self, "K2Node_DynamicCast_bSuccess_1")))) goto L_741;
  L_499:; 
  L_500:; (*u64_pframe(self, "CallFunc_GetAbilitySystemComponent_ReturnValue_1")) = U64_CALL(self, "GetAbilitySystemComponent", (*u64_pframe(self, "K2Node_DynamicCast_AsGSCharacter_Base_1")));
  L_529:; (*u64_pframe(self, "CallFunc_IsValid_ReturnValue_1")) = U64_CALL(self, "IsValid", (*u64_pframe(self, "CallFunc_GetAbilitySystemComponent_ReturnValue_1")));
  L_558:; (*u64_pframe(self, "CallFunc_IsAlive_ReturnValue")) = ({ u64_v _o = (*u64_pframe(self, "K2Node_DynamicCast_AsGSCharacter_Base_1")); U64_CALL(u64_asobj(_o), "IsAlive"); });
  L_604:; (*u64_pframe(self, "CallFunc_BooleanAND_ReturnValue")) = U64_CALL(self, "BooleanAND", (*u64_pframe(self, "CallFunc_IsAlive_ReturnValue")), (*u64_pframe(self, "CallFunc_IsValid_ReturnValue_1")));
  L_642:; 
  L_643:; 
  L_644:; if (!u64_true((*u64_pframe(self, "CallFunc_BooleanAND_ReturnValue")))) goto L_741;
  L_658:; 
  L_659:; (*u64_pframe(self, "CallFunc_GetAbilitySystemComponent_ReturnValue_1")) = U64_CALL(self, "GetAbilitySystemComponent", (*u64_pframe(self, "K2Node_DynamicCast_AsGSCharacter_Base_1")));
  L_688:; 
  L_689:; ({ u64_v _o = (*u64_pframe(self, "CallFunc_GetAbilitySystemComponent_ReturnValue_1")); U64_CALL(u64_asobj(_o), "RemoveActiveGameplayEffectBySourceEffect", (*u64_ivar(self, "BuffGameplayEffectClass")), u64_objv(0), u64_int(-1)); });
  L_740:; 
  L_741:; goto u64_ret;
  L_743:; 
  u64_ret:; u64_set_return(L.ReturnValue);

}

void bp_BP_HealthManaStaminaShieldRegenVolume__BndEvt__Box_K2Node_ComponentBoundEvent_1_ComponentEndOverlapSignature__DelegateSignature(u64_obj *self, const u64_v *args, int argc) {
  (void)args; (void)argc;
  struct { u64_v OtherActor; u64_v OtherBodyIndex; u64_v OtherComp; u64_v OverlappedComponent; u64_v ReturnValue; u64_v _unsupported; } L; memset(&L, 0, sizeof L); (void)L;
  U64_FLOW_DECL; (void)flow_n;
  if (argc > 0) L.OverlappedComponent = args[0];
  if (argc > 1) L.OtherActor = args[1];
  if (argc > 2) L.OtherComp = args[2];
  if (argc > 3) L.OtherBodyIndex = args[3];
  L_0:; 
  L_1:; 
  L_2:; 
  L_3:; *u64_pframe(self, "K2Node_ComponentBoundEvent_OverlappedComponent") = L.OverlappedComponent;
  L_21:; *u64_pframe(self, "K2Node_ComponentBoundEvent_OtherActor") = L.OtherActor;
  L_39:; *u64_pframe(self, "K2Node_ComponentBoundEvent_OtherComp") = L.OtherComp;
  L_57:; *u64_pframe(self, "K2Node_ComponentBoundEvent_OtherBodyIndex") = L.OtherBodyIndex;
  L_75:; 
  L_76:; 
  L_77:; U64_CALL(self, "ExecuteUbergraph_BP_HealthManaStaminaShieldRegenVolume", u64_int(416));
  L_92:; 
  L_93:; goto u64_ret;
  L_95:; 
  u64_ret:; u64_set_return(L.ReturnValue);

}

void bp_BP_HealthManaStaminaShieldRegenVolume__BndEvt__Box_K2Node_ComponentBoundEvent_0_ComponentBeginOverlapSignature__DelegateSignature(u64_obj *self, const u64_v *args, int argc) {
  (void)args; (void)argc;
  struct { u64_v OtherActor; u64_v OtherBodyIndex; u64_v OtherComp; u64_v OverlappedComponent; u64_v ReturnValue; u64_v SweepResult; u64_v _unsupported; u64_v bFromSweep; } L; memset(&L, 0, sizeof L); (void)L;
  U64_FLOW_DECL; (void)flow_n;
  if (argc > 0) L.OverlappedComponent = args[0];
  if (argc > 1) L.OtherActor = args[1];
  if (argc > 2) L.OtherComp = args[2];
  if (argc > 3) L.OtherBodyIndex = args[3];
  if (argc > 4) L.bFromSweep = args[4];
  if (argc > 5) L.SweepResult = args[5].t == U64_REF ? *args[5].u.r : args[5];
  L_0:; 
  L_1:; 
  L_2:; 
  L_3:; *u64_pframe(self, "K2Node_ComponentBoundEvent_OverlappedComponent_1") = L.OverlappedComponent;
  L_21:; *u64_pframe(self, "K2Node_ComponentBoundEvent_OtherActor_1") = L.OtherActor;
  L_39:; *u64_pframe(self, "K2Node_ComponentBoundEvent_OtherComp_1") = L.OtherComp;
  L_57:; *u64_pframe(self, "K2Node_ComponentBoundEvent_OtherBodyIndex_1") = L.OtherBodyIndex;
  L_75:; *u64_pframe(self, "K2Node_ComponentBoundEvent_bFromSweep") = L.bFromSweep;
  L_93:; *u64_pframe(self, "K2Node_ComponentBoundEvent_SweepResult") = L.SweepResult;
  L_111:; 
  L_112:; 
  L_113:; U64_CALL(self, "ExecuteUbergraph_BP_HealthManaStaminaShieldRegenVolume", u64_int(10));
  L_128:; 
  L_129:; goto u64_ret;
  L_131:; 
  u64_ret:; u64_set_return(L.ReturnValue);
  if (argc > 5) u64_out(args[5], L.SweepResult);
}
