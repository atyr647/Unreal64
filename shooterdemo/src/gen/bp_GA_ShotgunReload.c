#include "u64_rt.h"

void bp_GA_ShotgunReload__ExecuteUbergraph_GA_ShotgunReload(u64_obj *self, const u64_v *args, int argc) {
  (void)args; (void)argc;
  struct { u64_v ReturnValue; u64_v _unsupported; } L; memset(&L, 0, sizeof L); (void)L;
  U64_FLOW_DECL; (void)flow_n;
  if (argc > 0) (*u64_pframe(self, "EntryPoint")) = args[0];
  L_0:; u64_dispatch: switch (u64_as_int((*u64_pframe(self, "EntryPoint")))) { case 0: goto L_0; case 10: goto L_10; case 11: goto L_11; case 84: goto L_84; case 85: goto L_85; case 127: goto L_127; case 164: goto L_164; case 165: goto L_165; case 253: goto L_253; case 254: goto L_254; case 255: goto L_255; case 296: goto L_296; case 297: goto L_297; case 351: goto L_351; case 397: goto L_397; case 398: goto L_398; case 443: goto L_443; case 444: goto L_444; case 449: goto L_449; case 450: goto L_450; case 451: goto L_451; case 452: goto L_452; case 479: goto L_479; case 480: goto L_480; case 481: goto L_481; case 508: goto L_508; case 509: goto L_509; case 514: goto L_514; case 515: goto L_515; case 516: goto L_516; case 517: goto L_517; case 544: goto L_544; case 545: goto L_545; case 546: goto L_546; case 573: goto L_573; case 574: goto L_574; case 579: goto L_579; case 580: goto L_580; case 581: goto L_581; case 582: goto L_582; case 609: goto L_609; case 610: goto L_610; case 611: goto L_611; case 638: goto L_638; case 639: goto L_639; case 644: goto L_644; case 645: goto L_645; case 646: goto L_646; case 647: goto L_647; case 674: goto L_674; case 675: goto L_675; case 676: goto L_676; case 703: goto L_703; case 704: goto L_704; case 709: goto L_709; case 710: goto L_710; case 711: goto L_711; case 712: goto L_712; case 739: goto L_739; case 740: goto L_740; case 741: goto L_741; case 768: goto L_768; case 769: goto L_769; case 774: goto L_774; case 775: goto L_775; case 776: goto L_776; case 777: goto L_777; case 804: goto L_804; case 805: goto L_805; case 806: goto L_806; case 833: goto L_833; case 834: goto L_834; case 835: goto L_835; case 858: goto L_858; case 859: goto L_859; case 888: goto L_888; case 889: goto L_889; case 890: goto L_890; case 904: goto L_904; case 905: goto L_905; case 928: goto L_928; case 929: goto L_929; case 970: goto L_970; case 971: goto L_971; case 972: goto L_972; case 1004: goto L_1004; case 1005: goto L_1005; case 1010: goto L_1010; case 1011: goto L_1011; case 1012: goto L_1012; case 1013: goto L_1013; case 1040: goto L_1040; case 1041: goto L_1041; case 1042: goto L_1042; case 1069: goto L_1069; case 1070: goto L_1070; case 1071: goto L_1071; case 1081: goto L_1081; case 1082: goto L_1082; case 1087: goto L_1087; case 1088: goto L_1088; case 1089: goto L_1089; case 1090: goto L_1090; case 1117: goto L_1117; case 1118: goto L_1118; case 1119: goto L_1119; case 1146: goto L_1146; case 1147: goto L_1147; case 1152: goto L_1152; case 1153: goto L_1153; case 1154: goto L_1154; case 1155: goto L_1155; case 1182: goto L_1182; case 1183: goto L_1183; case 1184: goto L_1184; case 1211: goto L_1211; case 1212: goto L_1212; case 1217: goto L_1217; case 1218: goto L_1218; case 1219: goto L_1219; case 1220: goto L_1220; case 1247: goto L_1247; case 1248: goto L_1248; case 1249: goto L_1249; case 1276: goto L_1276; case 1277: goto L_1277; case 1278: goto L_1278; case 1301: goto L_1301; case 1302: goto L_1302; case 1331: goto L_1331; case 1332: goto L_1332; case 1333: goto L_1333; case 1347: goto L_1347; case 1348: goto L_1348; case 1371: goto L_1371; case 1372: goto L_1372; case 1413: goto L_1413; case 1414: goto L_1414; case 1415: goto L_1415; case 1447: goto L_1447; case 1448: goto L_1448; case 1453: goto L_1453; case 1454: goto L_1454; case 1455: goto L_1455; case 1509: goto L_1509; case 1563: goto L_1563; case 1609: goto L_1609; case 1610: goto L_1610; case 1637: goto L_1637; case 1638: goto L_1638; case 1688: goto L_1688; case 1726: goto L_1726; case 1727: goto L_1727; case 1728: goto L_1728; case 1742: goto L_1742; case 1743: goto L_1743; case 1744: goto L_1744; case 1771: goto L_1771; case 1772: goto L_1772; case 1777: goto L_1777; case 1827: goto L_1827; case 1828: goto L_1828; case 1855: goto L_1855; case 1856: goto L_1856; case 1861: goto L_1861; case 1862: goto L_1862; case 1863: goto L_1863; case 1864: goto L_1864; case 1878: goto L_1878; case 1879: goto L_1879; case 1884: goto L_1884; case 1885: goto L_1885; case 1886: goto L_1886; case 1928: goto L_1928; case 1929: goto L_1929; case 2044: goto L_2044; case 2045: goto L_2045; case 2074: goto L_2074; case 2075: goto L_2075; case 2076: goto L_2076; case 2090: goto L_2090; case 2091: goto L_2091; case 2114: goto L_2114; case 2115: goto L_2115; case 2156: goto L_2156; case 2157: goto L_2157; case 2180: goto L_2180; case 2181: goto L_2181; case 2222: goto L_2222; case 2223: goto L_2223; case 2246: goto L_2246; case 2247: goto L_2247; case 2288: goto L_2288; case 2289: goto L_2289; case 2312: goto L_2312; case 2313: goto L_2313; case 2354: goto L_2354; case 2355: goto L_2355; case 2378: goto L_2378; case 2379: goto L_2379; case 2420: goto L_2420; case 2421: goto L_2421; case 2422: goto L_2422; case 2454: goto L_2454; case 2455: goto L_2455; case 2497: goto L_2497; case 2498: goto L_2498; case 2640: goto L_2640; case 2641: goto L_2641; case 2670: goto L_2670; case 2671: goto L_2671; case 2672: goto L_2672; case 2686: goto L_2686; case 2687: goto L_2687; case 2710: goto L_2710; case 2711: goto L_2711; case 2752: goto L_2752; case 2753: goto L_2753; case 2776: goto L_2776; case 2777: goto L_2777; case 2818: goto L_2818; case 2819: goto L_2819; case 2842: goto L_2842; case 2843: goto L_2843; case 2884: goto L_2884; case 2885: goto L_2885; case 2908: goto L_2908; case 2909: goto L_2909; case 2950: goto L_2950; case 2951: goto L_2951; case 2974: goto L_2974; case 2975: goto L_2975; case 3016: goto L_3016; case 3017: goto L_3017; case 3018: goto L_3018; case 3050: goto L_3050; case 3051: goto L_3051; case 3056: goto L_3056; case 3076: goto L_3076; case 3077: goto L_3077; case 3113: goto L_3113; case 3142: goto L_3142; case 3143: goto L_3143; case 3157: goto L_3157; case 3158: goto L_3158; case 3159: goto L_3159; case 3178: goto L_3178; case 3179: goto L_3179; case 3208: goto L_3208; case 3209: goto L_3209; case 3210: goto L_3210; case 3224: goto L_3224; case 3225: goto L_3225; case 3230: goto L_3230; case 3250: goto L_3250; case 3251: goto L_3251; case 3287: goto L_3287; case 3316: goto L_3316; case 3317: goto L_3317; case 3331: goto L_3331; case 3332: goto L_3332; case 3333: goto L_3333; case 3352: goto L_3352; case 3353: goto L_3353; case 3358: goto L_3358; case 3359: goto L_3359; case 3360: goto L_3360; case 3361: goto L_3361; case 3362: goto L_3362; case 3363: goto L_3363; case 3364: goto L_3364; case 3393: goto L_3393; case 3394: goto L_3394; case 3395: goto L_3395; case 3409: goto L_3409; case 3410: goto L_3410; case 3415: goto L_3415; case 3416: goto L_3416; case 3417: goto L_3417; case 3419: goto L_3419; }
  L_10:; 
  L_11:; (*u64_pframe(self, "CallFunc_MakeEffectContainerSpec_ReturnValue")) = U64_CALL(self, "MakeEffectContainerSpec", u64_name("Ability"), (*u64_pframe(self, "Temp_struct_Variable")), u64_int(-1));
  L_84:; 
  L_85:; (*u64_pframe(self, "CallFunc_Multiply_IntInt_ReturnValue")) = U64_CALL(self, "Multiply_IntInt", (*u64_ivar(self, "AmountWeCanReload")), u64_int(-1));
  L_127:; (*u64_pframe(self, "CallFunc_Conv_IntToFloat_ReturnValue")) = U64_CALL(self, "Conv_IntToFloat", (*u64_pframe(self, "CallFunc_Multiply_IntInt_ReturnValue")));
  L_164:; 
  L_165:; (*u64_pframe(self, "CallFunc_AssignTagSetByCallerMagnitude_ReturnValue")) = U64_CALL(self, "AssignTagSetByCallerMagnitude", u64_array_get((*u64_member(&(*u64_pframe(self, "CallFunc_MakeEffectContainerSpec_ReturnValue")), "TargetGameplayEffectSpecs")), u64_as_int(u64_int(0))), u64_name("Data.ReloadAmount.Reserve"), (*u64_pframe(self, "CallFunc_Conv_IntToFloat_ReturnValue")));
  L_253:; 
  L_254:; 
  L_255:; (*u64_pframe(self, "CallFunc_ApplyEffectContainerSpec_ReturnValue")) = U64_CALL(self, "ApplyEffectContainerSpec", (*u64_pframe(self, "CallFunc_MakeEffectContainerSpec_ReturnValue")));
  L_296:; 
  L_297:; (*u64_pframe(self, "CallFunc_GetPrimaryClipAmmo_ReturnValue_1")) = ({ u64_v _o = (*u64_ivar(self, "SourceWeapon")); U64_CALL(u64_asobj(_o), "GetPrimaryClipAmmo"); });
  L_351:; (*u64_pframe(self, "CallFunc_Add_IntInt_ReturnValue")) = U64_CALL(self, "Add_IntInt", (*u64_pframe(self, "CallFunc_GetPrimaryClipAmmo_ReturnValue_1")), (*u64_ivar(self, "AmountWeCanReload")));
  L_397:; 
  L_398:; ({ u64_v _o = (*u64_ivar(self, "SourceWeapon")); U64_CALL(u64_asobj(_o), "SetPrimaryClipAmmo", (*u64_pframe(self, "CallFunc_Add_IntInt_ReturnValue"))); });
  L_443:; 
  L_444:; goto L_3417;
  L_449:; 
  L_450:; 
  L_451:; 
  L_452:; (*u64_pframe(self, "Temp_struct_Variable_2")) = (*u64_pframe(self, "K2Node_CustomEvent_EventTag_9"));
  L_479:; 
  L_480:; 
  L_481:; (*u64_pframe(self, "Temp_struct_Variable_1")) = (*u64_pframe(self, "K2Node_CustomEvent_EventData_9"));
  L_508:; 
  L_509:; goto L_3417;
  L_514:; 
  L_515:; 
  L_516:; 
  L_517:; (*u64_pframe(self, "Temp_struct_Variable_2")) = (*u64_pframe(self, "K2Node_CustomEvent_EventTag_8"));
  L_544:; 
  L_545:; 
  L_546:; (*u64_pframe(self, "Temp_struct_Variable_1")) = (*u64_pframe(self, "K2Node_CustomEvent_EventData_8"));
  L_573:; 
  L_574:; goto L_3417;
  L_579:; 
  L_580:; 
  L_581:; 
  L_582:; (*u64_pframe(self, "Temp_struct_Variable_2")) = (*u64_pframe(self, "K2Node_CustomEvent_EventTag_7"));
  L_609:; 
  L_610:; 
  L_611:; (*u64_pframe(self, "Temp_struct_Variable_1")) = (*u64_pframe(self, "K2Node_CustomEvent_EventData_7"));
  L_638:; 
  L_639:; goto L_3417;
  L_644:; 
  L_645:; 
  L_646:; 
  L_647:; (*u64_pframe(self, "Temp_struct_Variable_2")) = (*u64_pframe(self, "K2Node_CustomEvent_EventTag_6"));
  L_674:; 
  L_675:; 
  L_676:; (*u64_pframe(self, "Temp_struct_Variable_1")) = (*u64_pframe(self, "K2Node_CustomEvent_EventData_6"));
  L_703:; 
  L_704:; goto L_3417;
  L_709:; 
  L_710:; 
  L_711:; 
  L_712:; (*u64_pframe(self, "Temp_struct_Variable_2")) = (*u64_pframe(self, "K2Node_CustomEvent_EventTag_5"));
  L_739:; 
  L_740:; 
  L_741:; (*u64_pframe(self, "Temp_struct_Variable_1")) = (*u64_pframe(self, "K2Node_CustomEvent_EventData_5"));
  L_768:; 
  L_769:; goto L_3417;
  L_774:; 
  L_775:; 
  L_776:; 
  L_777:; (*u64_pframe(self, "Temp_struct_Variable_4")) = (*u64_pframe(self, "K2Node_CustomEvent_EventTag_4"));
  L_804:; 
  L_805:; 
  L_806:; (*u64_pframe(self, "Temp_struct_Variable_3")) = (*u64_pframe(self, "K2Node_CustomEvent_EventData_4"));
  L_833:; 
  L_834:; 
  L_835:; (*u64_pframe(self, "CallFunc_WaitNetSync_ReturnValue")) = U64_CALL(self, "WaitNetSync", u64_objv(self), u64_int(1));
  L_858:; 
  L_859:; (*u64_pframe(self, "CallFunc_IsValid_ReturnValue")) = U64_CALL(self, "IsValid", (*u64_pframe(self, "CallFunc_WaitNetSync_ReturnValue")));
  L_888:; 
  L_889:; 
  L_890:; if (!u64_true((*u64_pframe(self, "CallFunc_IsValid_ReturnValue")))) goto L_3417;
  L_904:; 
  L_905:; (*u64_pframe(self, "K2Node_CreateDelegate_OutputDelegate_10")) = u64_delegate(u64_objv(self), "OnSync_3BCEE229463F0C5D3C551A8105552B96");
  L_928:; 
  L_929:; u64_mc_add(&(*u64_ivar(u64_asobj((*u64_pframe(self, "CallFunc_WaitNetSync_ReturnValue"))), "OnSync")), (*u64_pframe(self, "K2Node_CreateDelegate_OutputDelegate_10")));
  L_970:; 
  L_971:; 
  L_972:; ({ u64_v _o = (*u64_pframe(self, "CallFunc_WaitNetSync_ReturnValue")); U64_CALL(u64_asobj(_o), "ReadyForActivation"); });
  L_1004:; 
  L_1005:; goto L_3417;
  L_1010:; 
  L_1011:; 
  L_1012:; 
  L_1013:; (*u64_pframe(self, "Temp_struct_Variable_4")) = (*u64_pframe(self, "K2Node_CustomEvent_EventTag_3"));
  L_1040:; 
  L_1041:; 
  L_1042:; (*u64_pframe(self, "Temp_struct_Variable_3")) = (*u64_pframe(self, "K2Node_CustomEvent_EventData_3"));
  L_1069:; 
  L_1070:; 
  L_1071:; U64_CALL(self, "K2_CancelAbility");
  L_1081:; 
  L_1082:; goto L_3417;
  L_1087:; 
  L_1088:; 
  L_1089:; 
  L_1090:; (*u64_pframe(self, "Temp_struct_Variable_4")) = (*u64_pframe(self, "K2Node_CustomEvent_EventTag_2"));
  L_1117:; 
  L_1118:; 
  L_1119:; (*u64_pframe(self, "Temp_struct_Variable_3")) = (*u64_pframe(self, "K2Node_CustomEvent_EventData_2"));
  L_1146:; 
  L_1147:; goto L_1070;
  L_1152:; 
  L_1153:; 
  L_1154:; 
  L_1155:; (*u64_pframe(self, "Temp_struct_Variable_4")) = (*u64_pframe(self, "K2Node_CustomEvent_EventTag_1"));
  L_1182:; 
  L_1183:; 
  L_1184:; (*u64_pframe(self, "Temp_struct_Variable_3")) = (*u64_pframe(self, "K2Node_CustomEvent_EventData_1"));
  L_1211:; 
  L_1212:; goto L_3417;
  L_1217:; 
  L_1218:; 
  L_1219:; 
  L_1220:; (*u64_pframe(self, "Temp_struct_Variable_4")) = (*u64_pframe(self, "K2Node_CustomEvent_EventTag"));
  L_1247:; 
  L_1248:; 
  L_1249:; (*u64_pframe(self, "Temp_struct_Variable_3")) = (*u64_pframe(self, "K2Node_CustomEvent_EventData"));
  L_1276:; 
  L_1277:; 
  L_1278:; (*u64_pframe(self, "CallFunc_WaitNetSync_ReturnValue_1")) = U64_CALL(self, "WaitNetSync", u64_objv(self), u64_int(0));
  L_1301:; 
  L_1302:; (*u64_pframe(self, "CallFunc_IsValid_ReturnValue_1")) = U64_CALL(self, "IsValid", (*u64_pframe(self, "CallFunc_WaitNetSync_ReturnValue_1")));
  L_1331:; 
  L_1332:; 
  L_1333:; if (!u64_true((*u64_pframe(self, "CallFunc_IsValid_ReturnValue_1")))) goto L_3417;
  L_1347:; 
  L_1348:; (*u64_pframe(self, "K2Node_CreateDelegate_OutputDelegate_11")) = u64_delegate(u64_objv(self), "OnSync_15DB9A6948DE9ADB7B9EB1A2ED020EDB");
  L_1371:; 
  L_1372:; u64_mc_add(&(*u64_ivar(u64_asobj((*u64_pframe(self, "CallFunc_WaitNetSync_ReturnValue_1"))), "OnSync")), (*u64_pframe(self, "K2Node_CreateDelegate_OutputDelegate_11")));
  L_1413:; 
  L_1414:; 
  L_1415:; ({ u64_v _o = (*u64_pframe(self, "CallFunc_WaitNetSync_ReturnValue_1")); U64_CALL(u64_asobj(_o), "ReadyForActivation"); });
  L_1447:; 
  L_1448:; goto L_3417;
  L_1453:; 
  L_1454:; 
  L_1455:; (*u64_pframe(self, "CallFunc_GetMaxPrimaryClipAmmo_ReturnValue")) = ({ u64_v _o = (*u64_ivar(self, "SourceWeapon")); U64_CALL(u64_asobj(_o), "GetMaxPrimaryClipAmmo"); });
  L_1509:; (*u64_pframe(self, "CallFunc_GetPrimaryClipAmmo_ReturnValue")) = ({ u64_v _o = (*u64_ivar(self, "SourceWeapon")); U64_CALL(u64_asobj(_o), "GetPrimaryClipAmmo"); });
  L_1563:; (*u64_pframe(self, "CallFunc_Subtract_IntInt_ReturnValue")) = U64_CALL(self, "Subtract_IntInt", (*u64_pframe(self, "CallFunc_GetMaxPrimaryClipAmmo_ReturnValue")), (*u64_pframe(self, "CallFunc_GetPrimaryClipAmmo_ReturnValue")));
  L_1609:; 
  L_1610:; (*u64_ivar(self, "AmountWeWantToReload")) = (*u64_pframe(self, "CallFunc_Subtract_IntInt_ReturnValue"));
  L_1637:; 
  L_1638:; (*u64_pframe(self, "CallFunc_GetPrimaryReserveAmmo_ReturnValue")) = ({ u64_v _o = (*u64_ivar(self, "OwningHero")); U64_CALL(u64_asobj(_o), "GetPrimaryReserveAmmo"); });
  L_1688:; (*u64_pframe(self, "CallFunc_LessEqual_IntInt_ReturnValue")) = U64_CALL(self, "LessEqual_IntInt", (*u64_ivar(self, "AmountWeWantToReload")), (*u64_pframe(self, "CallFunc_GetPrimaryReserveAmmo_ReturnValue")));
  L_1726:; 
  L_1727:; 
  L_1728:; if (!u64_true((*u64_pframe(self, "CallFunc_LessEqual_IntInt_ReturnValue")))) goto L_1777;
  L_1742:; 
  L_1743:; 
  L_1744:; (*u64_ivar(self, "AmountWeCanReload")) = (*u64_ivar(self, "AmountWeWantToReload"));
  L_1771:; 
  L_1772:; goto L_10;
  L_1777:; (*u64_pframe(self, "CallFunc_GetPrimaryReserveAmmo_ReturnValue")) = ({ u64_v _o = (*u64_ivar(self, "OwningHero")); U64_CALL(u64_asobj(_o), "GetPrimaryReserveAmmo"); });
  L_1827:; 
  L_1828:; (*u64_ivar(self, "AmountWeCanReload")) = (*u64_pframe(self, "CallFunc_GetPrimaryReserveAmmo_ReturnValue"));
  L_1855:; 
  L_1856:; goto L_10;
  L_1861:; 
  L_1862:; 
  L_1863:; 
  L_1864:; U64_CALL(self, "K2_EndAbility");
  L_1878:; 
  L_1879:; goto L_3417;
  L_1884:; 
  L_1885:; 
  L_1886:; (*u64_pframe(self, "CallFunc_GetThirdPersonMesh_ReturnValue")) = ({ u64_v _o = (*u64_ivar(self, "OwningHero")); U64_CALL(u64_asobj(_o), "GetThirdPersonMesh"); });
  L_1928:; 
  L_1929:; (*u64_pframe(self, "CallFunc_PlayMontageForMeshAndWaitForEvent_ReturnValue")) = U64_CALL(self, "PlayMontageForMeshAndWaitForEvent", u64_objv(self), u64_name("None"), (*u64_pframe(self, "CallFunc_GetThirdPersonMesh_ReturnValue")), u64_objv(u64_find("Reload_Montage")), u64_int(0) /* struct const */, u64_float(1.0f), u64_name("None"), u64_bool(0), u64_float(1.0f), u64_bool(1), u64_float(0.0f), u64_float(0.0f));
  L_2044:; 
  L_2045:; (*u64_pframe(self, "CallFunc_IsValid_ReturnValue_4")) = U64_CALL(self, "IsValid", (*u64_pframe(self, "CallFunc_PlayMontageForMeshAndWaitForEvent_ReturnValue")));
  L_2074:; 
  L_2075:; 
  L_2076:; if (!u64_true((*u64_pframe(self, "CallFunc_IsValid_ReturnValue_4")))) goto L_2455;
  L_2090:; 
  L_2091:; (*u64_pframe(self, "K2Node_CreateDelegate_OutputDelegate_4")) = u64_delegate(u64_objv(self), "OnCompleted_509569DE4A112ADF211346AAAF6500D6");
  L_2114:; 
  L_2115:; u64_mc_add(&(*u64_ivar(u64_asobj((*u64_pframe(self, "CallFunc_PlayMontageForMeshAndWaitForEvent_ReturnValue"))), "OnCompleted")), (*u64_pframe(self, "K2Node_CreateDelegate_OutputDelegate_4")));
  L_2156:; 
  L_2157:; (*u64_pframe(self, "K2Node_CreateDelegate_OutputDelegate_3")) = u64_delegate(u64_objv(self), "OnBlendOut_509569DE4A112ADF211346AAAF6500D6");
  L_2180:; 
  L_2181:; u64_mc_add(&(*u64_ivar(u64_asobj((*u64_pframe(self, "CallFunc_PlayMontageForMeshAndWaitForEvent_ReturnValue"))), "OnBlendOut")), (*u64_pframe(self, "K2Node_CreateDelegate_OutputDelegate_3")));
  L_2222:; 
  L_2223:; (*u64_pframe(self, "K2Node_CreateDelegate_OutputDelegate_2")) = u64_delegate(u64_objv(self), "OnInterrupted_509569DE4A112ADF211346AAAF6500D6");
  L_2246:; 
  L_2247:; u64_mc_add(&(*u64_ivar(u64_asobj((*u64_pframe(self, "CallFunc_PlayMontageForMeshAndWaitForEvent_ReturnValue"))), "OnInterrupted")), (*u64_pframe(self, "K2Node_CreateDelegate_OutputDelegate_2")));
  L_2288:; 
  L_2289:; (*u64_pframe(self, "K2Node_CreateDelegate_OutputDelegate_1")) = u64_delegate(u64_objv(self), "OnCancelled_509569DE4A112ADF211346AAAF6500D6");
  L_2312:; 
  L_2313:; u64_mc_add(&(*u64_ivar(u64_asobj((*u64_pframe(self, "CallFunc_PlayMontageForMeshAndWaitForEvent_ReturnValue"))), "OnCancelled")), (*u64_pframe(self, "K2Node_CreateDelegate_OutputDelegate_1")));
  L_2354:; 
  L_2355:; (*u64_pframe(self, "K2Node_CreateDelegate_OutputDelegate")) = u64_delegate(u64_objv(self), "EventReceived_509569DE4A112ADF211346AAAF6500D6");
  L_2378:; 
  L_2379:; u64_mc_add(&(*u64_ivar(u64_asobj((*u64_pframe(self, "CallFunc_PlayMontageForMeshAndWaitForEvent_ReturnValue"))), "EventReceived")), (*u64_pframe(self, "K2Node_CreateDelegate_OutputDelegate")));
  L_2420:; 
  L_2421:; 
  L_2422:; ({ u64_v _o = (*u64_pframe(self, "CallFunc_PlayMontageForMeshAndWaitForEvent_ReturnValue")); U64_CALL(u64_asobj(_o), "ReadyForActivation"); });
  L_2454:; 
  L_2455:; (*u64_pframe(self, "CallFunc_GetFirstPersonMesh_ReturnValue")) = ({ u64_v _o = (*u64_ivar(self, "OwningHero")); U64_CALL(u64_asobj(_o), "GetFirstPersonMesh"); });
  L_2497:; 
  L_2498:; (*u64_pframe(self, "CallFunc_PlayMontageForMeshAndWaitForEvent_ReturnValue_1")) = U64_CALL(self, "PlayMontageForMeshAndWaitForEvent", u64_objv(self), u64_name("None"), (*u64_pframe(self, "CallFunc_GetFirstPersonMesh_ReturnValue")), u64_objv(u64_find("HeroFPP_RifleReload_Montage")), u64_int(0) /* struct const */, u64_float(1.0f), u64_name("None"), u64_bool(0), u64_float(1.0f), u64_bool(0), u64_float(0.0f), u64_float(0.0f));
  L_2640:; 
  L_2641:; (*u64_pframe(self, "CallFunc_IsValid_ReturnValue_3")) = U64_CALL(self, "IsValid", (*u64_pframe(self, "CallFunc_PlayMontageForMeshAndWaitForEvent_ReturnValue_1")));
  L_2670:; 
  L_2671:; 
  L_2672:; if (!u64_true((*u64_pframe(self, "CallFunc_IsValid_ReturnValue_3")))) goto L_3417;
  L_2686:; 
  L_2687:; (*u64_pframe(self, "K2Node_CreateDelegate_OutputDelegate_9")) = u64_delegate(u64_objv(self), "OnCompleted_84B755724B7B5A7234C350887A509000");
  L_2710:; 
  L_2711:; u64_mc_add(&(*u64_ivar(u64_asobj((*u64_pframe(self, "CallFunc_PlayMontageForMeshAndWaitForEvent_ReturnValue_1"))), "OnCompleted")), (*u64_pframe(self, "K2Node_CreateDelegate_OutputDelegate_9")));
  L_2752:; 
  L_2753:; (*u64_pframe(self, "K2Node_CreateDelegate_OutputDelegate_8")) = u64_delegate(u64_objv(self), "OnBlendOut_84B755724B7B5A7234C350887A509000");
  L_2776:; 
  L_2777:; u64_mc_add(&(*u64_ivar(u64_asobj((*u64_pframe(self, "CallFunc_PlayMontageForMeshAndWaitForEvent_ReturnValue_1"))), "OnBlendOut")), (*u64_pframe(self, "K2Node_CreateDelegate_OutputDelegate_8")));
  L_2818:; 
  L_2819:; (*u64_pframe(self, "K2Node_CreateDelegate_OutputDelegate_7")) = u64_delegate(u64_objv(self), "OnInterrupted_84B755724B7B5A7234C350887A509000");
  L_2842:; 
  L_2843:; u64_mc_add(&(*u64_ivar(u64_asobj((*u64_pframe(self, "CallFunc_PlayMontageForMeshAndWaitForEvent_ReturnValue_1"))), "OnInterrupted")), (*u64_pframe(self, "K2Node_CreateDelegate_OutputDelegate_7")));
  L_2884:; 
  L_2885:; (*u64_pframe(self, "K2Node_CreateDelegate_OutputDelegate_6")) = u64_delegate(u64_objv(self), "OnCancelled_84B755724B7B5A7234C350887A509000");
  L_2908:; 
  L_2909:; u64_mc_add(&(*u64_ivar(u64_asobj((*u64_pframe(self, "CallFunc_PlayMontageForMeshAndWaitForEvent_ReturnValue_1"))), "OnCancelled")), (*u64_pframe(self, "K2Node_CreateDelegate_OutputDelegate_6")));
  L_2950:; 
  L_2951:; (*u64_pframe(self, "K2Node_CreateDelegate_OutputDelegate_5")) = u64_delegate(u64_objv(self), "EventReceived_84B755724B7B5A7234C350887A509000");
  L_2974:; 
  L_2975:; u64_mc_add(&(*u64_ivar(u64_asobj((*u64_pframe(self, "CallFunc_PlayMontageForMeshAndWaitForEvent_ReturnValue_1"))), "EventReceived")), (*u64_pframe(self, "K2Node_CreateDelegate_OutputDelegate_5")));
  L_3016:; 
  L_3017:; 
  L_3018:; ({ u64_v _o = (*u64_pframe(self, "CallFunc_PlayMontageForMeshAndWaitForEvent_ReturnValue_1")); U64_CALL(u64_asobj(_o), "ReadyForActivation"); });
  L_3050:; 
  L_3051:; goto L_3417;
  L_3056:; (*u64_pframe(self, "CallFunc_GetCurrentSourceObject_ReturnValue")) = U64_CALL(self, "GetCurrentSourceObject");
  L_3076:; 
  L_3077:; (*u64_pframe(self, "K2Node_DynamicCast_AsGSWeapon")) = u64_cast((*u64_pframe(self, "CallFunc_GetCurrentSourceObject_ReturnValue")), "GSWeapon");
  L_3113:; (*u64_pframe(self, "K2Node_DynamicCast_bSuccess")) = (*u64_pframe(self, "K2Node_DynamicCast_AsGSWeapon"));
  L_3142:; 
  L_3143:; if (!u64_true((*u64_pframe(self, "K2Node_DynamicCast_bSuccess")))) goto L_3417;
  L_3157:; 
  L_3158:; 
  L_3159:; (*u64_ivar(self, "SourceWeapon")) = (*u64_pframe(self, "K2Node_DynamicCast_AsGSWeapon"));
  L_3178:; 
  L_3179:; (*u64_pframe(self, "CallFunc_IsValid_ReturnValue_2")) = U64_CALL(self, "IsValid", (*u64_ivar(self, "OwningHero")));
  L_3208:; 
  L_3209:; 
  L_3210:; if (!u64_true((*u64_pframe(self, "CallFunc_IsValid_ReturnValue_2")))) goto L_3230;
  L_3224:; 
  L_3225:; goto L_1884;
  L_3230:; (*u64_pframe(self, "CallFunc_GetAvatarActorFromActorInfo_ReturnValue")) = U64_CALL(self, "GetAvatarActorFromActorInfo");
  L_3250:; 
  L_3251:; (*u64_pframe(self, "K2Node_DynamicCast_AsBP_Hero_Character")) = u64_cast((*u64_pframe(self, "CallFunc_GetAvatarActorFromActorInfo_ReturnValue")), "BP_HeroCharacter");
  L_3287:; (*u64_pframe(self, "K2Node_DynamicCast_bSuccess_1")) = (*u64_pframe(self, "K2Node_DynamicCast_AsBP_Hero_Character"));
  L_3316:; 
  L_3317:; if (!u64_true((*u64_pframe(self, "K2Node_DynamicCast_bSuccess_1")))) goto L_3417;
  L_3331:; 
  L_3332:; 
  L_3333:; (*u64_ivar(self, "OwningHero")) = (*u64_pframe(self, "K2Node_DynamicCast_AsBP_Hero_Character"));
  L_3352:; 
  L_3353:; goto L_1884;
  L_3358:; 
  L_3359:; 
  L_3360:; 
  L_3361:; 
  L_3362:; 
  L_3363:; 
  L_3364:; (*u64_pframe(self, "CallFunc_IsValid_ReturnValue_5")) = U64_CALL(self, "IsValid", (*u64_ivar(self, "SourceWeapon")));
  L_3393:; 
  L_3394:; 
  L_3395:; if (!u64_true((*u64_pframe(self, "CallFunc_IsValid_ReturnValue_5")))) goto L_3056;
  L_3409:; 
  L_3410:; goto L_3179;
  L_3415:; 
  L_3416:; 
  L_3417:; goto u64_ret;
  L_3419:; 
  u64_ret:; u64_set_return(L.ReturnValue);

}

