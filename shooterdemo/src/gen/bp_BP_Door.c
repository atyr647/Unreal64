#include "u64_rt.h"

void bp_BP_Door__ExecuteUbergraph_BP_Door(u64_obj *self, const u64_v *args, int argc) {
  (void)args; (void)argc;
  struct { u64_v ReturnValue; u64_v _unsupported; } L; memset(&L, 0, sizeof L); (void)L;
  U64_FLOW_DECL; (void)flow_n;
  if (argc > 0) (*u64_pframe(self, "EntryPoint")) = args[0];
  L_0:; U64_PUSH(&&L_2531);
  L_5:; u64_dispatch: switch (u64_as_int((*u64_pframe(self, "EntryPoint")))) { case 0: goto L_0; case 5: goto L_5; case 15: goto L_15; case 16: goto L_16; case 17: goto L_17; case 55: goto L_55; case 56: goto L_56; case 57: goto L_57; case 67: goto L_67; case 68: goto L_68; case 69: goto L_69; case 70: goto L_70; case 71: goto L_71; case 72: goto L_72; case 92: goto L_92; case 93: goto L_93; case 94: goto L_94; case 108: goto L_108; case 109: goto L_109; case 110: goto L_110; case 111: goto L_111; case 132: goto L_132; case 206: goto L_206; case 207: goto L_207; case 208: goto L_208; case 222: goto L_222; case 223: goto L_223; case 244: goto L_244; case 245: goto L_245; case 305: goto L_305; case 306: goto L_306; case 307: goto L_307; case 308: goto L_308; case 309: goto L_309; case 310: goto L_310; case 331: goto L_331; case 385: goto L_385; case 386: goto L_386; case 459: goto L_459; case 460: goto L_460; case 461: goto L_461; case 488: goto L_488; case 489: goto L_489; case 490: goto L_490; case 491: goto L_491; case 492: goto L_492; case 493: goto L_493; case 494: goto L_494; case 526: goto L_526; case 527: goto L_527; case 528: goto L_528; case 529: goto L_529; case 530: goto L_530; case 576: goto L_576; case 622: goto L_622; case 668: goto L_668; case 719: goto L_719; case 766: goto L_766; case 767: goto L_767; case 819: goto L_819; case 820: goto L_820; case 821: goto L_821; case 822: goto L_822; case 854: goto L_854; case 855: goto L_855; case 856: goto L_856; case 857: goto L_857; case 858: goto L_858; case 859: goto L_859; case 886: goto L_886; case 887: goto L_887; case 943: goto L_943; case 944: goto L_944; case 945: goto L_945; case 955: goto L_955; case 956: goto L_956; case 957: goto L_957; case 971: goto L_971; case 972: goto L_972; case 973: goto L_973; case 974: goto L_974; case 975: goto L_975; case 976: goto L_976; case 1003: goto L_1003; case 1004: goto L_1004; case 1060: goto L_1060; case 1061: goto L_1061; case 1062: goto L_1062; case 1072: goto L_1072; case 1073: goto L_1073; case 1074: goto L_1074; case 1088: goto L_1088; case 1089: goto L_1089; case 1090: goto L_1090; case 1119: goto L_1119; case 1120: goto L_1120; case 1121: goto L_1121; case 1135: goto L_1135; case 1136: goto L_1136; case 1137: goto L_1137; case 1169: goto L_1169; case 1170: goto L_1170; case 1238: goto L_1238; case 1239: goto L_1239; case 1266: goto L_1266; case 1267: goto L_1267; case 1313: goto L_1313; case 1350: goto L_1350; case 1396: goto L_1396; case 1397: goto L_1397; case 1424: goto L_1424; case 1425: goto L_1425; case 1426: goto L_1426; case 1431: goto L_1431; case 1432: goto L_1432; case 1437: goto L_1437; case 1438: goto L_1438; case 1439: goto L_1439; case 1444: goto L_1444; case 1445: goto L_1445; case 1486: goto L_1486; case 1487: goto L_1487; case 1488: goto L_1488; case 1517: goto L_1517; case 1518: goto L_1518; case 1519: goto L_1519; case 1533: goto L_1533; case 1534: goto L_1534; case 1535: goto L_1535; case 1567: goto L_1567; case 1568: goto L_1568; case 1636: goto L_1636; case 1637: goto L_1637; case 1664: goto L_1664; case 1665: goto L_1665; case 1711: goto L_1711; case 1748: goto L_1748; case 1790: goto L_1790; case 1836: goto L_1836; case 1837: goto L_1837; case 1864: goto L_1864; case 1865: goto L_1865; case 1866: goto L_1866; case 1871: goto L_1871; case 1872: goto L_1872; case 1877: goto L_1877; case 1878: goto L_1878; case 1879: goto L_1879; case 1884: goto L_1884; case 1885: goto L_1885; case 1926: goto L_1926; case 1927: goto L_1927; case 1928: goto L_1928; case 1929: goto L_1929; case 1930: goto L_1930; case 1931: goto L_1931; case 1932: goto L_1932; case 1933: goto L_1933; case 1934: goto L_1934; case 1935: goto L_1935; case 1936: goto L_1936; case 1937: goto L_1937; case 1938: goto L_1938; case 1939: goto L_1939; case 1960: goto L_1960; case 1961: goto L_1961; case 1999: goto L_1999; case 2000: goto L_2000; case 2029: goto L_2029; case 2030: goto L_2030; case 2031: goto L_2031; case 2041: goto L_2041; case 2042: goto L_2042; case 2065: goto L_2065; case 2066: goto L_2066; case 2107: goto L_2107; case 2108: goto L_2108; case 2131: goto L_2131; case 2132: goto L_2132; case 2173: goto L_2173; case 2174: goto L_2174; case 2175: goto L_2175; case 2211: goto L_2211; case 2212: goto L_2212; case 2213: goto L_2213; case 2214: goto L_2214; case 2215: goto L_2215; case 2220: goto L_2220; case 2221: goto L_2221; case 2222: goto L_2222; case 2227: goto L_2227; case 2228: goto L_2228; case 2229: goto L_2229; case 2230: goto L_2230; case 2231: goto L_2231; case 2277: goto L_2277; case 2323: goto L_2323; case 2369: goto L_2369; case 2420: goto L_2420; case 2467: goto L_2467; case 2468: goto L_2468; case 2520: goto L_2520; case 2521: goto L_2521; case 2522: goto L_2522; case 2523: goto L_2523; case 2524: goto L_2524; case 2525: goto L_2525; case 2526: goto L_2526; case 2527: goto L_2527; case 2528: goto L_2528; case 2529: goto L_2529; case 2530: goto L_2530; case 2531: goto L_2531; case 2533: goto L_2533; }
  L_15:; 
  L_16:; 
  L_17:; (*u64_pframe(self, "CallFunc_EqualEqual_ObjectObject_ReturnValue")) = U64_CALL(self, "EqualEqual_ObjectObject", (*u64_pframe(self, "K2Node_Event_InteractionComponent")), (*u64_ivar(self, "Button")));
  L_55:; 
  L_56:; 
  L_57:; if (!u64_true((*u64_pframe(self, "CallFunc_EqualEqual_ObjectObject_ReturnValue")))) goto *U64_POP();
  L_67:; 
  L_68:; 
  L_69:; 
  L_70:; 
  L_71:; 
  L_72:; (*u64_pframe(self, "CallFunc_HasAuthority_ReturnValue")) = U64_CALL(self, "HasAuthority");
  L_92:; 
  L_93:; 
  L_94:; if (!u64_true((*u64_pframe(self, "CallFunc_HasAuthority_ReturnValue")))) goto L_307;
  L_108:; 
  L_109:; 
  L_110:; 
  L_111:; (*u64_pframe(self, "CallFunc_GetAbilitySystemComponent_ReturnValue_1")) = U64_CALL(self, "GetAbilitySystemComponent", u64_objv(self));
  L_132:; (*u64_pframe(self, "CallFunc_HasMatchingGameplayTag_ReturnValue")) = ({ u64_v _o = (*u64_pframe(self, "CallFunc_GetAbilitySystemComponent_ReturnValue_1")); U64_CALL(u64_asobj(_o), "HasMatchingGameplayTag", u64_name("Object.Door.State.Open")); });
  L_206:; 
  L_207:; 
  L_208:; if (!u64_true((*u64_pframe(self, "CallFunc_HasMatchingGameplayTag_ReturnValue")))) goto L_310;
  L_222:; 
  L_223:; (*u64_pframe(self, "CallFunc_GetAbilitySystemComponent_ReturnValue_1")) = U64_CALL(self, "GetAbilitySystemComponent", u64_objv(self));
  L_244:; 
  L_245:; (*u64_pframe(self, "CallFunc_RemoveActiveGameplayEffect_ReturnValue")) = ({ u64_v _o = (*u64_pframe(self, "CallFunc_GetAbilitySystemComponent_ReturnValue_1")); U64_CALL(u64_asobj(_o), "RemoveActiveGameplayEffect", (*u64_ivar(self, "GE_DoorOpenHandle")), u64_int(-1)); });
  L_305:; 
  L_306:; goto *U64_POP();
  L_307:; 
  L_308:; 
  L_309:; goto *U64_POP();
  L_310:; (*u64_pframe(self, "CallFunc_GetAbilitySystemComponent_ReturnValue_1")) = U64_CALL(self, "GetAbilitySystemComponent", u64_objv(self));
  L_331:; (*u64_pframe(self, "CallFunc_MakeEffectContext_ReturnValue")) = ({ u64_v _o = (*u64_pframe(self, "CallFunc_GetAbilitySystemComponent_ReturnValue_1")); U64_CALL(u64_asobj(_o), "MakeEffectContext"); });
  L_385:; 
  L_386:; (*u64_pframe(self, "CallFunc_BP_ApplyGameplayEffectToSelf_ReturnValue")) = ({ u64_v _o = (*u64_pframe(self, "CallFunc_GetAbilitySystemComponent_ReturnValue_1")); U64_CALL(u64_asobj(_o), "BP_ApplyGameplayEffectToSelf", u64_objv(u64_find("GE_DoorOpen_C")), u64_float(0.0f), (*u64_pframe(self, "CallFunc_MakeEffectContext_ReturnValue"))); });
  L_459:; 
  L_460:; 
  L_461:; (*u64_ivar(self, "GE_DoorOpenHandle")) = (*u64_pframe(self, "CallFunc_BP_ApplyGameplayEffectToSelf_ReturnValue"));
  L_488:; 
  L_489:; goto *U64_POP();
  L_490:; 
  L_491:; 
  L_492:; goto *U64_POP();
  L_493:; 
  L_494:; ({ u64_v _o = (*u64_ivar(self, "DoorOpenTimeline")); U64_CALL(u64_asobj(_o), "Play"); });
  L_526:; 
  L_527:; goto *U64_POP();
  L_528:; 
  L_529:; 
  L_530:; (*u64_pframe(self, "CallFunc_Subtract_FloatFloat_ReturnValue")) = U64_CALL(self, "Subtract_FloatFloat", (*u64_ivar(self, "DoorCloseTimeline_Time_1CBEB1704FFCD69BAF449F89EA4CDDBD")), (*u64_ivar(self, "StartingTime")));
  L_576:; (*u64_pframe(self, "CallFunc_Subtract_FloatFloat_ReturnValue_3")) = U64_CALL(self, "Subtract_FloatFloat", (*u64_ivar(self, "TimeToOpenCloseDoor")), (*u64_ivar(self, "StartingTime")));
  L_622:; (*u64_pframe(self, "CallFunc_Divide_FloatFloat_ReturnValue_1")) = U64_CALL(self, "Divide_FloatFloat", (*u64_pframe(self, "CallFunc_Subtract_FloatFloat_ReturnValue")), (*u64_pframe(self, "CallFunc_Subtract_FloatFloat_ReturnValue_3")));
  L_668:; (*u64_pframe(self, "CallFunc_Lerp_ReturnValue_1")) = U64_CALL(self, "Lerp", (*u64_ivar(self, "DoorStartingRelativeLocationY")), u64_float(0.0f), (*u64_pframe(self, "CallFunc_Divide_FloatFloat_ReturnValue_1")));
  L_719:; (*u64_pframe(self, "CallFunc_MakeVector_ReturnValue_1")) = U64_CALL(self, "MakeVector", u64_float(0.0f), (*u64_pframe(self, "CallFunc_Lerp_ReturnValue_1")), u64_float(0.0f));
  L_766:; 
  L_767:; ({ u64_v _o = (*u64_ivar(self, "Door")); U64_CALL(u64_asobj(_o), "K2_SetRelativeLocation", (*u64_pframe(self, "CallFunc_MakeVector_ReturnValue_1")), u64_bool(0), u64_ref(&(*u64_pframe(self, "CallFunc_K2_SetRelativeLocation_SweepHitResult_1"))), u64_bool(0)); });
  L_819:; 
  L_820:; goto *U64_POP();
  L_821:; 
  L_822:; ({ u64_v _o = (*u64_ivar(self, "DoorCloseTimeline")); U64_CALL(u64_asobj(_o), "Play"); });
  L_854:; 
  L_855:; goto *U64_POP();
  L_856:; 
  L_857:; 
  L_858:; 
  L_859:; (*u64_pframe(self, "Temp_struct_Variable")) = (*u64_pframe(self, "K2Node_CustomEvent_Tag_1"));
  L_886:; 
  L_887:; (*u64_pframe(self, "CallFunc_EqualEqual_GameplayTag_ReturnValue")) = U64_CALL(self, "EqualEqual_GameplayTag", (*u64_pframe(self, "Temp_struct_Variable")), u64_name("Object.Door.State.Open"));
  L_943:; 
  L_944:; 
  L_945:; if (!u64_true((*u64_pframe(self, "CallFunc_EqualEqual_GameplayTag_ReturnValue")))) goto *U64_POP();
  L_955:; 
  L_956:; 
  L_957:; U64_CALL(self, "CloseDoor");
  L_971:; 
  L_972:; goto *U64_POP();
  L_973:; 
  L_974:; 
  L_975:; 
  L_976:; (*u64_pframe(self, "Temp_struct_Variable")) = (*u64_pframe(self, "K2Node_CustomEvent_Tag"));
  L_1003:; 
  L_1004:; (*u64_pframe(self, "CallFunc_EqualEqual_GameplayTag_ReturnValue")) = U64_CALL(self, "EqualEqual_GameplayTag", (*u64_pframe(self, "Temp_struct_Variable")), u64_name("Object.Door.State.Open"));
  L_1060:; 
  L_1061:; 
  L_1062:; if (!u64_true((*u64_pframe(self, "CallFunc_EqualEqual_GameplayTag_ReturnValue")))) goto *U64_POP();
  L_1072:; 
  L_1073:; 
  L_1074:; U64_CALL(self, "OpenDoor");
  L_1088:; 
  L_1089:; goto *U64_POP();
  L_1090:; (*u64_pframe(self, "CallFunc_IsValid_ReturnValue")) = U64_CALL(self, "IsValid", (*u64_ivar(self, "DoorCloseTimeline")));
  L_1119:; 
  L_1120:; 
  L_1121:; if (!u64_true((*u64_pframe(self, "CallFunc_IsValid_ReturnValue")))) goto L_1170;
  L_1135:; 
  L_1136:; 
  L_1137:; ({ u64_v _o = (*u64_ivar(self, "DoorCloseTimeline")); U64_CALL(u64_asobj(_o), "Stop"); });
  L_1169:; 
  L_1170:; U64_CALL(self, "BreakVector", ({ u64_v _o = (*u64_ivar(self, "Door")); (*u64_ivar(u64_asobj(_o), "RelativeLocation")); }), u64_ref(&(*u64_pframe(self, "CallFunc_BreakVector_X_1"))), u64_ref(&(*u64_pframe(self, "CallFunc_BreakVector_Y_1"))), u64_ref(&(*u64_pframe(self, "CallFunc_BreakVector_Z_1"))));
  L_1238:; 
  L_1239:; (*u64_ivar(self, "DoorStartingRelativeLocationY")) = (*u64_pframe(self, "CallFunc_BreakVector_Y_1"));
  L_1266:; 
  L_1267:; (*u64_pframe(self, "CallFunc_Divide_FloatFloat_ReturnValue_2")) = U64_CALL(self, "Divide_FloatFloat", (*u64_ivar(self, "DoorStartingRelativeLocationY")), (*u64_ivar(self, "MaxDoorMovementDistance")));
  L_1313:; (*u64_pframe(self, "CallFunc_Abs_ReturnValue")) = U64_CALL(self, "Abs", (*u64_pframe(self, "CallFunc_Divide_FloatFloat_ReturnValue_2")));
  L_1350:; (*u64_pframe(self, "CallFunc_Multiply_FloatFloat_ReturnValue")) = U64_CALL(self, "Multiply_FloatFloat", (*u64_pframe(self, "CallFunc_Abs_ReturnValue")), (*u64_ivar(self, "TimeToOpenCloseDoor")));
  L_1396:; 
  L_1397:; (*u64_ivar(self, "StartingTime")) = (*u64_pframe(self, "CallFunc_Multiply_FloatFloat_ReturnValue"));
  L_1424:; 
  L_1425:; 
  L_1426:; U64_PUSH(&&L_1437);
  L_1431:; 
  L_1432:; goto L_1444;
  L_1437:; 
  L_1438:; 
  L_1439:; goto L_493;
  L_1444:; 
  L_1445:; ({ u64_v _o = (*u64_ivar(self, "DoorOpenTimeline")); U64_CALL(u64_asobj(_o), "SetNewTime", (*u64_ivar(self, "StartingTime"))); });
  L_1486:; 
  L_1487:; goto *U64_POP();
  L_1488:; (*u64_pframe(self, "CallFunc_IsValid_ReturnValue_1")) = U64_CALL(self, "IsValid", (*u64_ivar(self, "DoorOpenTimeline")));
  L_1517:; 
  L_1518:; 
  L_1519:; if (!u64_true((*u64_pframe(self, "CallFunc_IsValid_ReturnValue_1")))) goto L_1568;
  L_1533:; 
  L_1534:; 
  L_1535:; ({ u64_v _o = (*u64_ivar(self, "DoorOpenTimeline")); U64_CALL(u64_asobj(_o), "Stop"); });
  L_1567:; 
  L_1568:; U64_CALL(self, "BreakVector", ({ u64_v _o = (*u64_ivar(self, "Door")); (*u64_ivar(u64_asobj(_o), "RelativeLocation")); }), u64_ref(&(*u64_pframe(self, "CallFunc_BreakVector_X"))), u64_ref(&(*u64_pframe(self, "CallFunc_BreakVector_Y"))), u64_ref(&(*u64_pframe(self, "CallFunc_BreakVector_Z"))));
  L_1636:; 
  L_1637:; (*u64_ivar(self, "DoorStartingRelativeLocationY")) = (*u64_pframe(self, "CallFunc_BreakVector_Y"));
  L_1664:; 
  L_1665:; (*u64_pframe(self, "CallFunc_Divide_FloatFloat_ReturnValue_3")) = U64_CALL(self, "Divide_FloatFloat", (*u64_ivar(self, "DoorStartingRelativeLocationY")), (*u64_ivar(self, "MaxDoorMovementDistance")));
  L_1711:; (*u64_pframe(self, "CallFunc_Abs_ReturnValue_1")) = U64_CALL(self, "Abs", (*u64_pframe(self, "CallFunc_Divide_FloatFloat_ReturnValue_3")));
  L_1748:; (*u64_pframe(self, "CallFunc_Subtract_FloatFloat_ReturnValue_4")) = U64_CALL(self, "Subtract_FloatFloat", u64_float(1.0f), (*u64_pframe(self, "CallFunc_Abs_ReturnValue_1")));
  L_1790:; (*u64_pframe(self, "CallFunc_Multiply_FloatFloat_ReturnValue_1")) = U64_CALL(self, "Multiply_FloatFloat", (*u64_pframe(self, "CallFunc_Subtract_FloatFloat_ReturnValue_4")), (*u64_ivar(self, "TimeToOpenCloseDoor")));
  L_1836:; 
  L_1837:; (*u64_ivar(self, "StartingTime")) = (*u64_pframe(self, "CallFunc_Multiply_FloatFloat_ReturnValue_1"));
  L_1864:; 
  L_1865:; 
  L_1866:; U64_PUSH(&&L_1877);
  L_1871:; 
  L_1872:; goto L_1884;
  L_1877:; 
  L_1878:; 
  L_1879:; goto L_821;
  L_1884:; 
  L_1885:; ({ u64_v _o = (*u64_ivar(self, "DoorCloseTimeline")); U64_CALL(u64_asobj(_o), "SetNewTime", (*u64_ivar(self, "StartingTime"))); });
  L_1926:; 
  L_1927:; goto *U64_POP();
  L_1928:; 
  L_1929:; 
  L_1930:; goto *U64_POP();
  L_1931:; 
  L_1932:; 
  L_1933:; goto *U64_POP();
  L_1934:; 
  L_1935:; 
  L_1936:; goto *U64_POP();
  L_1937:; 
  L_1938:; 
  L_1939:; (*u64_pframe(self, "CallFunc_GetAbilitySystemComponent_ReturnValue")) = U64_CALL(self, "GetAbilitySystemComponent", u64_objv(self));
  L_1960:; 
  L_1961:; (*u64_pframe(self, "CallFunc_ListenForGameplayTagAddedOrRemoved_ReturnValue")) = U64_CALL(self, "ListenForGameplayTagAddedOrRemoved", (*u64_pframe(self, "CallFunc_GetAbilitySystemComponent_ReturnValue")), (*u64_ivar(self, "ListenTags")));
  L_1999:; 
  L_2000:; (*u64_pframe(self, "CallFunc_IsValid_ReturnValue_2")) = U64_CALL(self, "IsValid", (*u64_pframe(self, "CallFunc_ListenForGameplayTagAddedOrRemoved_ReturnValue")));
  L_2029:; 
  L_2030:; 
  L_2031:; if (!u64_true((*u64_pframe(self, "CallFunc_IsValid_ReturnValue_2")))) goto *U64_POP();
  L_2041:; 
  L_2042:; (*u64_pframe(self, "K2Node_CreateDelegate_OutputDelegate_1")) = u64_delegate(u64_objv(self), "OnTagAdded_A2D5F3774A3A190F682FCA89D03B33FC");
  L_2065:; 
  L_2066:; u64_mc_add(&(*u64_ivar(u64_asobj((*u64_pframe(self, "CallFunc_ListenForGameplayTagAddedOrRemoved_ReturnValue"))), "OnTagAdded")), (*u64_pframe(self, "K2Node_CreateDelegate_OutputDelegate_1")));
  L_2107:; 
  L_2108:; (*u64_pframe(self, "K2Node_CreateDelegate_OutputDelegate")) = u64_delegate(u64_objv(self), "OnTagRemoved_A2D5F3774A3A190F682FCA89D03B33FC");
  L_2131:; 
  L_2132:; u64_mc_add(&(*u64_ivar(u64_asobj((*u64_pframe(self, "CallFunc_ListenForGameplayTagAddedOrRemoved_ReturnValue"))), "OnTagRemoved")), (*u64_pframe(self, "K2Node_CreateDelegate_OutputDelegate")));
  L_2173:; 
  L_2174:; 
  L_2175:; ({ u64_v _o = (*u64_pframe(self, "CallFunc_ListenForGameplayTagAddedOrRemoved_ReturnValue")); U64_CALL(u64_asobj(_o), "Activate"); });
  L_2211:; 
  L_2212:; goto *U64_POP();
  L_2213:; 
  L_2214:; 
  L_2215:; goto L_1090;
  L_2220:; 
  L_2221:; 
  L_2222:; goto L_1488;
  L_2227:; 
  L_2228:; 
  L_2229:; 
  L_2230:; 
  L_2231:; (*u64_pframe(self, "CallFunc_Subtract_FloatFloat_ReturnValue_1")) = U64_CALL(self, "Subtract_FloatFloat", (*u64_ivar(self, "DoorOpenTimeline_Time_F2789BDF43EA5C1E5F7DA1992BD359E3")), (*u64_ivar(self, "StartingTime")));
  L_2277:; (*u64_pframe(self, "CallFunc_Subtract_FloatFloat_ReturnValue_2")) = U64_CALL(self, "Subtract_FloatFloat", (*u64_ivar(self, "TimeToOpenCloseDoor")), (*u64_ivar(self, "StartingTime")));
  L_2323:; (*u64_pframe(self, "CallFunc_Divide_FloatFloat_ReturnValue")) = U64_CALL(self, "Divide_FloatFloat", (*u64_pframe(self, "CallFunc_Subtract_FloatFloat_ReturnValue_1")), (*u64_pframe(self, "CallFunc_Subtract_FloatFloat_ReturnValue_2")));
  L_2369:; (*u64_pframe(self, "CallFunc_Lerp_ReturnValue")) = U64_CALL(self, "Lerp", (*u64_ivar(self, "DoorStartingRelativeLocationY")), u64_float(-140.0f), (*u64_pframe(self, "CallFunc_Divide_FloatFloat_ReturnValue")));
  L_2420:; (*u64_pframe(self, "CallFunc_MakeVector_ReturnValue")) = U64_CALL(self, "MakeVector", u64_float(0.0f), (*u64_pframe(self, "CallFunc_Lerp_ReturnValue")), u64_float(0.0f));
  L_2467:; 
  L_2468:; ({ u64_v _o = (*u64_ivar(self, "Door")); U64_CALL(u64_asobj(_o), "K2_SetRelativeLocation", (*u64_pframe(self, "CallFunc_MakeVector_ReturnValue")), u64_bool(0), u64_ref(&(*u64_pframe(self, "CallFunc_K2_SetRelativeLocation_SweepHitResult"))), u64_bool(0)); });
  L_2520:; 
  L_2521:; goto *U64_POP();
  L_2522:; 
  L_2523:; 
  L_2524:; goto *U64_POP();
  L_2525:; 
  L_2526:; 
  L_2527:; goto *U64_POP();
  L_2528:; 
  L_2529:; 
  L_2530:; goto *U64_POP();
  L_2531:; goto u64_ret;
  L_2533:; 
  u64_ret:; u64_set_return(L.ReturnValue);

}

