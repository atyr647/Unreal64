#include "u64_rt.h"

void bp_GA_Revive__ExecuteUbergraph_GA_Revive(u64_obj *self, const u64_v *args, int argc) {
  (void)args; (void)argc;
  struct { u64_v ReturnValue; u64_v _unsupported; } L; memset(&L, 0, sizeof L); (void)L;
  U64_FLOW_DECL; (void)flow_n;
  if (argc > 0) (*u64_pframe(self, "EntryPoint")) = args[0];
  L_0:; u64_dispatch: switch (u64_as_int((*u64_pframe(self, "EntryPoint")))) { case 0: goto L_0; case 10: goto L_10; case 11: goto L_11; case 12: goto L_12; case 17: goto L_17; case 18: goto L_18; case 19: goto L_19; case 24: goto L_24; case 25: goto L_25; case 26: goto L_26; case 31: goto L_31; case 32: goto L_32; case 33: goto L_33; case 34: goto L_34; case 48: goto L_48; case 49: goto L_49; case 54: goto L_54; case 55: goto L_55; case 56: goto L_56; case 61: goto L_61; case 62: goto L_62; case 63: goto L_63; case 68: goto L_68; case 69: goto L_69; case 70: goto L_70; case 75: goto L_75; case 76: goto L_76; case 77: goto L_77; case 82: goto L_82; case 83: goto L_83; case 150: goto L_150; case 151: goto L_151; case 180: goto L_180; case 181: goto L_181; case 182: goto L_182; case 196: goto L_196; case 197: goto L_197; case 220: goto L_220; case 221: goto L_221; case 262: goto L_262; case 263: goto L_263; case 286: goto L_286; case 287: goto L_287; case 328: goto L_328; case 329: goto L_329; case 352: goto L_352; case 353: goto L_353; case 394: goto L_394; case 395: goto L_395; case 418: goto L_418; case 419: goto L_419; case 460: goto L_460; case 461: goto L_461; case 462: goto L_462; case 494: goto L_494; case 495: goto L_495; case 500: goto L_500; case 501: goto L_501; case 502: goto L_502; case 503: goto L_503; case 527: goto L_527; case 528: goto L_528; case 529: goto L_529; case 530: goto L_530; case 544: goto L_544; case 545: goto L_545; case 550: goto L_550; case 551: goto L_551; case 605: goto L_605; case 606: goto L_606; case 674: goto L_674; case 716: goto L_716; case 717: goto L_717; case 788: goto L_788; case 789: goto L_789; case 818: goto L_818; case 819: goto L_819; case 820: goto L_820; case 834: goto L_834; case 835: goto L_835; case 858: goto L_858; case 859: goto L_859; case 900: goto L_900; case 901: goto L_901; case 924: goto L_924; case 925: goto L_925; case 966: goto L_966; case 967: goto L_967; case 990: goto L_990; case 991: goto L_991; case 1032: goto L_1032; case 1033: goto L_1033; case 1056: goto L_1056; case 1057: goto L_1057; case 1098: goto L_1098; case 1099: goto L_1099; case 1100: goto L_1100; case 1132: goto L_1132; case 1133: goto L_1133; case 1134: goto L_1134; case 1181: goto L_1181; case 1182: goto L_1182; case 1183: goto L_1183; case 1210: goto L_1210; case 1211: goto L_1211; case 1216: goto L_1216; case 1217: goto L_1217; case 1218: goto L_1218; case 1238: goto L_1238; case 1239: goto L_1239; case 1275: goto L_1275; case 1304: goto L_1304; case 1305: goto L_1305; case 1319: goto L_1319; case 1320: goto L_1320; case 1325: goto L_1325; case 1327: goto L_1327; }
  L_10:; 
  L_11:; 
  L_12:; goto L_1325;
  L_17:; 
  L_18:; 
  L_19:; goto L_1325;
  L_24:; 
  L_25:; 
  L_26:; goto L_1325;
  L_31:; 
  L_32:; 
  L_33:; 
  L_34:; U64_CALL(self, "K2_EndAbility");
  L_48:; 
  L_49:; goto L_1325;
  L_54:; 
  L_55:; 
  L_56:; goto L_1325;
  L_61:; 
  L_62:; 
  L_63:; goto L_1325;
  L_68:; 
  L_69:; 
  L_70:; goto L_1325;
  L_75:; 
  L_76:; 
  L_77:; goto L_1325;
  L_82:; 
  L_83:; (*u64_pframe(self, "CallFunc_CreatePlayMontageAndWaitProxy_ReturnValue")) = U64_CALL(self, "CreatePlayMontageAndWaitProxy", u64_objv(self), u64_name("None"), u64_objv(u64_find("Death_Montage")), u64_float(1.0f), u64_name("None"), u64_bool(0), u64_float(1.0f));
  L_150:; 
  L_151:; (*u64_pframe(self, "CallFunc_IsValid_ReturnValue")) = U64_CALL(self, "IsValid", (*u64_pframe(self, "CallFunc_CreatePlayMontageAndWaitProxy_ReturnValue")));
  L_180:; 
  L_181:; 
  L_182:; if (!u64_true((*u64_pframe(self, "CallFunc_IsValid_ReturnValue")))) goto L_1325;
  L_196:; 
  L_197:; (*u64_pframe(self, "K2Node_CreateDelegate_OutputDelegate_7")) = u64_delegate(u64_objv(self), "OnCompleted_597500154FEE5C27D47DF897920F830B");
  L_220:; 
  L_221:; u64_mc_add(&(*u64_ivar(u64_asobj((*u64_pframe(self, "CallFunc_CreatePlayMontageAndWaitProxy_ReturnValue"))), "OnCompleted")), (*u64_pframe(self, "K2Node_CreateDelegate_OutputDelegate_7")));
  L_262:; 
  L_263:; (*u64_pframe(self, "K2Node_CreateDelegate_OutputDelegate_6")) = u64_delegate(u64_objv(self), "OnBlendOut_597500154FEE5C27D47DF897920F830B");
  L_286:; 
  L_287:; u64_mc_add(&(*u64_ivar(u64_asobj((*u64_pframe(self, "CallFunc_CreatePlayMontageAndWaitProxy_ReturnValue"))), "OnBlendOut")), (*u64_pframe(self, "K2Node_CreateDelegate_OutputDelegate_6")));
  L_328:; 
  L_329:; (*u64_pframe(self, "K2Node_CreateDelegate_OutputDelegate_5")) = u64_delegate(u64_objv(self), "OnInterrupted_597500154FEE5C27D47DF897920F830B");
  L_352:; 
  L_353:; u64_mc_add(&(*u64_ivar(u64_asobj((*u64_pframe(self, "CallFunc_CreatePlayMontageAndWaitProxy_ReturnValue"))), "OnInterrupted")), (*u64_pframe(self, "K2Node_CreateDelegate_OutputDelegate_5")));
  L_394:; 
  L_395:; (*u64_pframe(self, "K2Node_CreateDelegate_OutputDelegate_4")) = u64_delegate(u64_objv(self), "OnCancelled_597500154FEE5C27D47DF897920F830B");
  L_418:; 
  L_419:; u64_mc_add(&(*u64_ivar(u64_asobj((*u64_pframe(self, "CallFunc_CreatePlayMontageAndWaitProxy_ReturnValue"))), "OnCancelled")), (*u64_pframe(self, "K2Node_CreateDelegate_OutputDelegate_4")));
  L_460:; 
  L_461:; 
  L_462:; ({ u64_v _o = (*u64_pframe(self, "CallFunc_CreatePlayMontageAndWaitProxy_ReturnValue")); U64_CALL(u64_asobj(_o), "ReadyForActivation"); });
  L_494:; 
  L_495:; goto L_1325;
  L_500:; 
  L_501:; 
  L_502:; 
  L_503:; U64_CALL(self, "BP_RemoveGameplayEffectFromOwnerWithHandle", (*u64_ivar(self, "GE_RevivingHandle")), u64_int(-1));
  L_527:; 
  L_528:; 
  L_529:; 
  L_530:; if (!u64_true((*u64_pframe(self, "K2Node_Event_bWasCancelled")))) goto L_1325;
  L_544:; 
  L_545:; goto L_82;
  L_550:; 
  L_551:; (*u64_pframe(self, "CallFunc_GetPlayLength_ReturnValue")) = ({ u64_v _o = (*u64_ivar(self, "Montage")); U64_CALL(u64_asobj(_o), "GetPlayLength"); });
  L_605:; 
  L_606:; (*u64_pframe(self, "CallFunc_Divide_FloatFloat_ReturnValue")) = U64_CALL(self, "Divide_FloatFloat", ({ u64_v _o = (*u64_pframe(self, "K2Node_DynamicCast_AsGSHero_Character")); (*u64_ivar(u64_asobj(_o), "ReviveDuration")); }), (*u64_pframe(self, "CallFunc_GetPlayLength_ReturnValue")));
  L_674:; (*u64_pframe(self, "CallFunc_Divide_FloatFloat_ReturnValue_1")) = U64_CALL(self, "Divide_FloatFloat", u64_float(1.0f), (*u64_pframe(self, "CallFunc_Divide_FloatFloat_ReturnValue")));
  L_716:; 
  L_717:; (*u64_pframe(self, "CallFunc_CreatePlayMontageAndWaitProxy_ReturnValue_1")) = U64_CALL(self, "CreatePlayMontageAndWaitProxy", u64_objv(self), u64_name("None"), (*u64_ivar(self, "Montage")), (*u64_pframe(self, "CallFunc_Divide_FloatFloat_ReturnValue_1")), u64_name("None"), u64_bool(0), u64_float(1.0f));
  L_788:; 
  L_789:; (*u64_pframe(self, "CallFunc_IsValid_ReturnValue_1")) = U64_CALL(self, "IsValid", (*u64_pframe(self, "CallFunc_CreatePlayMontageAndWaitProxy_ReturnValue_1")));
  L_818:; 
  L_819:; 
  L_820:; if (!u64_true((*u64_pframe(self, "CallFunc_IsValid_ReturnValue_1")))) goto L_1133;
  L_834:; 
  L_835:; (*u64_pframe(self, "K2Node_CreateDelegate_OutputDelegate_3")) = u64_delegate(u64_objv(self), "OnCompleted_DDB0F2F94D6CE191A4D3F884988F034E");
  L_858:; 
  L_859:; u64_mc_add(&(*u64_ivar(u64_asobj((*u64_pframe(self, "CallFunc_CreatePlayMontageAndWaitProxy_ReturnValue_1"))), "OnCompleted")), (*u64_pframe(self, "K2Node_CreateDelegate_OutputDelegate_3")));
  L_900:; 
  L_901:; (*u64_pframe(self, "K2Node_CreateDelegate_OutputDelegate_2")) = u64_delegate(u64_objv(self), "OnBlendOut_DDB0F2F94D6CE191A4D3F884988F034E");
  L_924:; 
  L_925:; u64_mc_add(&(*u64_ivar(u64_asobj((*u64_pframe(self, "CallFunc_CreatePlayMontageAndWaitProxy_ReturnValue_1"))), "OnBlendOut")), (*u64_pframe(self, "K2Node_CreateDelegate_OutputDelegate_2")));
  L_966:; 
  L_967:; (*u64_pframe(self, "K2Node_CreateDelegate_OutputDelegate_1")) = u64_delegate(u64_objv(self), "OnInterrupted_DDB0F2F94D6CE191A4D3F884988F034E");
  L_990:; 
  L_991:; u64_mc_add(&(*u64_ivar(u64_asobj((*u64_pframe(self, "CallFunc_CreatePlayMontageAndWaitProxy_ReturnValue_1"))), "OnInterrupted")), (*u64_pframe(self, "K2Node_CreateDelegate_OutputDelegate_1")));
  L_1032:; 
  L_1033:; (*u64_pframe(self, "K2Node_CreateDelegate_OutputDelegate")) = u64_delegate(u64_objv(self), "OnCancelled_DDB0F2F94D6CE191A4D3F884988F034E");
  L_1056:; 
  L_1057:; u64_mc_add(&(*u64_ivar(u64_asobj((*u64_pframe(self, "CallFunc_CreatePlayMontageAndWaitProxy_ReturnValue_1"))), "OnCancelled")), (*u64_pframe(self, "K2Node_CreateDelegate_OutputDelegate")));
  L_1098:; 
  L_1099:; 
  L_1100:; ({ u64_v _o = (*u64_pframe(self, "CallFunc_CreatePlayMontageAndWaitProxy_ReturnValue_1")); U64_CALL(u64_asobj(_o), "ReadyForActivation"); });
  L_1132:; 
  L_1133:; 
  L_1134:; (*u64_pframe(self, "CallFunc_BP_ApplyGameplayEffectToOwner_ReturnValue")) = U64_CALL(self, "BP_ApplyGameplayEffectToOwner", u64_objv(u64_find("GE_Reviving_C")), u64_int(1), u64_int(1));
  L_1181:; 
  L_1182:; 
  L_1183:; (*u64_ivar(self, "GE_RevivingHandle")) = (*u64_pframe(self, "CallFunc_BP_ApplyGameplayEffectToOwner_ReturnValue"));
  L_1210:; 
  L_1211:; goto L_1325;
  L_1216:; 
  L_1217:; 
  L_1218:; (*u64_pframe(self, "CallFunc_GetAvatarActorFromActorInfo_ReturnValue")) = U64_CALL(self, "GetAvatarActorFromActorInfo");
  L_1238:; 
  L_1239:; (*u64_pframe(self, "K2Node_DynamicCast_AsGSHero_Character")) = u64_cast((*u64_pframe(self, "CallFunc_GetAvatarActorFromActorInfo_ReturnValue")), "GSHeroCharacter");
  L_1275:; (*u64_pframe(self, "K2Node_DynamicCast_bSuccess")) = (*u64_pframe(self, "K2Node_DynamicCast_AsGSHero_Character"));
  L_1304:; 
  L_1305:; if (!u64_true((*u64_pframe(self, "K2Node_DynamicCast_bSuccess")))) goto L_1325;
  L_1319:; 
  L_1320:; goto L_550;
  L_1325:; goto u64_ret;
  L_1327:; 
  u64_ret:; u64_set_return(L.ReturnValue);

}