void bp_GA_ShotgunReload__K2_OnEndAbility(u64_obj *self, const u64_v *args, int argc) {
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
  L_23:; U64_CALL(self, "ExecuteUbergraph_GA_ShotgunReload", u64_int(3415));
  L_38:; 
  L_39:; goto u64_ret;
  L_41:; 
  u64_ret:; u64_set_return(L.ReturnValue);

}

void bp_GA_ShotgunReload__K2_ActivateAbility(u64_obj *self, const u64_v *args, int argc) {
  (void)args; (void)argc;
  struct { u64_v ReturnValue; u64_v _unsupported; } L; memset(&L, 0, sizeof L); (void)L;
  U64_FLOW_DECL; (void)flow_n;
  L_0:; 
  L_1:; 
  L_2:; 
  L_3:; U64_CALL(self, "ExecuteUbergraph_GA_ShotgunReload", u64_int(3358));
  L_18:; 
  L_19:; goto u64_ret;
  L_21:; 
  u64_ret:; u64_set_return(L.ReturnValue);

}

void bp_GA_ShotgunReload__OnSync_15DB9A6948DE9ADB7B9EB1A2ED020EDB(u64_obj *self, const u64_v *args, int argc) {
  (void)args; (void)argc;
  struct { u64_v ReturnValue; u64_v _unsupported; } L; memset(&L, 0, sizeof L); (void)L;
  U64_FLOW_DECL; (void)flow_n;
  L_0:; 
  L_1:; 
  L_2:; 
  L_3:; U64_CALL(self, "ExecuteUbergraph_GA_ShotgunReload", u64_int(1861));
  L_18:; 
  L_19:; goto u64_ret;
  L_21:; 
  u64_ret:; u64_set_return(L.ReturnValue);

}

