#include "u64_rt.h"

void bp_UI_DamageNumber__ExecuteUbergraph_UI_DamageNumber(u64_obj *self, const u64_v *args, int argc) {
  (void)args; (void)argc;
  struct { u64_v ReturnValue; u64_v _unsupported; } L; memset(&L, 0, sizeof L); (void)L;
  U64_FLOW_DECL; (void)flow_n;
  if (argc > 0) (*u64_pframe(self, "EntryPoint")) = args[0];
  L_0:; u64_dispatch: switch (u64_as_int((*u64_pframe(self, "EntryPoint")))) { case 0: goto L_0; case 10: goto L_10; case 11: goto L_11; case 12: goto L_12; case 13: goto L_13; case 58: goto L_58; case 59: goto L_59; case 116: goto L_116; case 117: goto L_117; case 118: goto L_118; case 132: goto L_132; case 133: goto L_133; case 169: goto L_169; case 198: goto L_198; case 199: goto L_199; case 240: goto L_240; case 241: goto L_241; case 300: goto L_300; case 359: goto L_359; case 426: goto L_426; case 493: goto L_493; case 529: goto L_529; case 530: goto L_530; case 571: goto L_571; case 572: goto L_572; case 577: goto L_577; case 613: goto L_613; case 642: goto L_642; case 643: goto L_643; case 684: goto L_684; case 685: goto L_685; case 744: goto L_744; case 803: goto L_803; case 870: goto L_870; case 937: goto L_937; case 973: goto L_973; case 974: goto L_974; case 1015: goto L_1015; case 1016: goto L_1016; case 1021: goto L_1021; case 1022: goto L_1022; case 1023: goto L_1023; case 1024: goto L_1024; case 1071: goto L_1071; case 1072: goto L_1072; case 1074: goto L_1074; }
  L_10:; 
  L_11:; 
  L_12:; 
  L_13:; ({ u64_v _o = (*u64_ivar(self, "Damage")); U64_CALL(u64_asobj(_o), "SetText", (*u64_pframe(self, "K2Node_CustomEvent_InText"))); });
  L_58:; 
  L_59:; (*u64_pframe(self, "CallFunc_HasTag_ReturnValue")) = U64_CALL(self, "HasTag", (*u64_pframe(self, "K2Node_CustomEvent_Tags")), u64_name("Effect.Damage.HeadShot"), u64_bool(1));
  L_116:; 
  L_117:; 
  L_118:; if (!u64_true((*u64_pframe(self, "CallFunc_HasTag_ReturnValue")))) goto L_577;
  L_132:; 
  L_133:; (*u64_member(&(*u64_pframe(self, "K2Node_MakeStruct_SlateColor")), "SpecifiedColor")) = (*u64_ivar(self, "HeadShotColor"));
  L_169:; (*u64_member(&(*u64_pframe(self, "K2Node_MakeStruct_SlateColor")), "ColorUseRule")) = u64_int(0);
  L_198:; 
  L_199:; ({ u64_v _o = (*u64_ivar(self, "Damage")); U64_CALL(u64_asobj(_o), "SetColorAndOpacity", (*u64_pframe(self, "K2Node_MakeStruct_SlateColor"))); });
  L_240:; 
  L_241:; (*u64_member(&(*u64_pframe(self, "K2Node_MakeStruct_SlateFontInfo_1")), "FontObject")) = (*u64_member(&(*u64_ivar(u64_asobj((*u64_ivar(self, "Damage"))), "Font")), "FontObject"));
  L_300:; (*u64_member(&(*u64_pframe(self, "K2Node_MakeStruct_SlateFontInfo_1")), "FontMaterial")) = (*u64_member(&(*u64_ivar(u64_asobj((*u64_ivar(self, "Damage"))), "Font")), "FontMaterial"));
  L_359:; (*u64_member(&(*u64_pframe(self, "K2Node_MakeStruct_SlateFontInfo_1")), "OutlineSettings")) = (*u64_member(&(*u64_ivar(u64_asobj((*u64_ivar(self, "Damage"))), "Font")), "OutlineSettings"));
  L_426:; (*u64_member(&(*u64_pframe(self, "K2Node_MakeStruct_SlateFontInfo_1")), "TypefaceFontName")) = (*u64_member(&(*u64_ivar(u64_asobj((*u64_ivar(self, "Damage"))), "Font")), "TypefaceFontName"));
  L_493:; (*u64_member(&(*u64_pframe(self, "K2Node_MakeStruct_SlateFontInfo_1")), "Size")) = (*u64_ivar(self, "HeadShotFontSize"));
  L_529:; 
  L_530:; ({ u64_v _o = (*u64_ivar(self, "Damage")); U64_CALL(u64_asobj(_o), "SetFont", (*u64_pframe(self, "K2Node_MakeStruct_SlateFontInfo_1"))); });
  L_571:; 
  L_572:; goto L_1072;
  L_577:; (*u64_member(&(*u64_pframe(self, "K2Node_MakeStruct_SlateColor_1")), "SpecifiedColor")) = (*u64_ivar(self, "NormalColor"));
  L_613:; (*u64_member(&(*u64_pframe(self, "K2Node_MakeStruct_SlateColor_1")), "ColorUseRule")) = u64_int(0);
  L_642:; 
  L_643:; ({ u64_v _o = (*u64_ivar(self, "Damage")); U64_CALL(u64_asobj(_o), "SetColorAndOpacity", (*u64_pframe(self, "K2Node_MakeStruct_SlateColor_1"))); });
  L_684:; 
  L_685:; (*u64_member(&(*u64_pframe(self, "K2Node_MakeStruct_SlateFontInfo")), "FontObject")) = (*u64_member(&(*u64_ivar(u64_asobj((*u64_ivar(self, "Damage"))), "Font")), "FontObject"));
  L_744:; (*u64_member(&(*u64_pframe(self, "K2Node_MakeStruct_SlateFontInfo")), "FontMaterial")) = (*u64_member(&(*u64_ivar(u64_asobj((*u64_ivar(self, "Damage"))), "Font")), "FontMaterial"));
  L_803:; (*u64_member(&(*u64_pframe(self, "K2Node_MakeStruct_SlateFontInfo")), "OutlineSettings")) = (*u64_member(&(*u64_ivar(u64_asobj((*u64_ivar(self, "Damage"))), "Font")), "OutlineSettings"));
  L_870:; (*u64_member(&(*u64_pframe(self, "K2Node_MakeStruct_SlateFontInfo")), "TypefaceFontName")) = (*u64_member(&(*u64_ivar(u64_asobj((*u64_ivar(self, "Damage"))), "Font")), "TypefaceFontName"));
  L_937:; (*u64_member(&(*u64_pframe(self, "K2Node_MakeStruct_SlateFontInfo")), "Size")) = (*u64_ivar(self, "NormalFontSize"));
  L_973:; 
  L_974:; ({ u64_v _o = (*u64_ivar(self, "Damage")); U64_CALL(u64_asobj(_o), "SetFont", (*u64_pframe(self, "K2Node_MakeStruct_SlateFontInfo"))); });
  L_1015:; 
  L_1016:; goto L_1072;
  L_1021:; 
  L_1022:; 
  L_1023:; 
  L_1024:; (*u64_pframe(self, "CallFunc_PlayAnimation_ReturnValue")) = U64_CALL(self, "PlayAnimation", (*u64_ivar(self, "FadeAnimation")), u64_float(0.0f), u64_int(1), u64_int(0), u64_float(2.0f), u64_bool(0));
  L_1071:; 
  L_1072:; goto u64_ret;
  L_1074:; 
  u64_ret:; u64_set_return(L.ReturnValue);

}

