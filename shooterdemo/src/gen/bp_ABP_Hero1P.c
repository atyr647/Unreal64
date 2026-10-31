#include "u64_rt.h"

void bp_ABP_Hero1P__AnimGraph(u64_obj *self, const u64_v *args, int argc) {
  (void)args; (void)argc;
  struct { u64_v AnimGraph; u64_v ReturnValue; u64_v _unsupported; } L; memset(&L, 0, sizeof L); (void)L;
  U64_FLOW_DECL; (void)flow_n;
  if (argc > 0) L.AnimGraph = args[0].t == U64_REF ? *args[0].u.r : args[0];
  L_0:; 
  L_1:; 
  L_2:; 
  L_3:; L.AnimGraph = u64_int(0) /* struct const */;
  L_40:; 
  L_41:; goto u64_ret;
  L_43:; 
  u64_ret:; u64_set_return(L.ReturnValue);
  if (argc > 0) u64_out(args[0], L.AnimGraph);
}

void bp_ABP_Hero1P__BlueprintUpdateAnimation(u64_obj *self, const u64_v *args, int argc) {
  (void)args; (void)argc;
  struct { u64_v DeltaTimeX; u64_v ReturnValue; u64_v _unsupported; } L; memset(&L, 0, sizeof L); (void)L;
  U64_FLOW_DECL; (void)flow_n;
  if (argc > 0) L.DeltaTimeX = args[0];
  L_0:; 
  L_1:; 
  L_2:; 
  L_3:; *u64_pframe(self, "K2Node_Event_DeltaTimeX") = L.DeltaTimeX;
  L_21:; 
  L_22:; 
  L_23:; U64_CALL(self, "ExecuteUbergraph_ABP_Hero1P", u64_int(4782));
  L_38:; 
  L_39:; goto u64_ret;
  L_41:; 
  u64_ret:; u64_set_return(L.ReturnValue);

}

void bp_ABP_Hero1P__EvaluateGraphExposedInputs_ExecuteUbergraph_ABP_Hero1P_AnimGraphNode_TransitionResult_2253A16349819CA2F27163A2CCB9491C(u64_obj *self, const u64_v *args, int argc) {
  (void)args; (void)argc;
  struct { u64_v ReturnValue; u64_v _unsupported; } L; memset(&L, 0, sizeof L); (void)L;
  U64_FLOW_DECL; (void)flow_n;
  L_0:; 
  L_1:; 
  L_2:; 
  L_3:; U64_CALL(self, "ExecuteUbergraph_ABP_Hero1P", u64_int(2910));
  L_18:; 
  L_19:; goto u64_ret;
  L_21:; 
  u64_ret:; u64_set_return(L.ReturnValue);

}

void bp_ABP_Hero1P__EvaluateGraphExposedInputs_ExecuteUbergraph_ABP_Hero1P_AnimGraphNode_TransitionResult_4EE867754862ED00C1EEFC93FE096F39(u64_obj *self, const u64_v *args, int argc) {
  (void)args; (void)argc;
  struct { u64_v ReturnValue; u64_v _unsupported; } L; memset(&L, 0, sizeof L); (void)L;
  U64_FLOW_DECL; (void)flow_n;
  L_0:; 
  L_1:; 
  L_2:; 
  L_3:; U64_CALL(self, "ExecuteUbergraph_ABP_Hero1P", u64_int(3131));
  L_18:; 
  L_19:; goto u64_ret;
  L_21:; 
  u64_ret:; u64_set_return(L.ReturnValue);

}

void bp_ABP_Hero1P__EvaluateGraphExposedInputs_ExecuteUbergraph_ABP_Hero1P_AnimGraphNode_TransitionResult_565A03BB49EFAC2576DB6297AB757A23(u64_obj *self, const u64_v *args, int argc) {
  (void)args; (void)argc;
  struct { u64_v ReturnValue; u64_v _unsupported; } L; memset(&L, 0, sizeof L); (void)L;
  U64_FLOW_DECL; (void)flow_n;
  L_0:; 
  L_1:; 
  L_2:; 
  L_3:; U64_CALL(self, "ExecuteUbergraph_ABP_Hero1P", u64_int(3289));
  L_18:; 
  L_19:; goto u64_ret;
  L_21:; 
  u64_ret:; u64_set_return(L.ReturnValue);

}

void bp_ABP_Hero1P__EvaluateGraphExposedInputs_ExecuteUbergraph_ABP_Hero1P_AnimGraphNode_TransitionResult_63FC2E194661D0BF01F47A87EA67948B(u64_obj *self, const u64_v *args, int argc) {
  (void)args; (void)argc;
  struct { u64_v ReturnValue; u64_v _unsupported; } L; memset(&L, 0, sizeof L); (void)L;
  U64_FLOW_DECL; (void)flow_n;
  L_0:; 
  L_1:; 
  L_2:; 
  L_3:; U64_CALL(self, "ExecuteUbergraph_ABP_Hero1P", u64_int(3303));
  L_18:; 
  L_19:; goto u64_ret;
  L_21:; 
  u64_ret:; u64_set_return(L.ReturnValue);

}

void bp_ABP_Hero1P__EvaluateGraphExposedInputs_ExecuteUbergraph_ABP_Hero1P_AnimGraphNode_TransitionResult_A06C10B44CF313318E5AD0BC47C414E4(u64_obj *self, const u64_v *args, int argc) {
  (void)args; (void)argc;
  struct { u64_v ReturnValue; u64_v _unsupported; } L; memset(&L, 0, sizeof L); (void)L;
  U64_FLOW_DECL; (void)flow_n;
  L_0:; 
  L_1:; 
  L_2:; 
  L_3:; U64_CALL(self, "ExecuteUbergraph_ABP_Hero1P", u64_int(2903));
  L_18:; 
  L_19:; goto u64_ret;
  L_21:; 
  u64_ret:; u64_set_return(L.ReturnValue);

}

void bp_ABP_Hero1P__EvaluateGraphExposedInputs_ExecuteUbergraph_ABP_Hero1P_AnimGraphNode_TransitionResult_A69ACE4E4B15F980A19E44BD6E8859C5(u64_obj *self, const u64_v *args, int argc) {
  (void)args; (void)argc;
  struct { u64_v ReturnValue; u64_v _unsupported; } L; memset(&L, 0, sizeof L); (void)L;
  U64_FLOW_DECL; (void)flow_n;
  L_0:; 
  L_1:; 
  L_2:; 
  L_3:; U64_CALL(self, "ExecuteUbergraph_ABP_Hero1P", u64_int(3124));
  L_18:; 
  L_19:; goto u64_ret;
  L_21:; 
  u64_ret:; u64_set_return(L.ReturnValue);

}

void bp_ABP_Hero1P__EvaluateGraphExposedInputs_ExecuteUbergraph_ABP_Hero1P_AnimGraphNode_TransitionResult_CABAAC1949D5ECE6B8672597AD14D0A5(u64_obj *self, const u64_v *args, int argc) {
  (void)args; (void)argc;
  struct { u64_v ReturnValue; u64_v _unsupported; } L; memset(&L, 0, sizeof L); (void)L;
  U64_FLOW_DECL; (void)flow_n;
  L_0:; 
  L_1:; 
  L_2:; 
  L_3:; U64_CALL(self, "ExecuteUbergraph_ABP_Hero1P", u64_int(3138));
  L_18:; 
  L_19:; goto u64_ret;
  L_21:; 
  u64_ret:; u64_set_return(L.ReturnValue);

}

