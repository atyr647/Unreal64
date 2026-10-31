#include "u64_rt.h"

void bp_UI_HUD__ExecuteUbergraph_UI_HUD(u64_obj *self, const u64_v *args, int argc) {
  (void)args; (void)argc;
  struct { u64_v ReturnValue; u64_v _unsupported; } L; memset(&L, 0, sizeof L); (void)L;
  U64_FLOW_DECL; (void)flow_n;
  if (argc > 0) (*u64_pframe(self, "EntryPoint")) = args[0];
  L_0:; u64_dispatch: switch (u64_as_int((*u64_pframe(self, "EntryPoint")))) { case 0: goto L_0; case 10: goto L_10; case 11: goto L_11; case 12: goto L_12; case 13: goto L_13; case 40: goto L_40; case 41: goto L_41; case 42: goto L_42; case 69: goto L_69; case 70: goto L_70; case 71: goto L_71; case 98: goto L_98; case 99: goto L_99; case 198: goto L_198; case 199: goto L_199; case 200: goto L_200; case 214: goto L_214; case 215: goto L_215; case 216: goto L_216; case 239: goto L_239; case 240: goto L_240; case 245: goto L_245; case 350: goto L_350; case 351: goto L_351; case 352: goto L_352; case 366: goto L_366; case 367: goto L_367; case 368: goto L_368; case 391: goto L_391; case 392: goto L_392; case 397: goto L_397; case 514: goto L_514; case 515: goto L_515; case 516: goto L_516; case 530: goto L_530; case 531: goto L_531; case 532: goto L_532; case 555: goto L_555; case 556: goto L_556; case 561: goto L_561; case 656: goto L_656; case 657: goto L_657; case 658: goto L_658; case 672: goto L_672; case 673: goto L_673; case 674: goto L_674; case 697: goto L_697; case 698: goto L_698; case 703: goto L_703; case 804: goto L_804; case 805: goto L_805; case 806: goto L_806; case 820: goto L_820; case 821: goto L_821; case 822: goto L_822; case 845: goto L_845; case 846: goto L_846; case 851: goto L_851; case 964: goto L_964; case 965: goto L_965; case 966: goto L_966; case 980: goto L_980; case 981: goto L_981; case 982: goto L_982; case 1005: goto L_1005; case 1006: goto L_1006; case 1011: goto L_1011; case 1112: goto L_1112; case 1113: goto L_1113; case 1114: goto L_1114; case 1128: goto L_1128; case 1129: goto L_1129; case 1130: goto L_1130; case 1153: goto L_1153; case 1154: goto L_1154; case 1159: goto L_1159; case 1266: goto L_1266; case 1267: goto L_1267; case 1268: goto L_1268; case 1282: goto L_1282; case 1283: goto L_1283; case 1284: goto L_1284; case 1307: goto L_1307; case 1308: goto L_1308; case 1313: goto L_1313; case 1432: goto L_1432; case 1433: goto L_1433; case 1434: goto L_1434; case 1448: goto L_1448; case 1449: goto L_1449; case 1450: goto L_1450; case 1473: goto L_1473; case 1474: goto L_1474; case 1479: goto L_1479; case 1578: goto L_1578; case 1579: goto L_1579; case 1580: goto L_1580; case 1594: goto L_1594; case 1595: goto L_1595; case 1596: goto L_1596; case 1619: goto L_1619; case 1620: goto L_1620; case 1625: goto L_1625; case 1730: goto L_1730; case 1731: goto L_1731; case 1732: goto L_1732; case 1746: goto L_1746; case 1747: goto L_1747; case 1748: goto L_1748; case 1771: goto L_1771; case 1772: goto L_1772; case 1777: goto L_1777; case 1894: goto L_1894; case 1895: goto L_1895; case 1896: goto L_1896; case 1910: goto L_1910; case 1911: goto L_1911; case 1912: goto L_1912; case 1935: goto L_1935; case 1936: goto L_1936; case 1941: goto L_1941; case 2032: goto L_2032; case 2033: goto L_2033; case 2034: goto L_2034; case 2048: goto L_2048; case 2049: goto L_2049; case 2086: goto L_2086; case 2087: goto L_2087; case 2110: goto L_2110; case 2111: goto L_2111; case 2116: goto L_2116; case 2211: goto L_2211; case 2212: goto L_2212; case 2213: goto L_2213; case 2227: goto L_2227; case 2228: goto L_2228; case 2265: goto L_2265; case 2266: goto L_2266; case 2289: goto L_2289; case 2290: goto L_2290; case 2295: goto L_2295; case 2296: goto L_2296; case 2328: goto L_2328; case 2329: goto L_2329; case 2334: goto L_2334; case 2363: goto L_2363; case 2364: goto L_2364; case 2365: goto L_2365; case 2379: goto L_2379; case 2380: goto L_2380; case 2385: goto L_2385; case 2408: goto L_2408; case 2409: goto L_2409; case 2462: goto L_2462; case 2463: goto L_2463; case 2464: goto L_2464; case 2491: goto L_2491; case 2492: goto L_2492; case 2497: goto L_2497; case 2526: goto L_2526; case 2527: goto L_2527; case 2528: goto L_2528; case 2542: goto L_2542; case 2543: goto L_2543; case 2544: goto L_2544; case 2580: goto L_2580; case 2581: goto L_2581; case 2582: goto L_2582; case 2593: goto L_2593; case 2594: goto L_2594; case 2623: goto L_2623; case 2624: goto L_2624; case 2625: goto L_2625; case 2639: goto L_2639; case 2640: goto L_2640; case 2641: goto L_2641; case 2694: goto L_2694; case 2695: goto L_2695; case 2696: goto L_2696; case 2715: goto L_2715; case 2716: goto L_2716; case 2717: goto L_2717; case 2768: goto L_2768; case 2769: goto L_2769; case 2820: goto L_2820; case 2871: goto L_2871; case 2872: goto L_2872; case 2913: goto L_2913; case 2914: goto L_2914; case 2915: goto L_2915; case 2971: goto L_2971; case 2972: goto L_2972; case 2973: goto L_2973; case 3029: goto L_3029; case 3030: goto L_3030; case 3031: goto L_3031; case 3064: goto L_3064; case 3065: goto L_3065; case 3070: goto L_3070; case 3071: goto L_3071; case 3072: goto L_3072; case 3073: goto L_3073; case 3100: goto L_3100; case 3101: goto L_3101; case 3157: goto L_3157; case 3158: goto L_3158; case 3159: goto L_3159; case 3173: goto L_3173; case 3174: goto L_3174; case 3175: goto L_3175; case 3176: goto L_3176; case 3190: goto L_3190; case 3191: goto L_3191; case 3192: goto L_3192; case 3230: goto L_3230; case 3231: goto L_3231; case 3236: goto L_3236; case 3237: goto L_3237; case 3238: goto L_3238; case 3239: goto L_3239; case 3266: goto L_3266; case 3267: goto L_3267; case 3323: goto L_3323; case 3324: goto L_3324; case 3325: goto L_3325; case 3339: goto L_3339; case 3340: goto L_3340; case 3341: goto L_3341; case 3379: goto L_3379; case 3380: goto L_3380; case 3385: goto L_3385; case 3386: goto L_3386; case 3387: goto L_3387; case 3388: goto L_3388; case 3415: goto L_3415; case 3416: goto L_3416; case 3417: goto L_3417; case 3431: goto L_3431; case 3432: goto L_3432; case 3437: goto L_3437; case 3438: goto L_3438; case 3439: goto L_3439; case 3440: goto L_3440; case 3467: goto L_3467; case 3468: goto L_3468; case 3469: goto L_3469; case 3483: goto L_3483; case 3484: goto L_3484; case 3489: goto L_3489; case 3490: goto L_3490; case 3491: goto L_3491; case 3492: goto L_3492; case 3533: goto L_3533; case 3534: goto L_3534; case 3539: goto L_3539; case 3540: goto L_3540; case 3541: goto L_3541; case 3602: goto L_3602; case 3632: goto L_3632; case 3661: goto L_3661; case 3697: goto L_3697; case 3729: goto L_3729; case 3761: goto L_3761; case 3790: goto L_3790; case 3810: goto L_3810; case 3892: goto L_3892; case 3893: goto L_3893; case 3938: goto L_3938; case 3939: goto L_3939; case 3944: goto L_3944; case 3945: goto L_3945; case 3946: goto L_3946; case 3947: goto L_3947; case 3974: goto L_3974; case 3975: goto L_3975; case 3976: goto L_3976; case 3990: goto L_3990; case 3991: goto L_3991; case 3996: goto L_3996; case 3997: goto L_3997; case 3998: goto L_3998; case 3999: goto L_3999; case 4026: goto L_4026; case 4027: goto L_4027; case 4028: goto L_4028; case 4042: goto L_4042; case 4043: goto L_4043; case 4048: goto L_4048; case 4049: goto L_4049; case 4050: goto L_4050; case 4051: goto L_4051; case 4092: goto L_4092; case 4093: goto L_4093; case 4098: goto L_4098; case 4099: goto L_4099; case 4100: goto L_4100; case 4161: goto L_4161; case 4191: goto L_4191; case 4220: goto L_4220; case 4256: goto L_4256; case 4288: goto L_4288; case 4320: goto L_4320; case 4349: goto L_4349; case 4369: goto L_4369; case 4451: goto L_4451; case 4452: goto L_4452; case 4497: goto L_4497; case 4498: goto L_4498; case 4503: goto L_4503; case 4504: goto L_4504; case 4505: goto L_4505; case 4554: goto L_4554; case 4555: goto L_4555; case 4600: goto L_4600; case 4601: goto L_4601; case 4606: goto L_4606; case 4607: goto L_4607; case 4608: goto L_4608; case 4657: goto L_4657; case 4658: goto L_4658; case 4703: goto L_4703; case 4704: goto L_4704; case 4709: goto L_4709; case 4710: goto L_4710; case 4711: goto L_4711; case 4760: goto L_4760; case 4761: goto L_4761; case 4806: goto L_4806; case 4807: goto L_4807; case 4812: goto L_4812; case 4813: goto L_4813; case 4814: goto L_4814; case 4863: goto L_4863; case 4864: goto L_4864; case 4909: goto L_4909; case 4910: goto L_4910; case 4915: goto L_4915; case 4916: goto L_4916; case 4917: goto L_4917; case 4918: goto L_4918; case 4945: goto L_4945; case 4946: goto L_4946; case 4947: goto L_4947; case 4961: goto L_4961; case 4962: goto L_4962; case 4967: goto L_4967; case 4968: goto L_4968; case 4969: goto L_4969; case 4970: goto L_4970; case 4997: goto L_4997; case 4998: goto L_4998; case 4999: goto L_4999; case 5013: goto L_5013; case 5014: goto L_5014; case 5019: goto L_5019; case 5020: goto L_5020; case 5021: goto L_5021; case 5022: goto L_5022; case 5063: goto L_5063; case 5064: goto L_5064; case 5069: goto L_5069; case 5070: goto L_5070; case 5071: goto L_5071; case 5132: goto L_5132; case 5162: goto L_5162; case 5191: goto L_5191; case 5227: goto L_5227; case 5259: goto L_5259; case 5291: goto L_5291; case 5320: goto L_5320; case 5340: goto L_5340; case 5422: goto L_5422; case 5423: goto L_5423; case 5468: goto L_5468; case 5469: goto L_5469; case 5474: goto L_5474; case 5475: goto L_5475; case 5476: goto L_5476; case 5500: goto L_5500; case 5501: goto L_5501; case 5559: goto L_5559; case 5588: goto L_5588; case 5589: goto L_5589; case 5603: goto L_5603; case 5604: goto L_5604; case 5633: goto L_5633; case 6702: goto L_6702; case 6703: goto L_6703; case 6741: goto L_6741; case 6742: goto L_6742; case 6771: goto L_6771; case 6772: goto L_6772; case 6773: goto L_6773; case 6787: goto L_6787; case 6788: goto L_6788; case 6811: goto L_6811; case 6812: goto L_6812; case 6853: goto L_6853; case 6854: goto L_6854; case 6855: goto L_6855; case 6891: goto L_6891; case 6892: goto L_6892; case 6893: goto L_6893; case 6912: goto L_6912; case 6913: goto L_6913; case 6942: goto L_6942; case 6943: goto L_6943; case 7027: goto L_7027; case 7028: goto L_7028; case 7057: goto L_7057; case 7058: goto L_7058; case 7059: goto L_7059; case 7073: goto L_7073; case 7074: goto L_7074; case 7097: goto L_7097; case 7098: goto L_7098; case 7139: goto L_7139; case 7140: goto L_7140; case 7163: goto L_7163; case 7164: goto L_7164; case 7205: goto L_7205; case 7206: goto L_7206; case 7207: goto L_7207; case 7243: goto L_7243; case 7244: goto L_7244; case 7249: goto L_7249; case 7250: goto L_7250; case 7251: goto L_7251; case 7256: goto L_7256; case 7257: goto L_7257; case 7295: goto L_7295; case 7296: goto L_7296; case 7301: goto L_7301; case 7302: goto L_7302; case 7340: goto L_7340; case 7341: goto L_7341; case 7346: goto L_7346; case 7347: goto L_7347; case 7348: goto L_7348; case 7349: goto L_7349; case 7390: goto L_7390; case 7391: goto L_7391; case 7396: goto L_7396; case 7457: goto L_7457; case 7487: goto L_7487; case 7516: goto L_7516; case 7552: goto L_7552; case 7584: goto L_7584; case 7616: goto L_7616; case 7645: goto L_7645; case 7665: goto L_7665; case 7760: goto L_7760; case 7761: goto L_7761; case 7806: goto L_7806; case 7807: goto L_7807; case 7812: goto L_7812; case 7813: goto L_7813; case 7851: goto L_7851; case 7852: goto L_7852; case 7857: goto L_7857; case 7858: goto L_7858; case 7859: goto L_7859; case 7901: goto L_7901; case 7902: goto L_7902; case 7929: goto L_7929; case 7930: goto L_7930; case 7991: goto L_7991; case 8021: goto L_8021; case 8050: goto L_8050; case 8086: goto L_8086; case 8118: goto L_8118; case 8150: goto L_8150; case 8179: goto L_8179; case 8199: goto L_8199; case 8294: goto L_8294; case 8295: goto L_8295; case 8340: goto L_8340; case 8341: goto L_8341; case 8375: goto L_8375; case 8376: goto L_8376; case 8377: goto L_8377; case 8391: goto L_8391; case 8392: goto L_8392; case 8393: goto L_8393; case 8413: goto L_8413; case 8414: goto L_8414; case 8415: goto L_8415; case 8453: goto L_8453; case 8454: goto L_8454; case 8459: goto L_8459; case 8460: goto L_8460; case 8474: goto L_8474; case 8475: goto L_8475; case 8480: goto L_8480; case 8481: goto L_8481; case 8482: goto L_8482; case 8483: goto L_8483; case 8519: goto L_8519; case 8565: goto L_8565; case 8566: goto L_8566; case 8571: goto L_8571; case 8572: goto L_8572; case 8573: goto L_8573; case 8574: goto L_8574; case 8619: goto L_8619; case 8620: goto L_8620; case 8625: goto L_8625; case 8626: goto L_8626; case 8627: goto L_8627; case 8632: goto L_8632; case 8633: goto L_8633; case 8647: goto L_8647; case 8648: goto L_8648; case 8653: goto L_8653; case 8654: goto L_8654; case 8655: goto L_8655; case 8656: goto L_8656; case 8657: goto L_8657; case 8671: goto L_8671; case 8672: goto L_8672; case 8677: goto L_8677; case 8678: goto L_8678; case 8679: goto L_8679; case 8680: goto L_8680; case 8707: goto L_8707; case 8708: goto L_8708; case 8713: goto L_8713; case 8714: goto L_8714; case 8715: goto L_8715; case 8716: goto L_8716; case 8727: goto L_8727; case 8728: goto L_8728; case 8762: goto L_8762; case 8763: goto L_8763; case 8764: goto L_8764; case 8778: goto L_8778; case 8779: goto L_8779; case 8780: goto L_8780; case 8821: goto L_8821; case 8822: goto L_8822; case 8846: goto L_8846; case 8897: goto L_8897; case 8926: goto L_8926; case 9000: goto L_9000; case 9029: goto L_9029; case 9067: goto L_9067; case 9068: goto L_9068; case 9069: goto L_9069; case 9083: goto L_9083; case 9084: goto L_9084; case 9085: goto L_9085; case 9123: goto L_9123; case 9124: goto L_9124; case 9129: goto L_9129; case 9130: goto L_9130; case 9171: goto L_9171; case 9172: goto L_9172; case 9177: goto L_9177; case 9178: goto L_9178; case 9179: goto L_9179; case 9180: goto L_9180; case 9218: goto L_9218; case 9219: goto L_9219; case 9220: goto L_9220; case 9231: goto L_9231; case 9232: goto L_9232; case 9237: goto L_9237; case 9238: goto L_9238; case 9239: goto L_9239; case 9273: goto L_9273; case 9274: goto L_9274; case 9275: goto L_9275; case 9289: goto L_9289; case 9290: goto L_9290; case 9291: goto L_9291; case 9328: goto L_9328; case 9329: goto L_9329; case 9330: goto L_9330; case 9353: goto L_9353; case 9354: goto L_9354; case 9355: goto L_9355; case 9382: goto L_9382; case 9383: goto L_9383; case 9406: goto L_9406; case 9407: goto L_9407; case 9468: goto L_9468; case 9469: goto L_9469; case 9470: goto L_9470; case 9497: goto L_9497; case 9498: goto L_9498; case 9503: goto L_9503; case 9504: goto L_9504; case 9505: goto L_9505; case 9506: goto L_9506; case 9526: goto L_9526; case 9527: goto L_9527; case 9528: goto L_9528; case 9565: goto L_9565; case 9566: goto L_9566; case 9571: goto L_9571; case 9572: goto L_9572; case 9573: goto L_9573; case 9619: goto L_9619; case 9620: goto L_9620; case 9647: goto L_9647; case 9648: goto L_9648; case 9694: goto L_9694; case 9695: goto L_9695; case 9736: goto L_9736; case 9737: goto L_9737; case 9742: goto L_9742; case 9743: goto L_9743; case 9744: goto L_9744; case 9805: goto L_9805; case 9835: goto L_9835; case 9864: goto L_9864; case 9900: goto L_9900; case 9932: goto L_9932; case 9964: goto L_9964; case 9993: goto L_9993; case 10013: goto L_10013; case 10095: goto L_10095; case 10096: goto L_10096; case 10141: goto L_10141; case 10142: goto L_10142; case 10147: goto L_10147; case 10148: goto L_10148; case 10149: goto L_10149; case 10150: goto L_10150; case 10177: goto L_10177; case 10178: goto L_10178; case 10183: goto L_10183; case 10184: goto L_10184; case 10185: goto L_10185; case 10186: goto L_10186; case 10213: goto L_10213; case 10214: goto L_10214; case 10219: goto L_10219; case 10221: goto L_10221; }
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
  L_99:; (*u64_pframe(self, "CallFunc_EqualEqual_GameplayAttributeGameplayAttribute_ReturnValue_13")) = U64_CALL(self, "EqualEqual_GameplayAttributeGameplayAttribute", (*u64_pframe(self, "Temp_struct_Variable")), u64_str("Health"));
  L_198:; 
  L_199:; 
  L_200:; if (!u64_true((*u64_pframe(self, "CallFunc_EqualEqual_GameplayAttributeGameplayAttribute_ReturnValue_13")))) goto L_245;
  L_214:; 
  L_215:; 
  L_216:; U64_CALL(self, "SetCurrentHealth", (*u64_pframe(self, "Temp_float_Variable_1")));
  L_239:; 
  L_240:; goto L_10219;
  L_245:; (*u64_pframe(self, "CallFunc_EqualEqual_GameplayAttributeGameplayAttribute_ReturnValue_12")) = U64_CALL(self, "EqualEqual_GameplayAttributeGameplayAttribute", (*u64_pframe(self, "Temp_struct_Variable")), u64_str("MaxHealth"));
  L_350:; 
  L_351:; 
  L_352:; if (!u64_true((*u64_pframe(self, "CallFunc_EqualEqual_GameplayAttributeGameplayAttribute_ReturnValue_12")))) goto L_397;
  L_366:; 
  L_367:; 
  L_368:; U64_CALL(self, "SetMaxHealth", (*u64_pframe(self, "Temp_float_Variable_1")));
  L_391:; 
  L_392:; goto L_10219;
  L_397:; (*u64_pframe(self, "CallFunc_EqualEqual_GameplayAttributeGameplayAttribute_ReturnValue_11")) = U64_CALL(self, "EqualEqual_GameplayAttributeGameplayAttribute", (*u64_pframe(self, "Temp_struct_Variable")), u64_str("HealthRegenRate"));
  L_514:; 
  L_515:; 
  L_516:; if (!u64_true((*u64_pframe(self, "CallFunc_EqualEqual_GameplayAttributeGameplayAttribute_ReturnValue_11")))) goto L_561;
  L_530:; 
  L_531:; 
  L_532:; U64_CALL(self, "SetHealthRegenRate", (*u64_pframe(self, "Temp_float_Variable_1")));
  L_555:; 
  L_556:; goto L_10219;
  L_561:; (*u64_pframe(self, "CallFunc_EqualEqual_GameplayAttributeGameplayAttribute_ReturnValue_10")) = U64_CALL(self, "EqualEqual_GameplayAttributeGameplayAttribute", (*u64_pframe(self, "Temp_struct_Variable")), u64_str("Mana"));
  L_656:; 
  L_657:; 
  L_658:; if (!u64_true((*u64_pframe(self, "CallFunc_EqualEqual_GameplayAttributeGameplayAttribute_ReturnValue_10")))) goto L_703;
  L_672:; 
  L_673:; 
  L_674:; U64_CALL(self, "SetCurrentMana", (*u64_pframe(self, "Temp_float_Variable_1")));
  L_697:; 
  L_698:; goto L_10219;
  L_703:; (*u64_pframe(self, "CallFunc_EqualEqual_GameplayAttributeGameplayAttribute_ReturnValue_9")) = U64_CALL(self, "EqualEqual_GameplayAttributeGameplayAttribute", (*u64_pframe(self, "Temp_struct_Variable")), u64_str("MaxMana"));
  L_804:; 
  L_805:; 
  L_806:; if (!u64_true((*u64_pframe(self, "CallFunc_EqualEqual_GameplayAttributeGameplayAttribute_ReturnValue_9")))) goto L_851;
  L_820:; 
  L_821:; 
  L_822:; U64_CALL(self, "SetMaxMana", (*u64_pframe(self, "Temp_float_Variable_1")));
  L_845:; 
  L_846:; goto L_10219;
  L_851:; (*u64_pframe(self, "CallFunc_EqualEqual_GameplayAttributeGameplayAttribute_ReturnValue_8")) = U64_CALL(self, "EqualEqual_GameplayAttributeGameplayAttribute", (*u64_pframe(self, "Temp_struct_Variable")), u64_str("ManaRegenRate"));
  L_964:; 
  L_965:; 
  L_966:; if (!u64_true((*u64_pframe(self, "CallFunc_EqualEqual_GameplayAttributeGameplayAttribute_ReturnValue_8")))) goto L_1011;
  L_980:; 
  L_981:; 
  L_982:; U64_CALL(self, "SetManaRegenRate", (*u64_pframe(self, "Temp_float_Variable_1")));
  L_1005:; 
  L_1006:; goto L_10219;
  L_1011:; (*u64_pframe(self, "CallFunc_EqualEqual_GameplayAttributeGameplayAttribute_ReturnValue_7")) = U64_CALL(self, "EqualEqual_GameplayAttributeGameplayAttribute", (*u64_pframe(self, "Temp_struct_Variable")), u64_str("Stamina"));
  L_1112:; 
  L_1113:; 
  L_1114:; if (!u64_true((*u64_pframe(self, "CallFunc_EqualEqual_GameplayAttributeGameplayAttribute_ReturnValue_7")))) goto L_1159;
  L_1128:; 
  L_1129:; 
  L_1130:; U64_CALL(self, "SetCurrentStamina", (*u64_pframe(self, "Temp_float_Variable_1")));
  L_1153:; 
  L_1154:; goto L_10219;
  L_1159:; (*u64_pframe(self, "CallFunc_EqualEqual_GameplayAttributeGameplayAttribute_ReturnValue_6")) = U64_CALL(self, "EqualEqual_GameplayAttributeGameplayAttribute", (*u64_pframe(self, "Temp_struct_Variable")), u64_str("MaxStamina"));
  L_1266:; 
  L_1267:; 
  L_1268:; if (!u64_true((*u64_pframe(self, "CallFunc_EqualEqual_GameplayAttributeGameplayAttribute_ReturnValue_6")))) goto L_1313;
  L_1282:; 
  L_1283:; 
  L_1284:; U64_CALL(self, "SetMaxStamina", (*u64_pframe(self, "Temp_float_Variable_1")));
  L_1307:; 
  L_1308:; goto L_10219;
  L_1313:; (*u64_pframe(self, "CallFunc_EqualEqual_GameplayAttributeGameplayAttribute_ReturnValue_5")) = U64_CALL(self, "EqualEqual_GameplayAttributeGameplayAttribute", (*u64_pframe(self, "Temp_struct_Variable")), u64_str("StaminaRegenRate"));
  L_1432:; 
  L_1433:; 
  L_1434:; if (!u64_true((*u64_pframe(self, "CallFunc_EqualEqual_GameplayAttributeGameplayAttribute_ReturnValue_5")))) goto L_1479;
  L_1448:; 
  L_1449:; 
  L_1450:; U64_CALL(self, "SetStaminaRegenRate", (*u64_pframe(self, "Temp_float_Variable_1")));
  L_1473:; 
  L_1474:; goto L_10219;
  L_1479:; (*u64_pframe(self, "CallFunc_EqualEqual_GameplayAttributeGameplayAttribute_ReturnValue_2")) = U64_CALL(self, "EqualEqual_GameplayAttributeGameplayAttribute", (*u64_pframe(self, "Temp_struct_Variable")), u64_str("Shield"));
  L_1578:; 
  L_1579:; 
  L_1580:; if (!u64_true((*u64_pframe(self, "CallFunc_EqualEqual_GameplayAttributeGameplayAttribute_ReturnValue_2")))) goto L_1625;
  L_1594:; 
  L_1595:; 
  L_1596:; U64_CALL(self, "SetCurrentShield", (*u64_pframe(self, "Temp_float_Variable_1")));
  L_1619:; 
  L_1620:; goto L_10219;
  L_1625:; (*u64_pframe(self, "CallFunc_EqualEqual_GameplayAttributeGameplayAttribute_ReturnValue")) = U64_CALL(self, "EqualEqual_GameplayAttributeGameplayAttribute", (*u64_pframe(self, "Temp_struct_Variable")), u64_str("MaxShield"));
  L_1730:; 
  L_1731:; 
  L_1732:; if (!u64_true((*u64_pframe(self, "CallFunc_EqualEqual_GameplayAttributeGameplayAttribute_ReturnValue")))) goto L_1777;
  L_1746:; 
  L_1747:; 
  L_1748:; U64_CALL(self, "SetMaxShield", (*u64_pframe(self, "Temp_float_Variable_1")));
  L_1771:; 
  L_1772:; goto L_10219;
  L_1777:; (*u64_pframe(self, "CallFunc_EqualEqual_GameplayAttributeGameplayAttribute_ReturnValue_1")) = U64_CALL(self, "EqualEqual_GameplayAttributeGameplayAttribute", (*u64_pframe(self, "Temp_struct_Variable")), u64_str("ShieldRegenRate"));
  L_1894:; 
  L_1895:; 
  L_1896:; if (!u64_true((*u64_pframe(self, "CallFunc_EqualEqual_GameplayAttributeGameplayAttribute_ReturnValue_1")))) goto L_1941;
  L_1910:; 
  L_1911:; 
  L_1912:; U64_CALL(self, "SetShieldRegenRate", (*u64_pframe(self, "Temp_float_Variable_1")));
  L_1935:; 
  L_1936:; goto L_10219;
  L_1941:; (*u64_pframe(self, "CallFunc_EqualEqual_GameplayAttributeGameplayAttribute_ReturnValue_4")) = U64_CALL(self, "EqualEqual_GameplayAttributeGameplayAttribute", (*u64_pframe(self, "Temp_struct_Variable")), u64_str("XP"));
  L_2032:; 
  L_2033:; 
  L_2034:; if (!u64_true((*u64_pframe(self, "CallFunc_EqualEqual_GameplayAttributeGameplayAttribute_ReturnValue_4")))) goto L_2116;
  L_2048:; 
  L_2049:; (*u64_pframe(self, "CallFunc_FTrunc_ReturnValue_1")) = U64_CALL(self, "FTrunc", (*u64_pframe(self, "Temp_float_Variable_1")));
  L_2086:; 
  L_2087:; U64_CALL(self, "SetExperience", (*u64_pframe(self, "CallFunc_FTrunc_ReturnValue_1")));
  L_2110:; 
  L_2111:; goto L_10219;
  L_2116:; (*u64_pframe(self, "CallFunc_EqualEqual_GameplayAttributeGameplayAttribute_ReturnValue_3")) = U64_CALL(self, "EqualEqual_GameplayAttributeGameplayAttribute", (*u64_pframe(self, "Temp_struct_Variable")), u64_str("Gold"));
  L_2211:; 
  L_2212:; 
  L_2213:; if (!u64_true((*u64_pframe(self, "CallFunc_EqualEqual_GameplayAttributeGameplayAttribute_ReturnValue_3")))) goto L_10219;
  L_2227:; 
  L_2228:; (*u64_pframe(self, "CallFunc_FTrunc_ReturnValue")) = U64_CALL(self, "FTrunc", (*u64_pframe(self, "Temp_float_Variable_1")));
  L_2265:; 
  L_2266:; U64_CALL(self, "SetGold", (*u64_pframe(self, "CallFunc_FTrunc_ReturnValue")));
  L_2289:; 
  L_2290:; goto L_10219;
  L_2295:; 
  L_2296:; ({ u64_v _o = (*u64_ivar(self, "AttributeChangeTask")); U64_CALL(u64_asobj(_o), "EndTask"); });
  L_2328:; 
  L_2329:; goto L_10219;
  L_2334:; (*u64_pframe(self, "CallFunc_IsValid_ReturnValue")) = U64_CALL(self, "IsValid", (*u64_ivar(self, "AttributeChangeTask")));
  L_2363:; 
  L_2364:; 
  L_2365:; if (!u64_true((*u64_pframe(self, "CallFunc_IsValid_ReturnValue")))) goto L_10219;
  L_2379:; 
  L_2380:; goto L_2295;
  L_2385:; (*u64_pframe(self, "K2Node_CreateDelegate_OutputDelegate_1")) = u64_delegate(u64_objv(self), "RespawnCountdown");
  L_2408:; 
  L_2409:; (*u64_pframe(self, "CallFunc_K2_SetTimerDelegate_ReturnValue")) = U64_CALL(self, "K2_SetTimerDelegate", (*u64_pframe(self, "K2Node_CreateDelegate_OutputDelegate_1")), u64_float(1.0f), u64_bool(1), u64_float(0.0f), u64_float(0.0f));
  L_2462:; 
  L_2463:; 
  L_2464:; (*u64_ivar(self, "Timer_RespawnCountdown")) = (*u64_pframe(self, "CallFunc_K2_SetTimerDelegate_ReturnValue"));
  L_2491:; 
  L_2492:; goto L_10219;
  L_2497:; (*u64_pframe(self, "CallFunc_IsValid_ReturnValue_1")) = U64_CALL(self, "IsValid", (*u64_ivar(self, "Reticle")));
  L_2526:; 
  L_2527:; 
  L_2528:; if (!u64_true((*u64_pframe(self, "CallFunc_IsValid_ReturnValue_1")))) goto L_2594;
  L_2542:; 
  L_2543:; 
  L_2544:; ({ u64_v _o = (*u64_ivar(self, "Reticle")); U64_CALL(u64_asobj(_o), "RemoveFromParent"); });
  L_2580:; 
  L_2581:; 
  L_2582:; (*u64_ivar(self, "Reticle")) = u64_objv(0);
  L_2593:; 
  L_2594:; (*u64_pframe(self, "CallFunc_IsValidClass_ReturnValue")) = U64_CALL(self, "IsValidClass", (*u64_pframe(self, "K2Node_Event_ReticleClass")));
  L_2623:; 
  L_2624:; 
  L_2625:; if (!u64_true((*u64_pframe(self, "CallFunc_IsValidClass_ReturnValue")))) goto L_10219;
  L_2639:; 
  L_2640:; 
  L_2641:; (*u64_pframe(self, "CallFunc_Create_ReturnValue")) = ({ u64_v _o = u64_objv(u64_find("Default__WidgetBlueprintLibrary")); U64_CALL(u64_asobj(_o), "Create", u64_objv(self), (*u64_pframe(self, "K2Node_Event_ReticleClass")), u64_objv(0)); });
  L_2694:; 
  L_2695:; 
  L_2696:; (*u64_ivar(self, "Reticle")) = (*u64_pframe(self, "CallFunc_Create_ReturnValue"));
  L_2715:; 
  L_2716:; 
  L_2717:; (*u64_pframe(self, "CallFunc_AddChildToCanvas_ReturnValue")) = ({ u64_v _o = (*u64_ivar(self, "CanvasPanel_Main")); U64_CALL(u64_asobj(_o), "AddChildToCanvas", (*u64_ivar(self, "Reticle"))); });
  L_2768:; 
  L_2769:; (*u64_member(&(*u64_pframe(self, "K2Node_MakeStruct_Anchors")), "Minimum")) = u64_int(0) /* struct const */;
  L_2820:; (*u64_member(&(*u64_pframe(self, "K2Node_MakeStruct_Anchors")), "Maximum")) = u64_int(0) /* struct const */;
  L_2871:; 
  L_2872:; ({ u64_v _o = (*u64_pframe(self, "CallFunc_AddChildToCanvas_ReturnValue")); U64_CALL(u64_asobj(_o), "SetAnchors", (*u64_pframe(self, "K2Node_MakeStruct_Anchors"))); });
  L_2913:; 
  L_2914:; 
  L_2915:; ({ u64_v _o = (*u64_pframe(self, "CallFunc_AddChildToCanvas_ReturnValue")); U64_CALL(u64_asobj(_o), "SetPosition", u64_int(0) /* struct const */); });
  L_2971:; 
  L_2972:; 
  L_2973:; ({ u64_v _o = (*u64_pframe(self, "CallFunc_AddChildToCanvas_ReturnValue")); U64_CALL(u64_asobj(_o), "SetAlignment", u64_int(0) /* struct const */); });
  L_3029:; 
  L_3030:; 
  L_3031:; ({ u64_v _o = (*u64_pframe(self, "CallFunc_AddChildToCanvas_ReturnValue")); U64_CALL(u64_asobj(_o), "SetAutoSize", u64_bool(1)); });
  L_3064:; 
  L_3065:; goto L_10219;
  L_3070:; 
  L_3071:; 
  L_3072:; 
  L_3073:; (*u64_pframe(self, "Temp_struct_Variable_1")) = (*u64_pframe(self, "K2Node_CustomEvent_Tag_1"));
  L_3100:; 
  L_3101:; (*u64_pframe(self, "CallFunc_EqualEqual_GameplayTag_ReturnValue")) = U64_CALL(self, "EqualEqual_GameplayTag", (*u64_pframe(self, "Temp_struct_Variable_1")), u64_name("Ability.BlocksInteraction"));
  L_3157:; 
  L_3158:; 
  L_3159:; if (!u64_true((*u64_pframe(self, "CallFunc_EqualEqual_GameplayTag_ReturnValue")))) goto L_10219;
  L_3173:; 
  L_3174:; 
  L_3175:; 
  L_3176:; if (!u64_true((*u64_ivar(self, "InteractionPromptVisible")))) goto L_10219;
  L_3190:; 
  L_3191:; 
  L_3192:; ({ u64_v _o = (*u64_ivar(self, "CanvasPanel_Interaction")); U64_CALL(u64_asobj(_o), "SetVisibility", u64_int(4)); });
  L_3230:; 
  L_3231:; goto L_10219;
  L_3236:; 
  L_3237:; 
  L_3238:; 
  L_3239:; (*u64_pframe(self, "Temp_struct_Variable_1")) = (*u64_pframe(self, "K2Node_CustomEvent_Tag"));
  L_3266:; 
  L_3267:; (*u64_pframe(self, "CallFunc_EqualEqual_GameplayTag_ReturnValue")) = U64_CALL(self, "EqualEqual_GameplayTag", (*u64_pframe(self, "Temp_struct_Variable_1")), u64_name("Ability.BlocksInteraction"));
  L_3323:; 
  L_3324:; 
  L_3325:; if (!u64_true((*u64_pframe(self, "CallFunc_EqualEqual_GameplayTag_ReturnValue")))) goto L_10219;
  L_3339:; 
  L_3340:; 
  L_3341:; ({ u64_v _o = (*u64_ivar(self, "CanvasPanel_Interaction")); U64_CALL(u64_asobj(_o), "SetVisibility", u64_int(2)); });
  L_3379:; 
  L_3380:; goto L_10219;
  L_3385:; 
  L_3386:; 
  L_3387:; 
  L_3388:; (*u64_ivar(self, "CurrentMana")) = (*u64_pframe(self, "K2Node_Event_CurrentMana"));
  L_3415:; 
  L_3416:; 
  L_3417:; U64_CALL(self, "SetMana");
  L_3431:; 
  L_3432:; goto L_10219;
  L_3437:; 
  L_3438:; 
  L_3439:; 
  L_3440:; (*u64_ivar(self, "MaxMana")) = (*u64_pframe(self, "K2Node_Event_MaxMana"));
  L_3467:; 
  L_3468:; 
  L_3469:; U64_CALL(self, "SetMana");
  L_3483:; 
  L_3484:; goto L_10219;
  L_3489:; 
  L_3490:; 
  L_3491:; 
  L_3492:; ({ u64_v _o = (*u64_ivar(self, "ProgressBar_Mana")); U64_CALL(u64_asobj(_o), "SetPercent", (*u64_pframe(self, "K2Node_Event_ManaPercentage"))); });
  L_3533:; 
  L_3534:; goto L_10219;
  L_3539:; 
  L_3540:; 
  L_3541:; (*u64_pframe(self, "CallFunc_Conv_FloatToText_ReturnValue_2")) = U64_CALL(self, "Conv_FloatToText", (*u64_pframe(self, "K2Node_Event_ManaRegenRate")), u64_int(0), u64_bool(0), u64_bool(1), u64_int(1), u64_int(4), u64_int(1), u64_int(1));
  L_3602:; (*u64_member(&(*u64_pframe(self, "K2Node_MakeStruct_FormatArgumentData_2")), "ArgumentName")) = u64_str("H");
  L_3632:; (*u64_member(&(*u64_pframe(self, "K2Node_MakeStruct_FormatArgumentData_2")), "ArgumentValueType")) = u64_int(4);
  L_3661:; (*u64_member(&(*u64_pframe(self, "K2Node_MakeStruct_FormatArgumentData_2")), "ArgumentValue")) = (*u64_pframe(self, "CallFunc_Conv_FloatToText_ReturnValue_2"));
  L_3697:; (*u64_member(&(*u64_pframe(self, "K2Node_MakeStruct_FormatArgumentData_2")), "ArgumentValueInt")) = u64_int(0);
  L_3729:; (*u64_member(&(*u64_pframe(self, "K2Node_MakeStruct_FormatArgumentData_2")), "ArgumentValueFloat")) = u64_float(0.0f);
  L_3761:; (*u64_member(&(*u64_pframe(self, "K2Node_MakeStruct_FormatArgumentData_2")), "ArgumentValueGender")) = u64_int(0);
  L_3790:; u64_array_set(&(*u64_pframe(self, "K2Node_MakeArray_Array_2")), 1, (u64_v[]){u64_int(0), (*u64_pframe(self, "K2Node_MakeStruct_FormatArgumentData_2"))} + 1);
  L_3810:; (*u64_pframe(self, "CallFunc_Format_ReturnValue_2")) = U64_CALL(self, "Format", u64_str(""), (*u64_pframe(self, "K2Node_MakeArray_Array_2")));
  L_3892:; 
  L_3893:; ({ u64_v _o = (*u64_ivar(self, "TextBlock_ManaRegen")); U64_CALL(u64_asobj(_o), "SetText", (*u64_pframe(self, "CallFunc_Format_ReturnValue_2"))); });
  L_3938:; 
  L_3939:; goto L_10219;
  L_3944:; 
  L_3945:; 
  L_3946:; 
  L_3947:; (*u64_ivar(self, "Current_Stamina")) = (*u64_pframe(self, "K2Node_Event_CurrentStamina"));
  L_3974:; 
  L_3975:; 
  L_3976:; U64_CALL(self, "SetStamina");
  L_3990:; 
  L_3991:; goto L_10219;
  L_3996:; 
  L_3997:; 
  L_3998:; 
  L_3999:; (*u64_ivar(self, "MaxStamina")) = (*u64_pframe(self, "K2Node_Event_MaxStamina"));
  L_4026:; 
  L_4027:; 
  L_4028:; U64_CALL(self, "SetStamina");
  L_4042:; 
  L_4043:; goto L_10219;
  L_4048:; 
  L_4049:; 
  L_4050:; 
  L_4051:; ({ u64_v _o = (*u64_ivar(self, "ProgressBar_Stamina")); U64_CALL(u64_asobj(_o), "SetPercent", (*u64_pframe(self, "K2Node_Event_StaminaPercentage"))); });
  L_4092:; 
  L_4093:; goto L_10219;
  L_4098:; 
  L_4099:; 
  L_4100:; (*u64_pframe(self, "CallFunc_Conv_FloatToText_ReturnValue_3")) = U64_CALL(self, "Conv_FloatToText", (*u64_pframe(self, "K2Node_Event_StaminaRegenRate")), u64_int(0), u64_bool(0), u64_bool(1), u64_int(1), u64_int(4), u64_int(1), u64_int(1));
  L_4161:; (*u64_member(&(*u64_pframe(self, "K2Node_MakeStruct_FormatArgumentData_3")), "ArgumentName")) = u64_str("H");
  L_4191:; (*u64_member(&(*u64_pframe(self, "K2Node_MakeStruct_FormatArgumentData_3")), "ArgumentValueType")) = u64_int(4);
  L_4220:; (*u64_member(&(*u64_pframe(self, "K2Node_MakeStruct_FormatArgumentData_3")), "ArgumentValue")) = (*u64_pframe(self, "CallFunc_Conv_FloatToText_ReturnValue_3"));
  L_4256:; (*u64_member(&(*u64_pframe(self, "K2Node_MakeStruct_FormatArgumentData_3")), "ArgumentValueInt")) = u64_int(0);
  L_4288:; (*u64_member(&(*u64_pframe(self, "K2Node_MakeStruct_FormatArgumentData_3")), "ArgumentValueFloat")) = u64_float(0.0f);
  L_4320:; (*u64_member(&(*u64_pframe(self, "K2Node_MakeStruct_FormatArgumentData_3")), "ArgumentValueGender")) = u64_int(0);
  L_4349:; u64_array_set(&(*u64_pframe(self, "K2Node_MakeArray_Array_3")), 1, (u64_v[]){u64_int(0), (*u64_pframe(self, "K2Node_MakeStruct_FormatArgumentData_3"))} + 1);
  L_4369:; (*u64_pframe(self, "CallFunc_Format_ReturnValue_3")) = U64_CALL(self, "Format", u64_str(""), (*u64_pframe(self, "K2Node_MakeArray_Array_3")));
  L_4451:; 
  L_4452:; ({ u64_v _o = (*u64_ivar(self, "TextBlock_StaminaRegen")); U64_CALL(u64_asobj(_o), "SetText", (*u64_pframe(self, "CallFunc_Format_ReturnValue_3"))); });
  L_4497:; 
  L_4498:; goto L_10219;
  L_4503:; 
  L_4504:; 
  L_4505:; (*u64_pframe(self, "CallFunc_Conv_IntToText_ReturnValue")) = U64_CALL(self, "Conv_IntToText", (*u64_pframe(self, "K2Node_Event_Experience")), u64_bool(0), u64_bool(1), u64_int(1), u64_int(4));
  L_4554:; 
  L_4555:; ({ u64_v _o = (*u64_ivar(self, "TextBlock_XP")); U64_CALL(u64_asobj(_o), "SetText", (*u64_pframe(self, "CallFunc_Conv_IntToText_ReturnValue"))); });
  L_4600:; 
  L_4601:; goto L_10219;
  L_4606:; 
  L_4607:; 
  L_4608:; (*u64_pframe(self, "CallFunc_Conv_IntToText_ReturnValue_1")) = U64_CALL(self, "Conv_IntToText", (*u64_pframe(self, "K2Node_Event_Gold")), u64_bool(0), u64_bool(1), u64_int(1), u64_int(4));
  L_4657:; 
  L_4658:; ({ u64_v _o = (*u64_ivar(self, "TextBlock_Gold")); U64_CALL(u64_asobj(_o), "SetText", (*u64_pframe(self, "CallFunc_Conv_IntToText_ReturnValue_1"))); });
  L_4703:; 
  L_4704:; goto L_10219;
  L_4709:; 
  L_4710:; 
  L_4711:; (*u64_pframe(self, "CallFunc_Conv_IntToText_ReturnValue_2")) = U64_CALL(self, "Conv_IntToText", (*u64_pframe(self, "K2Node_Event_ClipAmmo")), u64_bool(0), u64_bool(1), u64_int(1), u64_int(4));
  L_4760:; 
  L_4761:; ({ u64_v _o = (*u64_ivar(self, "TextBlock_ClipAmmo")); U64_CALL(u64_asobj(_o), "SetText", (*u64_pframe(self, "CallFunc_Conv_IntToText_ReturnValue_2"))); });
  L_4806:; 
  L_4807:; goto L_10219;
  L_4812:; 
  L_4813:; 
  L_4814:; (*u64_pframe(self, "CallFunc_Conv_IntToText_ReturnValue_3")) = U64_CALL(self, "Conv_IntToText", (*u64_pframe(self, "K2Node_Event_ReserveAmmo")), u64_bool(0), u64_bool(1), u64_int(1), u64_int(4));
  L_4863:; 
  L_4864:; ({ u64_v _o = (*u64_ivar(self, "TextBlock_ReserveAmmo")); U64_CALL(u64_asobj(_o), "SetText", (*u64_pframe(self, "CallFunc_Conv_IntToText_ReturnValue_3"))); });
  L_4909:; 
  L_4910:; goto L_10219;
  L_4915:; 
  L_4916:; 
  L_4917:; 
  L_4918:; (*u64_ivar(self, "CurrentShield")) = (*u64_pframe(self, "K2Node_Event_CurrentShield"));
  L_4945:; 
  L_4946:; 
  L_4947:; U64_CALL(self, "SetShield");
  L_4961:; 
  L_4962:; goto L_10219;
  L_4967:; 
  L_4968:; 
  L_4969:; 
  L_4970:; (*u64_ivar(self, "MaxShield")) = (*u64_pframe(self, "K2Node_Event_MaxShield"));
  L_4997:; 
  L_4998:; 
  L_4999:; U64_CALL(self, "SetShield");
  L_5013:; 
  L_5014:; goto L_10219;
  L_5019:; 
  L_5020:; 
  L_5021:; 
  L_5022:; ({ u64_v _o = (*u64_ivar(self, "ProgressBar_Shield")); U64_CALL(u64_asobj(_o), "SetPercent", (*u64_pframe(self, "K2Node_Event_ShieldPercentage"))); });
  L_5063:; 
  L_5064:; goto L_10219;
  L_5069:; 
  L_5070:; 
  L_5071:; (*u64_pframe(self, "CallFunc_Conv_FloatToText_ReturnValue_4")) = U64_CALL(self, "Conv_FloatToText", (*u64_pframe(self, "K2Node_Event_ShieldRegenRate")), u64_int(0), u64_bool(0), u64_bool(1), u64_int(1), u64_int(4), u64_int(1), u64_int(1));
  L_5132:; (*u64_member(&(*u64_pframe(self, "K2Node_MakeStruct_FormatArgumentData_4")), "ArgumentName")) = u64_str("H");
  L_5162:; (*u64_member(&(*u64_pframe(self, "K2Node_MakeStruct_FormatArgumentData_4")), "ArgumentValueType")) = u64_int(4);
  L_5191:; (*u64_member(&(*u64_pframe(self, "K2Node_MakeStruct_FormatArgumentData_4")), "ArgumentValue")) = (*u64_pframe(self, "CallFunc_Conv_FloatToText_ReturnValue_4"));
  L_5227:; (*u64_member(&(*u64_pframe(self, "K2Node_MakeStruct_FormatArgumentData_4")), "ArgumentValueInt")) = u64_int(0);
  L_5259:; (*u64_member(&(*u64_pframe(self, "K2Node_MakeStruct_FormatArgumentData_4")), "ArgumentValueFloat")) = u64_float(0.0f);
  L_5291:; (*u64_member(&(*u64_pframe(self, "K2Node_MakeStruct_FormatArgumentData_4")), "ArgumentValueGender")) = u64_int(0);
  L_5320:; u64_array_set(&(*u64_pframe(self, "K2Node_MakeArray_Array_4")), 1, (u64_v[]){u64_int(0), (*u64_pframe(self, "K2Node_MakeStruct_FormatArgumentData_4"))} + 1);
  L_5340:; (*u64_pframe(self, "CallFunc_Format_ReturnValue_4")) = U64_CALL(self, "Format", u64_str(""), (*u64_pframe(self, "K2Node_MakeArray_Array_4")));
  L_5422:; 
  L_5423:; ({ u64_v _o = (*u64_ivar(self, "TextBlock_ShieldRegen")); U64_CALL(u64_asobj(_o), "SetText", (*u64_pframe(self, "CallFunc_Format_ReturnValue_4"))); });
  L_5468:; 
  L_5469:; goto L_10219;
  L_5474:; 
  L_5475:; 
  L_5476:; (*u64_pframe(self, "CallFunc_GetOwningPlayer_ReturnValue")) = U64_CALL(self, "GetOwningPlayer");
  L_5500:; 
  L_5501:; (*u64_pframe(self, "K2Node_DynamicCast_AsGSPlayer_State")) = u64_cast(({ u64_v _o = (*u64_pframe(self, "CallFunc_GetOwningPlayer_ReturnValue")); (*u64_ivar(u64_asobj(_o), "PlayerState")); }), "GSPlayerState");
  L_5559:; (*u64_pframe(self, "K2Node_DynamicCast_bSuccess")) = (*u64_pframe(self, "K2Node_DynamicCast_AsGSPlayer_State"));
  L_5588:; 
  L_5589:; if (!u64_true((*u64_pframe(self, "K2Node_DynamicCast_bSuccess")))) goto L_10219;
  L_5603:; 
  L_5604:; (*u64_pframe(self, "CallFunc_GetAbilitySystemComponent_ReturnValue")) = U64_CALL(self, "GetAbilitySystemComponent", (*u64_pframe(self, "K2Node_DynamicCast_AsGSPlayer_State")));
  L_5633:; u64_array_set(&(*u64_pframe(self, "K2Node_MakeArray_Array_5")), 14, (u64_v[]){u64_int(0), u64_str("Health"), u64_str("MaxHealth"), u64_str("HealthRegenRate"), u64_str("Mana"), u64_str("MaxMana"), u64_str("ManaRegenRate"), u64_str("Stamina"), u64_str("MaxStamina"), u64_str("StaminaRegenRate"), u64_str("XP"), u64_str("Gold"), u64_str("Shield"), u64_str("MaxShield"), u64_str("ShieldRegenRate")} + 1);
  L_6702:; 
  L_6703:; (*u64_pframe(self, "CallFunc_ListenForAttributesChange_ReturnValue")) = U64_CALL(self, "ListenForAttributesChange", (*u64_pframe(self, "CallFunc_GetAbilitySystemComponent_ReturnValue")), (*u64_pframe(self, "K2Node_MakeArray_Array_5")));
  L_6741:; 
  L_6742:; (*u64_pframe(self, "CallFunc_IsValid_ReturnValue_2")) = U64_CALL(self, "IsValid", (*u64_pframe(self, "CallFunc_ListenForAttributesChange_ReturnValue")));
  L_6771:; 
  L_6772:; 
  L_6773:; if (!u64_true((*u64_pframe(self, "CallFunc_IsValid_ReturnValue_2")))) goto L_6892;
  L_6787:; 
  L_6788:; (*u64_pframe(self, "K2Node_CreateDelegate_OutputDelegate")) = u64_delegate(u64_objv(self), "OnAttributeChanged_CCD21B84410C5C76D20E0D9410EECC4F");
  L_6811:; 
  L_6812:; u64_mc_add(&(*u64_ivar(u64_asobj((*u64_pframe(self, "CallFunc_ListenForAttributesChange_ReturnValue"))), "OnAttributeChanged")), (*u64_pframe(self, "K2Node_CreateDelegate_OutputDelegate")));
  L_6853:; 
  L_6854:; 
  L_6855:; ({ u64_v _o = (*u64_pframe(self, "CallFunc_ListenForAttributesChange_ReturnValue")); U64_CALL(u64_asobj(_o), "Activate"); });
  L_6891:; 
  L_6892:; 
  L_6893:; (*u64_ivar(self, "AttributeChangeTask")) = (*u64_pframe(self, "CallFunc_ListenForAttributesChange_ReturnValue"));
  L_6912:; 
  L_6913:; (*u64_pframe(self, "CallFunc_GetAbilitySystemComponent_ReturnValue")) = U64_CALL(self, "GetAbilitySystemComponent", (*u64_pframe(self, "K2Node_DynamicCast_AsGSPlayer_State")));
  L_6942:; 
  L_6943:; (*u64_pframe(self, "CallFunc_ListenForGameplayTagAddedOrRemoved_ReturnValue")) = U64_CALL(self, "ListenForGameplayTagAddedOrRemoved", (*u64_pframe(self, "CallFunc_GetAbilitySystemComponent_ReturnValue")), u64_int(0) /* struct const */);
  L_7027:; 
  L_7028:; (*u64_pframe(self, "CallFunc_IsValid_ReturnValue_3")) = U64_CALL(self, "IsValid", (*u64_pframe(self, "CallFunc_ListenForGameplayTagAddedOrRemoved_ReturnValue")));
  L_7057:; 
  L_7058:; 
  L_7059:; if (!u64_true((*u64_pframe(self, "CallFunc_IsValid_ReturnValue_3")))) goto L_10219;
  L_7073:; 
  L_7074:; (*u64_pframe(self, "K2Node_CreateDelegate_OutputDelegate_4")) = u64_delegate(u64_objv(self), "OnTagAdded_A125A0334AE35991A63334982F940767");
  L_7097:; 
  L_7098:; u64_mc_add(&(*u64_ivar(u64_asobj((*u64_pframe(self, "CallFunc_ListenForGameplayTagAddedOrRemoved_ReturnValue"))), "OnTagAdded")), (*u64_pframe(self, "K2Node_CreateDelegate_OutputDelegate_4")));
  L_7139:; 
  L_7140:; (*u64_pframe(self, "K2Node_CreateDelegate_OutputDelegate_3")) = u64_delegate(u64_objv(self), "OnTagRemoved_A125A0334AE35991A63334982F940767");
  L_7163:; 
  L_7164:; u64_mc_add(&(*u64_ivar(u64_asobj((*u64_pframe(self, "CallFunc_ListenForGameplayTagAddedOrRemoved_ReturnValue"))), "OnTagRemoved")), (*u64_pframe(self, "K2Node_CreateDelegate_OutputDelegate_3")));
  L_7205:; 
  L_7206:; 
  L_7207:; ({ u64_v _o = (*u64_pframe(self, "CallFunc_ListenForGameplayTagAddedOrRemoved_ReturnValue")); U64_CALL(u64_asobj(_o), "Activate"); });
  L_7243:; 
  L_7244:; goto L_10219;
  L_7249:; 
  L_7250:; 
  L_7251:; goto L_2334;
  L_7256:; 
  L_7257:; ({ u64_v _o = (*u64_ivar(self, "TextBlock_ConfirmCancel")); U64_CALL(u64_asobj(_o), "SetVisibility", u64_int(1)); });
  L_7295:; 
  L_7296:; goto L_10219;
  L_7301:; 
  L_7302:; ({ u64_v _o = (*u64_ivar(self, "TextBlock_ConfirmCancel")); U64_CALL(u64_asobj(_o), "SetVisibility", u64_int(0)); });
  L_7340:; 
  L_7341:; goto L_10219;
  L_7346:; 
  L_7347:; 
  L_7348:; 
  L_7349:; ({ u64_v _o = (*u64_ivar(self, "ProgressBar_Health")); U64_CALL(u64_asobj(_o), "SetPercent", (*u64_pframe(self, "K2Node_Event_HealthPercentage"))); });
  L_7390:; 
  L_7391:; goto L_10219;
  L_7396:; (*u64_pframe(self, "CallFunc_Conv_FloatToText_ReturnValue")) = U64_CALL(self, "Conv_FloatToText", (*u64_ivar(self, "RespawnTimeRemaining")), u64_int(0), u64_bool(0), u64_bool(1), u64_int(1), u64_int(4), u64_int(0), u64_int(0));
  L_7457:; (*u64_member(&(*u64_pframe(self, "K2Node_MakeStruct_FormatArgumentData")), "ArgumentName")) = u64_str("R");
  L_7487:; (*u64_member(&(*u64_pframe(self, "K2Node_MakeStruct_FormatArgumentData")), "ArgumentValueType")) = u64_int(4);
  L_7516:; (*u64_member(&(*u64_pframe(self, "K2Node_MakeStruct_FormatArgumentData")), "ArgumentValue")) = (*u64_pframe(self, "CallFunc_Conv_FloatToText_ReturnValue"));
  L_7552:; (*u64_member(&(*u64_pframe(self, "K2Node_MakeStruct_FormatArgumentData")), "ArgumentValueInt")) = u64_int(0);
  L_7584:; (*u64_member(&(*u64_pframe(self, "K2Node_MakeStruct_FormatArgumentData")), "ArgumentValueFloat")) = u64_float(0.0f);
  L_7616:; (*u64_member(&(*u64_pframe(self, "K2Node_MakeStruct_FormatArgumentData")), "ArgumentValueGender")) = u64_int(0);
  L_7645:; u64_array_set(&(*u64_pframe(self, "K2Node_MakeArray_Array")), 1, (u64_v[]){u64_int(0), (*u64_pframe(self, "K2Node_MakeStruct_FormatArgumentData"))} + 1);
  L_7665:; (*u64_pframe(self, "CallFunc_Format_ReturnValue")) = U64_CALL(self, "Format", u64_str(""), (*u64_pframe(self, "K2Node_MakeArray_Array")));
  L_7760:; 
  L_7761:; ({ u64_v _o = (*u64_ivar(self, "TextBlock_RespawnCountdown")); U64_CALL(u64_asobj(_o), "SetText", (*u64_pframe(self, "CallFunc_Format_ReturnValue"))); });
  L_7806:; 
  L_7807:; goto L_2385;
  L_7812:; 
  L_7813:; ({ u64_v _o = (*u64_ivar(self, "TextBlock_RespawnCountdown")); U64_CALL(u64_asobj(_o), "SetVisibility", u64_int(0)); });
  L_7851:; 
  L_7852:; goto L_7396;
  L_7857:; 
  L_7858:; 
  L_7859:; (*u64_pframe(self, "CallFunc_Subtract_FloatFloat_ReturnValue")) = U64_CALL(self, "Subtract_FloatFloat", (*u64_ivar(self, "RespawnTimeRemaining")), u64_float(1.0f));
  L_7901:; 
  L_7902:; (*u64_ivar(self, "RespawnTimeRemaining")) = (*u64_pframe(self, "CallFunc_Subtract_FloatFloat_ReturnValue"));
  L_7929:; 
  L_7930:; (*u64_pframe(self, "CallFunc_Conv_FloatToText_ReturnValue_1")) = U64_CALL(self, "Conv_FloatToText", (*u64_ivar(self, "RespawnTimeRemaining")), u64_int(0), u64_bool(0), u64_bool(1), u64_int(1), u64_int(4), u64_int(0), u64_int(0));
  L_7991:; (*u64_member(&(*u64_pframe(self, "K2Node_MakeStruct_FormatArgumentData_1")), "ArgumentName")) = u64_str("R");
  L_8021:; (*u64_member(&(*u64_pframe(self, "K2Node_MakeStruct_FormatArgumentData_1")), "ArgumentValueType")) = u64_int(4);
  L_8050:; (*u64_member(&(*u64_pframe(self, "K2Node_MakeStruct_FormatArgumentData_1")), "ArgumentValue")) = (*u64_pframe(self, "CallFunc_Conv_FloatToText_ReturnValue_1"));
  L_8086:; (*u64_member(&(*u64_pframe(self, "K2Node_MakeStruct_FormatArgumentData_1")), "ArgumentValueInt")) = u64_int(0);
  L_8118:; (*u64_member(&(*u64_pframe(self, "K2Node_MakeStruct_FormatArgumentData_1")), "ArgumentValueFloat")) = u64_float(0.0f);
  L_8150:; (*u64_member(&(*u64_pframe(self, "K2Node_MakeStruct_FormatArgumentData_1")), "ArgumentValueGender")) = u64_int(0);
  L_8179:; u64_array_set(&(*u64_pframe(self, "K2Node_MakeArray_Array_1")), 1, (u64_v[]){u64_int(0), (*u64_pframe(self, "K2Node_MakeStruct_FormatArgumentData_1"))} + 1);
  L_8199:; (*u64_pframe(self, "CallFunc_Format_ReturnValue_1")) = U64_CALL(self, "Format", u64_str(""), (*u64_pframe(self, "K2Node_MakeArray_Array_1")));
  L_8294:; 
  L_8295:; ({ u64_v _o = (*u64_ivar(self, "TextBlock_RespawnCountdown")); U64_CALL(u64_asobj(_o), "SetText", (*u64_pframe(self, "CallFunc_Format_ReturnValue_1"))); });
  L_8340:; 
  L_8341:; (*u64_pframe(self, "CallFunc_LessEqual_FloatFloat_ReturnValue")) = U64_CALL(self, "LessEqual_FloatFloat", (*u64_ivar(self, "RespawnTimeRemaining")), u64_float(0.0f));
  L_8375:; 
  L_8376:; 
  L_8377:; if (!u64_true((*u64_pframe(self, "CallFunc_LessEqual_FloatFloat_ReturnValue")))) goto L_10219;
  L_8391:; 
  L_8392:; 
  L_8393:; U64_CALL(self, "K2_ClearAndInvalidateTimerHandle", u64_objv(self), (*u64_ivar(self, "Timer_RespawnCountdown")));
  L_8413:; 
  L_8414:; 
  L_8415:; ({ u64_v _o = (*u64_ivar(self, "TextBlock_RespawnCountdown")); U64_CALL(u64_asobj(_o), "SetVisibility", u64_int(1)); });
  L_8453:; 
  L_8454:; goto L_10219;
  L_8459:; 
  L_8460:; U64_CALL(self, "SetHealth");
  L_8474:; 
  L_8475:; goto L_10219;
  L_8480:; 
  L_8481:; 
  L_8482:; 
  L_8483:; (*u64_pframe(self, "CallFunc_SetBrushFromAtlasInterface_AtlasRegion_CastInput")) = (*u64_pframe(self, "K2Node_Event_Sprite"));
  L_8519:; ({ u64_v _o = (*u64_ivar(self, "Image_EquippedWeapon")); U64_CALL(u64_asobj(_o), "SetBrushFromAtlasInterface", u64_ref(&(*u64_pframe(self, "CallFunc_SetBrushFromAtlasInterface_AtlasRegion_CastInput"))), u64_bool(1)); });
  L_8565:; 
  L_8566:; goto L_10219;
  L_8571:; 
  L_8572:; 
  L_8573:; 
  L_8574:; ({ u64_v _o = (*u64_ivar(self, "TextBlock_WeaponStatus")); U64_CALL(u64_asobj(_o), "SetText", (*u64_pframe(self, "K2Node_Event_StatusText"))); });
  L_8619:; 
  L_8620:; goto L_10219;
  L_8625:; 
  L_8626:; 
  L_8627:; goto L_2497;
  L_8632:; 
  L_8633:; U64_CALL(self, "SetHealth");
  L_8647:; 
  L_8648:; goto L_10219;
  L_8653:; 
  L_8654:; 
  L_8655:; 
  L_8656:; 
  L_8657:; if (!u64_true((*u64_pframe(self, "K2Node_Event_bShowText")))) goto L_7256;
  L_8671:; 
  L_8672:; goto L_7301;
  L_8677:; 
  L_8678:; 
  L_8679:; 
  L_8680:; (*u64_ivar(self, "RespawnTimeRemaining")) = (*u64_pframe(self, "K2Node_Event_RespawnTimeRemaining"));
  L_8707:; 
  L_8708:; goto L_7812;
  L_8713:; 
  L_8714:; 
  L_8715:; 
  L_8716:; (*u64_ivar(self, "InteractionPromptVisible")) = u64_bool(1);
  L_8727:; 
  L_8728:; (*u64_pframe(self, "CallFunc_Greater_FloatFloat_ReturnValue")) = U64_CALL(self, "Greater_FloatFloat", (*u64_pframe(self, "K2Node_Event_InteractionDuration_1")), u64_float(0.0f));
  L_8762:; 
  L_8763:; 
  L_8764:; if (!u64_true((*u64_pframe(self, "CallFunc_Greater_FloatFloat_ReturnValue")))) goto L_9129;
  L_8778:; 
  L_8779:; 
  L_8780:; ({ u64_v _o = (*u64_ivar(self, "WidgetSwitcher_InteractionPrompt")); U64_CALL(u64_asobj(_o), "SetActiveWidgetIndex", u64_int(1)); });
  L_8821:; 
  L_8822:; (*u64_pframe(self, "CallFunc_GetOwningPlayer_ReturnValue_1")) = U64_CALL(self, "GetOwningPlayer");
  L_8846:; (*u64_pframe(self, "CallFunc_GetAbilitySystemComponent_ReturnValue_1")) = U64_CALL(self, "GetAbilitySystemComponent", ({ u64_v _o = (*u64_pframe(self, "CallFunc_GetOwningPlayer_ReturnValue_1")); (*u64_ivar(u64_asobj(_o), "PlayerState")); }));
  L_8897:; (*u64_pframe(self, "CallFunc_IsValid_ReturnValue_4")) = U64_CALL(self, "IsValid", (*u64_pframe(self, "CallFunc_GetAbilitySystemComponent_ReturnValue_1")));
  L_8926:; (*u64_pframe(self, "CallFunc_HasMatchingGameplayTag_ReturnValue")) = ({ u64_v _o = (*u64_pframe(self, "CallFunc_GetAbilitySystemComponent_ReturnValue_1")); U64_CALL(u64_asobj(_o), "HasMatchingGameplayTag", u64_name("Ability.BlocksInteraction")); });
  L_9000:; (*u64_pframe(self, "CallFunc_Not_PreBool_ReturnValue")) = U64_CALL(self, "Not_PreBool", (*u64_pframe(self, "CallFunc_HasMatchingGameplayTag_ReturnValue")));
  L_9029:; (*u64_pframe(self, "CallFunc_BooleanAND_ReturnValue")) = U64_CALL(self, "BooleanAND", (*u64_pframe(self, "CallFunc_IsValid_ReturnValue_4")), (*u64_pframe(self, "CallFunc_Not_PreBool_ReturnValue")));
  L_9067:; 
  L_9068:; 
  L_9069:; if (!u64_true((*u64_pframe(self, "CallFunc_BooleanAND_ReturnValue")))) goto L_10219;
  L_9083:; 
  L_9084:; 
  L_9085:; ({ u64_v _o = (*u64_ivar(self, "CanvasPanel_Interaction")); U64_CALL(u64_asobj(_o), "SetVisibility", u64_int(4)); });
  L_9123:; 
  L_9124:; goto L_10219;
  L_9129:; 
  L_9130:; ({ u64_v _o = (*u64_ivar(self, "WidgetSwitcher_InteractionPrompt")); U64_CALL(u64_asobj(_o), "SetActiveWidgetIndex", u64_int(0)); });
  L_9171:; 
  L_9172:; goto L_8822;
  L_9177:; 
  L_9178:; 
  L_9179:; 
  L_9180:; ({ u64_v _o = (*u64_ivar(self, "CanvasPanel_Interaction")); U64_CALL(u64_asobj(_o), "SetVisibility", u64_int(2)); });
  L_9218:; 
  L_9219:; 
  L_9220:; (*u64_ivar(self, "InteractionPromptVisible")) = u64_bool(0);
  L_9231:; 
  L_9232:; goto L_10219;
  L_9237:; 
  L_9238:; 
  L_9239:; (*u64_pframe(self, "CallFunc_Greater_FloatFloat_ReturnValue_1")) = U64_CALL(self, "Greater_FloatFloat", (*u64_pframe(self, "K2Node_Event_InteractionDuration")), u64_float(0.0f));
  L_9273:; 
  L_9274:; 
  L_9275:; if (!u64_true((*u64_pframe(self, "CallFunc_Greater_FloatFloat_ReturnValue_1")))) goto L_10219;
  L_9289:; 
  L_9290:; 
  L_9291:; ({ u64_v _o = (*u64_ivar(self, "ProgressBar_InteractionDuration")); U64_CALL(u64_asobj(_o), "SetPercent", u64_float(0.0f)); });
  L_9328:; 
  L_9329:; 
  L_9330:; (*u64_ivar(self, "CurrentInteractionTime")) = u64_float(0.0f);
  L_9353:; 
  L_9354:; 
  L_9355:; (*u64_ivar(self, "InteractionDuration")) = (*u64_pframe(self, "K2Node_Event_InteractionDuration"));
  L_9382:; 
  L_9383:; (*u64_pframe(self, "K2Node_CreateDelegate_OutputDelegate_2")) = u64_delegate(u64_objv(self), "InteractionProgressTick");
  L_9406:; 
  L_9407:; (*u64_pframe(self, "CallFunc_K2_SetTimerDelegate_ReturnValue_1")) = U64_CALL(self, "K2_SetTimerDelegate", (*u64_pframe(self, "K2Node_CreateDelegate_OutputDelegate_2")), (*u64_ivar(self, "InteractionTick")), u64_bool(1), (*u64_ivar(self, "InteractionTick")), u64_float(0.0f));
  L_9468:; 
  L_9469:; 
  L_9470:; (*u64_ivar(self, "Timer_Interaction")) = (*u64_pframe(self, "CallFunc_K2_SetTimerDelegate_ReturnValue_1"));
  L_9497:; 
  L_9498:; goto L_10219;
  L_9503:; 
  L_9504:; 
  L_9505:; 
  L_9506:; U64_CALL(self, "K2_ClearAndInvalidateTimerHandle", u64_objv(self), (*u64_ivar(self, "Timer_Interaction")));
  L_9526:; 
  L_9527:; 
  L_9528:; ({ u64_v _o = (*u64_ivar(self, "ProgressBar_InteractionDuration")); U64_CALL(u64_asobj(_o), "SetPercent", u64_float(0.0f)); });
  L_9565:; 
  L_9566:; goto L_10219;
  L_9571:; 
  L_9572:; 
  L_9573:; (*u64_pframe(self, "CallFunc_Add_FloatFloat_ReturnValue")) = U64_CALL(self, "Add_FloatFloat", (*u64_ivar(self, "CurrentInteractionTime")), (*u64_ivar(self, "InteractionTick")));
  L_9619:; 
  L_9620:; (*u64_ivar(self, "CurrentInteractionTime")) = (*u64_pframe(self, "CallFunc_Add_FloatFloat_ReturnValue"));
  L_9647:; 
  L_9648:; (*u64_pframe(self, "CallFunc_Divide_FloatFloat_ReturnValue")) = U64_CALL(self, "Divide_FloatFloat", (*u64_ivar(self, "CurrentInteractionTime")), (*u64_ivar(self, "InteractionDuration")));
  L_9694:; 
  L_9695:; ({ u64_v _o = (*u64_ivar(self, "ProgressBar_InteractionDuration")); U64_CALL(u64_asobj(_o), "SetPercent", (*u64_pframe(self, "CallFunc_Divide_FloatFloat_ReturnValue"))); });
  L_9736:; 
  L_9737:; goto L_10219;
  L_9742:; 
  L_9743:; 
  L_9744:; (*u64_pframe(self, "CallFunc_Conv_FloatToText_ReturnValue_5")) = U64_CALL(self, "Conv_FloatToText", (*u64_pframe(self, "K2Node_Event_HealthRegenRate")), u64_int(0), u64_bool(0), u64_bool(1), u64_int(1), u64_int(4), u64_int(1), u64_int(1));
  L_9805:; (*u64_member(&(*u64_pframe(self, "K2Node_MakeStruct_FormatArgumentData_5")), "ArgumentName")) = u64_str("H");
  L_9835:; (*u64_member(&(*u64_pframe(self, "K2Node_MakeStruct_FormatArgumentData_5")), "ArgumentValueType")) = u64_int(4);
  L_9864:; (*u64_member(&(*u64_pframe(self, "K2Node_MakeStruct_FormatArgumentData_5")), "ArgumentValue")) = (*u64_pframe(self, "CallFunc_Conv_FloatToText_ReturnValue_5"));
  L_9900:; (*u64_member(&(*u64_pframe(self, "K2Node_MakeStruct_FormatArgumentData_5")), "ArgumentValueInt")) = u64_int(0);
  L_9932:; (*u64_member(&(*u64_pframe(self, "K2Node_MakeStruct_FormatArgumentData_5")), "ArgumentValueFloat")) = u64_float(0.0f);
  L_9964:; (*u64_member(&(*u64_pframe(self, "K2Node_MakeStruct_FormatArgumentData_5")), "ArgumentValueGender")) = u64_int(0);
  L_9993:; u64_array_set(&(*u64_pframe(self, "K2Node_MakeArray_Array_6")), 1, (u64_v[]){u64_int(0), (*u64_pframe(self, "K2Node_MakeStruct_FormatArgumentData_5"))} + 1);
  L_10013:; (*u64_pframe(self, "CallFunc_Format_ReturnValue_5")) = U64_CALL(self, "Format", u64_str(""), (*u64_pframe(self, "K2Node_MakeArray_Array_6")));
  L_10095:; 
  L_10096:; ({ u64_v _o = (*u64_ivar(self, "TextBlock_HealthRegen")); U64_CALL(u64_asobj(_o), "SetText", (*u64_pframe(self, "CallFunc_Format_ReturnValue_5"))); });
  L_10141:; 
  L_10142:; goto L_10219;
  L_10147:; 
  L_10148:; 
  L_10149:; 
  L_10150:; (*u64_ivar(self, "MaxHealth")) = (*u64_pframe(self, "K2Node_Event_MaxHealth"));
  L_10177:; 
  L_10178:; goto L_8459;
  L_10183:; 
  L_10184:; 
  L_10185:; 
  L_10186:; (*u64_ivar(self, "CurrentHealth")) = (*u64_pframe(self, "K2Node_Event_CurrentHealth"));
  L_10213:; 
  L_10214:; goto L_8632;
  L_10219:; goto u64_ret;
  L_10221:; 
  u64_ret:; u64_set_return(L.ReturnValue);

}

