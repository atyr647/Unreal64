#include "u64_rt.h"

void bp_BP_Chest__ExecuteUbergraph_BP_Chest(u64_obj *self, const u64_v *args, int argc) {
  (void)args; (void)argc;
  struct { u64_v ReturnValue; u64_v _unsupported; } L; memset(&L, 0, sizeof L); (void)L;
  U64_FLOW_DECL; (void)flow_n;
  if (argc > 0) (*u64_pframe(self, "EntryPoint")) = args[0];
  L_0:; U64_PUSH(&&L_3351);
  L_5:; u64_dispatch: switch (u64_as_int((*u64_pframe(self, "EntryPoint")))) { case 0: goto L_0; case 5: goto L_5; case 15: goto L_15; case 16: goto L_16; case 17: goto L_17; case 37: goto L_37; case 38: goto L_38; case 39: goto L_39; case 53: goto L_53; case 54: goto L_54; case 55: goto L_55; case 56: goto L_56; case 77: goto L_77; case 131: goto L_131; case 132: goto L_132; case 205: goto L_205; case 206: goto L_206; case 207: goto L_207; case 234: goto L_234; case 235: goto L_235; case 236: goto L_236; case 237: goto L_237; case 238: goto L_238; case 239: goto L_239; case 240: goto L_240; case 241: goto L_241; case 242: goto L_242; case 278: goto L_278; case 279: goto L_279; case 280: goto L_280; case 316: goto L_316; case 317: goto L_317; case 318: goto L_318; case 319: goto L_319; case 320: goto L_320; case 321: goto L_321; case 357: goto L_357; case 358: goto L_358; case 359: goto L_359; case 395: goto L_395; case 396: goto L_396; case 397: goto L_397; case 431: goto L_431; case 432: goto L_432; case 433: goto L_433; case 485: goto L_485; case 486: goto L_486; case 487: goto L_487; case 488: goto L_488; case 489: goto L_489; case 490: goto L_490; case 517: goto L_517; case 518: goto L_518; case 556: goto L_556; case 557: goto L_557; case 558: goto L_558; case 568: goto L_568; case 569: goto L_569; case 570: goto L_570; case 584: goto L_584; case 585: goto L_585; case 586: goto L_586; case 587: goto L_587; case 588: goto L_588; case 589: goto L_589; case 616: goto L_616; case 617: goto L_617; case 655: goto L_655; case 656: goto L_656; case 657: goto L_657; case 667: goto L_667; case 668: goto L_668; case 669: goto L_669; case 683: goto L_683; case 684: goto L_684; case 685: goto L_685; case 686: goto L_686; case 718: goto L_718; case 719: goto L_719; case 720: goto L_720; case 749: goto L_749; case 750: goto L_750; case 751: goto L_751; case 761: goto L_761; case 762: goto L_762; case 767: goto L_767; case 768: goto L_768; case 769: goto L_769; case 770: goto L_770; case 771: goto L_771; case 772: goto L_772; case 773: goto L_773; case 774: goto L_774; case 775: goto L_775; case 776: goto L_776; case 777: goto L_777; case 778: goto L_778; case 816: goto L_816; case 817: goto L_817; case 818: goto L_818; case 828: goto L_828; case 829: goto L_829; case 830: goto L_830; case 831: goto L_831; case 836: goto L_836; case 837: goto L_837; case 838: goto L_838; case 876: goto L_876; case 877: goto L_877; case 878: goto L_878; case 888: goto L_888; case 889: goto L_889; case 890: goto L_890; case 891: goto L_891; case 892: goto L_892; case 893: goto L_893; case 913: goto L_913; case 914: goto L_914; case 915: goto L_915; case 929: goto L_929; case 930: goto L_930; case 931: goto L_931; case 932: goto L_932; case 961: goto L_961; case 1015: goto L_1015; case 1036: goto L_1036; case 1037: goto L_1037; case 1119: goto L_1119; case 1120: goto L_1120; case 1141: goto L_1141; case 1142: goto L_1142; case 1202: goto L_1202; case 1203: goto L_1203; case 1204: goto L_1204; case 1209: goto L_1209; case 1210: goto L_1210; case 1215: goto L_1215; case 1216: goto L_1216; case 1217: goto L_1217; case 1218: goto L_1218; case 1241: goto L_1241; case 1242: goto L_1242; case 1270: goto L_1270; case 1329: goto L_1329; case 1366: goto L_1366; case 1425: goto L_1425; case 1484: goto L_1484; case 1485: goto L_1485; case 1527: goto L_1527; case 1528: goto L_1528; case 1556: goto L_1556; case 1615: goto L_1615; case 1616: goto L_1616; case 1654: goto L_1654; case 1655: goto L_1655; case 1656: goto L_1656; case 1657: goto L_1657; case 1658: goto L_1658; case 1659: goto L_1659; case 1660: goto L_1660; case 1683: goto L_1683; case 1684: goto L_1684; case 1743: goto L_1743; case 1780: goto L_1780; case 1839: goto L_1839; case 1867: goto L_1867; case 1926: goto L_1926; case 1927: goto L_1927; case 1969: goto L_1969; case 1970: goto L_1970; case 1998: goto L_1998; case 2057: goto L_2057; case 2058: goto L_2058; case 2096: goto L_2096; case 2097: goto L_2097; case 2098: goto L_2098; case 2099: goto L_2099; case 2100: goto L_2100; case 2101: goto L_2101; case 2146: goto L_2146; case 2147: goto L_2147; case 2168: goto L_2168; case 2169: goto L_2169; case 2207: goto L_2207; case 2208: goto L_2208; case 2237: goto L_2237; case 2238: goto L_2238; case 2239: goto L_2239; case 2253: goto L_2253; case 2254: goto L_2254; case 2277: goto L_2277; case 2278: goto L_2278; case 2319: goto L_2319; case 2320: goto L_2320; case 2343: goto L_2343; case 2344: goto L_2344; case 2385: goto L_2385; case 2386: goto L_2386; case 2387: goto L_2387; case 2423: goto L_2423; case 2424: goto L_2424; case 2425: goto L_2425; case 2444: goto L_2444; case 2445: goto L_2445; case 2446: goto L_2446; case 2495: goto L_2495; case 2496: goto L_2496; case 2497: goto L_2497; case 2498: goto L_2498; case 2499: goto L_2499; case 2500: goto L_2500; case 2553: goto L_2553; case 2554: goto L_2554; case 2555: goto L_2555; case 2592: goto L_2592; case 2593: goto L_2593; case 2594: goto L_2594; case 2599: goto L_2599; case 2600: goto L_2600; case 2605: goto L_2605; case 2606: goto L_2606; case 2607: goto L_2607; case 2608: goto L_2608; case 2640: goto L_2640; case 2641: goto L_2641; case 2642: goto L_2642; case 2643: goto L_2643; case 2644: goto L_2644; case 2702: goto L_2702; case 2703: goto L_2703; case 2704: goto L_2704; case 2738: goto L_2738; case 2739: goto L_2739; case 2740: goto L_2740; case 2741: goto L_2741; case 2742: goto L_2742; case 2743: goto L_2743; case 2796: goto L_2796; case 2797: goto L_2797; case 2798: goto L_2798; case 2835: goto L_2835; case 2836: goto L_2836; case 2837: goto L_2837; case 2838: goto L_2838; case 2896: goto L_2896; case 2897: goto L_2897; case 2898: goto L_2898; case 2899: goto L_2899; case 2904: goto L_2904; case 2905: goto L_2905; case 2947: goto L_2947; case 2993: goto L_2993; case 3035: goto L_3035; case 3090: goto L_3090; case 3145: goto L_3145; case 3146: goto L_3146; case 3198: goto L_3198; case 3199: goto L_3199; case 3200: goto L_3200; case 3201: goto L_3201; case 3202: goto L_3202; case 3207: goto L_3207; case 3208: goto L_3208; case 3209: goto L_3209; case 3214: goto L_3214; case 3215: goto L_3215; case 3216: goto L_3216; case 3254: goto L_3254; case 3255: goto L_3255; case 3256: goto L_3256; case 3266: goto L_3266; case 3267: goto L_3267; case 3288: goto L_3288; case 3289: goto L_3289; case 3349: goto L_3349; case 3350: goto L_3350; case 3351: goto L_3351; case 3353: goto L_3353; }
  L_15:; 
  L_16:; 
  L_17:; (*u64_pframe(self, "CallFunc_HasAuthority_ReturnValue")) = U64_CALL(self, "HasAuthority");
  L_37:; 
  L_38:; 
  L_39:; if (!u64_true((*u64_pframe(self, "CallFunc_HasAuthority_ReturnValue")))) goto L_236;
  L_53:; 
  L_54:; 
  L_55:; 
  L_56:; (*u64_pframe(self, "CallFunc_GetAbilitySystemComponent_ReturnValue_3")) = U64_CALL(self, "GetAbilitySystemComponent", u64_objv(self));
  L_77:; (*u64_pframe(self, "CallFunc_MakeEffectContext_ReturnValue_1")) = ({ u64_v _o = (*u64_pframe(self, "CallFunc_GetAbilitySystemComponent_ReturnValue_3")); U64_CALL(u64_asobj(_o), "MakeEffectContext"); });
  L_131:; 
  L_132:; (*u64_pframe(self, "CallFunc_BP_ApplyGameplayEffectToSelf_ReturnValue")) = ({ u64_v _o = (*u64_pframe(self, "CallFunc_GetAbilitySystemComponent_ReturnValue_3")); U64_CALL(u64_asobj(_o), "BP_ApplyGameplayEffectToSelf", u64_objv(u64_find("GE_Interacting_C")), u64_float(0.0f), (*u64_pframe(self, "CallFunc_MakeEffectContext_ReturnValue_1"))); });
  L_205:; 
  L_206:; 
  L_207:; (*u64_ivar(self, "GE_InteractingHandle")) = (*u64_pframe(self, "CallFunc_BP_ApplyGameplayEffectToSelf_ReturnValue"));
  L_234:; 
  L_235:; goto *U64_POP();
  L_236:; 
  L_237:; 
  L_238:; goto *U64_POP();
  L_239:; 
  L_240:; 
  L_241:; 
  L_242:; ({ u64_v _o = (*u64_ivar(self, "RotatingMovement")); U64_CALL(u64_asobj(_o), "StopMovementImmediately"); });
  L_278:; 
  L_279:; 
  L_280:; ({ u64_v _o = (*u64_ivar(self, "RotatingMovement")); U64_CALL(u64_asobj(_o), "Deactivate"); });
  L_316:; 
  L_317:; goto *U64_POP();
  L_318:; 
  L_319:; 
  L_320:; 
  L_321:; ({ u64_v _o = (*u64_ivar(self, "RotatingMovement")); U64_CALL(u64_asobj(_o), "StopMovementImmediately"); });
  L_357:; 
  L_358:; 
  L_359:; ({ u64_v _o = (*u64_ivar(self, "RotatingMovement")); U64_CALL(u64_asobj(_o), "Deactivate"); });
  L_395:; 
  L_396:; 
  L_397:; ({ u64_v _o = (*u64_ivar(self, "SM_Forge_Chest_Lock")); U64_CALL(u64_asobj(_o), "SetVisibility", u64_bool(1), u64_bool(1)); });
  L_431:; 
  L_432:; 
  L_433:; ({ u64_v _o = (*u64_ivar(self, "SM_Forge_Chest_Lock")); U64_CALL(u64_asobj(_o), "K2_SetRelativeLocation", (*u64_ivar(self, "LockStartingRelativeLocation")), u64_bool(0), u64_ref(&(*u64_pframe(self, "CallFunc_K2_SetRelativeLocation_SweepHitResult"))), u64_bool(0)); });
  L_485:; 
  L_486:; goto *U64_POP();
  L_487:; 
  L_488:; 
  L_489:; 
  L_490:; (*u64_pframe(self, "Temp_struct_Variable")) = (*u64_pframe(self, "K2Node_CustomEvent_Tag_1"));
  L_517:; 
  L_518:; (*u64_pframe(self, "CallFunc_EqualEqual_GameplayTag_ReturnValue")) = U64_CALL(self, "EqualEqual_GameplayTag", (*u64_pframe(self, "Temp_struct_Variable")), (*u64_ivar(self, "OpenTag")));
  L_556:; 
  L_557:; 
  L_558:; if (!u64_true((*u64_pframe(self, "CallFunc_EqualEqual_GameplayTag_ReturnValue")))) goto *U64_POP();
  L_568:; 
  L_569:; 
  L_570:; U64_CALL(self, "CloseChest");
  L_584:; 
  L_585:; goto *U64_POP();
  L_586:; 
  L_587:; 
  L_588:; 
  L_589:; (*u64_pframe(self, "Temp_struct_Variable")) = (*u64_pframe(self, "K2Node_CustomEvent_Tag"));
  L_616:; 
  L_617:; (*u64_pframe(self, "CallFunc_EqualEqual_GameplayTag_ReturnValue")) = U64_CALL(self, "EqualEqual_GameplayTag", (*u64_pframe(self, "Temp_struct_Variable")), (*u64_ivar(self, "OpenTag")));
  L_655:; 
  L_656:; 
  L_657:; if (!u64_true((*u64_pframe(self, "CallFunc_EqualEqual_GameplayTag_ReturnValue")))) goto *U64_POP();
  L_667:; 
  L_668:; 
  L_669:; U64_CALL(self, "OpenChest");
  L_683:; 
  L_684:; goto *U64_POP();
  L_685:; 
  L_686:; ({ u64_v _o = (*u64_ivar(self, "ListenForTagAddRemovedTask")); U64_CALL(u64_asobj(_o), "EndTask"); });
  L_718:; 
  L_719:; goto *U64_POP();
  L_720:; (*u64_pframe(self, "CallFunc_IsValid_ReturnValue")) = U64_CALL(self, "IsValid", (*u64_ivar(self, "ListenForTagAddRemovedTask")));
  L_749:; 
  L_750:; 
  L_751:; if (!u64_true((*u64_pframe(self, "CallFunc_IsValid_ReturnValue")))) goto *U64_POP();
  L_761:; 
  L_762:; goto L_685;
  L_767:; 
  L_768:; 
  L_769:; goto *U64_POP();
  L_770:; 
  L_771:; 
  L_772:; goto *U64_POP();
  L_773:; 
  L_774:; 
  L_775:; goto *U64_POP();
  L_776:; 
  L_777:; 
  L_778:; (*u64_pframe(self, "CallFunc_EqualEqual_ObjectObject_ReturnValue")) = U64_CALL(self, "EqualEqual_ObjectObject", (*u64_pframe(self, "K2Node_Event_InteractionComponent_2")), (*u64_ivar(self, "SM_Forge_Chest_Lock")));
  L_816:; 
  L_817:; 
  L_818:; if (!u64_true((*u64_pframe(self, "CallFunc_EqualEqual_ObjectObject_ReturnValue")))) goto *U64_POP();
  L_828:; 
  L_829:; 
  L_830:; 
  L_831:; goto L_15;
  L_836:; 
  L_837:; 
  L_838:; (*u64_pframe(self, "CallFunc_EqualEqual_ObjectObject_ReturnValue_1")) = U64_CALL(self, "EqualEqual_ObjectObject", (*u64_pframe(self, "K2Node_Event_InteractionComponent_1")), (*u64_ivar(self, "SM_Forge_Chest_Lock")));
  L_876:; 
  L_877:; 
  L_878:; if (!u64_true((*u64_pframe(self, "CallFunc_EqualEqual_ObjectObject_ReturnValue_1")))) goto *U64_POP();
  L_888:; 
  L_889:; 
  L_890:; 
  L_891:; 
  L_892:; 
  L_893:; (*u64_pframe(self, "CallFunc_HasAuthority_ReturnValue_1")) = U64_CALL(self, "HasAuthority");
  L_913:; 
  L_914:; 
  L_915:; if (!u64_true((*u64_pframe(self, "CallFunc_HasAuthority_ReturnValue_1")))) goto L_1656;
  L_929:; 
  L_930:; 
  L_931:; 
  L_932:; (*u64_pframe(self, "CallFunc_GetAbilitySystemComponent_ReturnValue")) = U64_CALL(self, "GetAbilitySystemComponent", (*u64_pframe(self, "K2Node_Event_InteractingActor")));
  L_961:; (*u64_pframe(self, "CallFunc_MakeEffectContext_ReturnValue")) = ({ u64_v _o = (*u64_pframe(self, "CallFunc_GetAbilitySystemComponent_ReturnValue")); U64_CALL(u64_asobj(_o), "MakeEffectContext"); });
  L_1015:; (*u64_pframe(self, "CallFunc_GetAbilitySystemComponent_ReturnValue_1")) = U64_CALL(self, "GetAbilitySystemComponent", u64_objv(self));
  L_1036:; 
  L_1037:; (*u64_pframe(self, "CallFunc_BP_ApplyGameplayEffectToTarget_ReturnValue")) = ({ u64_v _o = (*u64_pframe(self, "CallFunc_GetAbilitySystemComponent_ReturnValue")); U64_CALL(u64_asobj(_o), "BP_ApplyGameplayEffectToTarget", u64_objv(u64_find("GE_ChestOpen_C")), (*u64_pframe(self, "CallFunc_GetAbilitySystemComponent_ReturnValue_1")), u64_float(0.0f), (*u64_pframe(self, "CallFunc_MakeEffectContext_ReturnValue"))); });
  L_1119:; 
  L_1120:; (*u64_pframe(self, "CallFunc_GetAbilitySystemComponent_ReturnValue_4")) = U64_CALL(self, "GetAbilitySystemComponent", u64_objv(self));
  L_1141:; 
  L_1142:; (*u64_pframe(self, "CallFunc_RemoveActiveGameplayEffect_ReturnValue")) = ({ u64_v _o = (*u64_pframe(self, "CallFunc_GetAbilitySystemComponent_ReturnValue_4")); U64_CALL(u64_asobj(_o), "RemoveActiveGameplayEffect", (*u64_ivar(self, "GE_InteractingHandle")), u64_int(-1)); });
  L_1202:; 
  L_1203:; 
  L_1204:; U64_PUSH(&&L_1215);
  L_1209:; 
  L_1210:; goto L_1659;
  L_1215:; 
  L_1216:; 
  L_1217:; 
  L_1218:; U64_CALL(self, "GetLocationToSpawnItem", u64_ref(&(*u64_pframe(self, "CallFunc_GetLocationToSpawnItem_Location_1"))));
  L_1241:; 
  L_1242:; (*u64_pframe(self, "CallFunc_K2_GetActorRotation_ReturnValue")) = U64_CALL(self, "K2_GetActorRotation");
  L_1270:; (*u64_pframe(self, "CallFunc_Array_Length_ReturnValue_1")) = ({ u64_v _o = u64_objv(u64_find("Default__KismetArrayLibrary")); U64_CALL(u64_asobj(_o), "Array_Length", u64_ref(&(*u64_ivar(self, "ItemClasses")))); });
  L_1329:; (*u64_pframe(self, "CallFunc_RandomInteger_ReturnValue_1")) = U64_CALL(self, "RandomInteger", (*u64_pframe(self, "CallFunc_Array_Length_ReturnValue_1")));
  L_1366:; ({ u64_v _o = u64_objv(u64_find("Default__KismetArrayLibrary")); U64_CALL(u64_asobj(_o), "Array_Get", u64_ref(&(*u64_ivar(self, "ItemClasses"))), (*u64_pframe(self, "CallFunc_RandomInteger_ReturnValue_1")), u64_ref(&(*u64_pframe(self, "CallFunc_Array_Get_Item_1")))); });
  L_1425:; (*u64_pframe(self, "CallFunc_MakeTransform_ReturnValue_1")) = U64_CALL(self, "MakeTransform", (*u64_pframe(self, "CallFunc_GetLocationToSpawnItem_Location_1")), (*u64_pframe(self, "CallFunc_K2_GetActorRotation_ReturnValue")), u64_vec(1.0f, 1.0f, 1.0f));
  L_1484:; 
  L_1485:; (*u64_pframe(self, "CallFunc_BeginDeferredActorSpawnFromClass_ReturnValue_1")) = U64_CALL(self, "BeginDeferredActorSpawnFromClass", u64_objv(self), (*u64_pframe(self, "CallFunc_Array_Get_Item_1")), (*u64_pframe(self, "CallFunc_MakeTransform_ReturnValue_1")), u64_int(0), u64_objv(0));
  L_1527:; 
  L_1528:; (*u64_pframe(self, "CallFunc_K2_GetActorRotation_ReturnValue")) = U64_CALL(self, "K2_GetActorRotation");
  L_1556:; (*u64_pframe(self, "CallFunc_MakeTransform_ReturnValue_1")) = U64_CALL(self, "MakeTransform", (*u64_pframe(self, "CallFunc_GetLocationToSpawnItem_Location_1")), (*u64_pframe(self, "CallFunc_K2_GetActorRotation_ReturnValue")), u64_vec(1.0f, 1.0f, 1.0f));
  L_1615:; 
  L_1616:; (*u64_pframe(self, "CallFunc_FinishSpawningActor_ReturnValue_1")) = U64_CALL(self, "FinishSpawningActor", (*u64_pframe(self, "CallFunc_BeginDeferredActorSpawnFromClass_ReturnValue_1")), (*u64_pframe(self, "CallFunc_MakeTransform_ReturnValue_1")));
  L_1654:; 
  L_1655:; goto *U64_POP();
  L_1656:; 
  L_1657:; 
  L_1658:; goto *U64_POP();
  L_1659:; 
  L_1660:; U64_CALL(self, "GetLocationToSpawnItem", u64_ref(&(*u64_pframe(self, "CallFunc_GetLocationToSpawnItem_Location"))));
  L_1683:; 
  L_1684:; (*u64_pframe(self, "CallFunc_Array_Length_ReturnValue")) = ({ u64_v _o = u64_objv(u64_find("Default__KismetArrayLibrary")); U64_CALL(u64_asobj(_o), "Array_Length", u64_ref(&(*u64_ivar(self, "WeaponClasses")))); });
  L_1743:; (*u64_pframe(self, "CallFunc_RandomInteger_ReturnValue")) = U64_CALL(self, "RandomInteger", (*u64_pframe(self, "CallFunc_Array_Length_ReturnValue")));
  L_1780:; ({ u64_v _o = u64_objv(u64_find("Default__KismetArrayLibrary")); U64_CALL(u64_asobj(_o), "Array_Get", u64_ref(&(*u64_ivar(self, "WeaponClasses"))), (*u64_pframe(self, "CallFunc_RandomInteger_ReturnValue")), u64_ref(&(*u64_pframe(self, "CallFunc_Array_Get_Item")))); });
  L_1839:; (*u64_pframe(self, "CallFunc_K2_GetActorRotation_ReturnValue")) = U64_CALL(self, "K2_GetActorRotation");
  L_1867:; (*u64_pframe(self, "CallFunc_MakeTransform_ReturnValue")) = U64_CALL(self, "MakeTransform", (*u64_pframe(self, "CallFunc_GetLocationToSpawnItem_Location")), (*u64_pframe(self, "CallFunc_K2_GetActorRotation_ReturnValue")), u64_vec(1.0f, 1.0f, 1.0f));
  L_1926:; 
  L_1927:; (*u64_pframe(self, "CallFunc_BeginDeferredActorSpawnFromClass_ReturnValue")) = U64_CALL(self, "BeginDeferredActorSpawnFromClass", u64_objv(self), (*u64_pframe(self, "CallFunc_Array_Get_Item")), (*u64_pframe(self, "CallFunc_MakeTransform_ReturnValue")), u64_int(0), u64_objv(0));
  L_1969:; 
  L_1970:; (*u64_pframe(self, "CallFunc_K2_GetActorRotation_ReturnValue")) = U64_CALL(self, "K2_GetActorRotation");
  L_1998:; (*u64_pframe(self, "CallFunc_MakeTransform_ReturnValue")) = U64_CALL(self, "MakeTransform", (*u64_pframe(self, "CallFunc_GetLocationToSpawnItem_Location")), (*u64_pframe(self, "CallFunc_K2_GetActorRotation_ReturnValue")), u64_vec(1.0f, 1.0f, 1.0f));
  L_2057:; 
  L_2058:; (*u64_pframe(self, "CallFunc_FinishSpawningActor_ReturnValue")) = U64_CALL(self, "FinishSpawningActor", (*u64_pframe(self, "CallFunc_BeginDeferredActorSpawnFromClass_ReturnValue")), (*u64_pframe(self, "CallFunc_MakeTransform_ReturnValue")));
  L_2096:; 
  L_2097:; goto *U64_POP();
  L_2098:; 
  L_2099:; 
  L_2100:; 
  L_2101:; ({ u64_v _o = (*u64_ivar(self, "RotatingMovement")); U64_CALL(u64_asobj(_o), "SetUpdatedComponent", (*u64_ivar(self, "SM_Forge_Chest_Top"))); });
  L_2146:; 
  L_2147:; (*u64_pframe(self, "CallFunc_GetAbilitySystemComponent_ReturnValue_2")) = U64_CALL(self, "GetAbilitySystemComponent", u64_objv(self));
  L_2168:; 
  L_2169:; (*u64_pframe(self, "CallFunc_ListenForGameplayTagAddedOrRemoved_ReturnValue")) = U64_CALL(self, "ListenForGameplayTagAddedOrRemoved", (*u64_pframe(self, "CallFunc_GetAbilitySystemComponent_ReturnValue_2")), (*u64_ivar(self, "ListenTags")));
  L_2207:; 
  L_2208:; (*u64_pframe(self, "CallFunc_IsValid_ReturnValue_1")) = U64_CALL(self, "IsValid", (*u64_pframe(self, "CallFunc_ListenForGameplayTagAddedOrRemoved_ReturnValue")));
  L_2237:; 
  L_2238:; 
  L_2239:; if (!u64_true((*u64_pframe(self, "CallFunc_IsValid_ReturnValue_1")))) goto L_2424;
  L_2253:; 
  L_2254:; (*u64_pframe(self, "K2Node_CreateDelegate_OutputDelegate")) = u64_delegate(u64_objv(self), "OnTagAdded_AE9DC3C84C991C86EDAE13BF093FE743");
  L_2277:; 
  L_2278:; u64_mc_add(&(*u64_ivar(u64_asobj((*u64_pframe(self, "CallFunc_ListenForGameplayTagAddedOrRemoved_ReturnValue"))), "OnTagAdded")), (*u64_pframe(self, "K2Node_CreateDelegate_OutputDelegate")));
  L_2319:; 
  L_2320:; (*u64_pframe(self, "K2Node_CreateDelegate_OutputDelegate_1")) = u64_delegate(u64_objv(self), "OnTagRemoved_AE9DC3C84C991C86EDAE13BF093FE743");
  L_2343:; 
  L_2344:; u64_mc_add(&(*u64_ivar(u64_asobj((*u64_pframe(self, "CallFunc_ListenForGameplayTagAddedOrRemoved_ReturnValue"))), "OnTagRemoved")), (*u64_pframe(self, "K2Node_CreateDelegate_OutputDelegate_1")));
  L_2385:; 
  L_2386:; 
  L_2387:; ({ u64_v _o = (*u64_pframe(self, "CallFunc_ListenForGameplayTagAddedOrRemoved_ReturnValue")); U64_CALL(u64_asobj(_o), "Activate"); });
  L_2423:; 
  L_2424:; 
  L_2425:; (*u64_ivar(self, "ListenForTagAddRemovedTask")) = (*u64_pframe(self, "CallFunc_ListenForGameplayTagAddedOrRemoved_ReturnValue"));
  L_2444:; 
  L_2445:; 
  L_2446:; (*u64_ivar(self, "LockStartingRelativeLocation")) = ({ u64_v _o = (*u64_ivar(self, "SM_Forge_Chest_Lock")); (*u64_ivar(u64_asobj(_o), "RelativeLocation")); });
  L_2495:; 
  L_2496:; goto *U64_POP();
  L_2497:; 
  L_2498:; 
  L_2499:; 
  L_2500:; (*u64_ivar(u64_asobj((*u64_ivar(self, "RotatingMovement"))), "RotationRate")) = u64_vec(180.0f, 0.0f, 0.0f);
  L_2553:; 
  L_2554:; 
  L_2555:; ({ u64_v _o = (*u64_ivar(self, "RotatingMovement")); U64_CALL(u64_asobj(_o), "Activate", u64_bool(1)); });
  L_2592:; 
  L_2593:; 
  L_2594:; U64_PUSH(&&L_2605);
  L_2599:; 
  L_2600:; goto L_2642;
  L_2605:; 
  L_2606:; 
  L_2607:; 
  L_2608:; ({ u64_v _o = (*u64_ivar(self, "LockFallOff")); U64_CALL(u64_asobj(_o), "PlayFromStart"); });
  L_2640:; 
  L_2641:; goto *U64_POP();
  L_2642:; 
  L_2643:; 
  L_2644:; U64_CALL(self, "Delay", u64_objv(self), (*u64_ivar(self, "ChestTopAnimationDuration")), u64_latent(u64_objv(self), u64_name("ExecuteUbergraph_BP_Chest").u.s, u64_int(239).u.i));
  L_2702:; goto *U64_POP();
  L_2703:; 
  L_2704:; ({ u64_v _o = (*u64_ivar(self, "SM_Forge_Chest_Lock")); U64_CALL(u64_asobj(_o), "SetVisibility", u64_bool(0), u64_bool(1)); });
  L_2738:; 
  L_2739:; goto *U64_POP();
  L_2740:; 
  L_2741:; 
  L_2742:; 
  L_2743:; (*u64_ivar(u64_asobj((*u64_ivar(self, "RotatingMovement"))), "RotationRate")) = u64_vec(-180.0f, 0.0f, 0.0f);
  L_2796:; 
  L_2797:; 
  L_2798:; ({ u64_v _o = (*u64_ivar(self, "RotatingMovement")); U64_CALL(u64_asobj(_o), "Activate", u64_bool(1)); });
  L_2835:; 
  L_2836:; 
  L_2837:; 
  L_2838:; U64_CALL(self, "Delay", u64_objv(self), (*u64_ivar(self, "ChestTopAnimationDuration")), u64_latent(u64_objv(self), u64_name("ExecuteUbergraph_BP_Chest").u.s, u64_int(318).u.i));
  L_2896:; goto *U64_POP();
  L_2897:; 
  L_2898:; 
  L_2899:; goto L_720;
  L_2904:; 
  L_2905:; (*u64_pframe(self, "CallFunc_Divide_FloatFloat_ReturnValue")) = U64_CALL(self, "Divide_FloatFloat", (*u64_ivar(self, "LockFallOff_Time_B1ECEBE24B16CD25615E1AA3FE47F140")), u64_float(0.20000000298023224f));
  L_2947:; U64_CALL(self, "BreakVector", (*u64_ivar(self, "LockStartingRelativeLocation")), u64_ref(&(*u64_pframe(self, "CallFunc_BreakVector_X"))), u64_ref(&(*u64_pframe(self, "CallFunc_BreakVector_Y"))), u64_ref(&(*u64_pframe(self, "CallFunc_BreakVector_Z"))));
  L_2993:; (*u64_pframe(self, "CallFunc_Subtract_FloatFloat_ReturnValue")) = U64_CALL(self, "Subtract_FloatFloat", (*u64_pframe(self, "CallFunc_BreakVector_Z")), u64_float(60.0f));
  L_3035:; (*u64_pframe(self, "CallFunc_Lerp_ReturnValue")) = U64_CALL(self, "Lerp", (*u64_pframe(self, "CallFunc_BreakVector_Z")), (*u64_pframe(self, "CallFunc_Subtract_FloatFloat_ReturnValue")), (*u64_pframe(self, "CallFunc_Divide_FloatFloat_ReturnValue")));
  L_3090:; (*u64_pframe(self, "CallFunc_MakeVector_ReturnValue")) = U64_CALL(self, "MakeVector", (*u64_pframe(self, "CallFunc_BreakVector_X")), (*u64_pframe(self, "CallFunc_BreakVector_Y")), (*u64_pframe(self, "CallFunc_Lerp_ReturnValue")));
  L_3145:; 
  L_3146:; ({ u64_v _o = (*u64_ivar(self, "SM_Forge_Chest_Lock")); U64_CALL(u64_asobj(_o), "K2_SetRelativeLocation", (*u64_pframe(self, "CallFunc_MakeVector_ReturnValue")), u64_bool(0), u64_ref(&(*u64_pframe(self, "CallFunc_K2_SetRelativeLocation_SweepHitResult_1"))), u64_bool(0)); });
  L_3198:; 
  L_3199:; goto *U64_POP();
  L_3200:; 
  L_3201:; 
  L_3202:; goto L_2905;
  L_3207:; 
  L_3208:; 
  L_3209:; goto L_2703;
  L_3214:; 
  L_3215:; 
  L_3216:; (*u64_pframe(self, "CallFunc_EqualEqual_ObjectObject_ReturnValue_2")) = U64_CALL(self, "EqualEqual_ObjectObject", (*u64_pframe(self, "K2Node_Event_InteractionComponent")), (*u64_ivar(self, "SM_Forge_Chest_Lock")));
  L_3254:; 
  L_3255:; 
  L_3256:; if (!u64_true((*u64_pframe(self, "CallFunc_EqualEqual_ObjectObject_ReturnValue_2")))) goto *U64_POP();
  L_3266:; 
  L_3267:; (*u64_pframe(self, "CallFunc_GetAbilitySystemComponent_ReturnValue_5")) = U64_CALL(self, "GetAbilitySystemComponent", u64_objv(self));
  L_3288:; 
  L_3289:; (*u64_pframe(self, "CallFunc_RemoveActiveGameplayEffect_ReturnValue_1")) = ({ u64_v _o = (*u64_pframe(self, "CallFunc_GetAbilitySystemComponent_ReturnValue_5")); U64_CALL(u64_asobj(_o), "RemoveActiveGameplayEffect", (*u64_ivar(self, "GE_InteractingHandle")), u64_int(-1)); });
  L_3349:; 
  L_3350:; goto *U64_POP();
  L_3351:; goto u64_ret;
  L_3353:; 
  u64_ret:; u64_set_return(L.ReturnValue);

}

