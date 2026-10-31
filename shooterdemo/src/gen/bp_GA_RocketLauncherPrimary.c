#include "u64_rt.h"

void bp_GA_RocketLauncherPrimary__ExecuteUbergraph_GA_RocketLauncherPrimary(u64_obj *self, const u64_v *args, int argc) {
  (void)args; (void)argc;
  struct { u64_v ReturnValue; u64_v _unsupported; } L; memset(&L, 0, sizeof L); (void)L;
  U64_FLOW_DECL; (void)flow_n;
  if (argc > 0) (*u64_pframe(self, "EntryPoint")) = args[0];
  L_0:; u64_dispatch: switch (u64_as_int((*u64_pframe(self, "EntryPoint")))) { case 0: goto L_0; case 10: goto L_10; case 11: goto L_11; case 45: goto L_45; case 46: goto L_46; case 47: goto L_47; case 48: goto L_48; case 62: goto L_62; case 63: goto L_63; case 64: goto L_64; case 78: goto L_78; case 79: goto L_79; case 84: goto L_84; case 113: goto L_113; case 114: goto L_114; case 115: goto L_115; case 129: goto L_129; case 130: goto L_130; case 131: goto L_131; case 132: goto L_132; case 152: goto L_152; case 226: goto L_226; case 227: goto L_227; case 228: goto L_228; case 242: goto L_242; case 243: goto L_243; case 244: goto L_244; case 270: goto L_270; case 271: goto L_271; case 300: goto L_300; case 301: goto L_301; case 302: goto L_302; case 316: goto L_316; case 317: goto L_317; case 340: goto L_340; case 341: goto L_341; case 382: goto L_382; case 383: goto L_383; case 384: goto L_384; case 416: goto L_416; case 417: goto L_417; case 422: goto L_422; case 442: goto L_442; case 443: goto L_443; case 481: goto L_481; case 482: goto L_482; case 483: goto L_483; case 519: goto L_519; case 548: goto L_548; case 549: goto L_549; case 563: goto L_563; case 564: goto L_564; case 565: goto L_565; case 584: goto L_584; case 585: goto L_585; case 590: goto L_590; case 591: goto L_591; case 612: goto L_612; case 613: goto L_613; case 642: goto L_642; case 643: goto L_643; case 644: goto L_644; case 658: goto L_658; case 659: goto L_659; case 682: goto L_682; case 683: goto L_683; case 724: goto L_724; case 725: goto L_725; case 726: goto L_726; case 758: goto L_758; case 759: goto L_759; case 764: goto L_764; case 765: goto L_765; case 766: goto L_766; case 771: goto L_771; case 772: goto L_772; case 773: goto L_773; case 778: goto L_778; case 779: goto L_779; case 780: goto L_780; case 781: goto L_781; case 782: goto L_782; case 811: goto L_811; case 812: goto L_812; case 813: goto L_813; case 827: goto L_827; case 828: goto L_828; case 857: goto L_857; case 858: goto L_858; case 859: goto L_859; case 873: goto L_873; case 874: goto L_874; case 879: goto L_879; case 899: goto L_899; case 900: goto L_900; case 936: goto L_936; case 965: goto L_965; case 966: goto L_966; case 980: goto L_980; case 981: goto L_981; case 1049: goto L_1049; case 1050: goto L_1050; case 1077: goto L_1077; case 1078: goto L_1078; case 1083: goto L_1083; case 1103: goto L_1103; case 1104: goto L_1104; case 1140: goto L_1140; case 1169: goto L_1169; case 1170: goto L_1170; case 1184: goto L_1184; case 1185: goto L_1185; case 1186: goto L_1186; case 1205: goto L_1205; case 1206: goto L_1206; case 1211: goto L_1211; case 1212: goto L_1212; case 1213: goto L_1213; case 1218: goto L_1218; case 1220: goto L_1220; }
  L_10:; 
  L_11:; (*u64_pframe(self, "CallFunc_BatchRPCTryActivateAbility_ReturnValue")) = U64_CALL(self, "BatchRPCTryActivateAbility", (*u64_ivar(self, "InstantAbilityHandle")), u64_bool(1));
  L_45:; 
  L_46:; 
  L_47:; 
  L_48:; if (!u64_true((*u64_pframe(self, "CallFunc_BatchRPCTryActivateAbility_ReturnValue")))) goto L_63;
  L_62:; 
  L_63:; 
  L_64:; U64_CALL(self, "K2_EndAbility");
  L_78:; 
  L_79:; goto L_1218;
  L_84:; (*u64_pframe(self, "CallFunc_IsValid_ReturnValue")) = U64_CALL(self, "IsValid", (*u64_ivar(self, "InstantAbility")));
  L_113:; 
  L_114:; 
  L_115:; if (!u64_true((*u64_pframe(self, "CallFunc_IsValid_ReturnValue")))) goto L_422;
  L_129:; 
  L_130:; 
  L_131:; 
  L_132:; (*u64_pframe(self, "CallFunc_GetAbilitySystemComponentFromActorInfo_ReturnValue_2")) = U64_CALL(self, "GetAbilitySystemComponentFromActorInfo");
  L_152:; (*u64_pframe(self, "CallFunc_HasMatchingGameplayTag_ReturnValue")) = ({ u64_v _o = (*u64_pframe(self, "CallFunc_GetAbilitySystemComponentFromActorInfo_ReturnValue_2")); U64_CALL(u64_asobj(_o), "HasMatchingGameplayTag", u64_name("State.Sprinting")); });
  L_226:; 
  L_227:; 
  L_228:; if (!u64_true((*u64_pframe(self, "CallFunc_HasMatchingGameplayTag_ReturnValue")))) goto L_590;
  L_242:; 
  L_243:; 
  L_244:; (*u64_pframe(self, "CallFunc_WaitDelay_ReturnValue")) = U64_CALL(self, "WaitDelay", u64_objv(self), u64_float(0.029999999329447746f));
  L_270:; 
  L_271:; (*u64_pframe(self, "CallFunc_IsValid_ReturnValue_1")) = U64_CALL(self, "IsValid", (*u64_pframe(self, "CallFunc_WaitDelay_ReturnValue")));
  L_300:; 
  L_301:; 
  L_302:; if (!u64_true((*u64_pframe(self, "CallFunc_IsValid_ReturnValue_1")))) goto L_1218;
  L_316:; 
  L_317:; (*u64_pframe(self, "K2Node_CreateDelegate_OutputDelegate")) = u64_delegate(u64_objv(self), "OnFinish_3BD4DFD746956C7D0423D493A6ABB33A");
  L_340:; 
  L_341:; u64_mc_add(&(*u64_ivar(u64_asobj((*u64_pframe(self, "CallFunc_WaitDelay_ReturnValue"))), "OnFinish")), (*u64_pframe(self, "K2Node_CreateDelegate_OutputDelegate")));
  L_382:; 
  L_383:; 
  L_384:; ({ u64_v _o = (*u64_pframe(self, "CallFunc_WaitDelay_ReturnValue")); U64_CALL(u64_asobj(_o), "ReadyForActivation"); });
  L_416:; 
  L_417:; goto L_1218;
  L_422:; (*u64_pframe(self, "CallFunc_GetAbilitySystemComponentFromActorInfo_ReturnValue_1")) = U64_CALL(self, "GetAbilitySystemComponentFromActorInfo");
  L_442:; 
  L_443:; (*u64_pframe(self, "CallFunc_GetPrimaryAbilityInstanceFromHandle_ReturnValue")) = U64_CALL(self, "GetPrimaryAbilityInstanceFromHandle", (*u64_pframe(self, "CallFunc_GetAbilitySystemComponentFromActorInfo_ReturnValue_1")), (*u64_ivar(self, "InstantAbilityHandle")));
  L_481:; 
  L_482:; 
  L_483:; (*u64_pframe(self, "K2Node_DynamicCast_AsGA_Rocket_Launcher_Primary_Instant")) = u64_cast((*u64_pframe(self, "CallFunc_GetPrimaryAbilityInstanceFromHandle_ReturnValue")), "GA_RocketLauncherPrimaryInstant");
  L_519:; (*u64_pframe(self, "K2Node_DynamicCast_bSuccess_2")) = (*u64_pframe(self, "K2Node_DynamicCast_AsGA_Rocket_Launcher_Primary_Instant"));
  L_548:; 
  L_549:; if (!u64_true((*u64_pframe(self, "K2Node_DynamicCast_bSuccess_2")))) goto L_1218;
  L_563:; 
  L_564:; 
  L_565:; (*u64_ivar(self, "InstantAbility")) = (*u64_pframe(self, "K2Node_DynamicCast_AsGA_Rocket_Launcher_Primary_Instant"));
  L_584:; 
  L_585:; goto L_130;
  L_590:; 
  L_591:; (*u64_pframe(self, "CallFunc_WaitDelayOneFrame_ReturnValue")) = U64_CALL(self, "WaitDelayOneFrame", u64_objv(self));
  L_612:; 
  L_613:; (*u64_pframe(self, "CallFunc_IsValid_ReturnValue_2")) = U64_CALL(self, "IsValid", (*u64_pframe(self, "CallFunc_WaitDelayOneFrame_ReturnValue")));
  L_642:; 
  L_643:; 
  L_644:; if (!u64_true((*u64_pframe(self, "CallFunc_IsValid_ReturnValue_2")))) goto L_1218;
  L_658:; 
  L_659:; (*u64_pframe(self, "K2Node_CreateDelegate_OutputDelegate_1")) = u64_delegate(u64_objv(self), "OnFinish_ECF018FA4142409405F2BD94A3124734");
  L_682:; 
  L_683:; u64_mc_add(&(*u64_ivar(u64_asobj((*u64_pframe(self, "CallFunc_WaitDelayOneFrame_ReturnValue"))), "OnFinish")), (*u64_pframe(self, "K2Node_CreateDelegate_OutputDelegate_1")));
  L_724:; 
  L_725:; 
  L_726:; ({ u64_v _o = (*u64_pframe(self, "CallFunc_WaitDelayOneFrame_ReturnValue")); U64_CALL(u64_asobj(_o), "ReadyForActivation"); });
  L_758:; 
  L_759:; goto L_1218;
  L_764:; 
  L_765:; 
  L_766:; goto L_10;
  L_771:; 
  L_772:; 
  L_773:; goto L_10;
  L_778:; 
  L_779:; 
  L_780:; 
  L_781:; 
  L_782:; (*u64_pframe(self, "CallFunc_IsValid_ReturnValue_3")) = U64_CALL(self, "IsValid", (*u64_ivar(self, "SourceWeapon")));
  L_811:; 
  L_812:; 
  L_813:; if (!u64_true((*u64_pframe(self, "CallFunc_IsValid_ReturnValue_3")))) goto L_1083;
  L_827:; 
  L_828:; (*u64_pframe(self, "CallFunc_IsAbilitySpecHandleValid_ReturnValue")) = U64_CALL(self, "IsAbilitySpecHandleValid", (*u64_ivar(self, "InstantAbilityHandle")));
  L_857:; 
  L_858:; 
  L_859:; if (!u64_true((*u64_pframe(self, "CallFunc_IsAbilitySpecHandleValid_ReturnValue")))) goto L_879;
  L_873:; 
  L_874:; goto L_84;
  L_879:; (*u64_pframe(self, "CallFunc_GetAbilitySystemComponentFromActorInfo_ReturnValue")) = U64_CALL(self, "GetAbilitySystemComponentFromActorInfo");
  L_899:; 
  L_900:; (*u64_pframe(self, "K2Node_DynamicCast_AsGSAbility_System_Component")) = u64_cast((*u64_pframe(self, "CallFunc_GetAbilitySystemComponentFromActorInfo_ReturnValue")), "GSAbilitySystemComponent");
  L_936:; (*u64_pframe(self, "K2Node_DynamicCast_bSuccess_1")) = (*u64_pframe(self, "K2Node_DynamicCast_AsGSAbility_System_Component"));
  L_965:; 
  L_966:; if (!u64_true((*u64_pframe(self, "K2Node_DynamicCast_bSuccess_1")))) goto L_1218;
  L_980:; 
  L_981:; (*u64_pframe(self, "CallFunc_FindAbilitySpecHandleForClass_ReturnValue")) = ({ u64_v _o = (*u64_pframe(self, "K2Node_DynamicCast_AsGSAbility_System_Component")); U64_CALL(u64_asobj(_o), "FindAbilitySpecHandleForClass", u64_objv(u64_find("GA_RocketLauncherPrimaryInstant_C")), (*u64_ivar(self, "SourceWeapon"))); });
  L_1049:; 
  L_1050:; (*u64_ivar(self, "InstantAbilityHandle")) = (*u64_pframe(self, "CallFunc_FindAbilitySpecHandleForClass_ReturnValue"));
  L_1077:; 
  L_1078:; goto L_84;
  L_1083:; (*u64_pframe(self, "CallFunc_GetCurrentSourceObject_ReturnValue")) = U64_CALL(self, "GetCurrentSourceObject");
  L_1103:; 
  L_1104:; (*u64_pframe(self, "K2Node_DynamicCast_AsGSWeapon")) = u64_cast((*u64_pframe(self, "CallFunc_GetCurrentSourceObject_ReturnValue")), "GSWeapon");
  L_1140:; (*u64_pframe(self, "K2Node_DynamicCast_bSuccess")) = (*u64_pframe(self, "K2Node_DynamicCast_AsGSWeapon"));
  L_1169:; 
  L_1170:; if (!u64_true((*u64_pframe(self, "K2Node_DynamicCast_bSuccess")))) goto L_1218;
  L_1184:; 
  L_1185:; 
  L_1186:; (*u64_ivar(self, "SourceWeapon")) = (*u64_pframe(self, "K2Node_DynamicCast_AsGSWeapon"));
  L_1205:; 
  L_1206:; goto L_828;
  L_1211:; 
  L_1212:; 
  L_1213:; goto L_778;
  L_1218:; goto u64_ret;
  L_1220:; 
  u64_ret:; u64_set_return(L.ReturnValue);

}