void bp_BP_Door__PostInteract(u64_obj *self, const u64_v *args, int argc) {
  (void)args; (void)argc;
  struct { u64_v InteractingActor; u64_v InteractionComponent; u64_v ReturnValue; u64_v _unsupported; } L; memset(&L, 0, sizeof L); (void)L;
  U64_FLOW_DECL; (void)flow_n;
  if (argc > 0) L.InteractingActor = args[0];
  if (argc > 1) L.InteractionComponent = args[1];
  L_0:; 
  L_1:; 
  L_2:; 
  L_3:; *u64_pframe(self, "K2Node_Event_InteractingActor") = L.InteractingActor;
  L_21:; *u64_pframe(self, "K2Node_Event_InteractionComponent") = L.InteractionComponent;
  L_39:; 
  L_40:; 
  L_41:; U64_CALL(self, "ExecuteUbergraph_BP_Door", u64_int(15));
  L_56:; 
  L_57:; goto u64_ret;
  L_59:; 
  u64_ret:; u64_set_return(L.ReturnValue);

}

void bp_BP_Door__CancelInteraction(u64_obj *self, const u64_v *args, int argc) {
  (void)args; (void)argc;
  struct { u64_v InteractionComponent; u64_v ReturnValue; u64_v _unsupported; } L; memset(&L, 0, sizeof L); (void)L;
  U64_FLOW_DECL; (void)flow_n;
  if (argc > 0) L.InteractionComponent = args[0];
  L_0:; 
  L_1:; 
  L_2:; 
  L_3:; *u64_pframe(self, "K2Node_Event_InteractionComponent_1") = L.InteractionComponent;
  L_21:; 
  L_22:; 
  L_23:; U64_CALL(self, "ExecuteUbergraph_BP_Door", u64_int(2525));
  L_38:; 
  L_39:; goto u64_ret;
  L_41:; 
  u64_ret:; u64_set_return(L.ReturnValue);

}