void bp_BP_Chest__CancelInteraction(u64_obj *self, const u64_v *args, int argc) {
  (void)args; (void)argc;
  struct { u64_v InteractionComponent; u64_v ReturnValue; u64_v _unsupported; } L; memset(&L, 0, sizeof L); (void)L;
  U64_FLOW_DECL; (void)flow_n;
  if (argc > 0) L.InteractionComponent = args[0];
  L_0:; 
  L_1:; 
  L_2:; 
  L_3:; *u64_pframe(self, "K2Node_Event_InteractionComponent") = L.InteractionComponent;
  L_21:; 
  L_22:; 
  L_23:; U64_CALL(self, "ExecuteUbergraph_BP_Chest", u64_int(3214));
  L_38:; 
  L_39:; goto u64_ret;
  L_41:; 
  u64_ret:; u64_set_return(L.ReturnValue);

}

void bp_BP_Chest__ReceiveEndPlay(u64_obj *self, const u64_v *args, int argc) {
  (void)args; (void)argc;
  struct { u64_v EndPlayReason; u64_v ReturnValue; u64_v _unsupported; } L; memset(&L, 0, sizeof L); (void)L;
  U64_FLOW_DECL; (void)flow_n;
  if (argc > 0) L.EndPlayReason = args[0];
  L_0:; 
  L_1:; 
  L_2:; 
  L_3:; *u64_pframe(self, "K2Node_Event_EndPlayReason") = L.EndPlayReason;
  L_21:; 
  L_22:; 
  L_23:; U64_CALL(self, "ExecuteUbergraph_BP_Chest", u64_int(2897));
  L_38:; 
  L_39:; goto u64_ret;
  L_41:; 
  u64_ret:; u64_set_return(L.ReturnValue);

}

