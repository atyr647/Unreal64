#include "u64_rt.h"

void bp_BP_DamageVolume__ExecuteUbergraph_BP_DamageVolume(u64_obj *self, const u64_v *args, int argc) {
  (void)args; (void)argc;
  struct { u64_v ReturnValue; u64_v _unsupported; } L; memset(&L, 0, sizeof L); (void)L;
  U64_FLOW_DECL; (void)flow_n;
  if (argc > 0) (*u64_pframe(self, "EntryPoint")) = args[0];
  L_0:; U64_PUSH(&&L_1186);
  L_5:; u64_dispatch: switch (u64_as_int((*u64_pframe(self, "EntryPoint")))) { case 0: goto L_0; case 5: goto L_5; case 15: goto L_15; case 16: goto L_16; case 17: goto L_17; case 18: goto L_18; case 38: goto L_38; case 39: goto L_39; case 40: goto L_40; case 54: goto L_54; case 55: goto L_55; case 56: goto L_56; case 57: goto L_57; case 58: goto L_58; case 59: goto L_59; case 60: goto L_60; case 61: goto L_61; case 62: goto L_62; case 85: goto L_85; case 86: goto L_86; case 87: goto L_87; case 110: goto L_110; case 111: goto L_111; case 122: goto L_122; case 172: goto L_172; case 231: goto L_231; case 269: goto L_269; case 270: goto L_270; case 271: goto L_271; case 285: goto L_285; case 286: goto L_286; case 287: goto L_287; case 314: goto L_314; case 315: goto L_315; case 316: goto L_316; case 321: goto L_321; case 322: goto L_322; case 327: goto L_327; case 328: goto L_328; case 329: goto L_329; case 371: goto L_371; case 372: goto L_372; case 399: goto L_399; case 400: goto L_400; case 405: goto L_405; case 406: goto L_406; case 407: goto L_407; case 408: goto L_408; case 409: goto L_409; case 410: goto L_410; case 421: goto L_421; case 471: goto L_471; case 530: goto L_530; case 531: goto L_531; case 567: goto L_567; case 596: goto L_596; case 597: goto L_597; case 607: goto L_607; case 608: goto L_608; case 654: goto L_654; case 683: goto L_683; case 712: goto L_712; case 750: goto L_750; case 751: goto L_751; case 752: goto L_752; case 762: goto L_762; case 763: goto L_763; case 792: goto L_792; case 846: goto L_846; case 923: goto L_923; case 924: goto L_924; case 997: goto L_997; case 998: goto L_998; case 1027: goto L_1027; case 1028: goto L_1028; case 1087: goto L_1087; case 1088: goto L_1088; case 1089: goto L_1089; case 1090: goto L_1090; case 1091: goto L_1091; case 1114: goto L_1114; case 1115: goto L_1115; case 1168: goto L_1168; case 1169: goto L_1169; case 1170: goto L_1170; case 1171: goto L_1171; case 1172: goto L_1172; case 1173: goto L_1173; case 1174: goto L_1174; case 1179: goto L_1179; case 1180: goto L_1180; case 1181: goto L_1181; case 1186: goto L_1186; case 1188: goto L_1188; }
  L_15:; 
  L_16:; 
  L_17:; goto *U64_POP();
  L_18:; (*u64_pframe(self, "CallFunc_HasAuthority_ReturnValue")) = U64_CALL(self, "HasAuthority");
  L_38:; 
  L_39:; 
  L_40:; if (!u64_true((*u64_pframe(self, "CallFunc_HasAuthority_ReturnValue")))) goto L_15;
  L_54:; 
  L_55:; 
  L_56:; 
  L_57:; 
  L_58:; 
  L_59:; 
  L_60:; 
  L_61:; 
  L_62:; (*u64_pframe(self, "Temp_int_Loop_Counter_Variable")) = u64_int(0);
  L_85:; 
  L_86:; 
  L_87:; (*u64_pframe(self, "Temp_int_Array_Index_Variable")) = u64_int(0);
  L_110:; 
  L_111:; u64_array_set(&(*u64_pframe(self, "CallFunc_GetOverlappingActors_OverlappingActors")), 0, (u64_v[]){u64_int(0)} + 1);
  L_122:; ({ u64_v _o = (*u64_ivar(self, "Box")); U64_CALL(u64_asobj(_o), "GetOverlappingActors", u64_ref(&(*u64_pframe(self, "CallFunc_GetOverlappingActors_OverlappingActors"))), u64_objv(u64_find("GSCharacterBase"))); });
  L_172:; (*u64_pframe(self, "CallFunc_Array_Length_ReturnValue")) = ({ u64_v _o = u64_objv(u64_find("Default__KismetArrayLibrary")); U64_CALL(u64_asobj(_o), "Array_Length", u64_ref(&(*u64_pframe(self, "CallFunc_GetOverlappingActors_OverlappingActors")))); });
  L_231:; (*u64_pframe(self, "CallFunc_Less_IntInt_ReturnValue")) = U64_CALL(self, "Less_IntInt", (*u64_pframe(self, "Temp_int_Loop_Counter_Variable")), (*u64_pframe(self, "CallFunc_Array_Length_ReturnValue")));
  L_269:; 
  L_270:; 
  L_271:; if (!u64_true((*u64_pframe(self, "CallFunc_Less_IntInt_ReturnValue")))) goto L_405;
  L_285:; 
  L_286:; 
  L_287:; (*u64_pframe(self, "Temp_int_Array_Index_Variable")) = (*u64_pframe(self, "Temp_int_Loop_Counter_Variable"));
  L_314:; 
  L_315:; 
  L_316:; U64_PUSH(&&L_327);
  L_321:; 
  L_322:; goto L_408;
  L_327:; 
  L_328:; 
  L_329:; (*u64_pframe(self, "CallFunc_Add_IntInt_ReturnValue")) = U64_CALL(self, "Add_IntInt", (*u64_pframe(self, "Temp_int_Loop_Counter_Variable")), u64_int(1));
  L_371:; 
  L_372:; (*u64_pframe(self, "Temp_int_Loop_Counter_Variable")) = (*u64_pframe(self, "CallFunc_Add_IntInt_ReturnValue"));
  L_399:; 
  L_400:; goto L_111;
  L_405:; 
  L_406:; 
  L_407:; goto *U64_POP();
  L_408:; 
  L_409:; 
  L_410:; u64_array_set(&(*u64_pframe(self, "CallFunc_GetOverlappingActors_OverlappingActors")), 0, (u64_v[]){u64_int(0)} + 1);
  L_421:; ({ u64_v _o = (*u64_ivar(self, "Box")); U64_CALL(u64_asobj(_o), "GetOverlappingActors", u64_ref(&(*u64_pframe(self, "CallFunc_GetOverlappingActors_OverlappingActors"))), u64_objv(u64_find("GSCharacterBase"))); });
  L_471:; ({ u64_v _o = u64_objv(u64_find("Default__KismetArrayLibrary")); U64_CALL(u64_asobj(_o), "Array_Get", u64_ref(&(*u64_pframe(self, "CallFunc_GetOverlappingActors_OverlappingActors"))), (*u64_pframe(self, "Temp_int_Array_Index_Variable")), u64_ref(&(*u64_pframe(self, "CallFunc_Array_Get_Item")))); });
  L_530:; 
  L_531:; (*u64_pframe(self, "K2Node_DynamicCast_AsGSCharacter_Base")) = u64_cast((*u64_pframe(self, "CallFunc_Array_Get_Item")), "GSCharacterBase");
  L_567:; (*u64_pframe(self, "K2Node_DynamicCast_bSuccess")) = (*u64_pframe(self, "K2Node_DynamicCast_AsGSCharacter_Base"));
  L_596:; 
  L_597:; if (!u64_true((*u64_pframe(self, "K2Node_DynamicCast_bSuccess")))) goto *U64_POP();
  L_607:; 
  L_608:; (*u64_pframe(self, "CallFunc_IsAlive_ReturnValue")) = ({ u64_v _o = (*u64_pframe(self, "K2Node_DynamicCast_AsGSCharacter_Base")); U64_CALL(u64_asobj(_o), "IsAlive"); });
  L_654:; (*u64_pframe(self, "CallFunc_GetAbilitySystemComponent_ReturnValue")) = U64_CALL(self, "GetAbilitySystemComponent", (*u64_pframe(self, "K2Node_DynamicCast_AsGSCharacter_Base")));
  L_683:; (*u64_pframe(self, "CallFunc_IsValid_ReturnValue")) = U64_CALL(self, "IsValid", (*u64_pframe(self, "CallFunc_GetAbilitySystemComponent_ReturnValue")));
  L_712:; (*u64_pframe(self, "CallFunc_BooleanAND_ReturnValue")) = U64_CALL(self, "BooleanAND", (*u64_pframe(self, "CallFunc_IsAlive_ReturnValue")), (*u64_pframe(self, "CallFunc_IsValid_ReturnValue")));
  L_750:; 
  L_751:; 
  L_752:; if (!u64_true((*u64_pframe(self, "CallFunc_BooleanAND_ReturnValue")))) goto *U64_POP();
  L_762:; 
  L_763:; (*u64_pframe(self, "CallFunc_GetAbilitySystemComponent_ReturnValue")) = U64_CALL(self, "GetAbilitySystemComponent", (*u64_pframe(self, "K2Node_DynamicCast_AsGSCharacter_Base")));
  L_792:; (*u64_pframe(self, "CallFunc_MakeEffectContext_ReturnValue")) = ({ u64_v _o = (*u64_pframe(self, "CallFunc_GetAbilitySystemComponent_ReturnValue")); U64_CALL(u64_asobj(_o), "MakeEffectContext"); });
  L_846:; (*u64_pframe(self, "CallFunc_MakeOutgoingSpec_ReturnValue")) = ({ u64_v _o = (*u64_pframe(self, "CallFunc_GetAbilitySystemComponent_ReturnValue")); U64_CALL(u64_asobj(_o), "MakeOutgoingSpec", (*u64_ivar(self, "DamageGameplayEffectClass")), u64_float(1.0f), (*u64_pframe(self, "CallFunc_MakeEffectContext_ReturnValue"))); });
  L_923:; 
  L_924:; (*u64_pframe(self, "CallFunc_AssignTagSetByCallerMagnitude_ReturnValue")) = U64_CALL(self, "AssignTagSetByCallerMagnitude", (*u64_pframe(self, "CallFunc_MakeOutgoingSpec_ReturnValue")), u64_name("Data.Damage"), (*u64_ivar(self, "Damage")));
  L_997:; 
  L_998:; (*u64_pframe(self, "CallFunc_GetAbilitySystemComponent_ReturnValue")) = U64_CALL(self, "GetAbilitySystemComponent", (*u64_pframe(self, "K2Node_DynamicCast_AsGSCharacter_Base")));
  L_1027:; 
  L_1028:; (*u64_pframe(self, "CallFunc_BP_ApplyGameplayEffectSpecToSelf_ReturnValue")) = ({ u64_v _o = (*u64_pframe(self, "CallFunc_GetAbilitySystemComponent_ReturnValue")); U64_CALL(u64_asobj(_o), "BP_ApplyGameplayEffectSpecToSelf", (*u64_pframe(self, "CallFunc_AssignTagSetByCallerMagnitude_ReturnValue"))); });
  L_1087:; 
  L_1088:; goto *U64_POP();
  L_1089:; 
  L_1090:; 
  L_1091:; (*u64_pframe(self, "K2Node_CreateDelegate_OutputDelegate")) = u64_delegate(u64_objv(self), "DamageTick");
  L_1114:; 
  L_1115:; (*u64_pframe(self, "CallFunc_K2_SetTimerDelegate_ReturnValue")) = U64_CALL(self, "K2_SetTimerDelegate", (*u64_pframe(self, "K2Node_CreateDelegate_OutputDelegate")), u64_float(1.0f), u64_bool(1), u64_float(0.0f), u64_float(0.0f));
  L_1168:; 
  L_1169:; goto *U64_POP();
  L_1170:; 
  L_1171:; 
  L_1172:; 
  L_1173:; 
  L_1174:; goto L_18;
  L_1179:; 
  L_1180:; 
  L_1181:; goto L_1170;
  L_1186:; goto u64_ret;
  L_1188:; 
  u64_ret:; u64_set_return(L.ReturnValue);

}