void bp_GA_Revive__K2_ActivateAbility(u64_obj *self, const u64_v *args, int argc) {
  (void)args; (void)argc;
  struct { u64_v ReturnValue; u64_v _unsupported; } L; memset(&L, 0, sizeof L); (void)L;
  U64_FLOW_DECL; (void)flow_n;
  L_0:; 
  L_1:; 
  L_2:; 
  L_3:; U64_CALL(self, "ExecuteUbergraph_GA_Revive", u64_int(1216));
  L_18:; 
  L_19:; goto u64_ret;
  L_21:; 
  u64_ret:; u64_set_return(L.ReturnValue);

}

void bp_GA_Revive__K2_OnEndAbility(u64_obj *self, const u64_v *args, int argc) {
  (void)args; (void)argc;
  struct { u64_v ReturnValue; u64_v _unsupported; u64_v bWasCancelled; } L; memset(&L, 0, sizeof L); (void)L;
  U64_FLOW_DECL; (void)flow_n;
  if (argc > 0) L.bWasCancelled = args[0];
  L_0:; 
  L_1:; 
  L_2:; 
  L_3:; *u64_pframe(self, "K2Node_Event_bWasCancelled") = L.bWasCancelled;
  L_21:; 
  L_22:; 
  L_23:; U64_CALL(self, "ExecuteUbergraph_GA_Revive", u64_int(500));
  L_38:; 
  L_39:; goto u64_ret;
  L_41:; 
  u64_ret:; u64_set_return(L.ReturnValue);

}