void bp_BP_Chest__CloseChest(u64_obj *self, const u64_v *args, int argc) {
  (void)args; (void)argc;
  struct { u64_v ReturnValue; u64_v _unsupported; } L; memset(&L, 0, sizeof L); (void)L;
  U64_FLOW_DECL; (void)flow_n;
  L_0:; 
  L_1:; 
  L_2:; 
  L_3:; U64_CALL(self, "ExecuteUbergraph_BP_Chest", u64_int(2740));
  L_18:; 
  L_19:; goto u64_ret;
  L_21:; 
  u64_ret:; u64_set_return(L.ReturnValue);

}

void bp_BP_Chest__OpenChest(u64_obj *self, const u64_v *args, int argc) {
  (void)args; (void)argc;
  struct { u64_v ReturnValue; u64_v _unsupported; } L; memset(&L, 0, sizeof L); (void)L;
  U64_FLOW_DECL; (void)flow_n;
  L_0:; 
  L_1:; 
  L_2:; 
  L_3:; U64_CALL(self, "ExecuteUbergraph_BP_Chest", u64_int(2497));
  L_18:; 
  L_19:; goto u64_ret;
  L_21:; 
  u64_ret:; u64_set_return(L.ReturnValue);

}

void bp_BP_Chest__ReceiveBeginPlay(u64_obj *self, const u64_v *args, int argc) {
  (void)args; (void)argc;
  struct { u64_v ReturnValue; u64_v _unsupported; } L; memset(&L, 0, sizeof L); (void)L;
  U64_FLOW_DECL; (void)flow_n;
  L_0:; 
  L_1:; 
  L_2:; 
  L_3:; U64_CALL(self, "ExecuteUbergraph_BP_Chest", u64_int(2098));
  L_18:; 
  L_19:; goto u64_ret;
  L_21:; 
  u64_ret:; u64_set_return(L.ReturnValue);

}