void bp_UI_HUD__SetCurrentHealth(u64_obj *self, const u64_v *args, int argc) {
  (void)args; (void)argc;
  struct { u64_v CurrentHealth; u64_v ReturnValue; u64_v _unsupported; } L; memset(&L, 0, sizeof L); (void)L;
  U64_FLOW_DECL; (void)flow_n;
  if (argc > 0) L.CurrentHealth = args[0];
  L_0:; 
  L_1:; 
  L_2:; 
  L_3:; *u64_pframe(self, "K2Node_Event_CurrentHealth") = L.CurrentHealth;
  L_21:; 
  L_22:; 
  L_23:; U64_CALL(self, "ExecuteUbergraph_UI_HUD", u64_int(10183));
  L_38:; 
  L_39:; goto u64_ret;
  L_41:; 
  u64_ret:; u64_set_return(L.ReturnValue);

}

void bp_UI_HUD__SetMaxHealth(u64_obj *self, const u64_v *args, int argc) {
  (void)args; (void)argc;
  struct { u64_v MaxHealth; u64_v ReturnValue; u64_v _unsupported; } L; memset(&L, 0, sizeof L); (void)L;
  U64_FLOW_DECL; (void)flow_n;
  if (argc > 0) L.MaxHealth = args[0];
  L_0:; 
  L_1:; 
  L_2:; 
  L_3:; *u64_pframe(self, "K2Node_Event_MaxHealth") = L.MaxHealth;
  L_21:; 
  L_22:; 
  L_23:; U64_CALL(self, "ExecuteUbergraph_UI_HUD", u64_int(10147));
  L_38:; 
  L_39:; goto u64_ret;
  L_41:; 
  u64_ret:; u64_set_return(L.ReturnValue);

}