void bp_BP_DamageVolume__DamageTick(u64_obj *self, const u64_v *args, int argc) {
  (void)args; (void)argc;
  struct { u64_v ReturnValue; u64_v _unsupported; } L; memset(&L, 0, sizeof L); (void)L;
  U64_FLOW_DECL; (void)flow_n;
  L_0:; 
  L_1:; 
  L_2:; 
  L_3:; U64_CALL(self, "ExecuteUbergraph_BP_DamageVolume", u64_int(1179));
  L_18:; 
  L_19:; goto u64_ret;
  L_21:; 
  u64_ret:; u64_set_return(L.ReturnValue);

}

void bp_BP_DamageVolume__ReceiveBeginPlay(u64_obj *self, const u64_v *args, int argc) {
  (void)args; (void)argc;
  struct { u64_v ReturnValue; u64_v _unsupported; } L; memset(&L, 0, sizeof L); (void)L;
  U64_FLOW_DECL; (void)flow_n;
  L_0:; 
  L_1:; 
  L_2:; 
  L_3:; U64_CALL(self, "ExecuteUbergraph_BP_DamageVolume", u64_int(1089));
  L_18:; 
  L_19:; goto u64_ret;
  L_21:; 
  u64_ret:; u64_set_return(L.ReturnValue);

}
