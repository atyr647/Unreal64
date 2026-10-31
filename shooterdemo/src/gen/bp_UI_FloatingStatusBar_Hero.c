#include "u64_rt.h"

void bp_UI_FloatingStatusBar_Hero__ExecuteUbergraph_UI_FloatingStatusBar_Hero(u64_obj *self, const u64_v *args, int argc) {
  (void)args; (void)argc;
  struct { u64_v ReturnValue; u64_v _unsupported; } L; memset(&L, 0, sizeof L); (void)L;
  U64_FLOW_DECL; (void)flow_n;
  if (argc > 0) (*u64_pframe(self, "EntryPoint")) = args[0];
  L_0:; u64_dispatch: switch (u64_as_int((*u64_pframe(self, "EntryPoint")))) { case 0: goto L_0; case 10: goto L_10; case 11: goto L_11; case 12: goto L_12; case 13: goto L_13; case 40: goto L_40; case 41: goto L_41; case 42: goto L_42; case 69: goto L_69; case 70: goto L_70; case 71: goto L_71; case 98: goto L_98; case 99: goto L_99; case 171: goto L_171; case 240: goto L_240; case 278: goto L_278; case 279: goto L_279; case 280: goto L_280; case 294: goto L_294; case 295: goto L_295; case 384: goto L_384; case 470: goto L_470; case 516: goto L_516; case 517: goto L_517; case 540: goto L_540; case 541: goto L_541; case 546: goto L_546; case 616: goto L_616; case 683: goto L_683; case 721: goto L_721; case 722: goto L_722; case 723: goto L_723; case 737: goto L_737; case 738: goto L_738; case 825: goto L_825; case 909: goto L_909; case 955: goto L_955; case 956: goto L_956; case 979: goto L_979; case 980: goto L_980; case 985: goto L_985; case 1057: goto L_1057; case 1126: goto L_1126; case 1164: goto L_1164; case 1165: goto L_1165; case 1166: goto L_1166; case 1180: goto L_1180; case 1181: goto L_1181; case 1270: goto L_1270; case 1356: goto L_1356; case 1402: goto L_1402; case 1403: goto L_1403; case 1426: goto L_1426; case 1427: goto L_1427; case 1432: goto L_1432; case 1433: goto L_1433; case 1465: goto L_1465; case 1466: goto L_1466; case 1471: goto L_1471; case 1500: goto L_1500; case 1501: goto L_1501; case 1502: goto L_1502; case 1516: goto L_1516; case 1517: goto L_1517; case 1522: goto L_1522; case 1523: goto L_1523; case 1524: goto L_1524; case 1525: goto L_1525; case 1583: goto L_1583; case 1612: goto L_1612; case 1613: goto L_1613; case 1627: goto L_1627; case 1628: goto L_1628; case 1657: goto L_1657; case 1658: goto L_1658; case 1677: goto L_1677; case 1678: goto L_1678; case 1934: goto L_1934; case 1935: goto L_1935; case 1973: goto L_1973; case 1974: goto L_1974; case 2003: goto L_2003; case 2004: goto L_2004; case 2005: goto L_2005; case 2019: goto L_2019; case 2020: goto L_2020; case 2043: goto L_2043; case 2044: goto L_2044; case 2085: goto L_2085; case 2086: goto L_2086; case 2087: goto L_2087; case 2123: goto L_2123; case 2124: goto L_2124; case 2125: goto L_2125; case 2144: goto L_2144; case 2145: goto L_2145; case 2150: goto L_2150; case 2151: goto L_2151; case 2152: goto L_2152; case 2157: goto L_2157; case 2158: goto L_2158; case 2159: goto L_2159; case 2160: goto L_2160; case 2201: goto L_2201; case 2202: goto L_2202; case 2207: goto L_2207; case 2208: goto L_2208; case 2209: goto L_2209; case 2210: goto L_2210; case 2251: goto L_2251; case 2252: goto L_2252; case 2257: goto L_2257; case 2258: goto L_2258; case 2259: goto L_2259; case 2260: goto L_2260; case 2305: goto L_2305; case 2306: goto L_2306; case 2311: goto L_2311; case 2312: goto L_2312; case 2313: goto L_2313; case 2314: goto L_2314; case 2355: goto L_2355; case 2356: goto L_2356; case 2358: goto L_2358; }
  L_10:; 
  L_11:; 
  L_12:; 
  L_13:; (*u64_pframe(self, "Temp_struct_Variable")) = (*u64_pframe(self, "K2Node_CustomEvent_Attribute"));
  L_40:; 
  L_41:; 
  L_42:; (*u64_pframe(self, "Temp_float_Variable_1")) = (*u64_pframe(self, "K2Node_CustomEvent_NewValue"));
  L_69:; 
  L_70:; 
  L_71:; (*u64_pframe(self, "Temp_float_Variable")) = (*u64_pframe(self, "K2Node_CustomEvent_OldValue"));
  L_98:; 
  L_99:; (*u64_pframe(self, "CallFunc_EqualEqual_GameplayAttributeGameplayAttribute_ReturnValue_4")) = U64_CALL(self, "EqualEqual_GameplayAttributeGameplayAttribute", (*u64_pframe(self, "Temp_struct_Variable")), u64_str("MaxHealth"));
  L_171:; (*u64_pframe(self, "CallFunc_EqualEqual_GameplayAttributeGameplayAttribute_ReturnValue_5")) = U64_CALL(self, "EqualEqual_GameplayAttributeGameplayAttribute", (*u64_pframe(self, "Temp_struct_Variable")), u64_str("Health"));
  L_240:; (*u64_pframe(self, "CallFunc_BooleanOR_ReturnValue_2")) = U64_CALL(self, "BooleanOR", (*u64_pframe(self, "CallFunc_EqualEqual_GameplayAttributeGameplayAttribute_ReturnValue_5")), (*u64_pframe(self, "CallFunc_EqualEqual_GameplayAttributeGameplayAttribute_ReturnValue_4")));
  L_278:; 
  L_279:; 
  L_280:; if (!u64_true((*u64_pframe(self, "CallFunc_BooleanOR_ReturnValue_2")))) goto L_546;
  L_294:; 
  L_295:; (*u64_pframe(self, "CallFunc_GetFloatAttributeFromAbilitySystemComponent_ReturnValue_4")) = U64_CALL(self, "GetFloatAttributeFromAbilitySystemComponent", (*u64_ivar(self, "OwningASC")), u64_str("MaxHealth"), u64_ref(&(*u64_pframe(self, "CallFunc_GetFloatAttributeFromAbilitySystemComponent_bSuccessfullyFoundAttribute_4"))));
  L_384:; (*u64_pframe(self, "CallFunc_GetFloatAttributeFromAbilitySystemComponent_ReturnValue_5")) = U64_CALL(self, "GetFloatAttributeFromAbilitySystemComponent", (*u64_ivar(self, "OwningASC")), u64_str("Health"), u64_ref(&(*u64_pframe(self, "CallFunc_GetFloatAttributeFromAbilitySystemComponent_bSuccessfullyFoundAttribute_5"))));
  L_470:; (*u64_pframe(self, "CallFunc_Divide_FloatFloat_ReturnValue_2")) = U64_CALL(self, "Divide_FloatFloat", (*u64_pframe(self, "CallFunc_GetFloatAttributeFromAbilitySystemComponent_ReturnValue_5")), (*u64_pframe(self, "CallFunc_GetFloatAttributeFromAbilitySystemComponent_ReturnValue_4")));
  L_516:; 
  L_517:; U64_CALL(self, "SetHealthPercentage", (*u64_pframe(self, "CallFunc_Divide_FloatFloat_ReturnValue_2")));
  L_540:; 
  L_541:; goto L_2356;
  L_546:; (*u64_pframe(self, "CallFunc_EqualEqual_GameplayAttributeGameplayAttribute_ReturnValue_2")) = U64_CALL(self, "EqualEqual_GameplayAttributeGameplayAttribute", (*u64_pframe(self, "Temp_struct_Variable")), u64_str("MaxMana"));
  L_616:; (*u64_pframe(self, "CallFunc_EqualEqual_GameplayAttributeGameplayAttribute_ReturnValue_3")) = U64_CALL(self, "EqualEqual_GameplayAttributeGameplayAttribute", (*u64_pframe(self, "Temp_struct_Variable")), u64_str("Mana"));
  L_683:; (*u64_pframe(self, "CallFunc_BooleanOR_ReturnValue_1")) = U64_CALL(self, "BooleanOR", (*u64_pframe(self, "CallFunc_EqualEqual_GameplayAttributeGameplayAttribute_ReturnValue_3")), (*u64_pframe(self, "CallFunc_EqualEqual_GameplayAttributeGameplayAttribute_ReturnValue_2")));
  L_721:; 
  L_722:; 
  L_723:; if (!u64_true((*u64_pframe(self, "CallFunc_BooleanOR_ReturnValue_1")))) goto L_985;
  L_737:; 
  L_738:; (*u64_pframe(self, "CallFunc_GetFloatAttributeFromAbilitySystemComponent_ReturnValue_2")) = U64_CALL(self, "GetFloatAttributeFromAbilitySystemComponent", (*u64_ivar(self, "OwningASC")), u64_str("MaxMana"), u64_ref(&(*u64_pframe(self, "CallFunc_GetFloatAttributeFromAbilitySystemComponent_bSuccessfullyFoundAttribute_2"))));
  L_825:; (*u64_pframe(self, "CallFunc_GetFloatAttributeFromAbilitySystemComponent_ReturnValue_3")) = U64_CALL(self, "GetFloatAttributeFromAbilitySystemComponent", (*u64_ivar(self, "OwningASC")), u64_str("Mana"), u64_ref(&(*u64_pframe(self, "CallFunc_GetFloatAttributeFromAbilitySystemComponent_bSuccessfullyFoundAttribute_3"))));
  L_909:; (*u64_pframe(self, "CallFunc_Divide_FloatFloat_ReturnValue_1")) = U64_CALL(self, "Divide_FloatFloat", (*u64_pframe(self, "CallFunc_GetFloatAttributeFromAbilitySystemComponent_ReturnValue_3")), (*u64_pframe(self, "CallFunc_GetFloatAttributeFromAbilitySystemComponent_ReturnValue_2")));
  L_955:; 
  L_956:; U64_CALL(self, "SetManaPercentage", (*u64_pframe(self, "CallFunc_Divide_FloatFloat_ReturnValue_1")));
  L_979:; 
  L_980:; goto L_2356;
  L_985:; (*u64_pframe(self, "CallFunc_EqualEqual_GameplayAttributeGameplayAttribute_ReturnValue")) = U64_CALL(self, "EqualEqual_GameplayAttributeGameplayAttribute", (*u64_pframe(self, "Temp_struct_Variable")), u64_str("MaxShield"));
  L_1057:; (*u64_pframe(self, "CallFunc_EqualEqual_GameplayAttributeGameplayAttribute_ReturnValue_1")) = U64_CALL(self, "EqualEqual_GameplayAttributeGameplayAttribute", (*u64_pframe(self, "Temp_struct_Variable")), u64_str("Shield"));
  L_1126:; (*u64_pframe(self, "CallFunc_BooleanOR_ReturnValue")) = U64_CALL(self, "BooleanOR", (*u64_pframe(self, "CallFunc_EqualEqual_GameplayAttributeGameplayAttribute_ReturnValue_1")), (*u64_pframe(self, "CallFunc_EqualEqual_GameplayAttributeGameplayAttribute_ReturnValue")));
  L_1164:; 
  L_1165:; 
  L_1166:; if (!u64_true((*u64_pframe(self, "CallFunc_BooleanOR_ReturnValue")))) goto L_2356;
  L_1180:; 
  L_1181:; (*u64_pframe(self, "CallFunc_GetFloatAttributeFromAbilitySystemComponent_ReturnValue")) = U64_CALL(self, "GetFloatAttributeFromAbilitySystemComponent", (*u64_ivar(self, "OwningASC")), u64_str("MaxShield"), u64_ref(&(*u64_pframe(self, "CallFunc_GetFloatAttributeFromAbilitySystemComponent_bSuccessfullyFoundAttribute"))));
  L_1270:; (*u64_pframe(self, "CallFunc_GetFloatAttributeFromAbilitySystemComponent_ReturnValue_1")) = U64_CALL(self, "GetFloatAttributeFromAbilitySystemComponent", (*u64_ivar(self, "OwningASC")), u64_str("Shield"), u64_ref(&(*u64_pframe(self, "CallFunc_GetFloatAttributeFromAbilitySystemComponent_bSuccessfullyFoundAttribute_1"))));
  L_1356:; (*u64_pframe(self, "CallFunc_Divide_FloatFloat_ReturnValue")) = U64_CALL(self, "Divide_FloatFloat", (*u64_pframe(self, "CallFunc_GetFloatAttributeFromAbilitySystemComponent_ReturnValue_1")), (*u64_pframe(self, "CallFunc_GetFloatAttributeFromAbilitySystemComponent_ReturnValue")));
  L_1402:; 
  L_1403:; U64_CALL(self, "SetShieldPercentage", (*u64_pframe(self, "CallFunc_Divide_FloatFloat_ReturnValue")));
  L_1426:; 
  L_1427:; goto L_2356;
  L_1432:; 
  L_1433:; ({ u64_v _o = (*u64_ivar(self, "AttributesChangeTask")); U64_CALL(u64_asobj(_o), "EndTask"); });
  L_1465:; 
  L_1466:; goto L_2356;
  L_1471:; (*u64_pframe(self, "CallFunc_IsValid_ReturnValue")) = U64_CALL(self, "IsValid", (*u64_ivar(self, "AttributesChangeTask")));
  L_1500:; 
  L_1501:; 
  L_1502:; if (!u64_true((*u64_pframe(self, "CallFunc_IsValid_ReturnValue")))) goto L_2356;
  L_1516:; 
  L_1517:; goto L_1432;
  L_1522:; 
  L_1523:; 
  L_1524:; 
  L_1525:; (*u64_pframe(self, "K2Node_DynamicCast_AsGSPlayer_State")) = u64_cast(({ u64_v _o = (*u64_ivar(self, "OwningCharacter")); (*u64_ivar(u64_asobj(_o), "PlayerState")); }), "GSPlayerState");
  L_1583:; (*u64_pframe(self, "K2Node_DynamicCast_bSuccess")) = (*u64_pframe(self, "K2Node_DynamicCast_AsGSPlayer_State"));
  L_1612:; 
  L_1613:; if (!u64_true((*u64_pframe(self, "K2Node_DynamicCast_bSuccess")))) goto L_2356;
  L_1627:; 
  L_1628:; (*u64_pframe(self, "CallFunc_GetAbilitySystemComponent_ReturnValue")) = U64_CALL(self, "GetAbilitySystemComponent", (*u64_pframe(self, "K2Node_DynamicCast_AsGSPlayer_State")));
  L_1657:; 
  L_1658:; (*u64_ivar(self, "OwningASC")) = (*u64_pframe(self, "CallFunc_GetAbilitySystemComponent_ReturnValue"));
  L_1677:; 
  L_1678:; u64_array_set(&(*u64_pframe(self, "K2Node_MakeArray_Array")), 6, (u64_v[]){u64_int(0), u64_str("Health"), u64_str("MaxHealth"), u64_str("Mana"), u64_str("MaxMana"), u64_str("Shield"), u64_str("MaxShield")} + 1);
  L_1934:; 
  L_1935:; (*u64_pframe(self, "CallFunc_ListenForAttributesChange_ReturnValue")) = U64_CALL(self, "ListenForAttributesChange", (*u64_ivar(self, "OwningASC")), (*u64_pframe(self, "K2Node_MakeArray_Array")));
  L_1973:; 
  L_1974:; (*u64_pframe(self, "CallFunc_IsValid_ReturnValue_1")) = U64_CALL(self, "IsValid", (*u64_pframe(self, "CallFunc_ListenForAttributesChange_ReturnValue")));
  L_2003:; 
  L_2004:; 
  L_2005:; if (!u64_true((*u64_pframe(self, "CallFunc_IsValid_ReturnValue_1")))) goto L_2124;
  L_2019:; 
  L_2020:; (*u64_pframe(self, "K2Node_CreateDelegate_OutputDelegate")) = u64_delegate(u64_objv(self), "OnAttributeChanged_2CC475054AC4BFCBB6D3D6855FCDD6A7");
  L_2043:; 
  L_2044:; u64_mc_add(&(*u64_ivar(u64_asobj((*u64_pframe(self, "CallFunc_ListenForAttributesChange_ReturnValue"))), "OnAttributeChanged")), (*u64_pframe(self, "K2Node_CreateDelegate_OutputDelegate")));
  L_2085:; 
  L_2086:; 
  L_2087:; ({ u64_v _o = (*u64_pframe(self, "CallFunc_ListenForAttributesChange_ReturnValue")); U64_CALL(u64_asobj(_o), "Activate"); });
  L_2123:; 
  L_2124:; 
  L_2125:; (*u64_ivar(self, "AttributesChangeTask")) = (*u64_pframe(self, "CallFunc_ListenForAttributesChange_ReturnValue"));
  L_2144:; 
  L_2145:; goto L_2356;
  L_2150:; 
  L_2151:; 
  L_2152:; goto L_1471;
  L_2157:; 
  L_2158:; 
  L_2159:; 
  L_2160:; ({ u64_v _o = (*u64_ivar(self, "ProgressBar_Health")); U64_CALL(u64_asobj(_o), "SetPercent", (*u64_pframe(self, "K2Node_Event_HealthPercentage"))); });
  L_2201:; 
  L_2202:; goto L_2356;
  L_2207:; 
  L_2208:; 
  L_2209:; 
  L_2210:; ({ u64_v _o = (*u64_ivar(self, "ProgressBar_Mana")); U64_CALL(u64_asobj(_o), "SetPercent", (*u64_pframe(self, "K2Node_Event_ManaPercentage"))); });
  L_2251:; 
  L_2252:; goto L_2356;
  L_2257:; 
  L_2258:; 
  L_2259:; 
  L_2260:; ({ u64_v _o = (*u64_ivar(self, "TextBlock_Name")); U64_CALL(u64_asobj(_o), "SetText", (*u64_pframe(self, "K2Node_Event_NewName"))); });
  L_2305:; 
  L_2306:; goto L_2356;
  L_2311:; 
  L_2312:; 
  L_2313:; 
  L_2314:; ({ u64_v _o = (*u64_ivar(self, "ProgressBar_Shield")); U64_CALL(u64_asobj(_o), "SetPercent", (*u64_pframe(self, "K2Node_Event_ShieldPercentage"))); });
  L_2355:; 
  L_2356:; goto u64_ret;
  L_2358:; 
  u64_ret:; u64_set_return(L.ReturnValue);

}