void bp_UI_HUD__SetHealthRegenRate(u64_obj *self, const u64_v *args, int argc) {
  (void)args; (void)argc;
  struct { u64_v HealthRegenRate; u64_v ReturnValue; u64_v _unsupported; } L; memset(&L, 0, sizeof L); (void)L;
  U64_FLOW_DECL; (void)flow_n;
  if (argc > 0) L.HealthRegenRate = args[0];
  L_0:; 
  L_1:; 
  L_2:; 
  L_3:; *u64_pframe(self, "K2Node_Event_HealthRegenRate") = L.HealthRegenRate;
  L_21:; 
  L_22:; 
  L_23:; U64_CALL(self, "ExecuteUbergraph_UI_HUD", u64_int(9742));
  L_38:; 
  L_39:; goto u64_ret;
  L_41:; 
  u64_ret:; u64_set_return(L.ReturnValue);

}

void bp_UI_HUD__InteractionProgressTick(u64_obj *self, const u64_v *args, int argc) {
  (void)args; (void)argc;
  struct { u64_v ReturnValue; u64_v _unsupported; } L; memset(&L, 0, sizeof L); (void)L;
  U64_FLOW_DECL; (void)flow_n;
  L_0:; 
  L_1:; 
  L_2:; 
  L_3:; U64_CALL(self, "ExecuteUbergraph_UI_HUD", u64_int(9571));
  L_18:; 
  L_19:; goto u64_ret;
  L_21:; 
  u64_ret:; u64_set_return(L.ReturnValue);

}