void bp_GA_ShotgunReload__OnSync_3BCEE229463F0C5D3C551A8105552B96(u64_obj *self, const u64_v *args, int argc) {
  (void)args; (void)argc;
  struct { u64_v ReturnValue; u64_v _unsupported; } L; memset(&L, 0, sizeof L); (void)L;
  U64_FLOW_DECL; (void)flow_n;
  L_0:; 
  L_1:; 
  L_2:; 
  L_3:; U64_CALL(self, "ExecuteUbergraph_GA_ShotgunReload", u64_int(1453));
  L_18:; 
  L_19:; goto u64_ret;
  L_21:; 
  u64_ret:; u64_set_return(L.ReturnValue);

}

void bp_GA_ShotgunReload__OnCompleted_84B755724B7B5A7234C350887A509000(u64_obj *self, const u64_v *args, int argc) {
  (void)args; (void)argc;
  struct { u64_v EventData; u64_v EventTag; u64_v ReturnValue; u64_v _unsupported; } L; memset(&L, 0, sizeof L); (void)L;
  U64_FLOW_DECL; (void)flow_n;
  if (argc > 0) L.EventTag = args[0];
  if (argc > 1) L.EventData = args[1];
  L_0:; 
  L_1:; 
  L_2:; 
  L_3:; *u64_pframe(self, "K2Node_CustomEvent_EventTag") = L.EventTag;
  L_21:; *u64_pframe(self, "K2Node_CustomEvent_EventData") = L.EventData;
  L_39:; 
  L_40:; 
  L_41:; U64_CALL(self, "ExecuteUbergraph_GA_ShotgunReload", u64_int(1217));
  L_56:; 
  L_57:; goto u64_ret;
  L_59:; 
  u64_ret:; u64_set_return(L.ReturnValue);

}