void bp_UI_FloatingStatusBar_Hero__SetShieldPercentage(u64_obj *self, const u64_v *args, int argc) {
  (void)args; (void)argc;
  struct { u64_v ReturnValue; u64_v ShieldPercentage; u64_v _unsupported; } L; memset(&L, 0, sizeof L); (void)L;
  U64_FLOW_DECL; (void)flow_n;
  if (argc > 0) L.ShieldPercentage = args[0];
  L_0:; 
  L_1:; 
  L_2:; 
  L_3:; *u64_pframe(self, "K2Node_Event_ShieldPercentage") = L.ShieldPercentage;
  L_21:; 
  L_22:; 
  L_23:; U64_CALL(self, "ExecuteUbergraph_UI_FloatingStatusBar_Hero", u64_int(2311));
  L_38:; 
  L_39:; goto u64_ret;
  L_41:; 
  u64_ret:; u64_set_return(L.ReturnValue);

}

void bp_UI_FloatingStatusBar_Hero__SetCharacterName(u64_obj *self, const u64_v *args, int argc) {
  (void)args; (void)argc;
  struct { u64_v NewName; u64_v ReturnValue; u64_v _unsupported; } L; memset(&L, 0, sizeof L); (void)L;
  U64_FLOW_DECL; (void)flow_n;
  if (argc > 0) L.NewName = args[0].t == U64_REF ? *args[0].u.r : args[0];
  L_0:; 
  L_1:; 
  L_2:; 
  L_3:; *u64_pframe(self, "K2Node_Event_NewName") = L.NewName;
  L_21:; 
  L_22:; 
  L_23:; U64_CALL(self, "ExecuteUbergraph_UI_FloatingStatusBar_Hero", u64_int(2257));
  L_38:; 
  L_39:; goto u64_ret;
  L_41:; 
  u64_ret:; u64_set_return(L.ReturnValue);
  if (argc > 0) u64_out(args[0], L.NewName);
}

