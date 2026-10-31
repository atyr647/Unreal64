#include "u64_rt.h"

void bp_BP_HealthManaStaminaShieldVolume__ExecuteUbergraph_BP_HealthManaStaminaShieldVolume(u64_obj *self, const u64_v *args, int argc) {
  (void)args; (void)argc;
  struct { u64_v ReturnValue; u64_v _unsupported; } L; memset(&L, 0, sizeof L); (void)L;
  U64_FLOW_DECL; (void)flow_n;
  if (argc > 0) (*u64_pframe(self, "EntryPoint")) = args[0];
  L_0:; U64_PUSH(&&L_1019);
  L_5:; u64_dispatch: switch (u64_as_int((*u64_pframe(self, "EntryPoint")))) { case 0: goto L_0; case 5: goto L_5; case 15: goto L_15; case 16: goto L_16; case 17: goto L_17; case 37: goto L_37; case 38: goto L_38; case 39: goto L_39; case 53: goto L_53; case 54: goto L_54; case 55: goto L_55; case 56: goto L_56; case 57: goto L_57; case 58: goto L_58; case 59: goto L_59; case 60: goto L_60; case 61: goto L_61; case 84: goto L_84; case 85: goto L_85; case 86: goto L_86; case 109: goto L_109; case 110: goto L_110; case 121: goto L_121; case 171: goto L_171; case 230: goto L_230; case 268: goto L_268; case 269: goto L_269; case 270: goto L_270; case 284: goto L_284; case 285: goto L_285; case 286: goto L_286; case 313: goto L_313; case 314: goto L_314; case 315: goto L_315; case 320: goto L_320; case 321: goto L_321; case 326: goto L_326; case 327: goto L_327; case 328: goto L_328; case 370: goto L_370; case 371: goto L_371; case 398: goto L_398; case 399: goto L_399; case 404: goto L_404; case 405: goto L_405; case 406: goto L_406; case 407: goto L_407; case 408: goto L_408; case 409: goto L_409; case 410: goto L_410; case 411: goto L_411; case 412: goto L_412; case 423: goto L_423; case 473: goto L_473; case 532: goto L_532; case 533: goto L_533; case 569: goto L_569; case 598: goto L_598; case 599: goto L_599; case 609: goto L_609; case 610: goto L_610; case 656: goto L_656; case 685: goto L_685; case 714: goto L_714; case 752: goto L_752; case 753: goto L_753; case 754: goto L_754; case 764: goto L_764; case 765: goto L_765; case 794: goto L_794; case 848: goto L_848; case 849: goto L_849; case 922: goto L_922; case 923: goto L_923; case 924: goto L_924; case 925: goto L_925; case 926: goto L_926; case 949: goto L_949; case 950: goto L_950; case 1003: goto L_1003; case 1004: goto L_1004; case 1005: goto L_1005; case 1006: goto L_1006; case 1007: goto L_1007; case 1012: goto L_1012; case 1013: goto L_1013; case 1014: goto L_1014; case 1019: goto L_1019; case 1021: goto L_1021; }
  L_15:; 
  L_16:; 
  L_17:; (*u64_pframe(self, "CallFunc_HasAuthority_ReturnValue")) = U64_CALL(self, "HasAuthority");
  L_37:; 
  L_38:; 
  L_39:; if (!u64_true((*u64_pframe(self, "CallFunc_HasAuthority_ReturnValue")))) goto L_404;
  L_53:; 
  L_54:; 
  L_55:; 
  L_56:; 
  L_57:; 
  L_58:; 
  L_59:; 
  L_60:; 
  L_61:; (*u64_pframe(self, "Temp_int_Loop_Counter_Variable")) = u64_int(0);
  L_84:; 
  L_85:; 
  L_86:; (*u64_pframe(self, "Temp_int_Array_Index_Variable")) = u64_int(0);
  L_109:; 
  L_110:; u64_array_set(&(*u64_pframe(self, "CallFunc_GetOverlappingActors_OverlappingActors")), 0, (u64_v[]){u64_int(0)} + 1);
  L_121:; ({ u64_v _o = (*u64_ivar(self, "Box")); U64_CALL(u64_asobj(_o), "GetOverlappingActors", u64_ref(&(*u64_pframe(self, "CallFunc_GetOverlappingActors_OverlappingActors"))), u64_objv(u64_find("GSCharacterBase"))); });
  L_171:; (*u64_pframe(self, "CallFunc_Array_Length_ReturnValue")) = ({ u64_v _o = u64_objv(u64_find("Default__KismetArrayLibrary")); U64_CALL(u64_asobj(_o), "Array_Length", u64_ref(&(*u64_pframe(self, "CallFunc_GetOverlappingActors_OverlappingActors")))); });
  L_230:; (*u64_pframe(self, "CallFunc_Less_IntInt_ReturnValue")) = U64_CALL(self, "Less_IntInt", (*u64_pframe(self, "Temp_int_Loop_Counter_Variable")), (*u64_pframe(self, "CallFunc_Array_Length_ReturnValue")));
  L_268:; 
  L_269:; 
  L_270:; if (!u64_true((*u64_pframe(self, "CallFunc_Less_IntInt_ReturnValue")))) goto L_407;
  L_284:; 
  L_285:; 
  L_286:; (*u64_pframe(self, "Temp_int_Array_Index_Variable")) = (*u64_pframe(self, "Temp_int_Loop_Counter_Variable"));
  L_313:; 
  L_314:; 
  L_315:; U64_PUSH(&&L_326);
  L_320:; 
  L_321:; goto L_410;
  L_326:; 
  L_327:; 
  L_328:; (*u64_pframe(self, "CallFunc_Add_IntInt_ReturnValue")) = U64_CALL(self, "Add_IntInt", (*u64_pframe(self, "Temp_int_Loop_Counter_Variable")), u64_int(1));
  L_370:; 
  L_371:; (*u64_pframe(self, "Temp_int_Loop_Counter_Variable")) = (*u64_pframe(self, "CallFunc_Add_IntInt_ReturnValue"));
  L_398:; 
  L_399:; goto L_110;
  L_404:; 
  L_405:; 
  L_406:; goto *U64_POP();
  L_407:; 
  L_408:; 
  L_409:; goto *U64_POP();
  L_410:; 
  L_411:; 
  L_412:; u64_array_set(&(*u64_pframe(self, "CallFunc_GetOverlappingActors_OverlappingActors")), 0, (u64_v[]){u64_int(0)} + 1);
  L_423:; ({ u64_v _o = (*u64_ivar(self, "Box")); U64_CALL(u64_asobj(_o), "GetOverlappingActors", u64_ref(&(*u64_pframe(self, "CallFunc_GetOverlappingActors_OverlappingActors"))), u64_objv(u64_find("GSCharacterBase"))); });
  L_473:; ({ u64_v _o = u64_objv(u64_find("Default__KismetArrayLibrary")); U64_CALL(u64_asobj(_o), "Array_Get", u64_ref(&(*u64_pframe(self, "CallFunc_GetOverlappingActors_OverlappingActors"))), (*u64_pframe(self, "Temp_int_Array_Index_Variable")), u64_ref(&(*u64_pframe(self, "CallFunc_Array_Get_Item")))); });
  L_532:; 
  L_533:; (*u64_pframe(self, "K2Node_DynamicCast_AsGSCharacter_Base")) = u64_cast((*u64_pframe(self, "CallFunc_Array_Get_Item")), "GSCharacterBase");
  L_569:; (*u64_pframe(self, "K2Node_DynamicCast_bSuccess")) = (*u64_pframe(self, "K2Node_DynamicCast_AsGSCharacter_Base"));
  L_598:; 
  L_599:; if (!u64_true((*u64_pframe(self, "K2Node_DynamicCast_bSuccess")))) goto *U64_POP();
  L_609:; 
  L_610:; (*u64_pframe(self, "CallFunc_IsAlive_ReturnValue")) = ({ u64_v _o = (*u64_pframe(self, "K2Node_DynamicCast_AsGSCharacter_Base")); U64_CALL(u64_asobj(_o), "IsAlive"); });
  L_656:; (*u64_pframe(self, "CallFunc_GetAbilitySystemComponent_ReturnValue")) = U64_CALL(self, "GetAbilitySystemComponent", (*u64_pframe(self, "K2Node_DynamicCast_AsGSCharacter_Base")));
  L_685:; (*u64_pframe(self, "CallFunc_IsValid_ReturnValue")) = U64_CALL(self, "IsValid", (*u64_pframe(self, "CallFunc_GetAbilitySystemComponent_ReturnValue")));
  L_714:; (*u64_pframe(self, "CallFunc_BooleanAND_ReturnValue")) = U64_CALL(self, "BooleanAND", (*u64_pframe(self, "CallFunc_IsAlive_ReturnValue")), (*u64_pframe(self, "CallFunc_IsValid_ReturnValue")));
  L_752:; 
  L_753:; 
  L_754:; if (!u64_true((*u64_pframe(self, "CallFunc_BooleanAND_ReturnValue")))) goto *U64_POP();
  L_764:; 
  L_765:; (*u64_pframe(self, "CallFunc_GetAbilitySystemComponent_ReturnValue")) = U64_CALL(self, "GetAbilitySystemComponent", (*u64_pframe(self, "K2Node_DynamicCast_AsGSCharacter_Base")));
  L_794:; (*u64_pframe(self, "CallFunc_MakeEffectContext_ReturnValue")) = ({ u64_v _o = (*u64_pframe(self, "CallFunc_GetAbilitySystemComponent_ReturnValue")); U64_CALL(u64_asobj(_o), "MakeEffectContext"); });
  L_848:; 
  L_849:; (*u64_pframe(self, "CallFunc_BP_ApplyGameplayEffectToSelf_ReturnValue")) = ({ u64_v _o = (*u64_pframe(self, "CallFunc_GetAbilitySystemComponent_ReturnValue")); U64_CALL(u64_asobj(_o), "BP_ApplyGameplayEffectToSelf", u64_objv(u64_find("GE_HealthManaStaminaShieldVolume_C")), u64_float(1.0f), (*u64_pframe(self, "CallFunc_MakeEffectContext_ReturnValue"))); });
  L_922:; 
  L_923:; goto *U64_POP();
  L_924:; 
  L_925:; 
  L_926:; (*u64_pframe(self, "K2Node_CreateDelegate_OutputDelegate")) = u64_delegate(u64_objv(self), "DamageTick");
  L_949:; 
  L_950:; (*u64_pframe(self, "CallFunc_K2_SetTimerDelegate_ReturnValue")) = U64_CALL(self, "K2_SetTimerDelegate", (*u64_pframe(self, "K2Node_CreateDelegate_OutputDelegate")), u64_float(1.0f), u64_bool(1), u64_float(0.0f), u64_float(0.0f));
  L_1003:; 
  L_1004:; goto *U64_POP();
  L_1005:; 
  L_1006:; 
  L_1007:; goto L_15;
  L_1012:; 
  L_1013:; 
  L_1014:; goto L_1005;
  L_1019:; goto u64_ret;
  L_1021:; 
  u64_ret:; u64_set_return(L.ReturnValue);

}