void bp_BP_Chest__PostInteract(u64_obj *self, const u64_v *args, int argc) {
  (void)args; (void)argc;
  struct { u64_v InteractingActor; u64_v InteractionComponent; u64_v ReturnValue; u64_v _unsupported; } L; memset(&L, 0, sizeof L); (void)L;
  U64_FLOW_DECL; (void)flow_n;
  if (argc > 0) L.InteractingActor = args[0];
  if (argc > 1) L.InteractionComponent = args[1];
  L_0:; 
  L_1:; 
  L_2:; 
  L_3:; *u64_pframe(self, "K2Node_Event_InteractingActor") = L.InteractingActor;
  L_21:; *u64_pframe(self, "K2Node_Event_InteractionComponent_1") = L.InteractionComponent;
  L_39:; 
  L_40:; 
  L_41:; U64_CALL(self, "ExecuteUbergraph_BP_Chest", u64_int(836));
  L_56:; 
  L_57:; goto u64_ret;
  L_59:; 
  u64_ret:; u64_set_return(L.ReturnValue);

}

void bp_BP_Chest__PreInteract(u64_obj *self, const u64_v *args, int argc) {
  (void)args; (void)argc;
  struct { u64_v InteractingActor; u64_v InteractionComponent; u64_v ReturnValue; u64_v _unsupported; } L; memset(&L, 0, sizeof L); (void)L;
  U64_FLOW_DECL; (void)flow_n;
  if (argc > 0) L.InteractingActor = args[0];
  if (argc > 1) L.InteractionComponent = args[1];
  L_0:; 
  L_1:; 
  L_2:; 
  L_3:; *u64_pframe(self, "K2Node_Event_InteractingActor_1") = L.InteractingActor;
  L_21:; *u64_pframe(self, "K2Node_Event_InteractionComponent_2") = L.InteractionComponent;
  L_39:; 
  L_40:; 
  L_41:; U64_CALL(self, "ExecuteUbergraph_BP_Chest", u64_int(776));
  L_56:; 
  L_57:; goto u64_ret;
  L_59:; 
  u64_ret:; u64_set_return(L.ReturnValue);

}