void bp_BP_Door__InteractableCancelInteraction(u64_obj *self, const u64_v *args, int argc) {
  (void)args; (void)argc;
  struct { u64_v InteractionComponent; u64_v ReturnValue; u64_v _unsupported; } L; memset(&L, 0, sizeof L); (void)L;
  U64_FLOW_DECL; (void)flow_n;
  if (argc > 0) L.InteractionComponent = args[0];
  L_0:; 
  L_1:; 
  L_2:; 
  L_3:; *u64_pframe(self, "K2Node_Event_InteractionComponent_2") = L.InteractionComponent;
  L_21:; 
  L_22:; 
  L_23:; U64_CALL(self, "ExecuteUbergraph_BP_Door", u64_int(2522));
  L_38:; 
  L_39:; goto u64_ret;
  L_41:; 
  u64_ret:; u64_set_return(L.ReturnValue);

}

void bp_BP_Door__CloseDoor(u64_obj *self, const u64_v *args, int argc) {
  (void)args; (void)argc;
  struct { u64_v ReturnValue; u64_v _unsupported; } L; memset(&L, 0, sizeof L); (void)L;
  U64_FLOW_DECL; (void)flow_n;
  L_0:; 
  L_1:; 
  L_2:; 
  L_3:; U64_CALL(self, "ExecuteUbergraph_BP_Door", u64_int(2220));
  L_18:; 
  L_19:; goto u64_ret;
  L_21:; 
  u64_ret:; u64_set_return(L.ReturnValue);

}