void bp_UI_HUD__StopInteractionTimer(u64_obj *self, const u64_v *args, int argc) {
  (void)args; (void)argc;
  struct { u64_v ReturnValue; u64_v _unsupported; } L; memset(&L, 0, sizeof L); (void)L;
  U64_FLOW_DECL; (void)flow_n;
  L_0:; 
  L_1:; 
  L_2:; 
  L_3:; U64_CALL(self, "ExecuteUbergraph_UI_HUD", u64_int(9503));
  L_18:; 
  L_19:; goto u64_ret;
  L_21:; 
  u64_ret:; u64_set_return(L.ReturnValue);

}

void bp_UI_HUD__StartInteractionTimer(u64_obj *self, const u64_v *args, int argc) {
  (void)args; (void)argc;
  struct { u64_v InteractionDuration; u64_v ReturnValue; u64_v _unsupported; } L; memset(&L, 0, sizeof L); (void)L;
  U64_FLOW_DECL; (void)flow_n;
  if (argc > 0) L.InteractionDuration = args[0];
  L_0:; 
  L_1:; 
  L_2:; 
  L_3:; *u64_pframe(self, "K2Node_Event_InteractionDuration") = L.InteractionDuration;
  L_21:; 
  L_22:; 
  L_23:; U64_CALL(self, "ExecuteUbergraph_UI_HUD", u64_int(9237));
  L_38:; 
  L_39:; goto u64_ret;
  L_41:; 
  u64_ret:; u64_set_return(L.ReturnValue);

}