void bp_BP_Chest__UnregisterInteracter(u64_obj *self, const u64_v *args, int argc) {
  (void)args; (void)argc;
  struct { u64_v InteractingActor; u64_v InteractionComponent; u64_v ReturnValue; u64_v _unsupported; } L; memset(&L, 0, sizeof L); (void)L;
  U64_FLOW_DECL; (void)flow_n;
  if (argc > 0) L.InteractionComponent = args[0];
  if (argc > 1) L.InteractingActor = args[1];
  L_0:; 
  L_1:; 
  L_2:; 
  L_3:; *u64_pframe(self, "K2Node_Event_InteractionComponent_3") = L.InteractionComponent;
  L_21:; *u64_pframe(self, "K2Node_Event_InteractingActor_2") = L.InteractingActor;
  L_39:; 
  L_40:; 
  L_41:; U64_CALL(self, "ExecuteUbergraph_BP_Chest", u64_int(773));
  L_56:; 
  L_57:; goto u64_ret;
  L_59:; 
  u64_ret:; u64_set_return(L.ReturnValue);

}

void bp_BP_Chest__RegisterInteracter(u64_obj *self, const u64_v *args, int argc) {
  (void)args; (void)argc;
  struct { u64_v InteractingActor; u64_v InteractionComponent; u64_v ReturnValue; u64_v _unsupported; } L; memset(&L, 0, sizeof L); (void)L;
  U64_FLOW_DECL; (void)flow_n;
  if (argc > 0) L.InteractionComponent = args[0];
  if (argc > 1) L.InteractingActor = args[1];
  L_0:; 
  L_1:; 
  L_2:; 
  L_3:; *u64_pframe(self, "K2Node_Event_InteractionComponent_4") = L.InteractionComponent;
  L_21:; *u64_pframe(self, "K2Node_Event_InteractingActor_3") = L.InteractingActor;
  L_39:; 
  L_40:; 
  L_41:; U64_CALL(self, "ExecuteUbergraph_BP_Chest", u64_int(770));
  L_56:; 
  L_57:; goto u64_ret;
  L_59:; 
  u64_ret:; u64_set_return(L.ReturnValue);

}