void bp_GA_ShotgunReload__OnBlendOut_84B755724B7B5A7234C350887A509000(u64_obj *self, const u64_v *args, int argc) {
  (void)args; (void)argc;
  struct { u64_v EventData; u64_v EventTag; u64_v ReturnValue; u64_v _unsupported; } L; memset(&L, 0, sizeof L); (void)L;
  U64_FLOW_DECL; (void)flow_n;
  if (argc > 0) L.EventTag = args[0];
  if (argc > 1) L.EventData = args[1];
  L_0:; 
  L_1:; 
  L_2:; 
  L_3:; *u64_pframe(self, "K2Node_CustomEvent_EventTag_1") = L.EventTag;
  L_21:; *u64_pframe(self, "K2Node_CustomEvent_EventData_1") = L.EventData;
  L_39:; 
  L_40:; 
  L_41:; U64_CALL(self, "ExecuteUbergraph_GA_ShotgunReload", u64_int(1152));
  L_56:; 
  L_57:; goto u64_ret;
  L_59:; 
  u64_ret:; u64_set_return(L.ReturnValue);

}

void bp_GA_ShotgunReload__OnInterrupted_84B755724B7B5A7234C350887A509000(u64_obj *self, const u64_v *args, int argc) {
  (void)args; (void)argc;
  struct { u64_v EventData; u64_v EventTag; u64_v ReturnValue; u64_v _unsupported; } L; memset(&L, 0, sizeof L); (void)L;
  U64_FLOW_DECL; (void)flow_n;
  if (argc > 0) L.EventTag = args[0];
  if (argc > 1) L.EventData = args[1];
  L_0:; 
  L_1:; 
  L_2:; 
  L_3:; *u64_pframe(self, "K2Node_CustomEvent_EventTag_2") = L.EventTag;
  L_21:; *u64_pframe(self, "K2Node_CustomEvent_EventData_2") = L.EventData;
  L_39:; 
  L_40:; 
  L_41:; U64_CALL(self, "ExecuteUbergraph_GA_ShotgunReload", u64_int(1087));
  L_56:; 
  L_57:; goto u64_ret;
  L_59:; 
  u64_ret:; u64_set_return(L.ReturnValue);

}