void bp_UI_HUD__HideInteractionPrompt(u64_obj *self, const u64_v *args, int argc) {
  (void)args; (void)argc;
  struct { u64_v ReturnValue; u64_v _unsupported; } L; memset(&L, 0, sizeof L); (void)L;
  U64_FLOW_DECL; (void)flow_n;
  L_0:; 
  L_1:; 
  L_2:; 
  L_3:; U64_CALL(self, "ExecuteUbergraph_UI_HUD", u64_int(9177));
  L_18:; 
  L_19:; goto u64_ret;
  L_21:; 
  u64_ret:; u64_set_return(L.ReturnValue);

}

void bp_UI_HUD__ShowInteractionPrompt(u64_obj *self, const u64_v *args, int argc) {
  (void)args; (void)argc;
  struct { u64_v InteractionDuration; u64_v ReturnValue; u64_v _unsupported; } L; memset(&L, 0, sizeof L); (void)L;
  U64_FLOW_DECL; (void)flow_n;
  if (argc > 0) L.InteractionDuration = args[0];
  L_0:; 
  L_1:; 
  L_2:; 
  L_3:; *u64_pframe(self, "K2Node_Event_InteractionDuration_1") = L.InteractionDuration;
  L_21:; 
  L_22:; 
  L_23:; U64_CALL(self, "ExecuteUbergraph_UI_HUD", u64_int(8713));
  L_38:; 
  L_39:; goto u64_ret;
  L_41:; 
  u64_ret:; u64_set_return(L.ReturnValue);

}