void bp_BP_Chest__InteractableCancelInteraction(u64_obj *self, const u64_v *args, int argc) {
  (void)args; (void)argc;
  struct { u64_v InteractionComponent; u64_v ReturnValue; u64_v _unsupported; } L; memset(&L, 0, sizeof L); (void)L;
  U64_FLOW_DECL; (void)flow_n;
  if (argc > 0) L.InteractionComponent = args[0];
  L_0:; 
  L_1:; 
  L_2:; 
  L_3:; *u64_pframe(self, "K2Node_Event_InteractionComponent_5") = L.InteractionComponent;
  L_21:; 
  L_22:; 
  L_23:; U64_CALL(self, "ExecuteUbergraph_BP_Chest", u64_int(767));
  L_38:; 
  L_39:; goto u64_ret;
  L_41:; 
  u64_ret:; u64_set_return(L.ReturnValue);

}

void bp_BP_Chest__OnTagAdded_AE9DC3C84C991C86EDAE13BF093FE743(u64_obj *self, const u64_v *args, int argc) {
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
  L_23:; U64_CALL(self, "ExecuteUbergraph_BP_Chest", u64_int(586));
  L_38:; 
  L_39:; goto u64_ret;
  L_41:; 
  u64_ret:; u64_set_return(L.ReturnValue);

}

