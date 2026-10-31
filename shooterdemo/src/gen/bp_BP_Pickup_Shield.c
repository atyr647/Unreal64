#include "u64_rt.h"

void bp_BP_Pickup_Shield__ExecuteUbergraph_BP_Pickup_Shield(u64_obj *self, const u64_v *args, int argc) {
  (void)args; (void)argc;
  struct { u64_v ReturnValue; u64_v _unsupported; } L; memset(&L, 0, sizeof L); (void)L;
  U64_FLOW_DECL; (void)flow_n;
  if (argc > 0) (*u64_pframe(self, "EntryPoint")) = args[0];
  L_0:; u64_dispatch: switch (u64_as_int((*u64_pframe(self, "EntryPoint")))) { case 0: goto L_0; case 10: goto L_10; case 11: goto L_11; case 47: goto L_47; case 48: goto L_48; case 53: goto L_53; case 54: goto L_54; case 91: goto L_91; case 92: goto L_92; case 97: goto L_97; case 98: goto L_98; case 99: goto L_99; case 104: goto L_104; case 105: goto L_105; case 106: goto L_106; case 111: goto L_111; case 113: goto L_113; }
  L_10:; 
  L_11:; ({ u64_v _o = (*u64_ivar(self, "P_Shield")); U64_CALL(u64_asobj(_o), "Deactivate"); });
  L_47:; 
  L_48:; goto L_111;
  L_53:; 
  L_54:; ({ u64_v _o = (*u64_ivar(self, "P_Shield")); U64_CALL(u64_asobj(_o), "Activate", u64_bool(1)); });
  L_91:; 
  L_92:; goto L_111;
  L_97:; 
  L_98:; 
  L_99:; goto L_10;
  L_104:; 
  L_105:; 
  L_106:; goto L_53;
  L_111:; goto u64_ret;
  L_113:; 
  u64_ret:; u64_set_return(L.ReturnValue);

}

void bp_BP_Pickup_Shield__K2_OnRespawned(u64_obj *self, const u64_v *args, int argc) {
  (void)args; (void)argc;
  struct { u64_v ReturnValue; u64_v _unsupported; } L; memset(&L, 0, sizeof L); (void)L;
  U64_FLOW_DECL; (void)flow_n;
  L_0:; 
  L_1:; 
  L_2:; 
  L_3:; U64_CALL(self, "ExecuteUbergraph_BP_Pickup_Shield", u64_int(104));
  L_18:; 
  L_19:; goto u64_ret;
  L_21:; 
  u64_ret:; u64_set_return(L.ReturnValue);

}

void bp_BP_Pickup_Shield__K2_OnPickedUp(u64_obj *self, const u64_v *args, int argc) {
  (void)args; (void)argc;
  struct { u64_v ReturnValue; u64_v _unsupported; } L; memset(&L, 0, sizeof L); (void)L;
  U64_FLOW_DECL; (void)flow_n;
  L_0:; 
  L_1:; 
  L_2:; 
  L_3:; U64_CALL(self, "ExecuteUbergraph_BP_Pickup_Shield", u64_int(97));
  L_18:; 
  L_19:; goto u64_ret;
  L_21:; 
  u64_ret:; u64_set_return(L.ReturnValue);

}

void bp_BP_Pickup_Shield__K2_CanBePickedUp(u64_obj *self, const u64_v *args, int argc) {
  (void)args; (void)argc;
  struct { u64_v CallFunc_GetMaxShield_ReturnValue; u64_v CallFunc_GetShield_ReturnValue; u64_v CallFunc_Less_FloatFloat_ReturnValue; u64_v ReturnValue; u64_v TestCharacter; u64_v _unsupported; } L; memset(&L, 0, sizeof L); (void)L;
  U64_FLOW_DECL; (void)flow_n;
  if (argc > 0) L.TestCharacter = args[0];
  L_0:; 
  L_1:; 
  L_2:; L.CallFunc_GetShield_ReturnValue = ({ u64_v _o = L.TestCharacter; U64_CALL(u64_asobj(_o), "GetShield"); });
  L_52:; L.CallFunc_GetMaxShield_ReturnValue = ({ u64_v _o = L.TestCharacter; U64_CALL(u64_asobj(_o), "GetMaxShield"); });
  L_102:; L.CallFunc_Less_FloatFloat_ReturnValue = U64_CALL(self, "Less_FloatFloat", L.CallFunc_GetShield_ReturnValue, L.CallFunc_GetMaxShield_ReturnValue);
  L_140:; 
  L_141:; L.ReturnValue = L.CallFunc_Less_FloatFloat_ReturnValue;
  L_160:; 
  L_161:; L.ReturnValue = L.ReturnValue; goto u64_ret;
  L_171:; 
  u64_ret:; u64_set_return(L.ReturnValue);

}