void bp_UI_HUD__SetRespawnCountdown(u64_obj *self, const u64_v *args, int argc) {
  (void)args; (void)argc;
  struct { u64_v RespawnTimeRemaining; u64_v ReturnValue; u64_v _unsupported; } L; memset(&L, 0, sizeof L); (void)L;
  U64_FLOW_DECL; (void)flow_n;
  if (argc > 0) L.RespawnTimeRemaining = args[0];
  L_0:; 
  L_1:; 
  L_2:; 
  L_3:; *u64_pframe(self, "K2Node_Event_RespawnTimeRemaining") = L.RespawnTimeRemaining;
  L_21:; 
  L_22:; 
  L_23:; U64_CALL(self, "ExecuteUbergraph_UI_HUD", u64_int(8677));
  L_38:; 
  L_39:; goto u64_ret;
  L_41:; 
  u64_ret:; u64_set_return(L.ReturnValue);

}

void bp_UI_HUD__ShowAbilityConfirmPrompt(u64_obj *self, const u64_v *args, int argc) {
  (void)args; (void)argc;
  struct { u64_v ReturnValue; u64_v _unsupported; u64_v bShowText; } L; memset(&L, 0, sizeof L); (void)L;
  U64_FLOW_DECL; (void)flow_n;
  if (argc > 0) L.bShowText = args[0];
  L_0:; 
  L_1:; 
  L_2:; 
  L_3:; *u64_pframe(self, "K2Node_Event_bShowText") = L.bShowText;
  L_21:; 
  L_22:; 
  L_23:; U64_CALL(self, "ExecuteUbergraph_UI_HUD", u64_int(8653));
  L_38:; 
  L_39:; goto u64_ret;
  L_41:; 
  u64_ret:; u64_set_return(L.ReturnValue);

}

void bp_UI_HUD__SetReticle(u64_obj *self, const u64_v *args, int argc) {
  (void)args; (void)argc;
  struct { u64_v ReticleClass; u64_v ReturnValue; u64_v _unsupported; } L; memset(&L, 0, sizeof L); (void)L;
  U64_FLOW_DECL; (void)flow_n;
  if (argc > 0) L.ReticleClass = args[0];
  L_0:; 
  L_1:; 
  L_2:; 
  L_3:; *u64_pframe(self, "K2Node_Event_ReticleClass") = L.ReticleClass;
  L_21:; 
  L_22:; 
  L_23:; U64_CALL(self, "ExecuteUbergraph_UI_HUD", u64_int(8625));
  L_38:; 
  L_39:; goto u64_ret;
  L_41:; 
  u64_ret:; u64_set_return(L.ReturnValue);

}

void bp_UI_HUD__SetEquippedWeaponStatusText(u64_obj *self, const u64_v *args, int argc) {
  (void)args; (void)argc;
  struct { u64_v ReturnValue; u64_v StatusText; u64_v _unsupported; } L; memset(&L, 0, sizeof L); (void)L;
  U64_FLOW_DECL; (void)flow_n;
  if (argc > 0) L.StatusText = args[0].t == U64_REF ? *args[0].u.r : args[0];
  L_0:; 
  L_1:; 
  L_2:; 
  L_3:; *u64_pframe(self, "K2Node_Event_StatusText") = L.StatusText;
  L_21:; 
  L_22:; 
  L_23:; U64_CALL(self, "ExecuteUbergraph_UI_HUD", u64_int(8571));
  L_38:; 
  L_39:; goto u64_ret;
  L_41:; 
  u64_ret:; u64_set_return(L.ReturnValue);
  if (argc > 0) u64_out(args[0], L.StatusText);
}

void bp_UI_HUD__SetEquippedWeaponSprite(u64_obj *self, const u64_v *args, int argc) {
  (void)args; (void)argc;
  struct { u64_v ReturnValue; u64_v Sprite; u64_v _unsupported; } L; memset(&L, 0, sizeof L); (void)L;
  U64_FLOW_DECL; (void)flow_n;
  if (argc > 0) L.Sprite = args[0];
  L_0:; 
  L_1:; 
  L_2:; 
  L_3:; *u64_pframe(self, "K2Node_Event_Sprite") = L.Sprite;
  L_21:; 
  L_22:; 
  L_23:; U64_CALL(self, "ExecuteUbergraph_UI_HUD", u64_int(8480));
  L_38:; 
  L_39:; goto u64_ret;
  L_41:; 
  u64_ret:; u64_set_return(L.ReturnValue);

}

void bp_UI_HUD__RespawnCountdown(u64_obj *self, const u64_v *args, int argc) {
  (void)args; (void)argc;
  struct { u64_v ReturnValue; u64_v _unsupported; } L; memset(&L, 0, sizeof L); (void)L;
  U64_FLOW_DECL; (void)flow_n;
  L_0:; 
  L_1:; 
  L_2:; 
  L_3:; U64_CALL(self, "ExecuteUbergraph_UI_HUD", u64_int(7857));
  L_18:; 
  L_19:; goto u64_ret;
  L_21:; 
  u64_ret:; u64_set_return(L.ReturnValue);

}

void bp_UI_HUD__SetHealthPercentage(u64_obj *self, const u64_v *args, int argc) {
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
  L_23:; U64_CALL(self, "ExecuteUbergraph_UI_HUD", u64_int(7346));
  L_38:; 
  L_39:; goto u64_ret;
  L_41:; 
  u64_ret:; u64_set_return(L.ReturnValue);

}

void bp_UI_HUD__Destruct(u64_obj *self, const u64_v *args, int argc) {
  (void)args; (void)argc;
  struct { u64_v ReturnValue; u64_v _unsupported; } L; memset(&L, 0, sizeof L); (void)L;
  U64_FLOW_DECL; (void)flow_n;
  L_0:; 
  L_1:; 
  L_2:; 
  L_3:; U64_CALL(self, "ExecuteUbergraph_UI_HUD", u64_int(7249));
  L_18:; 
  L_19:; goto u64_ret;
  L_21:; 
  u64_ret:; u64_set_return(L.ReturnValue);

}

void bp_UI_HUD__Construct(u64_obj *self, const u64_v *args, int argc) {
  (void)args; (void)argc;
  struct { u64_v ReturnValue; u64_v _unsupported; } L; memset(&L, 0, sizeof L); (void)L;
  U64_FLOW_DECL; (void)flow_n;
  L_0:; 
  L_1:; 
  L_2:; 
  L_3:; U64_CALL(self, "ExecuteUbergraph_UI_HUD", u64_int(5474));
  L_18:; 
  L_19:; goto u64_ret;
  L_21:; 
  u64_ret:; u64_set_return(L.ReturnValue);

}

void bp_UI_HUD__SetShieldRegenRate(u64_obj *self, const u64_v *args, int argc) {
  (void)args; (void)argc;
  struct { u64_v ReturnValue; u64_v ShieldRegenRate; u64_v _unsupported; } L; memset(&L, 0, sizeof L); (void)L;
  U64_FLOW_DECL; (void)flow_n;
  if (argc > 0) L.ShieldRegenRate = args[0];
  L_0:; 
  L_1:; 
  L_2:; 
  L_3:; *u64_pframe(self, "K2Node_Event_ShieldRegenRate") = L.ShieldRegenRate;
  L_21:; 
  L_22:; 
  L_23:; U64_CALL(self, "ExecuteUbergraph_UI_HUD", u64_int(5069));
  L_38:; 
  L_39:; goto u64_ret;
  L_41:; 
  u64_ret:; u64_set_return(L.ReturnValue);

}

void bp_UI_HUD__SetShieldPercentage(u64_obj *self, const u64_v *args, int argc) {
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
  L_23:; U64_CALL(self, "ExecuteUbergraph_UI_HUD", u64_int(5019));
  L_38:; 
  L_39:; goto u64_ret;
  L_41:; 
  u64_ret:; u64_set_return(L.ReturnValue);

}

void bp_UI_HUD__SetMaxShield(u64_obj *self, const u64_v *args, int argc) {
  (void)args; (void)argc;
  struct { u64_v MaxShield; u64_v ReturnValue; u64_v _unsupported; } L; memset(&L, 0, sizeof L); (void)L;
  U64_FLOW_DECL; (void)flow_n;
  if (argc > 0) L.MaxShield = args[0];
  L_0:; 
  L_1:; 
  L_2:; 
  L_3:; *u64_pframe(self, "K2Node_Event_MaxShield") = L.MaxShield;
  L_21:; 
  L_22:; 
  L_23:; U64_CALL(self, "ExecuteUbergraph_UI_HUD", u64_int(4967));
  L_38:; 
  L_39:; goto u64_ret;
  L_41:; 
  u64_ret:; u64_set_return(L.ReturnValue);

}