void bp_GA_Revive__OnCompleted_597500154FEE5C27D47DF897920F830B(u64_obj *self, const u64_v *args, int argc) {
  (void)args; (void)argc;
  struct { u64_v ReturnValue; u64_v _unsupported; } L; memset(&L, 0, sizeof L); (void)L;
  U64_FLOW_DECL; (void)flow_n;
  L_0:; 
  L_1:; 
  L_2:; 
  L_3:; U64_CALL(self, "ExecuteUbergraph_GA_Revive", u64_int(75));
  L_18:; 
  L_19:; goto u64_ret;
  L_21:; 
  u64_ret:; u64_set_return(L.ReturnValue);

}

void bp_GA_Revive__OnBlendOut_597500154FEE5C27D47DF897920F830B(u64_obj *self, const u64_v *args, int argc) {
  (void)args; (void)argc;
  struct { u64_v ReturnValue; u64_v _unsupported; } L; memset(&L, 0, sizeof L); (void)L;
  U64_FLOW_DECL; (void)flow_n;
  L_0:; 
  L_1:; 
  L_2:; 
  L_3:; U64_CALL(self, "ExecuteUbergraph_GA_Revive", u64_int(68));
  L_18:; 
  L_19:; goto u64_ret;
  L_21:; 
  u64_ret:; u64_set_return(L.ReturnValue);

}