void bp_BP_Door__OpenDoor(u64_obj *self, const u64_v *args, int argc) {
  (void)args; (void)argc;
  struct { u64_v ReturnValue; u64_v _unsupported; } L; memset(&L, 0, sizeof L); (void)L;
  U64_FLOW_DECL; (void)flow_n;
  L_0:; 
  L_1:; 
  L_2:; 
  L_3:; U64_CALL(self, "ExecuteUbergraph_BP_Door", u64_int(2213));
  L_18:; 
  L_19:; goto u64_ret;
  L_21:; 
  u64_ret:; u64_set_return(L.ReturnValue);

}

void bp_BP_Door__ReceiveBeginPlay(u64_obj *self, const u64_v *args, int argc) {
  (void)args; (void)argc;
  struct { u64_v ReturnValue; u64_v _unsupported; } L; memset(&L, 0, sizeof L); (void)L;
  U64_FLOW_DECL; (void)flow_n;
  L_0:; 
  L_1:; 
  L_2:; 
  L_3:; U64_CALL(self, "ExecuteUbergraph_BP_Door", u64_int(1937));
  L_18:; 
  L_19:; goto u64_ret;
  L_21:; 
  u64_ret:; u64_set_return(L.ReturnValue);

}

void bp_BP_Door__UnregisterInteracter(u64_obj *self, const u64_v *args, int argc) {
  (void)args; (void)argc;
  struct { u64_v InteractingActor; u64_v InteractionComponent; u64_v ReturnValue; u64_v _unsupported; } L; memset(&L, 0, sizeof L); (void)L;
  U64_FLOW_DECL; (void)flow_n;
  if (argc > 0) L.InteractionComponent = args[0];
  if (argc > 1) L.InteractingActor = args[1];
  L_0:; 
  L_1:; 
  L_2:; 
  L_3:; *u64_pframe(self, "K2Node_Event_InteractionComponent_3") = L.InteractionComponent;
  L_21:; *u64_pframe(self, "K2Node_Event_InteractingActor_1") = L.InteractingActor;
  L_39:; 
  L_40:; 
  L_41:; U64_CALL(self, "ExecuteUbergraph_BP_Door", u64_int(1934));
  L_56:; 
  L_57:; goto u64_ret;
  L_59:; 
  u64_ret:; u64_set_return(L.ReturnValue);

}

