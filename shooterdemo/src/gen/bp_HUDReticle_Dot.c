#include "u64_rt.h"

void bp_HUDReticle_Dot__ExecuteUbergraph_HUDReticle_Dot(u64_obj *self, const u64_v *args, int argc) {
  (void)args; (void)argc;
  struct { u64_v ReturnValue; u64_v _unsupported; } L; memset(&L, 0, sizeof L); (void)L;
  U64_FLOW_DECL; (void)flow_n;
  if (argc > 0) (*u64_pframe(self, "EntryPoint")) = args[0];
  L_0:; u64_dispatch: switch (u64_as_int((*u64_pframe(self, "EntryPoint")))) { case 0: goto L_0; case 10: goto L_10; case 11: goto L_11; case 12: goto L_12; case 48: goto L_48; case 77: goto L_77; case 78: goto L_78; case 119: goto L_119; case 120: goto L_120; case 122: goto L_122; }
  L_10:; 
  L_11:; 
  L_12:; (*u64_member(&(*u64_pframe(self, "K2Node_MakeStruct_SlateColor")), "SpecifiedColor")) = (*u64_pframe(self, "K2Node_Event_Color"));
  L_48:; (*u64_member(&(*u64_pframe(self, "K2Node_MakeStruct_SlateColor")), "ColorUseRule")) = u64_int(0);
  L_77:; 
  L_78:; ({ u64_v _o = (*u64_ivar(self, "Image_11")); U64_CALL(u64_asobj(_o), "SetBrushTintColor", (*u64_pframe(self, "K2Node_MakeStruct_SlateColor"))); });
  L_119:; 
  L_120:; goto u64_ret;
  L_122:; 
  u64_ret:; u64_set_return(L.ReturnValue);

}

void bp_HUDReticle_Dot__SetColor(u64_obj *self, const u64_v *args, int argc) {
  (void)args; (void)argc;
  struct { u64_v Color; u64_v ReturnValue; u64_v _unsupported; } L; memset(&L, 0, sizeof L); (void)L;
  U64_FLOW_DECL; (void)flow_n;
  if (argc > 0) L.Color = args[0];
  L_0:; 
  L_1:; 
  L_2:; 
  L_3:; *u64_pframe(self, "K2Node_Event_Color") = L.Color;
  L_21:; 
  L_22:; 
  L_23:; U64_CALL(self, "ExecuteUbergraph_HUDReticle_Dot", u64_int(10));
  L_38:; 
  L_39:; goto u64_ret;
  L_41:; 
  u64_ret:; u64_set_return(L.ReturnValue);

}