void bp_UI_HUD__SetCurrentShield(u64_obj *self, const u64_v *args, int argc) {
  (void)args; (void)argc;
  struct { u64_v CurrentShield; u64_v ReturnValue; u64_v _unsupported; } L; memset(&L, 0, sizeof L); (void)L;
  U64_FLOW_DECL; (void)flow_n;
  if (argc > 0) L.CurrentShield = args[0];
  L_0:; 
  L_1:; 
  L_2:; 
  L_3:; *u64_pframe(self, "K2Node_Event_CurrentShield") = L.CurrentShield;
  L_21:; 
  L_22:; 
  L_23:; U64_CALL(self, "ExecuteUbergraph_UI_HUD", u64_int(4915));
  L_38:; 
  L_39:; goto u64_ret;
  L_41:; 
  u64_ret:; u64_set_return(L.ReturnValue);

}

void bp_UI_HUD__SetPrimaryReserveAmmo(u64_obj *self, const u64_v *args, int argc) {
  (void)args; (void)argc;
  struct { u64_v ReserveAmmo; u64_v ReturnValue; u64_v _unsupported; } L; memset(&L, 0, sizeof L); (void)L;
  U64_FLOW_DECL; (void)flow_n;
  if (argc > 0) L.ReserveAmmo = args[0];
  L_0:; 
  L_1:; 
  L_2:; 
  L_3:; *u64_pframe(self, "K2Node_Event_ReserveAmmo") = L.ReserveAmmo;
  L_21:; 
  L_22:; 
  L_23:; U64_CALL(self, "ExecuteUbergraph_UI_HUD", u64_int(4812));
  L_38:; 
  L_39:; goto u64_ret;
  L_41:; 
  u64_ret:; u64_set_return(L.ReturnValue);

}

void bp_UI_HUD__SetPrimaryClipAmmo(u64_obj *self, const u64_v *args, int argc) {
  (void)args; (void)argc;
  struct { u64_v ClipAmmo; u64_v ReturnValue; u64_v _unsupported; } L; memset(&L, 0, sizeof L); (void)L;
  U64_FLOW_DECL; (void)flow_n;
  if (argc > 0) L.ClipAmmo = args[0];
  L_0:; 
  L_1:; 
  L_2:; 
  L_3:; *u64_pframe(self, "K2Node_Event_ClipAmmo") = L.ClipAmmo;
  L_21:; 
  L_22:; 
  L_23:; U64_CALL(self, "ExecuteUbergraph_UI_HUD", u64_int(4709));
  L_38:; 
  L_39:; goto u64_ret;
  L_41:; 
  u64_ret:; u64_set_return(L.ReturnValue);

}

void bp_UI_HUD__SetGold(u64_obj *self, const u64_v *args, int argc) {
  (void)args; (void)argc;
  struct { u64_v Gold; u64_v ReturnValue; u64_v _unsupported; } L; memset(&L, 0, sizeof L); (void)L;
  U64_FLOW_DECL; (void)flow_n;
  if (argc > 0) L.Gold = args[0];
  L_0:; 
  L_1:; 
  L_2:; 
  L_3:; *u64_pframe(self, "K2Node_Event_Gold") = L.Gold;
  L_21:; 
  L_22:; 
  L_23:; U64_CALL(self, "ExecuteUbergraph_UI_HUD", u64_int(4606));
  L_38:; 
  L_39:; goto u64_ret;
  L_41:; 
  u64_ret:; u64_set_return(L.ReturnValue);

}

void bp_UI_HUD__SetExperience(u64_obj *self, const u64_v *args, int argc) {
  (void)args; (void)argc;
  struct { u64_v Experience; u64_v ReturnValue; u64_v _unsupported; } L; memset(&L, 0, sizeof L); (void)L;
  U64_FLOW_DECL; (void)flow_n;
  if (argc > 0) L.Experience = args[0];
  L_0:; 
  L_1:; 
  L_2:; 
  L_3:; *u64_pframe(self, "K2Node_Event_Experience") = L.Experience;
  L_21:; 
  L_22:; 
  L_23:; U64_CALL(self, "ExecuteUbergraph_UI_HUD", u64_int(4503));
  L_38:; 
  L_39:; goto u64_ret;
  L_41:; 
  u64_ret:; u64_set_return(L.ReturnValue);

}

void bp_UI_HUD__SetStaminaRegenRate(u64_obj *self, const u64_v *args, int argc) {
  (void)args; (void)argc;
  struct { u64_v ReturnValue; u64_v StaminaRegenRate; u64_v _unsupported; } L; memset(&L, 0, sizeof L); (void)L;
  U64_FLOW_DECL; (void)flow_n;
  if (argc > 0) L.StaminaRegenRate = args[0];
  L_0:; 
  L_1:; 
  L_2:; 
  L_3:; *u64_pframe(self, "K2Node_Event_StaminaRegenRate") = L.StaminaRegenRate;
  L_21:; 
  L_22:; 
  L_23:; U64_CALL(self, "ExecuteUbergraph_UI_HUD", u64_int(4098));
  L_38:; 
  L_39:; goto u64_ret;
  L_41:; 
  u64_ret:; u64_set_return(L.ReturnValue);

}

void bp_UI_HUD__SetStaminaPercentage(u64_obj *self, const u64_v *args, int argc) {
  (void)args; (void)argc;
  struct { u64_v ReturnValue; u64_v StaminaPercentage; u64_v _unsupported; } L; memset(&L, 0, sizeof L); (void)L;
  U64_FLOW_DECL; (void)flow_n;
  if (argc > 0) L.StaminaPercentage = args[0];
  L_0:; 
  L_1:; 
  L_2:; 
  L_3:; *u64_pframe(self, "K2Node_Event_StaminaPercentage") = L.StaminaPercentage;
  L_21:; 
  L_22:; 
  L_23:; U64_CALL(self, "ExecuteUbergraph_UI_HUD", u64_int(4048));
  L_38:; 
  L_39:; goto u64_ret;
  L_41:; 
  u64_ret:; u64_set_return(L.ReturnValue);

}

void bp_UI_HUD__SetMaxStamina(u64_obj *self, const u64_v *args, int argc) {
  (void)args; (void)argc;
  struct { u64_v MaxStamina; u64_v ReturnValue; u64_v _unsupported; } L; memset(&L, 0, sizeof L); (void)L;
  U64_FLOW_DECL; (void)flow_n;
  if (argc > 0) L.MaxStamina = args[0];
  L_0:; 
  L_1:; 
  L_2:; 
  L_3:; *u64_pframe(self, "K2Node_Event_MaxStamina") = L.MaxStamina;
  L_21:; 
  L_22:; 
  L_23:; U64_CALL(self, "ExecuteUbergraph_UI_HUD", u64_int(3996));
  L_38:; 
  L_39:; goto u64_ret;
  L_41:; 
  u64_ret:; u64_set_return(L.ReturnValue);

}

void bp_UI_HUD__SetCurrentStamina(u64_obj *self, const u64_v *args, int argc) {
  (void)args; (void)argc;
  struct { u64_v CurrentStamina; u64_v ReturnValue; u64_v _unsupported; } L; memset(&L, 0, sizeof L); (void)L;
  U64_FLOW_DECL; (void)flow_n;
  if (argc > 0) L.CurrentStamina = args[0];
  L_0:; 
  L_1:; 
  L_2:; 
  L_3:; *u64_pframe(self, "K2Node_Event_CurrentStamina") = L.CurrentStamina;
  L_21:; 
  L_22:; 
  L_23:; U64_CALL(self, "ExecuteUbergraph_UI_HUD", u64_int(3944));
  L_38:; 
  L_39:; goto u64_ret;
  L_41:; 
  u64_ret:; u64_set_return(L.ReturnValue);

}

void bp_UI_HUD__SetManaRegenRate(u64_obj *self, const u64_v *args, int argc) {
  (void)args; (void)argc;
  struct { u64_v ManaRegenRate; u64_v ReturnValue; u64_v _unsupported; } L; memset(&L, 0, sizeof L); (void)L;
  U64_FLOW_DECL; (void)flow_n;
  if (argc > 0) L.ManaRegenRate = args[0];
  L_0:; 
  L_1:; 
  L_2:; 
  L_3:; *u64_pframe(self, "K2Node_Event_ManaRegenRate") = L.ManaRegenRate;
  L_21:; 
  L_22:; 
  L_23:; U64_CALL(self, "ExecuteUbergraph_UI_HUD", u64_int(3539));
  L_38:; 
  L_39:; goto u64_ret;
  L_41:; 
  u64_ret:; u64_set_return(L.ReturnValue);

}

void bp_UI_HUD__SetManaPercentage(u64_obj *self, const u64_v *args, int argc) {
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
  L_23:; U64_CALL(self, "ExecuteUbergraph_UI_HUD", u64_int(3489));
  L_38:; 
  L_39:; goto u64_ret;
  L_41:; 
  u64_ret:; u64_set_return(L.ReturnValue);

}

void bp_UI_HUD__SetMaxMana(u64_obj *self, const u64_v *args, int argc) {
  (void)args; (void)argc;
  struct { u64_v MaxMana; u64_v ReturnValue; u64_v _unsupported; } L; memset(&L, 0, sizeof L); (void)L;
  U64_FLOW_DECL; (void)flow_n;
  if (argc > 0) L.MaxMana = args[0];
  L_0:; 
  L_1:; 
  L_2:; 
  L_3:; *u64_pframe(self, "K2Node_Event_MaxMana") = L.MaxMana;
  L_21:; 
  L_22:; 
  L_23:; U64_CALL(self, "ExecuteUbergraph_UI_HUD", u64_int(3437));
  L_38:; 
  L_39:; goto u64_ret;
  L_41:; 
  u64_ret:; u64_set_return(L.ReturnValue);

}

void bp_UI_HUD__SetCurrentMana(u64_obj *self, const u64_v *args, int argc) {
  (void)args; (void)argc;
  struct { u64_v CurrentMana; u64_v ReturnValue; u64_v _unsupported; } L; memset(&L, 0, sizeof L); (void)L;
  U64_FLOW_DECL; (void)flow_n;
  if (argc > 0) L.CurrentMana = args[0];
  L_0:; 
  L_1:; 
  L_2:; 
  L_3:; *u64_pframe(self, "K2Node_Event_CurrentMana") = L.CurrentMana;
  L_21:; 
  L_22:; 
  L_23:; U64_CALL(self, "ExecuteUbergraph_UI_HUD", u64_int(3385));
  L_38:; 
  L_39:; goto u64_ret;
  L_41:; 
  u64_ret:; u64_set_return(L.ReturnValue);

}

void bp_UI_HUD__OnTagAdded_A125A0334AE35991A63334982F940767(u64_obj *self, const u64_v *args, int argc) {
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
  L_23:; U64_CALL(self, "ExecuteUbergraph_UI_HUD", u64_int(3236));
  L_38:; 
  L_39:; goto u64_ret;
  L_41:; 
  u64_ret:; u64_set_return(L.ReturnValue);

}

void bp_UI_HUD__OnTagRemoved_A125A0334AE35991A63334982F940767(u64_obj *self, const u64_v *args, int argc) {
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
  L_23:; U64_CALL(self, "ExecuteUbergraph_UI_HUD", u64_int(3070));
  L_38:; 
  L_39:; goto u64_ret;
  L_41:; 
  u64_ret:; u64_set_return(L.ReturnValue);

}

void bp_UI_HUD__OnAttributeChanged_CCD21B84410C5C76D20E0D9410EECC4F(u64_obj *self, const u64_v *args, int argc) {
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
  L_59:; U64_CALL(self, "ExecuteUbergraph_UI_HUD", u64_int(10));
  L_74:; 
  L_75:; goto u64_ret;
  L_77:; 
  u64_ret:; u64_set_return(L.ReturnValue);

}

void bp_UI_HUD__SetHealth(u64_obj *self, const u64_v *args, int argc) {
  (void)args; (void)argc;
  struct { u64_v CallFunc_Conv_FloatToText_ReturnValue; u64_v CallFunc_Conv_FloatToText_ReturnValue_1; u64_v CallFunc_Divide_FloatFloat_ReturnValue; u64_v CallFunc_FMax_ReturnValue; u64_v CallFunc_Format_ReturnValue; u64_v K2Node_MakeArray_Array; u64_v K2Node_MakeStruct_FormatArgumentData; u64_v K2Node_MakeStruct_FormatArgumentData_1; u64_v ReturnValue; u64_v _unsupported; } L; memset(&L, 0, sizeof L); (void)L;
  U64_FLOW_DECL; (void)flow_n;
  L_0:; 
  L_1:; 
  L_2:; L.CallFunc_Conv_FloatToText_ReturnValue = U64_CALL(self, "Conv_FloatToText", (*u64_ivar(self, "MaxHealth")), u64_int(0), u64_bool(0), u64_bool(1), u64_int(1), u64_int(4), u64_int(0), u64_int(0));
  L_63:; L.CallFunc_Conv_FloatToText_ReturnValue_1 = U64_CALL(self, "Conv_FloatToText", (*u64_ivar(self, "CurrentHealth")), u64_int(0), u64_bool(0), u64_bool(1), u64_int(1), u64_int(4), u64_int(0), u64_int(0));
  L_124:; (*u64_member(&L.K2Node_MakeStruct_FormatArgumentData, "ArgumentName")) = u64_str("M");
  L_154:; (*u64_member(&L.K2Node_MakeStruct_FormatArgumentData, "ArgumentValueType")) = u64_int(4);
  L_183:; (*u64_member(&L.K2Node_MakeStruct_FormatArgumentData, "ArgumentValue")) = L.CallFunc_Conv_FloatToText_ReturnValue;
  L_219:; (*u64_member(&L.K2Node_MakeStruct_FormatArgumentData, "ArgumentValueInt")) = u64_int(0);
  L_251:; (*u64_member(&L.K2Node_MakeStruct_FormatArgumentData, "ArgumentValueFloat")) = u64_float(0.0f);
  L_283:; (*u64_member(&L.K2Node_MakeStruct_FormatArgumentData, "ArgumentValueGender")) = u64_int(0);
  L_312:; (*u64_member(&L.K2Node_MakeStruct_FormatArgumentData_1, "ArgumentName")) = u64_str("C");
  L_342:; (*u64_member(&L.K2Node_MakeStruct_FormatArgumentData_1, "ArgumentValueType")) = u64_int(4);
  L_371:; (*u64_member(&L.K2Node_MakeStruct_FormatArgumentData_1, "ArgumentValue")) = L.CallFunc_Conv_FloatToText_ReturnValue_1;
  L_407:; (*u64_member(&L.K2Node_MakeStruct_FormatArgumentData_1, "ArgumentValueInt")) = u64_int(0);
  L_439:; (*u64_member(&L.K2Node_MakeStruct_FormatArgumentData_1, "ArgumentValueFloat")) = u64_float(0.0f);
  L_471:; (*u64_member(&L.K2Node_MakeStruct_FormatArgumentData_1, "ArgumentValueGender")) = u64_int(0);
  L_500:; u64_array_set(&L.K2Node_MakeArray_Array, 2, (u64_v[]){u64_int(0), L.K2Node_MakeStruct_FormatArgumentData_1, L.K2Node_MakeStruct_FormatArgumentData} + 1);
  L_529:; L.CallFunc_Format_ReturnValue = U64_CALL(self, "Format", u64_str(""), L.K2Node_MakeArray_Array);
  L_615:; 
  L_616:; ({ u64_v _o = (*u64_ivar(self, "TextBlock_Health")); U64_CALL(u64_asobj(_o), "SetText", L.CallFunc_Format_ReturnValue); });
  L_661:; 
  L_662:; L.CallFunc_FMax_ReturnValue = U64_CALL(self, "FMax", (*u64_ivar(self, "MaxHealth")), u64_float(1.0f));
  L_704:; L.CallFunc_Divide_FloatFloat_ReturnValue = U64_CALL(self, "Divide_FloatFloat", (*u64_ivar(self, "CurrentHealth")), L.CallFunc_FMax_ReturnValue);
  L_750:; 
  L_751:; U64_CALL(self, "SetHealthPercentage", L.CallFunc_Divide_FloatFloat_ReturnValue);
  L_774:; 
  L_775:; goto u64_ret;
  L_777:; 
  u64_ret:; u64_set_return(L.ReturnValue);

}