void bp_BP_Chest__OnTagRemoved_AE9DC3C84C991C86EDAE13BF093FE743(u64_obj *self, const u64_v *args, int argc) {
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
  L_23:; U64_CALL(self, "ExecuteUbergraph_BP_Chest", u64_int(487));
  L_38:; 
  L_39:; goto u64_ret;
  L_41:; 
  u64_ret:; u64_set_return(L.ReturnValue);

}

void bp_BP_Chest__LockFallOff__UpdateFunc(u64_obj *self, const u64_v *args, int argc) {
  (void)args; (void)argc;
  struct { u64_v ReturnValue; u64_v _unsupported; } L; memset(&L, 0, sizeof L); (void)L;
  U64_FLOW_DECL; (void)flow_n;
  L_0:; 
  L_1:; 
  L_2:; 
  L_3:; U64_CALL(self, "ExecuteUbergraph_BP_Chest", u64_int(3200));
  L_18:; 
  L_19:; goto u64_ret;
  L_21:; 
  u64_ret:; u64_set_return(L.ReturnValue);

}

void bp_BP_Chest__LockFallOff__FinishedFunc(u64_obj *self, const u64_v *args, int argc) {
  (void)args; (void)argc;
  struct { u64_v ReturnValue; u64_v _unsupported; } L; memset(&L, 0, sizeof L); (void)L;
  U64_FLOW_DECL; (void)flow_n;
  L_0:; 
  L_1:; 
  L_2:; 
  L_3:; U64_CALL(self, "ExecuteUbergraph_BP_Chest", u64_int(3207));
  L_18:; 
  L_19:; goto u64_ret;
  L_21:; 
  u64_ret:; u64_set_return(L.ReturnValue);

}

void bp_BP_Chest__GetLocationToSpawnItem(u64_obj *self, const u64_v *args, int argc) {
  (void)args; (void)argc;
  struct { u64_v CallFunc_Add_VectorVector_ReturnValue; u64_v CallFunc_DegCos_ReturnValue; u64_v CallFunc_DegSin_ReturnValue; u64_v CallFunc_K2_GetActorLocation_ReturnValue; u64_v CallFunc_MakeVector_ReturnValue; u64_v CallFunc_Multiply_FloatFloat_ReturnValue; u64_v CallFunc_Multiply_FloatFloat_ReturnValue_1; u64_v CallFunc_RandomFloatInRange_ReturnValue; u64_v Degrees; u64_v EllipseXRadius; u64_v EllipseYRadius; u64_v Location; u64_v ReturnValue; u64_v _unsupported; } L; memset(&L, 0, sizeof L); (void)L;
  U64_FLOW_DECL; (void)flow_n;
  if (argc > 0) L.Location = args[0].t == U64_REF ? *args[0].u.r : args[0];
  L_0:; 
  L_1:; 
  L_2:; 
  L_3:; L.EllipseXRadius = u64_float(100.0f);
  L_26:; 
  L_27:; 
  L_28:; L.EllipseYRadius = u64_float(150.0f);
  L_51:; 
  L_52:; 
  L_53:; L.Degrees = u64_float(0.0f);
  L_76:; 
  L_77:; L.CallFunc_RandomFloatInRange_ReturnValue = U64_CALL(self, "RandomFloatInRange", u64_float(90.0f), u64_float(270.0f));
  L_115:; 
  L_116:; L.Degrees = L.CallFunc_RandomFloatInRange_ReturnValue;
  L_143:; 
  L_144:; L.CallFunc_DegSin_ReturnValue = U64_CALL(self, "DegSin", L.Degrees);
  L_181:; L.CallFunc_DegCos_ReturnValue = U64_CALL(self, "DegCos", L.Degrees);
  L_218:; L.CallFunc_Multiply_FloatFloat_ReturnValue = U64_CALL(self, "Multiply_FloatFloat", L.EllipseYRadius, L.CallFunc_DegSin_ReturnValue);
  L_264:; L.CallFunc_K2_GetActorLocation_ReturnValue = U64_CALL(self, "K2_GetActorLocation");
  L_292:; L.CallFunc_Multiply_FloatFloat_ReturnValue_1 = U64_CALL(self, "Multiply_FloatFloat", L.EllipseXRadius, L.CallFunc_DegCos_ReturnValue);
  L_338:; L.CallFunc_MakeVector_ReturnValue = U64_CALL(self, "MakeVector", L.CallFunc_Multiply_FloatFloat_ReturnValue_1, L.CallFunc_Multiply_FloatFloat_ReturnValue, u64_float(70.0f));
  L_389:; L.CallFunc_Add_VectorVector_ReturnValue = U64_CALL(self, "Add_VectorVector", L.CallFunc_K2_GetActorLocation_ReturnValue, L.CallFunc_MakeVector_ReturnValue);
  L_435:; 
  L_436:; L.Location = L.CallFunc_Add_VectorVector_ReturnValue;
  L_463:; 
  L_464:; goto u64_ret;
  L_466:; 
  u64_ret:; u64_set_return(L.ReturnValue);
  if (argc > 0) u64_out(args[0], L.Location);
}