void bp_GA_ShotgunReload__OnCancelled_84B755724B7B5A7234C350887A509000(u64_obj *self, const u64_v *args, int argc) {
  (void)args; (void)argc;
  struct { u64_v EventData; u64_v EventTag; u64_v ReturnValue; u64_v _unsupported; } L; memset(&L, 0, sizeof L); (void)L;
  U64_FLOW_DECL; (void)flow_n;
  if (argc > 0) L.EventTag = args[0];
  if (argc > 1) L.EventData = args[1];
  L_0:; 
  L_1:; 
  L_2:; 
  L_3:; *u64_pframe(self, "K2Node_CustomEvent_EventTag_3") = L.EventTag;
  L_21:; *u64_pframe(self, "K2Node_CustomEvent_EventData_3") = L.EventData;
  L_39:; 
  L_40:; 
  L_41:; U64_CALL(self, "ExecuteUbergraph_GA_ShotgunReload", u64_int(1010));
  L_56:; 
  L_57:; goto u64_ret;
  L_59:; 
  u64_ret:; u64_set_return(L.ReturnValue);

}

void bp_GA_ShotgunReload__EventReceived_84B755724B7B5A7234C350887A509000(u64_obj *self, const u64_v *args, int argc) {
  (void)args; (void)argc;
  struct { u64_v EventData; u64_v EventTag; u64_v ReturnValue; u64_v _unsupported; } L; memset(&L, 0, sizeof L); (void)L;
  U64_FLOW_DECL; (void)flow_n;
  if (argc > 0) L.EventTag = args[0];
  if (argc > 1) L.EventData = args[1];
  L_0:; 
  L_1:; 
  L_2:; 
  L_3:; *u64_pframe(self, "K2Node_CustomEvent_EventTag_4") = L.EventTag;
  L_21:; *u64_pframe(self, "K2Node_CustomEvent_EventData_4") = L.EventData;
  L_39:; 
  L_40:; 
  L_41:; U64_CALL(self, "ExecuteUbergraph_GA_ShotgunReload", u64_int(774));
  L_56:; 
  L_57:; goto u64_ret;
  L_59:; 
  u64_ret:; u64_set_return(L.ReturnValue);

}

