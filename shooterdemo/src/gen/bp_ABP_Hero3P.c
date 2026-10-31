#include "u64_rt.h"

void bp_ABP_Hero3P__ExecuteUbergraph_ABP_Hero3P(u64_obj *self, const u64_v *args, int argc) {
  (void)args; (void)argc;
  struct { u64_v ReturnValue; u64_v _unsupported; } L; memset(&L, 0, sizeof L); (void)L;
  U64_FLOW_DECL; (void)flow_n;
  if (argc > 0) (*u64_pframe(self, "EntryPoint")) = args[0];
  L_0:; u64_dispatch: switch (u64_as_int((*u64_pframe(self, "EntryPoint")))) { case 0: goto L_0; case 10: goto L_10; case 11: goto L_11; case 12: goto L_12; case 17: goto L_17; case 50: goto L_50; case 84: goto L_84; case 85: goto L_85; case 113: goto L_113; case 114: goto L_114; case 119: goto L_119; case 120: goto L_120; case 121: goto L_121; case 122: goto L_122; case 123: goto L_123; case 147: goto L_147; case 176: goto L_176; case 177: goto L_177; case 178: goto L_178; case 192: goto L_192; case 193: goto L_193; case 194: goto L_194; case 195: goto L_195; case 219: goto L_219; case 220: goto L_220; case 256: goto L_256; case 285: goto L_285; case 286: goto L_286; case 300: goto L_300; case 301: goto L_301; case 302: goto L_302; case 321: goto L_321; case 322: goto L_322; case 351: goto L_351; case 352: goto L_352; case 388: goto L_388; case 417: goto L_417; case 418: goto L_418; case 432: goto L_432; case 433: goto L_433; case 434: goto L_434; case 453: goto L_453; case 454: goto L_454; case 500: goto L_500; case 546: goto L_546; case 547: goto L_547; case 566: goto L_566; case 567: goto L_567; case 621: goto L_621; case 658: goto L_658; case 659: goto L_659; case 686: goto L_686; case 687: goto L_687; case 741: goto L_741; case 791: goto L_791; case 837: goto L_837; case 838: goto L_838; case 865: goto L_865; case 866: goto L_866; case 916: goto L_916; case 970: goto L_970; case 1016: goto L_1016; case 1062: goto L_1062; case 1063: goto L_1063; case 1090: goto L_1090; case 1091: goto L_1091; case 1141: goto L_1141; case 1195: goto L_1195; case 1241: goto L_1241; case 1287: goto L_1287; case 1288: goto L_1288; case 1315: goto L_1315; case 1316: goto L_1316; case 1366: goto L_1366; case 1420: goto L_1420; case 1466: goto L_1466; case 1512: goto L_1512; case 1513: goto L_1513; case 1540: goto L_1540; case 1541: goto L_1541; case 1595: goto L_1595; case 1632: goto L_1632; case 1704: goto L_1704; case 1791: goto L_1791; case 1828: goto L_1828; case 1862: goto L_1862; case 1900: goto L_1900; case 1901: goto L_1901; case 1920: goto L_1920; case 1921: goto L_1921; case 1971: goto L_1971; case 2047: goto L_2047; case 2089: goto L_2089; case 2135: goto L_2135; case 2177: goto L_2177; case 2178: goto L_2178; case 2205: goto L_2205; case 2206: goto L_2206; case 2211: goto L_2211; case 2244: goto L_2244; case 2278: goto L_2278; case 2279: goto L_2279; case 2307: goto L_2307; case 2308: goto L_2308; case 2313: goto L_2313; case 2314: goto L_2314; case 2315: goto L_2315; case 2320: goto L_2320; case 2321: goto L_2321; case 2322: goto L_2322; case 2327: goto L_2327; case 2328: goto L_2328; case 2329: goto L_2329; case 2334: goto L_2334; case 2336: goto L_2336; }
  L_10:; 
  L_11:; 
  L_12:; goto L_2334;
  L_17:; (*u64_pframe(self, "CallFunc_GetInstanceAssetPlayerTimeFromEnd_ReturnValue")) = U64_CALL(self, "GetInstanceAssetPlayerTimeFromEnd", u64_int(3));
  L_50:; (*u64_pframe(self, "CallFunc_LessEqual_FloatFloat_ReturnValue")) = U64_CALL(self, "LessEqual_FloatFloat", (*u64_pframe(self, "CallFunc_GetInstanceAssetPlayerTimeFromEnd_ReturnValue")), u64_float(0.30000001192092896f));
  L_84:; 
  L_85:; (*u64_member(&(*u64_ivar(self, "AnimGraphNode_TransitionResult_C38F580E460C7AAE38E92DB5C7F3ECC5")), "bCanEnterTransition")) = (*u64_pframe(self, "CallFunc_LessEqual_FloatFloat_ReturnValue"));
  L_113:; 
  L_114:; goto L_2334;
  L_119:; 
  L_120:; 
  L_121:; 
  L_122:; 
  L_123:; (*u64_pframe(self, "CallFunc_TryGetPawnOwner_ReturnValue")) = U64_CALL(self, "TryGetPawnOwner");
  L_147:; (*u64_pframe(self, "CallFunc_IsValid_ReturnValue")) = U64_CALL(self, "IsValid", (*u64_pframe(self, "CallFunc_TryGetPawnOwner_ReturnValue")));
  L_176:; 
  L_177:; 
  L_178:; if (!u64_true((*u64_pframe(self, "CallFunc_IsValid_ReturnValue")))) goto L_10;
  L_192:; 
  L_193:; 
  L_194:; 
  L_195:; (*u64_pframe(self, "CallFunc_TryGetPawnOwner_ReturnValue")) = U64_CALL(self, "TryGetPawnOwner");
  L_219:; 
  L_220:; (*u64_pframe(self, "K2Node_DynamicCast_AsGSCharacter_Base")) = u64_cast((*u64_pframe(self, "CallFunc_TryGetPawnOwner_ReturnValue")), "GSCharacterBase");
  L_256:; (*u64_pframe(self, "K2Node_DynamicCast_bSuccess")) = (*u64_pframe(self, "K2Node_DynamicCast_AsGSCharacter_Base"));
  L_285:; 
  L_286:; if (!u64_true((*u64_pframe(self, "K2Node_DynamicCast_bSuccess")))) goto L_2334;
  L_300:; 
  L_301:; 
  L_302:; (*u64_ivar(self, "GSCharacter")) = (*u64_pframe(self, "K2Node_DynamicCast_AsGSCharacter_Base"));
  L_321:; 
  L_322:; (*u64_pframe(self, "CallFunc_GetAbilitySystemComponent_ReturnValue")) = U64_CALL(self, "GetAbilitySystemComponent", (*u64_ivar(self, "GSCharacter")));
  L_351:; 
  L_352:; (*u64_pframe(self, "K2Node_DynamicCast_AsGSAbility_System_Component")) = u64_cast((*u64_pframe(self, "CallFunc_GetAbilitySystemComponent_ReturnValue")), "GSAbilitySystemComponent");
  L_388:; (*u64_pframe(self, "K2Node_DynamicCast_bSuccess_1")) = (*u64_pframe(self, "K2Node_DynamicCast_AsGSAbility_System_Component"));
  L_417:; 
  L_418:; if (!u64_true((*u64_pframe(self, "K2Node_DynamicCast_bSuccess_1")))) goto L_454;
  L_432:; 
  L_433:; 
  L_434:; (*u64_ivar(self, "ASC")) = (*u64_pframe(self, "K2Node_DynamicCast_AsGSAbility_System_Component"));
  L_453:; 
  L_454:; (*u64_pframe(self, "CallFunc_GetMovementComponent_ReturnValue")) = ({ u64_v _o = (*u64_ivar(self, "GSCharacter")); U64_CALL(u64_asobj(_o), "GetMovementComponent"); });
  L_500:; (*u64_pframe(self, "CallFunc_IsFalling_ReturnValue")) = ({ u64_v _o = (*u64_pframe(self, "CallFunc_GetMovementComponent_ReturnValue")); U64_CALL(u64_asobj(_o), "IsFalling"); });
  L_546:; 
  L_547:; (*u64_ivar(self, "IsInAir")) = (*u64_pframe(self, "CallFunc_IsFalling_ReturnValue"));
  L_566:; 
  L_567:; (*u64_pframe(self, "CallFunc_GetVelocity_ReturnValue")) = ({ u64_v _o = (*u64_ivar(self, "GSCharacter")); U64_CALL(u64_asobj(_o), "GetVelocity"); });
  L_621:; (*u64_pframe(self, "CallFunc_VSize_ReturnValue")) = U64_CALL(self, "VSize", (*u64_pframe(self, "CallFunc_GetVelocity_ReturnValue")));
  L_658:; 
  L_659:; (*u64_ivar(self, "Speed")) = (*u64_pframe(self, "CallFunc_VSize_ReturnValue"));
  L_686:; 
  L_687:; (*u64_pframe(self, "CallFunc_GetVelocity_ReturnValue")) = ({ u64_v _o = (*u64_ivar(self, "GSCharacter")); U64_CALL(u64_asobj(_o), "GetVelocity"); });
  L_741:; (*u64_pframe(self, "CallFunc_K2_GetActorRotation_ReturnValue")) = ({ u64_v _o = (*u64_ivar(self, "GSCharacter")); U64_CALL(u64_asobj(_o), "K2_GetActorRotation"); });
  L_791:; (*u64_pframe(self, "CallFunc_CalculateDirection_ReturnValue")) = U64_CALL(self, "CalculateDirection", (*u64_pframe(self, "CallFunc_GetVelocity_ReturnValue")), (*u64_pframe(self, "CallFunc_K2_GetActorRotation_ReturnValue")));
  L_837:; 
  L_838:; (*u64_ivar(self, "Direction")) = (*u64_pframe(self, "CallFunc_CalculateDirection_ReturnValue"));
  L_865:; 
  L_866:; (*u64_pframe(self, "CallFunc_K2_GetActorRotation_ReturnValue_1")) = ({ u64_v _o = (*u64_ivar(self, "GSCharacter")); U64_CALL(u64_asobj(_o), "K2_GetActorRotation"); });
  L_916:; (*u64_pframe(self, "CallFunc_GetBaseAimRotation_ReturnValue")) = ({ u64_v _o = (*u64_ivar(self, "GSCharacter")); U64_CALL(u64_asobj(_o), "GetBaseAimRotation"); });
  L_970:; (*u64_pframe(self, "CallFunc_NormalizedDeltaRotator_ReturnValue")) = U64_CALL(self, "NormalizedDeltaRotator", (*u64_pframe(self, "CallFunc_GetBaseAimRotation_ReturnValue")), (*u64_pframe(self, "CallFunc_K2_GetActorRotation_ReturnValue_1")));
  L_1016:; U64_CALL(self, "BreakRotator", (*u64_pframe(self, "CallFunc_NormalizedDeltaRotator_ReturnValue")), u64_ref(&(*u64_pframe(self, "CallFunc_BreakRotator_Roll"))), u64_ref(&(*u64_pframe(self, "CallFunc_BreakRotator_Pitch"))), u64_ref(&(*u64_pframe(self, "CallFunc_BreakRotator_Yaw"))));
  L_1062:; 
  L_1063:; (*u64_ivar(self, "Roll")) = (*u64_pframe(self, "CallFunc_BreakRotator_Roll"));
  L_1090:; 
  L_1091:; (*u64_pframe(self, "CallFunc_K2_GetActorRotation_ReturnValue_1")) = ({ u64_v _o = (*u64_ivar(self, "GSCharacter")); U64_CALL(u64_asobj(_o), "K2_GetActorRotation"); });
  L_1141:; (*u64_pframe(self, "CallFunc_GetBaseAimRotation_ReturnValue")) = ({ u64_v _o = (*u64_ivar(self, "GSCharacter")); U64_CALL(u64_asobj(_o), "GetBaseAimRotation"); });
  L_1195:; (*u64_pframe(self, "CallFunc_NormalizedDeltaRotator_ReturnValue")) = U64_CALL(self, "NormalizedDeltaRotator", (*u64_pframe(self, "CallFunc_GetBaseAimRotation_ReturnValue")), (*u64_pframe(self, "CallFunc_K2_GetActorRotation_ReturnValue_1")));
  L_1241:; U64_CALL(self, "BreakRotator", (*u64_pframe(self, "CallFunc_NormalizedDeltaRotator_ReturnValue")), u64_ref(&(*u64_pframe(self, "CallFunc_BreakRotator_Roll"))), u64_ref(&(*u64_pframe(self, "CallFunc_BreakRotator_Pitch"))), u64_ref(&(*u64_pframe(self, "CallFunc_BreakRotator_Yaw"))));
  L_1287:; 
  L_1288:; (*u64_ivar(self, "Pitch")) = (*u64_pframe(self, "CallFunc_BreakRotator_Pitch"));
  L_1315:; 
  L_1316:; (*u64_pframe(self, "CallFunc_K2_GetActorRotation_ReturnValue_1")) = ({ u64_v _o = (*u64_ivar(self, "GSCharacter")); U64_CALL(u64_asobj(_o), "K2_GetActorRotation"); });
  L_1366:; (*u64_pframe(self, "CallFunc_GetBaseAimRotation_ReturnValue")) = ({ u64_v _o = (*u64_ivar(self, "GSCharacter")); U64_CALL(u64_asobj(_o), "GetBaseAimRotation"); });
  L_1420:; (*u64_pframe(self, "CallFunc_NormalizedDeltaRotator_ReturnValue")) = U64_CALL(self, "NormalizedDeltaRotator", (*u64_pframe(self, "CallFunc_GetBaseAimRotation_ReturnValue")), (*u64_pframe(self, "CallFunc_K2_GetActorRotation_ReturnValue_1")));
  L_1466:; U64_CALL(self, "BreakRotator", (*u64_pframe(self, "CallFunc_NormalizedDeltaRotator_ReturnValue")), u64_ref(&(*u64_pframe(self, "CallFunc_BreakRotator_Roll"))), u64_ref(&(*u64_pframe(self, "CallFunc_BreakRotator_Pitch"))), u64_ref(&(*u64_pframe(self, "CallFunc_BreakRotator_Yaw"))));
  L_1512:; 
  L_1513:; (*u64_ivar(self, "Yaw")) = (*u64_pframe(self, "CallFunc_BreakRotator_Yaw"));
  L_1540:; 
  L_1541:; (*u64_pframe(self, "CallFunc_GetVelocity_ReturnValue_1")) = ({ u64_v _o = (*u64_ivar(self, "GSCharacter")); U64_CALL(u64_asobj(_o), "GetVelocity"); });
  L_1595:; (*u64_pframe(self, "CallFunc_VSize_ReturnValue_1")) = U64_CALL(self, "VSize", (*u64_pframe(self, "CallFunc_GetVelocity_ReturnValue_1")));
  L_1632:; (*u64_pframe(self, "CallFunc_GetCurrentAcceleration_ReturnValue")) = ({ u64_v _o = ({ u64_v _o = (*u64_ivar(self, "GSCharacter")); (*u64_ivar(u64_asobj(_o), "CharacterMovement")); }); U64_CALL(u64_asobj(_o), "GetCurrentAcceleration"); });
  L_1704:; (*u64_pframe(self, "CallFunc_NearlyEqual_FloatFloat_ReturnValue")) = U64_CALL(self, "NearlyEqual_FloatFloat", (*u64_pframe(self, "CallFunc_VSize_ReturnValue_1")), ({ u64_v _o = ({ u64_v _o = (*u64_ivar(self, "GSCharacter")); (*u64_ivar(u64_asobj(_o), "CharacterMovement")); }); (*u64_ivar(u64_asobj(_o), "MaxWalkSpeed")); }), u64_float(0.10000000149011612f));
  L_1791:; (*u64_pframe(self, "CallFunc_VSize_ReturnValue_2")) = U64_CALL(self, "VSize", (*u64_pframe(self, "CallFunc_GetCurrentAcceleration_ReturnValue")));
  L_1828:; (*u64_pframe(self, "CallFunc_Greater_FloatFloat_ReturnValue")) = U64_CALL(self, "Greater_FloatFloat", (*u64_pframe(self, "CallFunc_VSize_ReturnValue_2")), u64_float(0.0f));
  L_1862:; (*u64_pframe(self, "CallFunc_BooleanOR_ReturnValue")) = U64_CALL(self, "BooleanOR", (*u64_pframe(self, "CallFunc_Greater_FloatFloat_ReturnValue")), (*u64_pframe(self, "CallFunc_NearlyEqual_FloatFloat_ReturnValue")));
  L_1900:; 
  L_1901:; (*u64_ivar(self, "IsAccelerating")) = (*u64_pframe(self, "CallFunc_BooleanOR_ReturnValue"));
  L_1920:; 
  L_1921:; (*u64_pframe(self, "CallFunc_GetMoveSpeedBaseValue_ReturnValue")) = ({ u64_v _o = (*u64_ivar(self, "GSCharacter")); U64_CALL(u64_asobj(_o), "GetMoveSpeedBaseValue"); });
  L_1971:; (*u64_pframe(self, "CallFunc_GetMaxSpeed_ReturnValue")) = ({ u64_v _o = ({ u64_v _o = (*u64_ivar(self, "GSCharacter")); (*u64_ivar(u64_asobj(_o), "CharacterMovement")); }); U64_CALL(u64_asobj(_o), "GetMaxSpeed"); });
  L_2047:; (*u64_pframe(self, "CallFunc_FMax_ReturnValue")) = U64_CALL(self, "FMax", (*u64_pframe(self, "CallFunc_GetMoveSpeedBaseValue_ReturnValue")), u64_float(0.009999999776482582f));
  L_2089:; (*u64_pframe(self, "CallFunc_Divide_FloatFloat_ReturnValue")) = U64_CALL(self, "Divide_FloatFloat", (*u64_pframe(self, "CallFunc_GetMaxSpeed_ReturnValue")), (*u64_pframe(self, "CallFunc_FMax_ReturnValue")));
  L_2135:; (*u64_pframe(self, "CallFunc_FMax_ReturnValue_1")) = U64_CALL(self, "FMax", (*u64_pframe(self, "CallFunc_Divide_FloatFloat_ReturnValue")), u64_float(1.0f));
  L_2177:; 
  L_2178:; (*u64_ivar(self, "MovementSpeedPlayRate")) = (*u64_pframe(self, "CallFunc_FMax_ReturnValue_1"));
  L_2205:; 
  L_2206:; goto L_2334;
  L_2211:; (*u64_pframe(self, "CallFunc_GetInstanceAssetPlayerTimeFromEnd_ReturnValue_1")) = U64_CALL(self, "GetInstanceAssetPlayerTimeFromEnd", u64_int(7));
  L_2244:; (*u64_pframe(self, "CallFunc_LessEqual_FloatFloat_ReturnValue_1")) = U64_CALL(self, "LessEqual_FloatFloat", (*u64_pframe(self, "CallFunc_GetInstanceAssetPlayerTimeFromEnd_ReturnValue_1")), u64_float(0.20000000298023224f));
  L_2278:; 
  L_2279:; (*u64_member(&(*u64_ivar(self, "AnimGraphNode_TransitionResult_CCC594714A0353C23B80B7A2593D8E87")), "bCanEnterTransition")) = (*u64_pframe(self, "CallFunc_LessEqual_FloatFloat_ReturnValue_1"));
  L_2307:; 
  L_2308:; goto L_2334;
  L_2313:; 
  L_2314:; 
  L_2315:; goto L_2211;
  L_2320:; 
  L_2321:; 
  L_2322:; goto L_17;
  L_2327:; 
  L_2328:; 
  L_2329:; goto L_119;
  L_2334:; goto u64_ret;
  L_2336:; 
  u64_ret:; u64_set_return(L.ReturnValue);

}