void bp_BP_HealthManaStaminaShieldVolume__DamageTick(u64_obj *self, const u64_v *args, int argc) {
  (void)args; (void)argc;
  struct { u64_v ReturnValue; u64_v _unsupported; } L; memset(&L, 0, sizeof L); (void)L;
  U64_FLOW_DECL; (void)flow_n;
  L_0:; 
  L_1:; 
  L_2:; 
  L_3:; U64_CALL(self, "ExecuteUbergraph_BP_HealthManaStaminaShieldVolume", u64_int(1012));
  L_18:; 
  L_19:; goto u64_ret;
  L_21:; 
  u64_ret:; u64_set_return(L.ReturnValue);

}

void bp_BP_HealthManaStaminaShieldVolume__ReceiveBeginPlay(u64_obj *self, const u64_v *args, int argc) {
  (void)args; (void)argc;
  struct { u64_v ReturnValue; u64_v _unsupported; } L; memset(&L, 0, sizeof L); (void)L;
  U64_FLOW_DECL; (void)flow_n;
  L_0:; 
  L_1:; 
  L_2:; 
  L_3:; U64_CALL(self, "ExecuteUbergraph_BP_HealthManaStaminaShieldVolume", u64_int(924));
  L_18:; 
  L_19:; goto u64_ret;
  L_21:; 
  u64_ret:; u64_set_return(L.ReturnValue);

}