void bp_GA_ShotgunReload__OnCompleted_509569DE4A112ADF211346AAAF6500D6(u64_obj *self, const u64_v *args, int argc) {
  (void)args; (void)argc;
  struct { u64_v EventData; u64_v EventTag; u64_v ReturnValue; u64_v _unsupported; } L; memset(&L, 0, sizeof L); (void)L;
  U64_FLOW_DECL; (void)flow_n;
  if (argc > 0) L.EventTag = args[0];
  if (argc > 1) L.EventData = args[1];
  L_0:; 
  L_1:; 
  L_2:; 
  L_3:; *u64_pframe(self, "K2Node_CustomEvent_EventTag_5") = L.EventTag;
  L_21:; *u64_pframe(self, "K2Node_CustomEvent_EventData_5") = L.EventData;
  L_39:; 
  L_40:; 
  L_41:; U64_CALL(self, "ExecuteUbergraph_GA_ShotgunReload", u64_int(709));
  L_56:; 
  L_57:; goto u64_ret;
  L_59:; 
  u64_ret:; u64_set_return(L.ReturnValue);

}

void bp_GA_ShotgunReload__OnBlendOut_509569DE4A112ADF211346AAAF6500D6(u64_obj *self, const u64_v *args, int argc) {
  (void)args; (void)argc;
  struct { u64_v EventData; u64_v EventTag; u64_v ReturnValue; u64_v _unsupported; } L; memset(&L, 0, sizeof L); (void)L;
  U64_FLOW_DECL; (void)flow_n;
  if (argc > 0) L.EventTag = args[0];
  if (argc > 1) L.EventData = args[1];
  L_0:; 
  L_1:; 
  L_2:; 
  L_3:; *u64_pframe(self, "K2Node_CustomEvent_EventTag_6") = L.EventTag;
  L_21:; *u64_pframe(self, "K2Node_CustomEvent_EventData_6") = L.EventData;
  L_39:; 
  L_40:; 
  L_41:; U64_CALL(self, "ExecuteUbergraph_GA_ShotgunReload", u64_int(644));
  L_56:; 
  L_57:; goto u64_ret;
  L_59:; 
  u64_ret:; u64_set_return(L.ReturnValue);

}