void bp_BP_Door__RegisterInteracter(u64_obj *self, const u64_v *args, int argc) {
  (void)args; (void)argc;
  struct { u64_v InteractingActor; u64_v InteractionComponent; u64_v ReturnValue; u64_v _unsupported; } L; memset(&L, 0, sizeof L); (void)L;
  U64_FLOW_DECL; (void)flow_n;
  if (argc > 0) L.InteractionComponent = args[0];
  if (argc > 1) L.InteractingActor = args[1];
  L_0:; 
  L_1:; 
  L_2:; 
  L_3:; *u64_pframe(self, "K2Node_Event_InteractionComponent_4") = L.InteractionComponent;
  L_21:; *u64_pframe(self, "K2Node_Event_InteractingActor_2") = L.InteractingActor;
  L_39:; 
  L_40:; 
  L_41:; U64_CALL(self, "ExecuteUbergraph_BP_Door", u64_int(1931));
  L_56:; 
  L_57:; goto u64_ret;
  L_59:; 
  u64_ret:; u64_set_return(L.ReturnValue);

}

void bp_BP_Door__PreInteract(u64_obj *self, const u64_v *args, int argc) {
  (void)args; (void)argc;
  struct { u64_v InteractingActor; u64_v InteractionComponent; u64_v ReturnValue; u64_v _unsupported; } L; memset(&L, 0, sizeof L); (void)L;
  U64_FLOW_DECL; (void)flow_n;
  if (argc > 0) L.InteractingActor = args[0];
  if (argc > 1) L.InteractionComponent = args[1];
  L_0:; 
  L_1:; 
  L_2:; 
  L_3:; *u64_pframe(self, "K2Node_Event_InteractingActor_3") = L.InteractingActor;
  L_21:; *u64_pframe(self, "K2Node_Event_InteractionComponent_5") = L.InteractionComponent;
  L_39:; 
  L_40:; 
  L_41:; U64_CALL(self, "ExecuteUbergraph_BP_Door", u64_int(1928));
  L_56:; 
  L_57:; goto u64_ret;
  L_59:; 
  u64_ret:; u64_set_return(L.ReturnValue);

}