void bp_ABP_Hero1P__EvaluateGraphExposedInputs_ExecuteUbergraph_ABP_Hero1P_AnimGraphNode_TransitionResult_D535047444CF400D305B0DBB0B4E5465(u64_obj *self, const u64_v *args, int argc) {
  (void)args; (void)argc;
  struct { u64_v ReturnValue; u64_v _unsupported; } L; memset(&L, 0, sizeof L); (void)L;
  U64_FLOW_DECL; (void)flow_n;
  L_0:; 
  L_1:; 
  L_2:; 
  L_3:; U64_CALL(self, "ExecuteUbergraph_ABP_Hero1P", u64_int(2660));
  L_18:; 
  L_19:; goto u64_ret;
  L_21:; 
  u64_ret:; u64_set_return(L.ReturnValue);

}

void bp_ABP_Hero1P__EvaluateGraphExposedInputs_ExecuteUbergraph_ABP_Hero1P_AnimGraphNode_TransitionResult_DBFDC8DF4BA55D7022E97CA43289D593(u64_obj *self, const u64_v *args, int argc) {
  (void)args; (void)argc;
  struct { u64_v ReturnValue; u64_v _unsupported; } L; memset(&L, 0, sizeof L); (void)L;
  U64_FLOW_DECL; (void)flow_n;
  L_0:; 
  L_1:; 
  L_2:; 
  L_3:; U64_CALL(self, "ExecuteUbergraph_ABP_Hero1P", u64_int(3282));
  L_18:; 
  L_19:; goto u64_ret;
  L_21:; 
  u64_ret:; u64_set_return(L.ReturnValue);

}

void bp_ABP_Hero1P__EvaluateGraphExposedInputs_ExecuteUbergraph_ABP_Hero1P_AnimGraphNode_TransitionResult_E2B9E0BA42B0AAD5F8A416909ADD5037(u64_obj *self, const u64_v *args, int argc) {
  (void)args; (void)argc;
  struct { u64_v ReturnValue; u64_v _unsupported; } L; memset(&L, 0, sizeof L); (void)L;
  U64_FLOW_DECL; (void)flow_n;
  L_0:; 
  L_1:; 
  L_2:; 
  L_3:; U64_CALL(self, "ExecuteUbergraph_ABP_Hero1P", u64_int(3296));
  L_18:; 
  L_19:; goto u64_ret;
  L_21:; 
  u64_ret:; u64_set_return(L.ReturnValue);

}

void bp_ABP_Hero1P__EvaluateGraphExposedInputs_ExecuteUbergraph_ABP_Hero1P_AnimGraphNode_TransitionResult_EECFDD6542851FCD294779800CB4A4D1(u64_obj *self, const u64_v *args, int argc) {
  (void)args; (void)argc;
  struct { u64_v ReturnValue; u64_v _unsupported; } L; memset(&L, 0, sizeof L); (void)L;
  U64_FLOW_DECL; (void)flow_n;
  L_0:; 
  L_1:; 
  L_2:; 
  L_3:; U64_CALL(self, "ExecuteUbergraph_ABP_Hero1P", u64_int(3275));
  L_18:; 
  L_19:; goto u64_ret;
  L_21:; 
  u64_ret:; u64_set_return(L.ReturnValue);

}