void bp_GA_Revive__OnInterrupted_597500154FEE5C27D47DF897920F830B(u64_obj *self, const u64_v *args, int argc) {
  (void)args; (void)argc;
  struct { u64_v ReturnValue; u64_v _unsupported; } L; memset(&L, 0, sizeof L); (void)L;
  U64_FLOW_DECL; (void)flow_n;
  L_0:; 
  L_1:; 
  L_2:; 
  L_3:; U64_CALL(self, "ExecuteUbergraph_GA_Revive", u64_int(61));
  L_18:; 
  L_19:; goto u64_ret;
  L_21:; 
  u64_ret:; u64_set_return(L.ReturnValue);

}

void bp_GA_Revive__OnCancelled_597500154FEE5C27D47DF897920F830B(u64_obj *self, const u64_v *args, int argc) {
  (void)args; (void)argc;
  struct { u64_v ReturnValue; u64_v _unsupported; } L; memset(&L, 0, sizeof L); (void)L;
  U64_FLOW_DECL; (void)flow_n;
  L_0:; 
  L_1:; 
  L_2:; 
  L_3:; U64_CALL(self, "ExecuteUbergraph_GA_Revive", u64_int(54));
  L_18:; 
  L_19:; goto u64_ret;
  L_21:; 
  u64_ret:; u64_set_return(L.ReturnValue);

}