void bp_BP_Door__OnTagAdded_A2D5F3774A3A190F682FCA89D03B33FC(u64_obj *self, const u64_v *args, int argc) {
  (void)args; (void)argc;
  struct { u64_v ReturnValue; u64_v Tag; u64_v _unsupported; } L; memset(&L, 0, sizeof L); (void)L;
  U64_FLOW_DECL; (void)flow_n;
  if (argc > 0) L.Tag = args[0];
  L_0:; 
  L_1:; 
  L_2:; 
  L_3:; *u64_pframe(self, "K2Node_CustomEvent_Tag") = L.Tag;
  L_21:; 
  L_22:; 
  L_23:; U64_CALL(self, "ExecuteUbergraph_BP_Door", u64_int(973));
  L_38:; 
  L_39:; goto u64_ret;
  L_41:; 
  u64_ret:; u64_set_return(L.ReturnValue);

}

void bp_BP_Door__OnTagRemoved_A2D5F3774A3A190F682FCA89D03B33FC(u64_obj *self, const u64_v *args, int argc) {
  (void)args; (void)argc;
  struct { u64_v ReturnValue; u64_v Tag; u64_v _unsupported; } L; memset(&L, 0, sizeof L); (void)L;
  U64_FLOW_DECL; (void)flow_n;
  if (argc > 0) L.Tag = args[0];
  L_0:; 
  L_1:; 
  L_2:; 
  L_3:; *u64_pframe(self, "K2Node_CustomEvent_Tag_1") = L.Tag;
  L_21:; 
  L_22:; 
  L_23:; U64_CALL(self, "ExecuteUbergraph_BP_Door", u64_int(856));
  L_38:; 
  L_39:; goto u64_ret;
  L_41:; 
  u64_ret:; u64_set_return(L.ReturnValue);

}