void bp_ABP_Hero1P__ExecuteUbergraph_ABP_Hero1P(u64_obj *self, const u64_v *args, int argc) {
  (void)args; (void)argc;
  struct { u64_v ReturnValue; u64_v _unsupported; } L; memset(&L, 0, sizeof L); (void)L;
  U64_FLOW_DECL; (void)flow_n;
  if (argc > 0) (*u64_pframe(self, "EntryPoint")) = args[0];
  L_0:; u64_dispatch: switch (u64_as_int((*u64_pframe(self, "EntryPoint")))) { case 0: goto L_0; case 10: goto L_10; case 11: goto L_11; case 22: goto L_22; case 23: goto L_23; case 52: goto L_52; case 53: goto L_53; case 54: goto L_54; case 68: goto L_68; case 69: goto L_69; case 146: goto L_146; case 223: goto L_223; case 261: goto L_261; case 262: goto L_262; case 263: goto L_263; case 277: goto L_277; case 278: goto L_278; case 279: goto L_279; case 290: goto L_290; case 291: goto L_291; case 296: goto L_296; case 297: goto L_297; case 308: goto L_308; case 309: goto L_309; case 314: goto L_314; case 345: goto L_345; case 376: goto L_376; case 414: goto L_414; case 415: goto L_415; case 443: goto L_443; case 444: goto L_444; case 449: goto L_449; case 483: goto L_483; case 521: goto L_521; case 522: goto L_522; case 550: goto L_550; case 551: goto L_551; case 556: goto L_556; case 587: goto L_587; case 625: goto L_625; case 656: goto L_656; case 694: goto L_694; case 695: goto L_695; case 723: goto L_723; case 724: goto L_724; case 729: goto L_729; case 760: goto L_760; case 791: goto L_791; case 829: goto L_829; case 830: goto L_830; case 858: goto L_858; case 859: goto L_859; case 864: goto L_864; case 893: goto L_893; case 932: goto L_932; case 970: goto L_970; case 971: goto L_971; case 999: goto L_999; case 1000: goto L_1000; case 1005: goto L_1005; case 1038: goto L_1038; case 1072: goto L_1072; case 1105: goto L_1105; case 1139: goto L_1139; case 1177: goto L_1177; case 1178: goto L_1178; case 1206: goto L_1206; case 1207: goto L_1207; case 1212: goto L_1212; case 1241: goto L_1241; case 1275: goto L_1275; case 1313: goto L_1313; case 1351: goto L_1351; case 1389: goto L_1389; case 1390: goto L_1390; case 1418: goto L_1418; case 1419: goto L_1419; case 1424: goto L_1424; case 1455: goto L_1455; case 1486: goto L_1486; case 1524: goto L_1524; case 1562: goto L_1562; case 1563: goto L_1563; case 1591: goto L_1591; case 1592: goto L_1592; case 1597: goto L_1597; case 1626: goto L_1626; case 1627: goto L_1627; case 1628: goto L_1628; case 1642: goto L_1642; case 1643: goto L_1643; case 1720: goto L_1720; case 1797: goto L_1797; case 1835: goto L_1835; case 1912: goto L_1912; case 1989: goto L_1989; case 2027: goto L_2027; case 2065: goto L_2065; case 2066: goto L_2066; case 2067: goto L_2067; case 2081: goto L_2081; case 2082: goto L_2082; case 2083: goto L_2083; case 2103: goto L_2103; case 2104: goto L_2104; case 2105: goto L_2105; case 2116: goto L_2116; case 2117: goto L_2117; case 2122: goto L_2122; case 2123: goto L_2123; case 2143: goto L_2143; case 2144: goto L_2144; case 2145: goto L_2145; case 2156: goto L_2156; case 2157: goto L_2157; case 2162: goto L_2162; case 2239: goto L_2239; case 2316: goto L_2316; case 2354: goto L_2354; case 2355: goto L_2355; case 2356: goto L_2356; case 2370: goto L_2370; case 2371: goto L_2371; case 2372: goto L_2372; case 2392: goto L_2392; case 2393: goto L_2393; case 2394: goto L_2394; case 2405: goto L_2405; case 2406: goto L_2406; case 2411: goto L_2411; case 2485: goto L_2485; case 2486: goto L_2486; case 2487: goto L_2487; case 2501: goto L_2501; case 2502: goto L_2502; case 2503: goto L_2503; case 2523: goto L_2523; case 2524: goto L_2524; case 2525: goto L_2525; case 2536: goto L_2536; case 2537: goto L_2537; case 2542: goto L_2542; case 2616: goto L_2616; case 2617: goto L_2617; case 2618: goto L_2618; case 2632: goto L_2632; case 2633: goto L_2633; case 2634: goto L_2634; case 2654: goto L_2654; case 2655: goto L_2655; case 2660: goto L_2660; case 2661: goto L_2661; case 2662: goto L_2662; case 2696: goto L_2696; case 2725: goto L_2725; case 2754: goto L_2754; case 2792: goto L_2792; case 2830: goto L_2830; case 2868: goto L_2868; case 2869: goto L_2869; case 2897: goto L_2897; case 2898: goto L_2898; case 2903: goto L_2903; case 2904: goto L_2904; case 2905: goto L_2905; case 2910: goto L_2910; case 2911: goto L_2911; case 2912: goto L_2912; case 2917: goto L_2917; case 2950: goto L_2950; case 2984: goto L_2984; case 3017: goto L_3017; case 3051: goto L_3051; case 3089: goto L_3089; case 3090: goto L_3090; case 3118: goto L_3118; case 3119: goto L_3119; case 3124: goto L_3124; case 3125: goto L_3125; case 3126: goto L_3126; case 3131: goto L_3131; case 3132: goto L_3132; case 3133: goto L_3133; case 3138: goto L_3138; case 3139: goto L_3139; case 3140: goto L_3140; case 3171: goto L_3171; case 3202: goto L_3202; case 3240: goto L_3240; case 3241: goto L_3241; case 3269: goto L_3269; case 3270: goto L_3270; case 3275: goto L_3275; case 3276: goto L_3276; case 3277: goto L_3277; case 3282: goto L_3282; case 3283: goto L_3283; case 3284: goto L_3284; case 3289: goto L_3289; case 3290: goto L_3290; case 3291: goto L_3291; case 3296: goto L_3296; case 3297: goto L_3297; case 3298: goto L_3298; case 3303: goto L_3303; case 3304: goto L_3304; case 3305: goto L_3305; case 3310: goto L_3310; case 3356: goto L_3356; case 3402: goto L_3402; case 3403: goto L_3403; case 3422: goto L_3422; case 3423: goto L_3423; case 3477: goto L_3477; case 3514: goto L_3514; case 3515: goto L_3515; case 3542: goto L_3542; case 3543: goto L_3543; case 3597: goto L_3597; case 3647: goto L_3647; case 3693: goto L_3693; case 3694: goto L_3694; case 3721: goto L_3721; case 3722: goto L_3722; case 3772: goto L_3772; case 3826: goto L_3826; case 3872: goto L_3872; case 3918: goto L_3918; case 3919: goto L_3919; case 3946: goto L_3946; case 3947: goto L_3947; case 3997: goto L_3997; case 4051: goto L_4051; case 4097: goto L_4097; case 4143: goto L_4143; case 4144: goto L_4144; case 4171: goto L_4171; case 4172: goto L_4172; case 4222: goto L_4222; case 4276: goto L_4276; case 4322: goto L_4322; case 4368: goto L_4368; case 4369: goto L_4369; case 4396: goto L_4396; case 4397: goto L_4397; case 4451: goto L_4451; case 4488: goto L_4488; case 4560: goto L_4560; case 4647: goto L_4647; case 4684: goto L_4684; case 4718: goto L_4718; case 4756: goto L_4756; case 4757: goto L_4757; case 4776: goto L_4776; case 4777: goto L_4777; case 4782: goto L_4782; case 4783: goto L_4783; case 4784: goto L_4784; case 4785: goto L_4785; case 4786: goto L_4786; case 4787: goto L_4787; case 4788: goto L_4788; case 4812: goto L_4812; case 4841: goto L_4841; case 4842: goto L_4842; case 4843: goto L_4843; case 4857: goto L_4857; case 4858: goto L_4858; case 4859: goto L_4859; case 4860: goto L_4860; case 4884: goto L_4884; case 4885: goto L_4885; case 4921: goto L_4921; case 4950: goto L_4950; case 4951: goto L_4951; case 4965: goto L_4965; case 4966: goto L_4966; case 4967: goto L_4967; case 4986: goto L_4986; case 4987: goto L_4987; case 5016: goto L_5016; case 5017: goto L_5017; case 5053: goto L_5053; case 5082: goto L_5082; case 5083: goto L_5083; case 5097: goto L_5097; case 5098: goto L_5098; case 5099: goto L_5099; case 5118: goto L_5118; case 5119: goto L_5119; case 5124: goto L_5124; case 5125: goto L_5125; case 5126: goto L_5126; case 5128: goto L_5128; }
  L_10:; 
  L_11:; (*u64_ivar(self, "IsRifle")) = u64_bool(0);
  L_22:; 
  L_23:; (*u64_pframe(self, "CallFunc_IsValid_ReturnValue")) = U64_CALL(self, "IsValid", (*u64_ivar(self, "ASC")));
  L_52:; 
  L_53:; 
  L_54:; if (!u64_true((*u64_pframe(self, "CallFunc_IsValid_ReturnValue")))) goto L_296;
  L_68:; 
  L_69:; (*u64_pframe(self, "CallFunc_K2_GetTagCount_ReturnValue")) = ({ u64_v _o = (*u64_ivar(self, "ASC")); U64_CALL(u64_asobj(_o), "K2_GetTagCount", u64_name("State.SprintingRemoval")); });
  L_146:; (*u64_pframe(self, "CallFunc_K2_GetTagCount_ReturnValue_1")) = ({ u64_v _o = (*u64_ivar(self, "ASC")); U64_CALL(u64_asobj(_o), "K2_GetTagCount", u64_name("State.Sprinting")); });
  L_223:; (*u64_pframe(self, "CallFunc_Greater_IntInt_ReturnValue")) = U64_CALL(self, "Greater_IntInt", (*u64_pframe(self, "CallFunc_K2_GetTagCount_ReturnValue_1")), (*u64_pframe(self, "CallFunc_K2_GetTagCount_ReturnValue")));
  L_261:; 
  L_262:; 
  L_263:; if (!u64_true((*u64_pframe(self, "CallFunc_Greater_IntInt_ReturnValue")))) goto L_296;
  L_277:; 
  L_278:; 
  L_279:; (*u64_ivar(self, "IsSprinting")) = u64_bool(1);
  L_290:; 
  L_291:; goto L_5126;
  L_296:; 
  L_297:; (*u64_ivar(self, "IsSprinting")) = u64_bool(0);
  L_308:; 
  L_309:; goto L_5126;
  L_314:; (*u64_pframe(self, "CallFunc_EqualEqual_ByteByte_ReturnValue")) = U64_CALL(self, "EqualEqual_ByteByte", (*u64_ivar(self, "WeaponState")), u64_int(1));
  L_345:; (*u64_pframe(self, "CallFunc_EqualEqual_ByteByte_ReturnValue_1")) = U64_CALL(self, "EqualEqual_ByteByte", (*u64_ivar(self, "WeaponState")), u64_int(3));
  L_376:; (*u64_pframe(self, "CallFunc_BooleanOR_ReturnValue")) = U64_CALL(self, "BooleanOR", (*u64_pframe(self, "CallFunc_EqualEqual_ByteByte_ReturnValue")), (*u64_pframe(self, "CallFunc_EqualEqual_ByteByte_ReturnValue_1")));
  L_414:; 
  L_415:; (*u64_member(&(*u64_ivar(self, "AnimGraphNode_TransitionResult_2")), "bCanEnterTransition")) = (*u64_pframe(self, "CallFunc_BooleanOR_ReturnValue"));
  L_443:; 
  L_444:; goto L_5126;
  L_449:; (*u64_pframe(self, "CallFunc_Greater_FloatFloat_ReturnValue")) = U64_CALL(self, "Greater_FloatFloat", (*u64_ivar(self, "Speed")), u64_float(0.0f));
  L_483:; (*u64_pframe(self, "CallFunc_BooleanAND_ReturnValue")) = U64_CALL(self, "BooleanAND", (*u64_ivar(self, "IsAccelerating")), (*u64_pframe(self, "CallFunc_Greater_FloatFloat_ReturnValue")));
  L_521:; 
  L_522:; (*u64_member(&(*u64_ivar(self, "AnimGraphNode_TransitionResult_1")), "bCanEnterTransition")) = (*u64_pframe(self, "CallFunc_BooleanAND_ReturnValue"));
  L_550:; 
  L_551:; goto L_5126;
  L_556:; (*u64_pframe(self, "CallFunc_NotEqual_ByteByte_ReturnValue")) = U64_CALL(self, "NotEqual_ByteByte", (*u64_ivar(self, "WeaponState")), u64_int(1));
  L_587:; (*u64_pframe(self, "CallFunc_BooleanAND_ReturnValue_1")) = U64_CALL(self, "BooleanAND", (*u64_ivar(self, "IsInAir")), (*u64_pframe(self, "CallFunc_NotEqual_ByteByte_ReturnValue")));
  L_625:; (*u64_pframe(self, "CallFunc_NotEqual_ByteByte_ReturnValue_1")) = U64_CALL(self, "NotEqual_ByteByte", (*u64_ivar(self, "WeaponState")), u64_int(3));
  L_656:; (*u64_pframe(self, "CallFunc_BooleanAND_ReturnValue_2")) = U64_CALL(self, "BooleanAND", u64_ref(&(*u64_pframe(self, "CallFunc_BooleanAND_ReturnValue_1"))), (*u64_pframe(self, "CallFunc_NotEqual_ByteByte_ReturnValue_1")));
  L_694:; 
  L_695:; (*u64_member(&(*u64_ivar(self, "AnimGraphNode_TransitionResult")), "bCanEnterTransition")) = (*u64_pframe(self, "CallFunc_BooleanAND_ReturnValue_2"));
  L_723:; 
  L_724:; goto L_5126;
  L_729:; (*u64_pframe(self, "CallFunc_EqualEqual_ByteByte_ReturnValue_2")) = U64_CALL(self, "EqualEqual_ByteByte", (*u64_ivar(self, "WeaponState")), u64_int(3));
  L_760:; (*u64_pframe(self, "CallFunc_EqualEqual_ByteByte_ReturnValue_3")) = U64_CALL(self, "EqualEqual_ByteByte", (*u64_ivar(self, "WeaponState")), u64_int(1));
  L_791:; (*u64_pframe(self, "CallFunc_BooleanOR_ReturnValue_1")) = U64_CALL(self, "BooleanOR", (*u64_pframe(self, "CallFunc_EqualEqual_ByteByte_ReturnValue_3")), (*u64_pframe(self, "CallFunc_EqualEqual_ByteByte_ReturnValue_2")));
  L_829:; 
  L_830:; (*u64_member(&(*u64_ivar(self, "AnimGraphNode_TransitionResult_6")), "bCanEnterTransition")) = (*u64_pframe(self, "CallFunc_BooleanOR_ReturnValue_1"));
  L_858:; 
  L_859:; goto L_5126;
  L_864:; (*u64_pframe(self, "CallFunc_Not_PreBool_ReturnValue_1")) = U64_CALL(self, "Not_PreBool", (*u64_ivar(self, "IsAccelerating")));
  L_893:; (*u64_pframe(self, "CallFunc_NearlyEqual_FloatFloat_ReturnValue")) = U64_CALL(self, "NearlyEqual_FloatFloat", (*u64_ivar(self, "Speed")), u64_float(0.0f), u64_float(0.009999999776482582f));
  L_932:; (*u64_pframe(self, "CallFunc_BooleanOR_ReturnValue_2")) = U64_CALL(self, "BooleanOR", (*u64_pframe(self, "CallFunc_Not_PreBool_ReturnValue_1")), (*u64_pframe(self, "CallFunc_NearlyEqual_FloatFloat_ReturnValue")));
  L_970:; 
  L_971:; (*u64_member(&(*u64_ivar(self, "AnimGraphNode_TransitionResult_10")), "bCanEnterTransition")) = (*u64_pframe(self, "CallFunc_BooleanOR_ReturnValue_2"));
  L_999:; 
  L_1000:; goto L_5126;
  L_1005:; (*u64_pframe(self, "CallFunc_GetInstanceAssetPlayerTimeFromEnd_ReturnValue")) = U64_CALL(self, "GetInstanceAssetPlayerTimeFromEnd", u64_int(19));
  L_1038:; (*u64_pframe(self, "CallFunc_LessEqual_FloatFloat_ReturnValue")) = U64_CALL(self, "LessEqual_FloatFloat", (*u64_pframe(self, "CallFunc_GetInstanceAssetPlayerTimeFromEnd_ReturnValue")), u64_float(0.10000000149011612f));
  L_1072:; (*u64_pframe(self, "CallFunc_GetInstanceAssetPlayerTimeFromEnd_ReturnValue_1")) = U64_CALL(self, "GetInstanceAssetPlayerTimeFromEnd", u64_int(20));
  L_1105:; (*u64_pframe(self, "CallFunc_LessEqual_FloatFloat_ReturnValue_1")) = U64_CALL(self, "LessEqual_FloatFloat", (*u64_pframe(self, "CallFunc_GetInstanceAssetPlayerTimeFromEnd_ReturnValue_1")), u64_float(0.10000000149011612f));
  L_1139:; (*u64_pframe(self, "CallFunc_BooleanOR_ReturnValue_3")) = U64_CALL(self, "BooleanOR", (*u64_pframe(self, "CallFunc_LessEqual_FloatFloat_ReturnValue")), (*u64_pframe(self, "CallFunc_LessEqual_FloatFloat_ReturnValue_1")));
  L_1177:; 
  L_1178:; (*u64_member(&(*u64_ivar(self, "AnimGraphNode_TransitionResult_7")), "bCanEnterTransition")) = (*u64_pframe(self, "CallFunc_BooleanOR_ReturnValue_3"));
  L_1206:; 
  L_1207:; goto L_5126;
  L_1212:; (*u64_pframe(self, "CallFunc_Not_PreBool_ReturnValue_2")) = U64_CALL(self, "Not_PreBool", (*u64_ivar(self, "IsInAir")));
  L_1241:; (*u64_pframe(self, "CallFunc_Greater_FloatFloat_ReturnValue_1")) = U64_CALL(self, "Greater_FloatFloat", (*u64_ivar(self, "Speed")), u64_float(600.0f));
  L_1275:; (*u64_pframe(self, "CallFunc_BooleanAND_ReturnValue_3")) = U64_CALL(self, "BooleanAND", (*u64_ivar(self, "IsSprinting")), (*u64_ivar(self, "IsAccelerating")));
  L_1313:; (*u64_pframe(self, "CallFunc_BooleanAND_ReturnValue_4")) = U64_CALL(self, "BooleanAND", u64_ref(&(*u64_pframe(self, "CallFunc_BooleanAND_ReturnValue_3"))), (*u64_pframe(self, "CallFunc_Greater_FloatFloat_ReturnValue_1")));
  L_1351:; (*u64_pframe(self, "CallFunc_BooleanAND_ReturnValue_5")) = U64_CALL(self, "BooleanAND", u64_ref(&(*u64_pframe(self, "CallFunc_BooleanAND_ReturnValue_4"))), (*u64_pframe(self, "CallFunc_Not_PreBool_ReturnValue_2")));
  L_1389:; 
  L_1390:; (*u64_member(&(*u64_ivar(self, "AnimGraphNode_TransitionResult_8")), "bCanEnterTransition")) = (*u64_pframe(self, "CallFunc_BooleanAND_ReturnValue_5"));
  L_1418:; 
  L_1419:; goto L_5126;
  L_1424:; (*u64_pframe(self, "CallFunc_NotEqual_ByteByte_ReturnValue_2")) = U64_CALL(self, "NotEqual_ByteByte", (*u64_ivar(self, "WeaponState")), u64_int(3));
  L_1455:; (*u64_pframe(self, "CallFunc_NotEqual_ByteByte_ReturnValue_3")) = U64_CALL(self, "NotEqual_ByteByte", (*u64_ivar(self, "WeaponState")), u64_int(1));
  L_1486:; (*u64_pframe(self, "CallFunc_BooleanAND_ReturnValue_6")) = U64_CALL(self, "BooleanAND", (*u64_ivar(self, "IsInAir")), (*u64_pframe(self, "CallFunc_NotEqual_ByteByte_ReturnValue_3")));
  L_1524:; (*u64_pframe(self, "CallFunc_BooleanAND_ReturnValue_7")) = U64_CALL(self, "BooleanAND", u64_ref(&(*u64_pframe(self, "CallFunc_BooleanAND_ReturnValue_6"))), (*u64_pframe(self, "CallFunc_NotEqual_ByteByte_ReturnValue_2")));
  L_1562:; 
  L_1563:; (*u64_member(&(*u64_ivar(self, "AnimGraphNode_TransitionResult_9")), "bCanEnterTransition")) = (*u64_pframe(self, "CallFunc_BooleanAND_ReturnValue_7"));
  L_1591:; 
  L_1592:; goto L_5126;
  L_1597:; (*u64_pframe(self, "CallFunc_IsValid_ReturnValue_1")) = U64_CALL(self, "IsValid", (*u64_ivar(self, "ASC")));
  L_1626:; 
  L_1627:; 
  L_1628:; if (!u64_true((*u64_pframe(self, "CallFunc_IsValid_ReturnValue_1")))) goto L_2122;
  L_1642:; 
  L_1643:; (*u64_pframe(self, "CallFunc_K2_GetTagCount_ReturnValue_2")) = ({ u64_v _o = (*u64_ivar(self, "ASC")); U64_CALL(u64_asobj(_o), "K2_GetTagCount", u64_name("Weapon.Shotgun.AimingRemoval")); });
  L_1720:; (*u64_pframe(self, "CallFunc_K2_GetTagCount_ReturnValue_3")) = ({ u64_v _o = (*u64_ivar(self, "ASC")); U64_CALL(u64_asobj(_o), "K2_GetTagCount", u64_name("Weapon.Shotgun.Aiming")); });
  L_1797:; (*u64_pframe(self, "CallFunc_Greater_IntInt_ReturnValue_1")) = U64_CALL(self, "Greater_IntInt", (*u64_pframe(self, "CallFunc_K2_GetTagCount_ReturnValue_3")), (*u64_pframe(self, "CallFunc_K2_GetTagCount_ReturnValue_2")));
  L_1835:; (*u64_pframe(self, "CallFunc_K2_GetTagCount_ReturnValue_6")) = ({ u64_v _o = (*u64_ivar(self, "ASC")); U64_CALL(u64_asobj(_o), "K2_GetTagCount", u64_name("Weapon.Rifle.AimingRemoval")); });
  L_1912:; (*u64_pframe(self, "CallFunc_K2_GetTagCount_ReturnValue_7")) = ({ u64_v _o = (*u64_ivar(self, "ASC")); U64_CALL(u64_asobj(_o), "K2_GetTagCount", u64_name("Weapon.Rifle.Aiming")); });
  L_1989:; (*u64_pframe(self, "CallFunc_Greater_IntInt_ReturnValue_3")) = U64_CALL(self, "Greater_IntInt", (*u64_pframe(self, "CallFunc_K2_GetTagCount_ReturnValue_7")), (*u64_pframe(self, "CallFunc_K2_GetTagCount_ReturnValue_6")));
  L_2027:; (*u64_pframe(self, "CallFunc_BooleanOR_ReturnValue_4")) = U64_CALL(self, "BooleanOR", (*u64_pframe(self, "CallFunc_Greater_IntInt_ReturnValue_3")), (*u64_pframe(self, "CallFunc_Greater_IntInt_ReturnValue_1")));
  L_2065:; 
  L_2066:; 
  L_2067:; if (!u64_true((*u64_pframe(self, "CallFunc_BooleanOR_ReturnValue_4")))) goto L_2162;
  L_2081:; 
  L_2082:; 
  L_2083:; (*u64_ivar(self, "WeaponState")) = u64_int(1);
  L_2103:; 
  L_2104:; 
  L_2105:; (*u64_ivar(self, "IsRifle")) = u64_bool(1);
  L_2116:; 
  L_2117:; goto L_23;
  L_2122:; 
  L_2123:; (*u64_ivar(self, "WeaponState")) = u64_int(0);
  L_2143:; 
  L_2144:; 
  L_2145:; (*u64_ivar(self, "IsRifle")) = u64_bool(1);
  L_2156:; 
  L_2157:; goto L_23;
  L_2162:; (*u64_pframe(self, "CallFunc_K2_GetTagCount_ReturnValue_4")) = ({ u64_v _o = (*u64_ivar(self, "ASC")); U64_CALL(u64_asobj(_o), "K2_GetTagCount", u64_name("Weapon.RocketLauncher.AimingRemoval")); });
  L_2239:; (*u64_pframe(self, "CallFunc_K2_GetTagCount_ReturnValue_5")) = ({ u64_v _o = (*u64_ivar(self, "ASC")); U64_CALL(u64_asobj(_o), "K2_GetTagCount", u64_name("Weapon.RocketLauncher.Aiming")); });
  L_2316:; (*u64_pframe(self, "CallFunc_Greater_IntInt_ReturnValue_2")) = U64_CALL(self, "Greater_IntInt", (*u64_pframe(self, "CallFunc_K2_GetTagCount_ReturnValue_5")), (*u64_pframe(self, "CallFunc_K2_GetTagCount_ReturnValue_4")));
  L_2354:; 
  L_2355:; 
  L_2356:; if (!u64_true((*u64_pframe(self, "CallFunc_Greater_IntInt_ReturnValue_2")))) goto L_2411;
  L_2370:; 
  L_2371:; 
  L_2372:; (*u64_ivar(self, "WeaponState")) = u64_int(3);
  L_2392:; 
  L_2393:; 
  L_2394:; (*u64_ivar(self, "IsRifle")) = u64_bool(0);
  L_2405:; 
  L_2406:; goto L_23;
  L_2411:; (*u64_pframe(self, "CallFunc_HasMatchingGameplayTag_ReturnValue_1")) = ({ u64_v _o = (*u64_ivar(self, "ASC")); U64_CALL(u64_asobj(_o), "HasMatchingGameplayTag", u64_name("Weapon.Equipped.Rifle")); });
  L_2485:; 
  L_2486:; 
  L_2487:; if (!u64_true((*u64_pframe(self, "CallFunc_HasMatchingGameplayTag_ReturnValue_1")))) goto L_2542;
  L_2501:; 
  L_2502:; 
  L_2503:; (*u64_ivar(self, "WeaponState")) = u64_int(0);
  L_2523:; 
  L_2524:; 
  L_2525:; (*u64_ivar(self, "IsRifle")) = u64_bool(1);
  L_2536:; 
  L_2537:; goto L_23;
  L_2542:; (*u64_pframe(self, "CallFunc_HasMatchingGameplayTag_ReturnValue")) = ({ u64_v _o = (*u64_ivar(self, "ASC")); U64_CALL(u64_asobj(_o), "HasMatchingGameplayTag", u64_name("Weapon.Equipped.RocketLauncher")); });
  L_2616:; 
  L_2617:; 
  L_2618:; if (!u64_true((*u64_pframe(self, "CallFunc_HasMatchingGameplayTag_ReturnValue")))) goto L_2122;
  L_2632:; 
  L_2633:; 
  L_2634:; (*u64_ivar(self, "WeaponState")) = u64_int(2);
  L_2654:; 
  L_2655:; goto L_10;
  L_2660:; 
  L_2661:; 
  L_2662:; (*u64_pframe(self, "CallFunc_Less_FloatFloat_ReturnValue")) = U64_CALL(self, "Less_FloatFloat", (*u64_ivar(self, "Speed")), u64_float(500.0f));
  L_2696:; (*u64_pframe(self, "CallFunc_Not_PreBool_ReturnValue")) = U64_CALL(self, "Not_PreBool", (*u64_ivar(self, "IsAccelerating")));
  L_2725:; (*u64_pframe(self, "CallFunc_Not_PreBool_ReturnValue_3")) = U64_CALL(self, "Not_PreBool", (*u64_ivar(self, "IsSprinting")));
  L_2754:; (*u64_pframe(self, "CallFunc_BooleanOR_ReturnValue_6")) = U64_CALL(self, "BooleanOR", (*u64_pframe(self, "CallFunc_Not_PreBool_ReturnValue_3")), (*u64_pframe(self, "CallFunc_Not_PreBool_ReturnValue")));
  L_2792:; (*u64_pframe(self, "CallFunc_BooleanOR_ReturnValue_7")) = U64_CALL(self, "BooleanOR", u64_ref(&(*u64_pframe(self, "CallFunc_BooleanOR_ReturnValue_6"))), (*u64_pframe(self, "CallFunc_Less_FloatFloat_ReturnValue")));
  L_2830:; (*u64_pframe(self, "CallFunc_BooleanOR_ReturnValue_8")) = U64_CALL(self, "BooleanOR", u64_ref(&(*u64_pframe(self, "CallFunc_BooleanOR_ReturnValue_7"))), (*u64_ivar(self, "IsInAir")));
  L_2868:; 
  L_2869:; (*u64_member(&(*u64_ivar(self, "AnimGraphNode_TransitionResult_12")), "bCanEnterTransition")) = (*u64_pframe(self, "CallFunc_BooleanOR_ReturnValue_8"));
  L_2897:; 
  L_2898:; goto L_5126;
  L_2903:; 
  L_2904:; 
  L_2905:; goto L_556;
  L_2910:; 
  L_2911:; 
  L_2912:; goto L_449;
  L_2917:; (*u64_pframe(self, "CallFunc_GetInstanceAssetPlayerTimeFromEnd_ReturnValue_2")) = U64_CALL(self, "GetInstanceAssetPlayerTimeFromEnd", u64_int(11));
  L_2950:; (*u64_pframe(self, "CallFunc_LessEqual_FloatFloat_ReturnValue_2")) = U64_CALL(self, "LessEqual_FloatFloat", (*u64_pframe(self, "CallFunc_GetInstanceAssetPlayerTimeFromEnd_ReturnValue_2")), u64_float(0.10000000149011612f));
  L_2984:; (*u64_pframe(self, "CallFunc_GetInstanceAssetPlayerTimeFromEnd_ReturnValue_3")) = U64_CALL(self, "GetInstanceAssetPlayerTimeFromEnd", u64_int(12));
  L_3017:; (*u64_pframe(self, "CallFunc_LessEqual_FloatFloat_ReturnValue_3")) = U64_CALL(self, "LessEqual_FloatFloat", (*u64_pframe(self, "CallFunc_GetInstanceAssetPlayerTimeFromEnd_ReturnValue_3")), u64_float(0.10000000149011612f));
  L_3051:; (*u64_pframe(self, "CallFunc_BooleanOR_ReturnValue_5")) = U64_CALL(self, "BooleanOR", (*u64_pframe(self, "CallFunc_LessEqual_FloatFloat_ReturnValue_2")), (*u64_pframe(self, "CallFunc_LessEqual_FloatFloat_ReturnValue_3")));
  L_3089:; 
  L_3090:; (*u64_member(&(*u64_ivar(self, "AnimGraphNode_TransitionResult_3")), "bCanEnterTransition")) = (*u64_pframe(self, "CallFunc_BooleanOR_ReturnValue_5"));
  L_3118:; 
  L_3119:; goto L_5126;
  L_3124:; 
  L_3125:; 
  L_3126:; goto L_314;
  L_3131:; 
  L_3132:; 
  L_3133:; goto L_2917;
  L_3138:; 
  L_3139:; 
  L_3140:; (*u64_pframe(self, "CallFunc_EqualEqual_ByteByte_ReturnValue_4")) = U64_CALL(self, "EqualEqual_ByteByte", (*u64_ivar(self, "WeaponState")), u64_int(1));
  L_3171:; (*u64_pframe(self, "CallFunc_EqualEqual_ByteByte_ReturnValue_5")) = U64_CALL(self, "EqualEqual_ByteByte", (*u64_ivar(self, "WeaponState")), u64_int(3));
  L_3202:; (*u64_pframe(self, "CallFunc_BooleanOR_ReturnValue_10")) = U64_CALL(self, "BooleanOR", (*u64_pframe(self, "CallFunc_EqualEqual_ByteByte_ReturnValue_4")), (*u64_pframe(self, "CallFunc_EqualEqual_ByteByte_ReturnValue_5")));
  L_3240:; 
  L_3241:; (*u64_member(&(*u64_ivar(self, "AnimGraphNode_TransitionResult_4")), "bCanEnterTransition")) = (*u64_pframe(self, "CallFunc_BooleanOR_ReturnValue_10"));
  L_3269:; 
  L_3270:; goto L_5126;
  L_3275:; 
  L_3276:; 
  L_3277:; goto L_864;
  L_3282:; 
  L_3283:; 
  L_3284:; goto L_729;
  L_3289:; 
  L_3290:; 
  L_3291:; goto L_1005;
  L_3296:; 
  L_3297:; 
  L_3298:; goto L_1424;
  L_3303:; 
  L_3304:; 
  L_3305:; goto L_1212;
  L_3310:; (*u64_pframe(self, "CallFunc_GetMovementComponent_ReturnValue")) = ({ u64_v _o = (*u64_ivar(self, "GSCharacter")); U64_CALL(u64_asobj(_o), "GetMovementComponent"); });
  L_3356:; (*u64_pframe(self, "CallFunc_IsFalling_ReturnValue")) = ({ u64_v _o = (*u64_pframe(self, "CallFunc_GetMovementComponent_ReturnValue")); U64_CALL(u64_asobj(_o), "IsFalling"); });
  L_3402:; 
  L_3403:; (*u64_ivar(self, "IsInAir")) = (*u64_pframe(self, "CallFunc_IsFalling_ReturnValue"));
  L_3422:; 
  L_3423:; (*u64_pframe(self, "CallFunc_GetVelocity_ReturnValue")) = ({ u64_v _o = (*u64_ivar(self, "GSCharacter")); U64_CALL(u64_asobj(_o), "GetVelocity"); });
  L_3477:; (*u64_pframe(self, "CallFunc_VSize_ReturnValue")) = U64_CALL(self, "VSize", (*u64_pframe(self, "CallFunc_GetVelocity_ReturnValue")));
  L_3514:; 
  L_3515:; (*u64_ivar(self, "Speed")) = (*u64_pframe(self, "CallFunc_VSize_ReturnValue"));
  L_3542:; 
  L_3543:; (*u64_pframe(self, "CallFunc_GetVelocity_ReturnValue")) = ({ u64_v _o = (*u64_ivar(self, "GSCharacter")); U64_CALL(u64_asobj(_o), "GetVelocity"); });
  L_3597:; (*u64_pframe(self, "CallFunc_K2_GetActorRotation_ReturnValue")) = ({ u64_v _o = (*u64_ivar(self, "GSCharacter")); U64_CALL(u64_asobj(_o), "K2_GetActorRotation"); });
  L_3647:; (*u64_pframe(self, "CallFunc_CalculateDirection_ReturnValue")) = U64_CALL(self, "CalculateDirection", (*u64_pframe(self, "CallFunc_GetVelocity_ReturnValue")), (*u64_pframe(self, "CallFunc_K2_GetActorRotation_ReturnValue")));
  L_3693:; 
  L_3694:; (*u64_ivar(self, "Direction")) = (*u64_pframe(self, "CallFunc_CalculateDirection_ReturnValue"));
  L_3721:; 
  L_3722:; (*u64_pframe(self, "CallFunc_K2_GetActorRotation_ReturnValue_1")) = ({ u64_v _o = (*u64_ivar(self, "GSCharacter")); U64_CALL(u64_asobj(_o), "K2_GetActorRotation"); });
  L_3772:; (*u64_pframe(self, "CallFunc_GetBaseAimRotation_ReturnValue")) = ({ u64_v _o = (*u64_ivar(self, "GSCharacter")); U64_CALL(u64_asobj(_o), "GetBaseAimRotation"); });
  L_3826:; (*u64_pframe(self, "CallFunc_NormalizedDeltaRotator_ReturnValue")) = U64_CALL(self, "NormalizedDeltaRotator", (*u64_pframe(self, "CallFunc_GetBaseAimRotation_ReturnValue")), (*u64_pframe(self, "CallFunc_K2_GetActorRotation_ReturnValue_1")));
  L_3872:; U64_CALL(self, "BreakRotator", (*u64_pframe(self, "CallFunc_NormalizedDeltaRotator_ReturnValue")), u64_ref(&(*u64_pframe(self, "CallFunc_BreakRotator_Roll"))), u64_ref(&(*u64_pframe(self, "CallFunc_BreakRotator_Pitch"))), u64_ref(&(*u64_pframe(self, "CallFunc_BreakRotator_Yaw"))));
  L_3918:; 
  L_3919:; (*u64_ivar(self, "Roll")) = (*u64_pframe(self, "CallFunc_BreakRotator_Roll"));
  L_3946:; 
  L_3947:; (*u64_pframe(self, "CallFunc_K2_GetActorRotation_ReturnValue_1")) = ({ u64_v _o = (*u64_ivar(self, "GSCharacter")); U64_CALL(u64_asobj(_o), "K2_GetActorRotation"); });
  L_3997:; (*u64_pframe(self, "CallFunc_GetBaseAimRotation_ReturnValue")) = ({ u64_v _o = (*u64_ivar(self, "GSCharacter")); U64_CALL(u64_asobj(_o), "GetBaseAimRotation"); });
  L_4051:; (*u64_pframe(self, "CallFunc_NormalizedDeltaRotator_ReturnValue")) = U64_CALL(self, "NormalizedDeltaRotator", (*u64_pframe(self, "CallFunc_GetBaseAimRotation_ReturnValue")), (*u64_pframe(self, "CallFunc_K2_GetActorRotation_ReturnValue_1")));
  L_4097:; U64_CALL(self, "BreakRotator", (*u64_pframe(self, "CallFunc_NormalizedDeltaRotator_ReturnValue")), u64_ref(&(*u64_pframe(self, "CallFunc_BreakRotator_Roll"))), u64_ref(&(*u64_pframe(self, "CallFunc_BreakRotator_Pitch"))), u64_ref(&(*u64_pframe(self, "CallFunc_BreakRotator_Yaw"))));
  L_4143:; 
  L_4144:; (*u64_ivar(self, "Pitch")) = (*u64_pframe(self, "CallFunc_BreakRotator_Pitch"));
  L_4171:; 
  L_4172:; (*u64_pframe(self, "CallFunc_K2_GetActorRotation_ReturnValue_1")) = ({ u64_v _o = (*u64_ivar(self, "GSCharacter")); U64_CALL(u64_asobj(_o), "K2_GetActorRotation"); });
  L_4222:; (*u64_pframe(self, "CallFunc_GetBaseAimRotation_ReturnValue")) = ({ u64_v _o = (*u64_ivar(self, "GSCharacter")); U64_CALL(u64_asobj(_o), "GetBaseAimRotation"); });
  L_4276:; (*u64_pframe(self, "CallFunc_NormalizedDeltaRotator_ReturnValue")) = U64_CALL(self, "NormalizedDeltaRotator", (*u64_pframe(self, "CallFunc_GetBaseAimRotation_ReturnValue")), (*u64_pframe(self, "CallFunc_K2_GetActorRotation_ReturnValue_1")));
  L_4322:; U64_CALL(self, "BreakRotator", (*u64_pframe(self, "CallFunc_NormalizedDeltaRotator_ReturnValue")), u64_ref(&(*u64_pframe(self, "CallFunc_BreakRotator_Roll"))), u64_ref(&(*u64_pframe(self, "CallFunc_BreakRotator_Pitch"))), u64_ref(&(*u64_pframe(self, "CallFunc_BreakRotator_Yaw"))));
  L_4368:; 
  L_4369:; (*u64_ivar(self, "Yaw")) = (*u64_pframe(self, "CallFunc_BreakRotator_Yaw"));
  L_4396:; 
  L_4397:; (*u64_pframe(self, "CallFunc_GetVelocity_ReturnValue_1")) = ({ u64_v _o = (*u64_ivar(self, "GSCharacter")); U64_CALL(u64_asobj(_o), "GetVelocity"); });
  L_4451:; (*u64_pframe(self, "CallFunc_VSize_ReturnValue_1")) = U64_CALL(self, "VSize", (*u64_pframe(self, "CallFunc_GetVelocity_ReturnValue_1")));
  L_4488:; (*u64_pframe(self, "CallFunc_GetCurrentAcceleration_ReturnValue")) = ({ u64_v _o = ({ u64_v _o = (*u64_ivar(self, "GSCharacter")); (*u64_ivar(u64_asobj(_o), "CharacterMovement")); }); U64_CALL(u64_asobj(_o), "GetCurrentAcceleration"); });
  L_4560:; (*u64_pframe(self, "CallFunc_NearlyEqual_FloatFloat_ReturnValue_1")) = U64_CALL(self, "NearlyEqual_FloatFloat", (*u64_pframe(self, "CallFunc_VSize_ReturnValue_1")), ({ u64_v _o = ({ u64_v _o = (*u64_ivar(self, "GSCharacter")); (*u64_ivar(u64_asobj(_o), "CharacterMovement")); }); (*u64_ivar(u64_asobj(_o), "MaxWalkSpeed")); }), u64_float(0.10000000149011612f));
  L_4647:; (*u64_pframe(self, "CallFunc_VSize_ReturnValue_2")) = U64_CALL(self, "VSize", (*u64_pframe(self, "CallFunc_GetCurrentAcceleration_ReturnValue")));
  L_4684:; (*u64_pframe(self, "CallFunc_Greater_FloatFloat_ReturnValue_2")) = U64_CALL(self, "Greater_FloatFloat", (*u64_pframe(self, "CallFunc_VSize_ReturnValue_2")), u64_float(0.0f));
  L_4718:; (*u64_pframe(self, "CallFunc_BooleanOR_ReturnValue_9")) = U64_CALL(self, "BooleanOR", (*u64_pframe(self, "CallFunc_Greater_FloatFloat_ReturnValue_2")), (*u64_pframe(self, "CallFunc_NearlyEqual_FloatFloat_ReturnValue_1")));
  L_4756:; 
  L_4757:; (*u64_ivar(self, "IsAccelerating")) = (*u64_pframe(self, "CallFunc_BooleanOR_ReturnValue_9"));
  L_4776:; 
  L_4777:; goto L_1597;
  L_4782:; 
  L_4783:; 
  L_4784:; 
  L_4785:; 
  L_4786:; 
  L_4787:; 
  L_4788:; (*u64_pframe(self, "CallFunc_TryGetPawnOwner_ReturnValue")) = U64_CALL(self, "TryGetPawnOwner");
  L_4812:; (*u64_pframe(self, "CallFunc_IsValid_ReturnValue_2")) = U64_CALL(self, "IsValid", (*u64_pframe(self, "CallFunc_TryGetPawnOwner_ReturnValue")));
  L_4841:; 
  L_4842:; 
  L_4843:; if (!u64_true((*u64_pframe(self, "CallFunc_IsValid_ReturnValue_2")))) goto L_5124;
  L_4857:; 
  L_4858:; 
  L_4859:; 
  L_4860:; (*u64_pframe(self, "CallFunc_TryGetPawnOwner_ReturnValue")) = U64_CALL(self, "TryGetPawnOwner");
  L_4884:; 
  L_4885:; (*u64_pframe(self, "K2Node_DynamicCast_AsGSCharacter_Base")) = u64_cast((*u64_pframe(self, "CallFunc_TryGetPawnOwner_ReturnValue")), "GSCharacterBase");
  L_4921:; (*u64_pframe(self, "K2Node_DynamicCast_bSuccess")) = (*u64_pframe(self, "K2Node_DynamicCast_AsGSCharacter_Base"));
  L_4950:; 
  L_4951:; if (!u64_true((*u64_pframe(self, "K2Node_DynamicCast_bSuccess")))) goto L_5126;
  L_4965:; 
  L_4966:; 
  L_4967:; (*u64_ivar(self, "GSCharacter")) = (*u64_pframe(self, "K2Node_DynamicCast_AsGSCharacter_Base"));
  L_4986:; 
  L_4987:; (*u64_pframe(self, "CallFunc_GetAbilitySystemComponent_ReturnValue")) = U64_CALL(self, "GetAbilitySystemComponent", (*u64_ivar(self, "GSCharacter")));
  L_5016:; 
  L_5017:; (*u64_pframe(self, "K2Node_DynamicCast_AsGSAbility_System_Component")) = u64_cast((*u64_pframe(self, "CallFunc_GetAbilitySystemComponent_ReturnValue")), "GSAbilitySystemComponent");
  L_5053:; (*u64_pframe(self, "K2Node_DynamicCast_bSuccess_1")) = (*u64_pframe(self, "K2Node_DynamicCast_AsGSAbility_System_Component"));
  L_5082:; 
  L_5083:; if (!u64_true((*u64_pframe(self, "K2Node_DynamicCast_bSuccess_1")))) goto L_3310;
  L_5097:; 
  L_5098:; 
  L_5099:; (*u64_ivar(self, "ASC")) = (*u64_pframe(self, "K2Node_DynamicCast_AsGSAbility_System_Component"));
  L_5118:; 
  L_5119:; goto L_3310;
  L_5124:; 
  L_5125:; 
  L_5126:; goto u64_ret;
  L_5128:; 
  u64_ret:; u64_set_return(L.ReturnValue);

}