void bp_GA_RocketLauncherPrimary__K2_ActivateAbility(u64_obj *self, const u64_v *args, int argc) {
  (void)args; (void)argc;
  struct { u64_v ReturnValue; u64_v _unsupported; } L; memset(&L, 0, sizeof L); (void)L;
  U64_FLOW_DECL; (void)flow_n;
  L_0:; 
  L_1:; 
  L_2:; 
  L_3:; U64_CALL(self, "ExecuteUbergraph_GA_RocketLauncherPrimary", u64_int(1211));
  L_18:; 
  L_19:; goto u64_ret;
  L_21:; 
  u64_ret:; u64_set_return(L.ReturnValue);

}

void bp_GA_RocketLauncherPrimary__OnFinish_ECF018FA4142409405F2BD94A3124734(u64_obj *self, const u64_v *args, int argc) {
  (void)args; (void)argc;
  struct { u64_v ReturnValue; u64_v _unsupported; } L; memset(&L, 0, sizeof L); (void)L;
  U64_FLOW_DECL; (void)flow_n;
  L_0:; 
  L_1:; 
  L_2:; 
  L_3:; U64_CALL(self, "ExecuteUbergraph_GA_RocketLauncherPrimary", u64_int(771));
  L_18:; 
  L_19:; goto u64_ret;
  L_21:; 
  u64_ret:; u64_set_return(L.ReturnValue);

}