void bp_BP_Door__DoorCloseTimeline__UpdateFunc(u64_obj *self, const u64_v *args, int argc) {
  (void)args; (void)argc;
  struct { u64_v ReturnValue; u64_v _unsupported; } L; memset(&L, 0, sizeof L); (void)L;
  U64_FLOW_DECL; (void)flow_n;
  L_0:; 
  L_1:; 
  L_2:; 
  L_3:; U64_CALL(self, "ExecuteUbergraph_BP_Door", u64_int(528));
  L_18:; 
  L_19:; goto u64_ret;
  L_21:; 
  u64_ret:; u64_set_return(L.ReturnValue);

}

void bp_BP_Door__DoorCloseTimeline__FinishedFunc(u64_obj *self, const u64_v *args, int argc) {
  (void)args; (void)argc;
  struct { u64_v ReturnValue; u64_v _unsupported; } L; memset(&L, 0, sizeof L); (void)L;
  U64_FLOW_DECL; (void)flow_n;
  L_0:; 
  L_1:; 
  L_2:; 
  L_3:; U64_CALL(self, "ExecuteUbergraph_BP_Door", u64_int(490));
  L_18:; 
  L_19:; goto u64_ret;
  L_21:; 
  u64_ret:; u64_set_return(L.ReturnValue);

}

void bp_BP_Door__DoorOpenTimeline__UpdateFunc(u64_obj *self, const u64_v *args, int argc) {
  (void)args; (void)argc;
  struct { u64_v ReturnValue; u64_v _unsupported; } L; memset(&L, 0, sizeof L); (void)L;
  U64_FLOW_DECL; (void)flow_n;
  L_0:; 
  L_1:; 
  L_2:; 
  L_3:; U64_CALL(self, "ExecuteUbergraph_BP_Door", u64_int(2229));
  L_18:; 
  L_19:; goto u64_ret;
  L_21:; 
  u64_ret:; u64_set_return(L.ReturnValue);

}

