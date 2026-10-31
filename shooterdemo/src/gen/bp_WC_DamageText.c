#include "u64_rt.h"

void bp_WC_DamageText__ExecuteUbergraph_WC_DamageText(u64_obj *self, const u64_v *args, int argc) {
  (void)args; (void)argc;
  struct { u64_v ReturnValue; u64_v _unsupported; } L; memset(&L, 0, sizeof L); (void)L;
  U64_FLOW_DECL; (void)flow_n;
  if (argc > 0) (*u64_pframe(self, "EntryPoint")) = args[0];
  L_0:; U64_PUSH(&&L_305);
  L_5:; u64_dispatch: switch (u64_as_int((*u64_pframe(self, "EntryPoint")))) { case 0: goto L_0; case 5: goto L_5; case 15: goto L_15; case 16: goto L_16; case 17: goto L_17; case 18: goto L_18; case 29: goto L_29; case 30: goto L_30; case 31: goto L_31; case 32: goto L_32; case 33: goto L_33; case 53: goto L_53; case 54: goto L_54; case 90: goto L_90; case 119: goto L_119; case 120: goto L_120; case 130: goto L_130; case 131: goto L_131; case 192: goto L_192; case 193: goto L_193; case 247: goto L_247; case 248: goto L_248; case 249: goto L_249; case 250: goto L_250; case 304: goto L_304; case 305: goto L_305; case 307: goto L_307; }
  L_15:; 
  L_16:; 
  L_17:; 
  L_18:; U64_CALL(self, "K2_DestroyComponent", u64_objv(self));
  L_29:; 
  L_30:; goto *U64_POP();
  L_31:; 
  L_32:; 
  L_33:; (*u64_pframe(self, "CallFunc_GetUserWidgetObject_ReturnValue")) = U64_CALL(self, "GetUserWidgetObject");
  L_53:; 
  L_54:; (*u64_pframe(self, "K2Node_DynamicCast_AsUI_Damage_Number")) = u64_cast((*u64_pframe(self, "CallFunc_GetUserWidgetObject_ReturnValue")), "UI_DamageNumber");
  L_90:; (*u64_pframe(self, "K2Node_DynamicCast_bSuccess")) = (*u64_pframe(self, "K2Node_DynamicCast_AsUI_Damage_Number"));
  L_119:; 
  L_120:; if (!u64_true((*u64_pframe(self, "K2Node_DynamicCast_bSuccess")))) goto *U64_POP();
  L_130:; 
  L_131:; (*u64_pframe(self, "CallFunc_Conv_FloatToText_ReturnValue")) = U64_CALL(self, "Conv_FloatToText", (*u64_pframe(self, "K2Node_Event_Damage")), u64_int(0), u64_bool(0), u64_bool(1), u64_int(1), u64_int(324), u64_int(0), u64_int(0));
  L_192:; 
  L_193:; ({ u64_v _o = (*u64_pframe(self, "K2Node_DynamicCast_AsUI_Damage_Number")); U64_CALL(u64_asobj(_o), "UpdateDamage", (*u64_pframe(self, "CallFunc_Conv_FloatToText_ReturnValue")), (*u64_pframe(self, "K2Node_Event_Tags"))); });
  L_247:; 
  L_248:; 
  L_249:; 
  L_250:; U64_CALL(self, "Delay", u64_objv(self), u64_float(1.0f), u64_latent(u64_objv(self), u64_name("ExecuteUbergraph_WC_DamageText").u.s, u64_int(15).u.i));
  L_304:; goto *U64_POP();
  L_305:; goto u64_ret;
  L_307:; 
  u64_ret:; u64_set_return(L.ReturnValue);

}

void bp_WC_DamageText__SetDamageText(u64_obj *self, const u64_v *args, int argc) {
  (void)args; (void)argc;
  struct { u64_v Damage; u64_v ReturnValue; u64_v Tags; u64_v _unsupported; } L; memset(&L, 0, sizeof L); (void)L;
  U64_FLOW_DECL; (void)flow_n;
  if (argc > 0) L.Damage = args[0];
  if (argc > 1) L.Tags = args[1].t == U64_REF ? *args[1].u.r : args[1];
  L_0:; 
  L_1:; 
  L_2:; 
  L_3:; *u64_pframe(self, "K2Node_Event_Damage") = L.Damage;
  L_21:; *u64_pframe(self, "K2Node_Event_Tags") = L.Tags;
  L_39:; 
  L_40:; 
  L_41:; U64_CALL(self, "ExecuteUbergraph_WC_DamageText", u64_int(31));
  L_56:; 
  L_57:; goto u64_ret;
  L_59:; 
  u64_ret:; u64_set_return(L.ReturnValue);
  if (argc > 1) u64_out(args[1], L.Tags);
}