void bp_UI_DamageNumber__Construct(u64_obj *self, const u64_v *args, int argc) {
  (void)args; (void)argc;
  struct { u64_v ReturnValue; u64_v _unsupported; } L; memset(&L, 0, sizeof L); (void)L;
  U64_FLOW_DECL; (void)flow_n;
  L_0:; 
  L_1:; 
  L_2:; 
  L_3:; U64_CALL(self, "ExecuteUbergraph_UI_DamageNumber", u64_int(1021));
  L_18:; 
  L_19:; goto u64_ret;
  L_21:; 
  u64_ret:; u64_set_return(L.ReturnValue);

}

void bp_UI_DamageNumber__UpdateDamage(u64_obj *self, const u64_v *args, int argc) {
  (void)args; (void)argc;
  struct { u64_v InText; u64_v ReturnValue; u64_v Tags; u64_v _unsupported; } L; memset(&L, 0, sizeof L); (void)L;
  U64_FLOW_DECL; (void)flow_n;
  if (argc > 0) L.InText = args[0];
  if (argc > 1) L.Tags = args[1];
  L_0:; 
  L_1:; 
  L_2:; 
  L_3:; *u64_pframe(self, "K2Node_CustomEvent_InText") = L.InText;
  L_21:; *u64_pframe(self, "K2Node_CustomEvent_Tags") = L.Tags;
  L_39:; 
  L_40:; 
  L_41:; U64_CALL(self, "ExecuteUbergraph_UI_DamageNumber", u64_int(10));
  L_56:; 
  L_57:; goto u64_ret;
  L_59:; 
  u64_ret:; u64_set_return(L.ReturnValue);

}