void bp_GA_ShotgunReload__OnInterrupted_509569DE4A112ADF211346AAAF6500D6(u64_obj *self, const u64_v *args, int argc) {
  (void)args; (void)argc;
  struct { u64_v EventData; u64_v EventTag; u64_v ReturnValue; u64_v _unsupported; } L; memset(&L, 0, sizeof L); (void)L;
  U64_FLOW_DECL; (void)flow_n;
  if (argc > 0) L.EventTag = args[0];
  if (argc > 1) L.EventData = args[1];
  L_0:; 
  L_1:; 
  L_2:; 
  L_3:; *u64_pframe(self, "K2Node_CustomEvent_EventTag_7") = L.EventTag;
  L_21:; *u64_pframe(self, "K2Node_CustomEvent_EventData_7") = L.EventData;
  L_39:; 
  L_40:; 
  L_41:; U64_CALL(self, "ExecuteUbergraph_GA_ShotgunReload", u64_int(579));
  L_56:; 
  L_57:; goto u64_ret;
  L_59:; 
  u64_ret:; u64_set_return(L.ReturnValue);

}

void bp_GA_ShotgunReload__OnCancelled_509569DE4A112ADF211346AAAF6500D6(u64_obj *self, const u64_v *args, int argc) {
  (void)args; (void)argc;
  struct { u64_v EventData; u64_v EventTag; u64_v ReturnValue; u64_v _unsupported; } L; memset(&L, 0, sizeof L); (void)L;
  U64_FLOW_DECL; (void)flow_n;
  if (argc > 0) L.EventTag = args[0];
  if (argc > 1) L.EventData = args[1];
  L_0:; 
  L_1:; 
  L_2:; 
  L_3:; *u64_pframe(self, "K2Node_CustomEvent_EventTag_8") = L.EventTag;
  L_21:; *u64_pframe(self, "K2Node_CustomEvent_EventData_8") = L.EventData;
  L_39:; 
  L_40:; 
  L_41:; U64_CALL(self, "ExecuteUbergraph_GA_ShotgunReload", u64_int(514));
  L_56:; 
  L_57:; goto u64_ret;
  L_59:; 
  u64_ret:; u64_set_return(L.ReturnValue);

}