void bp_UI_FloatingStatusBar_Hero__SetManaPercentage(u64_obj *self, const u64_v *args, int argc) {
  (void)args; (void)argc;
  struct { u64_v ManaPercentage; u64_v ReturnValue; u64_v _unsupported; } L; memset(&L, 0, sizeof L); (void)L;
  U64_FLOW_DECL; (void)flow_n;
  if (argc > 0) L.ManaPercentage = args[0];
  L_0:; 
  L_1:; 
  L_2:; 
  L_3:; *u64_pframe(self, "K2Node_Event_ManaPercentage") = L.ManaPercentage;
  L_21:; 
  L_22:; 
  L_23:; U64_CALL(self, "ExecuteUbergraph_UI_FloatingStatusBar_Hero", u64_int(2207));
  L_38:; 
  L_39:; goto u64_ret;
  L_41:; 
  u64_ret:; u64_set_return(L.ReturnValue);

}

void bp_UI_FloatingStatusBar_Hero__SetHealthPercentage(u64_obj *self, const u64_v *args, int argc) {
  (void)args; (void)argc;
  struct { u64_v HealthPercentage; u64_v ReturnValue; u64_v _unsupported; } L; memset(&L, 0, sizeof L); (void)L;
  U64_FLOW_DECL; (void)flow_n;
  if (argc > 0) L.HealthPercentage = args[0];
  L_0:; 
  L_1:; 
  L_2:; 
  L_3:; *u64_pframe(self, "K2Node_Event_HealthPercentage") = L.HealthPercentage;
  L_21:; 
  L_22:; 
  L_23:; U64_CALL(self, "ExecuteUbergraph_UI_FloatingStatusBar_Hero", u64_int(2157));
  L_38:; 
  L_39:; goto u64_ret;
  L_41:; 
  u64_ret:; u64_set_return(L.ReturnValue);

}