void bp_ABP_Hero3P__BlueprintUpdateAnimation(u64_obj *self, const u64_v *args, int argc) {
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
  L_23:; U64_CALL(self, "ExecuteUbergraph_ABP_Hero3P", u64_int(2327));
  L_38:; 
  L_39:; goto u64_ret;
  L_41:; 
  u64_ret:; u64_set_return(L.ReturnValue);

}

void bp_ABP_Hero3P__AnimGraph(u64_obj *self, const u64_v *args, int argc) {
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

void bp_ABP_Hero3P__EvaluateGraphExposedInputs_ExecuteUbergraph_ABP_Hero3P_AnimGraphNode_TransitionResult_C38F580E460C7AAE38E92DB5C7F3ECC5(u64_obj *self, const u64_v *args, int argc) {
  (void)args; (void)argc;
  struct { u64_v ReturnValue; u64_v _unsupported; } L; memset(&L, 0, sizeof L); (void)L;
  U64_FLOW_DECL; (void)flow_n;
  L_0:; 
  L_1:; 
  L_2:; 
  L_3:; U64_CALL(self, "ExecuteUbergraph_ABP_Hero3P", u64_int(2320));
  L_18:; 
  L_19:; goto u64_ret;
  L_21:; 
  u64_ret:; u64_set_return(L.ReturnValue);

}

void bp_ABP_Hero3P__EvaluateGraphExposedInputs_ExecuteUbergraph_ABP_Hero3P_AnimGraphNode_TransitionResult_CCC594714A0353C23B80B7A2593D8E87(u64_obj *self, const u64_v *args, int argc) {
  (void)args; (void)argc;
  struct { u64_v ReturnValue; u64_v _unsupported; } L; memset(&L, 0, sizeof L); (void)L;
  U64_FLOW_DECL; (void)flow_n;
  L_0:; 
  L_1:; 
  L_2:; 
  L_3:; U64_CALL(self, "ExecuteUbergraph_ABP_Hero3P", u64_int(2313));
  L_18:; 
  L_19:; goto u64_ret;
  L_21:; 
  u64_ret:; u64_set_return(L.ReturnValue);

}