void bp_BP_Door__DoorOpenTimeline__FinishedFunc(u64_obj *self, const u64_v *args, int argc) {
  (void)args; (void)argc;
  struct { u64_v ReturnValue; u64_v _unsupported; } L; memset(&L, 0, sizeof L); (void)L;
  U64_FLOW_DECL; (void)flow_n;
  L_0:; 
  L_1:; 
  L_2:; 
  L_3:; U64_CALL(self, "ExecuteUbergraph_BP_Door", u64_int(2528));
  L_18:; 
  L_19:; goto u64_ret;
  L_21:; 
  u64_ret:; u64_set_return(L.ReturnValue);

}

void bp_BP_Door__IsAvailableForInteraction(u64_obj *self, const u64_v *args, int argc) {
  (void)args; (void)argc;
  struct { u64_v CallFunc_EqualEqual_ObjectObject_ReturnValue; u64_v InteractionComponent; u64_v ReturnValue; u64_v _unsupported; } L; memset(&L, 0, sizeof L); (void)L;
  U64_FLOW_DECL; (void)flow_n;
  if (argc > 0) L.InteractionComponent = args[0];
  L_0:; 
  L_1:; 
  L_2:; L.CallFunc_EqualEqual_ObjectObject_ReturnValue = U64_CALL(self, "EqualEqual_ObjectObject", L.InteractionComponent, (*u64_ivar(self, "Button")));
  L_40:; 
  L_41:; L.ReturnValue = L.CallFunc_EqualEqual_ObjectObject_ReturnValue;
  L_60:; 
  L_61:; L.ReturnValue = L.ReturnValue; goto u64_ret;
  L_71:; 
  u64_ret:; u64_set_return(L.ReturnValue);

}

void bp_BP_Door__GetPreInteractSyncType(u64_obj *self, const u64_v *args, int argc) {
  (void)args; (void)argc;
  struct { u64_v InteractionComponent; u64_v ReturnValue; u64_v Type; u64_v _unsupported; u64_v bShouldSync; } L; memset(&L, 0, sizeof L); (void)L;
  U64_FLOW_DECL; (void)flow_n;
  if (argc > 0) L.bShouldSync = args[0].t == U64_REF ? *args[0].u.r : args[0];
  if (argc > 1) L.Type = args[1].t == U64_REF ? *args[1].u.r : args[1];
  if (argc > 2) L.InteractionComponent = args[2];
  L_0:; 
  L_1:; 
  L_2:; 
  L_3:; L.bShouldSync = u64_bool(0);
  L_14:; L.Type = u64_int(0);
  L_34:; 
  L_35:; goto u64_ret;
  L_37:; 
  u64_ret:; u64_set_return(L.ReturnValue);
  if (argc > 0) u64_out(args[0], L.bShouldSync);
  if (argc > 1) u64_out(args[1], L.Type);
}

void bp_BP_Door__GetPostInteractSyncType(u64_obj *self, const u64_v *args, int argc) {
  (void)args; (void)argc;
  struct { u64_v InteractionComponent; u64_v ReturnValue; u64_v Type; u64_v _unsupported; u64_v bShouldSync; } L; memset(&L, 0, sizeof L); (void)L;
  U64_FLOW_DECL; (void)flow_n;
  if (argc > 0) L.bShouldSync = args[0].t == U64_REF ? *args[0].u.r : args[0];
  if (argc > 1) L.Type = args[1].t == U64_REF ? *args[1].u.r : args[1];
  if (argc > 2) L.InteractionComponent = args[2];
  L_0:; 
  L_1:; 
  L_2:; 
  L_3:; L.bShouldSync = u64_bool(0);
  L_14:; L.Type = u64_int(0);
  L_34:; 
  L_35:; goto u64_ret;
  L_37:; 
  u64_ret:; u64_set_return(L.ReturnValue);
  if (argc > 0) u64_out(args[0], L.bShouldSync);
  if (argc > 1) u64_out(args[1], L.Type);
}

void bp_BP_Door__GetInteractionDuration(u64_obj *self, const u64_v *args, int argc) {
  (void)args; (void)argc;
  struct { u64_v CallFunc_EqualEqual_ObjectObject_ReturnValue; u64_v InteractionComponent; u64_v ReturnValue; u64_v _unsupported; } L; memset(&L, 0, sizeof L); (void)L;
  U64_FLOW_DECL; (void)flow_n;
  if (argc > 0) L.InteractionComponent = args[0];
  L_0:; 
  L_1:; 
  L_2:; L.CallFunc_EqualEqual_ObjectObject_ReturnValue = U64_CALL(self, "EqualEqual_ObjectObject", L.InteractionComponent, (*u64_ivar(self, "Button")));
  L_40:; 
  L_41:; 
  L_42:; if (!u64_true(L.CallFunc_EqualEqual_ObjectObject_ReturnValue)) goto L_87;
  L_56:; 
  L_57:; 
  L_58:; L.ReturnValue = u64_float(0.0f);
  L_81:; 
  L_82:; goto L_112;
  L_87:; 
  L_88:; L.ReturnValue = u64_float(0.0f);
  L_111:; 
  L_112:; L.ReturnValue = L.ReturnValue; goto u64_ret;
  L_122:; 
  u64_ret:; u64_set_return(L.ReturnValue);

}