void bp_UI_FloatingStatusBar_Hero__Destruct(u64_obj *self, const u64_v *args, int argc) {
  (void)args; (void)argc;
  struct { u64_v ReturnValue; u64_v _unsupported; } L; memset(&L, 0, sizeof L); (void)L;
  U64_FLOW_DECL; (void)flow_n;
  L_0:; 
  L_1:; 
  L_2:; 
  L_3:; U64_CALL(self, "ExecuteUbergraph_UI_FloatingStatusBar_Hero", u64_int(2150));
  L_18:; 
  L_19:; goto u64_ret;
  L_21:; 
  u64_ret:; u64_set_return(L.ReturnValue);

}

void bp_UI_FloatingStatusBar_Hero__Construct(u64_obj *self, const u64_v *args, int argc) {
  (void)args; (void)argc;
  struct { u64_v ReturnValue; u64_v _unsupported; } L; memset(&L, 0, sizeof L); (void)L;
  U64_FLOW_DECL; (void)flow_n;
  L_0:; 
  L_1:; 
  L_2:; 
  L_3:; U64_CALL(self, "ExecuteUbergraph_UI_FloatingStatusBar_Hero", u64_int(1522));
  L_18:; 
  L_19:; goto u64_ret;
  L_21:; 
  u64_ret:; u64_set_return(L.ReturnValue);

}