void bp_BP_Chest__IsAvailableForInteraction(u64_obj *self, const u64_v *args, int argc) {
  (void)args; (void)argc;
  struct { u64_v CallFunc_BooleanAND_ReturnValue; u64_v CallFunc_EqualEqual_ObjectObject_ReturnValue; u64_v CallFunc_GetAbilitySystemComponent_ReturnValue; u64_v CallFunc_HasAnyMatchingGameplayTags_ReturnValue; u64_v CallFunc_Not_PreBool_ReturnValue; u64_v InteractionComponent; u64_v ReturnValue; u64_v _unsupported; } L; memset(&L, 0, sizeof L); (void)L;
  U64_FLOW_DECL; (void)flow_n;
  if (argc > 0) L.InteractionComponent = args[0];
  L_0:; 
  L_1:; 
  L_2:; L.CallFunc_GetAbilitySystemComponent_ReturnValue = U64_CALL(self, "GetAbilitySystemComponent", u64_objv(self));
  L_23:; L.CallFunc_EqualEqual_ObjectObject_ReturnValue = U64_CALL(self, "EqualEqual_ObjectObject", L.InteractionComponent, (*u64_ivar(self, "SM_Forge_Chest_Lock")));
  L_61:; L.CallFunc_HasAnyMatchingGameplayTags_ReturnValue = ({ u64_v _o = L.CallFunc_GetAbilitySystemComponent_ReturnValue; U64_CALL(u64_asobj(_o), "HasAnyMatchingGameplayTags", (*u64_ivar(self, "AvailabilityTags"))); });
  L_117:; L.CallFunc_Not_PreBool_ReturnValue = U64_CALL(self, "Not_PreBool", L.CallFunc_HasAnyMatchingGameplayTags_ReturnValue);
  L_146:; L.CallFunc_BooleanAND_ReturnValue = U64_CALL(self, "BooleanAND", L.CallFunc_EqualEqual_ObjectObject_ReturnValue, L.CallFunc_Not_PreBool_ReturnValue);
  L_184:; 
  L_185:; L.ReturnValue = L.CallFunc_BooleanAND_ReturnValue;
  L_204:; 
  L_205:; L.ReturnValue = L.ReturnValue; goto u64_ret;
  L_215:; 
  u64_ret:; u64_set_return(L.ReturnValue);

}

void bp_BP_Chest__GetPreInteractSyncType(u64_obj *self, const u64_v *args, int argc) {
  (void)args; (void)argc;
  struct { u64_v CallFunc_EqualEqual_ObjectObject_ReturnValue; u64_v InteractionComponent; u64_v ReturnValue; u64_v Type; u64_v _unsupported; u64_v bShouldSync; } L; memset(&L, 0, sizeof L); (void)L;
  U64_FLOW_DECL; (void)flow_n;
  if (argc > 0) L.bShouldSync = args[0].t == U64_REF ? *args[0].u.r : args[0];
  if (argc > 1) L.Type = args[1].t == U64_REF ? *args[1].u.r : args[1];
  if (argc > 2) L.InteractionComponent = args[2];
  L_0:; 
  L_1:; 
  L_2:; L.CallFunc_EqualEqual_ObjectObject_ReturnValue = U64_CALL(self, "EqualEqual_ObjectObject", L.InteractionComponent, (*u64_ivar(self, "SM_Forge_Chest_Lock")));
  L_40:; 
  L_41:; 
  L_42:; if (!u64_true(L.CallFunc_EqualEqual_ObjectObject_ReturnValue)) goto L_95;
  L_56:; 
  L_57:; 
  L_58:; L.bShouldSync = u64_bool(1);
  L_69:; L.Type = u64_int(2);
  L_89:; 
  L_90:; goto L_128;
  L_95:; 
  L_96:; L.bShouldSync = u64_bool(0);
  L_107:; L.Type = u64_int(0);
  L_127:; 
  L_128:; goto u64_ret;
  L_130:; 
  u64_ret:; u64_set_return(L.ReturnValue);
  if (argc > 0) u64_out(args[0], L.bShouldSync);
  if (argc > 1) u64_out(args[1], L.Type);
}

void bp_BP_Chest__GetPostInteractSyncType(u64_obj *self, const u64_v *args, int argc) {
  (void)args; (void)argc;
  struct { u64_v CallFunc_EqualEqual_ObjectObject_ReturnValue; u64_v InteractionComponent; u64_v ReturnValue; u64_v Type; u64_v _unsupported; u64_v bShouldSync; } L; memset(&L, 0, sizeof L); (void)L;
  U64_FLOW_DECL; (void)flow_n;
  if (argc > 0) L.bShouldSync = args[0].t == U64_REF ? *args[0].u.r : args[0];
  if (argc > 1) L.Type = args[1].t == U64_REF ? *args[1].u.r : args[1];
  if (argc > 2) L.InteractionComponent = args[2];
  L_0:; 
  L_1:; 
  L_2:; L.CallFunc_EqualEqual_ObjectObject_ReturnValue = U64_CALL(self, "EqualEqual_ObjectObject", L.InteractionComponent, (*u64_ivar(self, "SM_Forge_Chest_Lock")));
  L_40:; 
  L_41:; 
  L_42:; if (!u64_true(L.CallFunc_EqualEqual_ObjectObject_ReturnValue)) goto L_95;
  L_56:; 
  L_57:; 
  L_58:; L.bShouldSync = u64_bool(0);
  L_69:; L.Type = u64_int(1);
  L_89:; 
  L_90:; goto L_128;
  L_95:; 
  L_96:; L.bShouldSync = u64_bool(0);
  L_107:; L.Type = u64_int(1);
  L_127:; 
  L_128:; goto u64_ret;
  L_130:; 
  u64_ret:; u64_set_return(L.ReturnValue);
  if (argc > 0) u64_out(args[0], L.bShouldSync);
  if (argc > 1) u64_out(args[1], L.Type);
}

void bp_BP_Chest__GetInteractionDuration(u64_obj *self, const u64_v *args, int argc) {
  (void)args; (void)argc;
  struct { u64_v CallFunc_EqualEqual_ObjectObject_ReturnValue; u64_v InteractionComponent; u64_v ReturnValue; u64_v _unsupported; } L; memset(&L, 0, sizeof L); (void)L;
  U64_FLOW_DECL; (void)flow_n;
  if (argc > 0) L.InteractionComponent = args[0];
  L_0:; 
  L_1:; 
  L_2:; L.CallFunc_EqualEqual_ObjectObject_ReturnValue = U64_CALL(self, "EqualEqual_ObjectObject", L.InteractionComponent, (*u64_ivar(self, "SM_Forge_Chest_Lock")));
  L_40:; 
  L_41:; 
  L_42:; if (!u64_true(L.CallFunc_EqualEqual_ObjectObject_ReturnValue)) goto L_91;
  L_56:; 
  L_57:; 
  L_58:; L.ReturnValue = (*u64_ivar(self, "OpenInteractionDuration"));
  L_85:; 
  L_86:; goto L_116;
  L_91:; 
  L_92:; L.ReturnValue = u64_float(0.0f);
  L_115:; 
  L_116:; L.ReturnValue = L.ReturnValue; goto u64_ret;
  L_126:; 
  u64_ret:; u64_set_return(L.ReturnValue);

}