void bp_GA_Revive__OnCompleted_DDB0F2F94D6CE191A4D3F884988F034E(u64_obj *self, const u64_v *args, int argc) {
  (void)args; (void)argc;
  struct { u64_v ReturnValue; u64_v _unsupported; } L; memset(&L, 0, sizeof L); (void)L;
  U64_FLOW_DECL; (void)flow_n;
  L_0:; 
  L_1:; 
  L_2:; 
  L_3:; U64_CALL(self, "ExecuteUbergraph_GA_Revive", u64_int(31));
  L_18:; 
  L_19:; goto u64_ret;
  L_21:; 
  u64_ret:; u64_set_return(L.ReturnValue);

}

void bp_GA_Revive__OnBlendOut_DDB0F2F94D6CE191A4D3F884988F034E(u64_obj *self, const u64_v *args, int argc) {
  (void)args; (void)argc;
  struct { u64_v ReturnValue; u64_v _unsupported; } L; memset(&L, 0, sizeof L); (void)L;
  U64_FLOW_DECL; (void)flow_n;
  L_0:; 
  L_1:; 
  L_2:; 
  L_3:; U64_CALL(self, "ExecuteUbergraph_GA_Revive", u64_int(24));
  L_18:; 
  L_19:; goto u64_ret;
  L_21:; 
  u64_ret:; u64_set_return(L.ReturnValue);

}

void bp_GA_Revive__OnInterrupted_DDB0F2F94D6CE191A4D3F884988F034E(u64_obj *self, const u64_v *args, int argc) {
  (void)args; (void)argc;
  struct { u64_v ReturnValue; u64_v _unsupported; } L; memset(&L, 0, sizeof L); (void)L;
  U64_FLOW_DECL; (void)flow_n;
  L_0:; 
  L_1:; 
  L_2:; 
  L_3:; U64_CALL(self, "ExecuteUbergraph_GA_Revive", u64_int(17));
  L_18:; 
  L_19:; goto u64_ret;
  L_21:; 
  u64_ret:; u64_set_return(L.ReturnValue);

}

void bp_GA_Revive__OnCancelled_DDB0F2F94D6CE191A4D3F884988F034E(u64_obj *self, const u64_v *args, int argc) {
  (void)args; (void)argc;
  struct { u64_v ReturnValue; u64_v _unsupported; } L; memset(&L, 0, sizeof L); (void)L;
  U64_FLOW_DECL; (void)flow_n;
  L_0:; 
  L_1:; 
  L_2:; 
  L_3:; U64_CALL(self, "ExecuteUbergraph_GA_Revive", u64_int(10));
  L_18:; 
  L_19:; goto u64_ret;
  L_21:; 
  u64_ret:; u64_set_return(L.ReturnValue);

}