void bp_UI_FloatingStatusBar_Hero__OnAttributeChanged_2CC475054AC4BFCBB6D3D6855FCDD6A7(u64_obj *self, const u64_v *args, int argc) {
  (void)args; (void)argc;
  struct { u64_v Attribute; u64_v NewValue; u64_v OldValue; u64_v ReturnValue; u64_v _unsupported; } L; memset(&L, 0, sizeof L); (void)L;
  U64_FLOW_DECL; (void)flow_n;
  if (argc > 0) L.Attribute = args[0];
  if (argc > 1) L.NewValue = args[1];
  if (argc > 2) L.OldValue = args[2];
  L_0:; 
  L_1:; 
  L_2:; 
  L_3:; *u64_pframe(self, "K2Node_CustomEvent_Attribute") = L.Attribute;
  L_21:; *u64_pframe(self, "K2Node_CustomEvent_NewValue") = L.NewValue;
  L_39:; *u64_pframe(self, "K2Node_CustomEvent_OldValue") = L.OldValue;
  L_57:; 
  L_58:; 
  L_59:; U64_CALL(self, "ExecuteUbergraph_UI_FloatingStatusBar_Hero", u64_int(10));
  L_74:; 
  L_75:; goto u64_ret;
  L_77:; 
  u64_ret:; u64_set_return(L.ReturnValue);

}