void bp_GA_ShotgunReload__EventReceived_509569DE4A112ADF211346AAAF6500D6(u64_obj *self, const u64_v *args, int argc) {
  (void)args; (void)argc;
  struct { u64_v EventData; u64_v EventTag; u64_v ReturnValue; u64_v _unsupported; } L; memset(&L, 0, sizeof L); (void)L;
  U64_FLOW_DECL; (void)flow_n;
  if (argc > 0) L.EventTag = args[0];
  if (argc > 1) L.EventData = args[1];
  L_0:; 
  L_1:; 
  L_2:; 
  L_3:; *u64_pframe(self, "K2Node_CustomEvent_EventTag_9") = L.EventTag;
  L_21:; *u64_pframe(self, "K2Node_CustomEvent_EventData_9") = L.EventData;
  L_39:; 
  L_40:; 
  L_41:; U64_CALL(self, "ExecuteUbergraph_GA_ShotgunReload", u64_int(449));
  L_56:; 
  L_57:; goto u64_ret;
  L_59:; 
  u64_ret:; u64_set_return(L.ReturnValue);

}

void bp_GA_ShotgunReload__K2_CanActivateAbility(u64_obj *self, const u64_v *args, int argc) {
  (void)args; (void)argc;
  struct { u64_v ActorInfo; u64_v CallFunc_BooleanAND_ReturnValue; u64_v CallFunc_GetMaxPrimaryClipAmmo_ReturnValue; u64_v CallFunc_GetPrimaryClipAmmo_ReturnValue; u64_v CallFunc_GetPrimaryReserveAmmo_ReturnValue; u64_v CallFunc_Greater_IntInt_ReturnValue; u64_v CallFunc_Less_IntInt_ReturnValue; u64_v K2Node_DynamicCast_AsBP_Hero_Character; u64_v K2Node_DynamicCast_bSuccess; u64_v RelevantTags; u64_v ReturnValue; u64_v _unsupported; } L; memset(&L, 0, sizeof L); (void)L;
  U64_FLOW_DECL; (void)flow_n;
  if (argc > 0) L.ActorInfo = args[0];
  if (argc > 1) L.RelevantTags = args[1].t == U64_REF ? *args[1].u.r : args[1];
  L_0:; 
  L_1:; 
  L_2:; 
  L_3:; L.K2Node_DynamicCast_AsBP_Hero_Character = u64_cast((*u64_member(&L.ActorInfo, "AvatarActor")), "BP_HeroCharacter");
  L_48:; L.K2Node_DynamicCast_bSuccess = L.K2Node_DynamicCast_AsBP_Hero_Character;
  L_77:; 
  L_78:; if (!u64_true(L.K2Node_DynamicCast_bSuccess)) goto L_420;
  L_92:; 
  L_93:; L.CallFunc_GetPrimaryClipAmmo_ReturnValue = ({ u64_v _o = L.K2Node_DynamicCast_AsBP_Hero_Character; U64_CALL(u64_asobj(_o), "GetPrimaryClipAmmo"); });
  L_143:; L.CallFunc_GetPrimaryReserveAmmo_ReturnValue = ({ u64_v _o = L.K2Node_DynamicCast_AsBP_Hero_Character; U64_CALL(u64_asobj(_o), "GetPrimaryReserveAmmo"); });
  L_193:; L.CallFunc_GetMaxPrimaryClipAmmo_ReturnValue = ({ u64_v _o = L.K2Node_DynamicCast_AsBP_Hero_Character; U64_CALL(u64_asobj(_o), "GetMaxPrimaryClipAmmo"); });
  L_243:; L.CallFunc_Greater_IntInt_ReturnValue = U64_CALL(self, "Greater_IntInt", L.CallFunc_GetPrimaryReserveAmmo_ReturnValue, u64_int(0));
  L_277:; L.CallFunc_Less_IntInt_ReturnValue = U64_CALL(self, "Less_IntInt", L.CallFunc_GetPrimaryClipAmmo_ReturnValue, L.CallFunc_GetMaxPrimaryClipAmmo_ReturnValue);
  L_315:; L.CallFunc_BooleanAND_ReturnValue = U64_CALL(self, "BooleanAND", L.CallFunc_Less_IntInt_ReturnValue, L.CallFunc_Greater_IntInt_ReturnValue);
  L_353:; 
  L_354:; L.RelevantTags = u64_int(0) /* struct const */;
  L_400:; L.ReturnValue = L.CallFunc_BooleanAND_ReturnValue;
  L_419:; 
  L_420:; L.ReturnValue = L.ReturnValue; goto u64_ret;
  L_430:; 
  u64_ret:; u64_set_return(L.ReturnValue);
  if (argc > 1) u64_out(args[1], L.RelevantTags);
}