void bp_UI_HUD__SetMana(u64_obj *self, const u64_v *args, int argc) {
  (void)args; (void)argc;
  struct { u64_v CallFunc_Conv_FloatToText_ReturnValue; u64_v CallFunc_Conv_FloatToText_ReturnValue_1; u64_v CallFunc_Divide_FloatFloat_ReturnValue; u64_v CallFunc_FMax_ReturnValue; u64_v CallFunc_Format_ReturnValue; u64_v K2Node_MakeArray_Array; u64_v K2Node_MakeStruct_FormatArgumentData; u64_v K2Node_MakeStruct_FormatArgumentData_1; u64_v ReturnValue; u64_v _unsupported; } L; memset(&L, 0, sizeof L); (void)L;
  U64_FLOW_DECL; (void)flow_n;
  L_0:; 
  L_1:; 
  L_2:; L.CallFunc_Conv_FloatToText_ReturnValue = U64_CALL(self, "Conv_FloatToText", (*u64_ivar(self, "MaxMana")), u64_int(0), u64_bool(0), u64_bool(1), u64_int(1), u64_int(4), u64_int(0), u64_int(0));
  L_63:; L.CallFunc_Conv_FloatToText_ReturnValue_1 = U64_CALL(self, "Conv_FloatToText", (*u64_ivar(self, "CurrentMana")), u64_int(0), u64_bool(0), u64_bool(1), u64_int(1), u64_int(4), u64_int(0), u64_int(0));
  L_124:; (*u64_member(&L.K2Node_MakeStruct_FormatArgumentData, "ArgumentName")) = u64_str("M");
  L_154:; (*u64_member(&L.K2Node_MakeStruct_FormatArgumentData, "ArgumentValueType")) = u64_int(4);
  L_183:; (*u64_member(&L.K2Node_MakeStruct_FormatArgumentData, "ArgumentValue")) = L.CallFunc_Conv_FloatToText_ReturnValue;
  L_219:; (*u64_member(&L.K2Node_MakeStruct_FormatArgumentData, "ArgumentValueInt")) = u64_int(0);
  L_251:; (*u64_member(&L.K2Node_MakeStruct_FormatArgumentData, "ArgumentValueFloat")) = u64_float(0.0f);
  L_283:; (*u64_member(&L.K2Node_MakeStruct_FormatArgumentData, "ArgumentValueGender")) = u64_int(0);
  L_312:; (*u64_member(&L.K2Node_MakeStruct_FormatArgumentData_1, "ArgumentName")) = u64_str("C");
  L_342:; (*u64_member(&L.K2Node_MakeStruct_FormatArgumentData_1, "ArgumentValueType")) = u64_int(4);
  L_371:; (*u64_member(&L.K2Node_MakeStruct_FormatArgumentData_1, "ArgumentValue")) = L.CallFunc_Conv_FloatToText_ReturnValue_1;
  L_407:; (*u64_member(&L.K2Node_MakeStruct_FormatArgumentData_1, "ArgumentValueInt")) = u64_int(0);
  L_439:; (*u64_member(&L.K2Node_MakeStruct_FormatArgumentData_1, "ArgumentValueFloat")) = u64_float(0.0f);
  L_471:; (*u64_member(&L.K2Node_MakeStruct_FormatArgumentData_1, "ArgumentValueGender")) = u64_int(0);
  L_500:; u64_array_set(&L.K2Node_MakeArray_Array, 2, (u64_v[]){u64_int(0), L.K2Node_MakeStruct_FormatArgumentData_1, L.K2Node_MakeStruct_FormatArgumentData} + 1);
  L_529:; L.CallFunc_Format_ReturnValue = U64_CALL(self, "Format", u64_str(""), L.K2Node_MakeArray_Array);
  L_615:; 
  L_616:; ({ u64_v _o = (*u64_ivar(self, "TextBlock_Mana")); U64_CALL(u64_asobj(_o), "SetText", L.CallFunc_Format_ReturnValue); });
  L_661:; 
  L_662:; L.CallFunc_FMax_ReturnValue = U64_CALL(self, "FMax", (*u64_ivar(self, "MaxMana")), u64_float(1.0f));
  L_704:; L.CallFunc_Divide_FloatFloat_ReturnValue = U64_CALL(self, "Divide_FloatFloat", (*u64_ivar(self, "CurrentMana")), L.CallFunc_FMax_ReturnValue);
  L_750:; 
  L_751:; U64_CALL(self, "SetManaPercentage", L.CallFunc_Divide_FloatFloat_ReturnValue);
  L_774:; 
  L_775:; goto u64_ret;
  L_777:; 
  u64_ret:; u64_set_return(L.ReturnValue);

}

void bp_UI_HUD__SetStamina(u64_obj *self, const u64_v *args, int argc) {
  (void)args; (void)argc;
  struct { u64_v CallFunc_Conv_FloatToText_ReturnValue; u64_v CallFunc_Conv_FloatToText_ReturnValue_1; u64_v CallFunc_Divide_FloatFloat_ReturnValue; u64_v CallFunc_FMax_ReturnValue; u64_v CallFunc_Format_ReturnValue; u64_v K2Node_MakeArray_Array; u64_v K2Node_MakeStruct_FormatArgumentData; u64_v K2Node_MakeStruct_FormatArgumentData_1; u64_v ReturnValue; u64_v _unsupported; } L; memset(&L, 0, sizeof L); (void)L;
  U64_FLOW_DECL; (void)flow_n;
  L_0:; 
  L_1:; 
  L_2:; L.CallFunc_Conv_FloatToText_ReturnValue = U64_CALL(self, "Conv_FloatToText", (*u64_ivar(self, "MaxStamina")), u64_int(0), u64_bool(0), u64_bool(1), u64_int(1), u64_int(4), u64_int(0), u64_int(0));
  L_63:; L.CallFunc_Conv_FloatToText_ReturnValue_1 = U64_CALL(self, "Conv_FloatToText", (*u64_ivar(self, "Current_Stamina")), u64_int(0), u64_bool(0), u64_bool(1), u64_int(1), u64_int(4), u64_int(0), u64_int(0));
  L_124:; (*u64_member(&L.K2Node_MakeStruct_FormatArgumentData, "ArgumentName")) = u64_str("M");
  L_154:; (*u64_member(&L.K2Node_MakeStruct_FormatArgumentData, "ArgumentValueType")) = u64_int(4);
  L_183:; (*u64_member(&L.K2Node_MakeStruct_FormatArgumentData, "ArgumentValue")) = L.CallFunc_Conv_FloatToText_ReturnValue;
  L_219:; (*u64_member(&L.K2Node_MakeStruct_FormatArgumentData, "ArgumentValueInt")) = u64_int(0);
  L_251:; (*u64_member(&L.K2Node_MakeStruct_FormatArgumentData, "ArgumentValueFloat")) = u64_float(0.0f);
  L_283:; (*u64_member(&L.K2Node_MakeStruct_FormatArgumentData, "ArgumentValueGender")) = u64_int(0);
  L_312:; (*u64_member(&L.K2Node_MakeStruct_FormatArgumentData_1, "ArgumentName")) = u64_str("C");
  L_342:; (*u64_member(&L.K2Node_MakeStruct_FormatArgumentData_1, "ArgumentValueType")) = u64_int(4);
  L_371:; (*u64_member(&L.K2Node_MakeStruct_FormatArgumentData_1, "ArgumentValue")) = L.CallFunc_Conv_FloatToText_ReturnValue_1;
  L_407:; (*u64_member(&L.K2Node_MakeStruct_FormatArgumentData_1, "ArgumentValueInt")) = u64_int(0);
  L_439:; (*u64_member(&L.K2Node_MakeStruct_FormatArgumentData_1, "ArgumentValueFloat")) = u64_float(0.0f);
  L_471:; (*u64_member(&L.K2Node_MakeStruct_FormatArgumentData_1, "ArgumentValueGender")) = u64_int(0);
  L_500:; u64_array_set(&L.K2Node_MakeArray_Array, 2, (u64_v[]){u64_int(0), L.K2Node_MakeStruct_FormatArgumentData_1, L.K2Node_MakeStruct_FormatArgumentData} + 1);
  L_529:; L.CallFunc_Format_ReturnValue = U64_CALL(self, "Format", u64_str(""), L.K2Node_MakeArray_Array);
  L_615:; 
  L_616:; ({ u64_v _o = (*u64_ivar(self, "TextBlock_Stamina")); U64_CALL(u64_asobj(_o), "SetText", L.CallFunc_Format_ReturnValue); });
  L_661:; 
  L_662:; L.CallFunc_FMax_ReturnValue = U64_CALL(self, "FMax", (*u64_ivar(self, "MaxStamina")), u64_float(1.0f));
  L_704:; L.CallFunc_Divide_FloatFloat_ReturnValue = U64_CALL(self, "Divide_FloatFloat", (*u64_ivar(self, "Current_Stamina")), L.CallFunc_FMax_ReturnValue);
  L_750:; 
  L_751:; U64_CALL(self, "SetStaminaPercentage", L.CallFunc_Divide_FloatFloat_ReturnValue);
  L_774:; 
  L_775:; goto u64_ret;
  L_777:; 
  u64_ret:; u64_set_return(L.ReturnValue);

}

void bp_UI_HUD__SetShield(u64_obj *self, const u64_v *args, int argc) {
  (void)args; (void)argc;
  struct { u64_v CallFunc_Conv_FloatToText_ReturnValue; u64_v CallFunc_Conv_FloatToText_ReturnValue_1; u64_v CallFunc_Divide_FloatFloat_ReturnValue; u64_v CallFunc_FMax_ReturnValue; u64_v CallFunc_Format_ReturnValue; u64_v K2Node_MakeArray_Array; u64_v K2Node_MakeStruct_FormatArgumentData; u64_v K2Node_MakeStruct_FormatArgumentData_1; u64_v ReturnValue; u64_v _unsupported; } L; memset(&L, 0, sizeof L); (void)L;
  U64_FLOW_DECL; (void)flow_n;
  L_0:; 
  L_1:; 
  L_2:; L.CallFunc_Conv_FloatToText_ReturnValue = U64_CALL(self, "Conv_FloatToText", (*u64_ivar(self, "MaxShield")), u64_int(0), u64_bool(0), u64_bool(1), u64_int(1), u64_int(4), u64_int(0), u64_int(0));
  L_63:; L.CallFunc_Conv_FloatToText_ReturnValue_1 = U64_CALL(self, "Conv_FloatToText", (*u64_ivar(self, "CurrentShield")), u64_int(0), u64_bool(0), u64_bool(1), u64_int(1), u64_int(4), u64_int(0), u64_int(0));
  L_124:; (*u64_member(&L.K2Node_MakeStruct_FormatArgumentData, "ArgumentName")) = u64_str("M");
  L_154:; (*u64_member(&L.K2Node_MakeStruct_FormatArgumentData, "ArgumentValueType")) = u64_int(4);
  L_183:; (*u64_member(&L.K2Node_MakeStruct_FormatArgumentData, "ArgumentValue")) = L.CallFunc_Conv_FloatToText_ReturnValue;
  L_219:; (*u64_member(&L.K2Node_MakeStruct_FormatArgumentData, "ArgumentValueInt")) = u64_int(0);
  L_251:; (*u64_member(&L.K2Node_MakeStruct_FormatArgumentData, "ArgumentValueFloat")) = u64_float(0.0f);
  L_283:; (*u64_member(&L.K2Node_MakeStruct_FormatArgumentData, "ArgumentValueGender")) = u64_int(0);
  L_312:; (*u64_member(&L.K2Node_MakeStruct_FormatArgumentData_1, "ArgumentName")) = u64_str("C");
  L_342:; (*u64_member(&L.K2Node_MakeStruct_FormatArgumentData_1, "ArgumentValueType")) = u64_int(4);
  L_371:; (*u64_member(&L.K2Node_MakeStruct_FormatArgumentData_1, "ArgumentValue")) = L.CallFunc_Conv_FloatToText_ReturnValue_1;
  L_407:; (*u64_member(&L.K2Node_MakeStruct_FormatArgumentData_1, "ArgumentValueInt")) = u64_int(0);
  L_439:; (*u64_member(&L.K2Node_MakeStruct_FormatArgumentData_1, "ArgumentValueFloat")) = u64_float(0.0f);
  L_471:; (*u64_member(&L.K2Node_MakeStruct_FormatArgumentData_1, "ArgumentValueGender")) = u64_int(0);
  L_500:; u64_array_set(&L.K2Node_MakeArray_Array, 2, (u64_v[]){u64_int(0), L.K2Node_MakeStruct_FormatArgumentData_1, L.K2Node_MakeStruct_FormatArgumentData} + 1);
  L_529:; L.CallFunc_Format_ReturnValue = U64_CALL(self, "Format", u64_str(""), L.K2Node_MakeArray_Array);
  L_615:; 
  L_616:; ({ u64_v _o = (*u64_ivar(self, "TextBlock_Shield")); U64_CALL(u64_asobj(_o), "SetText", L.CallFunc_Format_ReturnValue); });
  L_661:; 
  L_662:; L.CallFunc_FMax_ReturnValue = U64_CALL(self, "FMax", (*u64_ivar(self, "MaxShield")), u64_float(1.0f));
  L_704:; L.CallFunc_Divide_FloatFloat_ReturnValue = U64_CALL(self, "Divide_FloatFloat", (*u64_ivar(self, "CurrentShield")), L.CallFunc_FMax_ReturnValue);
  L_750:; 
  L_751:; U64_CALL(self, "SetShieldPercentage", L.CallFunc_Divide_FloatFloat_ReturnValue);
  L_774:; 
  L_775:; goto u64_ret;
  L_777:; 
  u64_ret:; u64_set_return(L.ReturnValue);

}