void bp_GA_RocketLauncherPrimary__OnFinish_3BD4DFD746956C7D0423D493A6ABB33A(u64_obj *self, const u64_v *args, int argc) {
  (void)args; (void)argc;
  struct { u64_v ReturnValue; u64_v _unsupported; } L; memset(&L, 0, sizeof L); (void)L;
  U64_FLOW_DECL; (void)flow_n;
  L_0:; 
  L_1:; 
  L_2:; 
  L_3:; U64_CALL(self, "ExecuteUbergraph_GA_RocketLauncherPrimary", u64_int(764));
  L_18:; 
  L_19:; goto u64_ret;
  L_21:; 
  u64_ret:; u64_set_return(L.ReturnValue);

}

void bp_GA_RocketLauncherPrimary__GSCheckCost(u64_obj *self, const u64_v *args, int argc) {
  (void)args; (void)argc;
  struct { u64_v ActorInfo; u64_v Handle; u64_v ReturnValue; u64_v _unsupported; } L; memset(&L, 0, sizeof L); (void)L;
  U64_FLOW_DECL; (void)flow_n;
  if (argc > 0) L.Handle = args[0];
  if (argc > 1) L.ActorInfo = args[1].t == U64_REF ? *args[1].u.r : args[1];
  L_0:; 
  u64_ret:; u64_set_return(L.ReturnValue);
  if (argc > 1) u64_out(args[1], L.ActorInfo);
}
