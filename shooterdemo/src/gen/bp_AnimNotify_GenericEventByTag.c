#include "u64_rt.h"

void bp_AnimNotify_GenericEventByTag__GetNotifyName(u64_obj *self, const u64_v *args, int argc) {
  (void)args; (void)argc;
  struct { u64_v CallFunc_Conv_NameToString_ReturnValue; u64_v CallFunc_GetTagName_ReturnValue; u64_v ReturnValue; u64_v _unsupported; } L; memset(&L, 0, sizeof L); (void)L;
  U64_FLOW_DECL; (void)flow_n;
  L_0:; 
  L_1:; 
  L_2:; L.CallFunc_GetTagName_ReturnValue = U64_CALL(self, "GetTagName", (*u64_ivar(self, "EventTag")));
  L_39:; L.CallFunc_Conv_NameToString_ReturnValue = U64_CALL(self, "Conv_NameToString", L.CallFunc_GetTagName_ReturnValue);
  L_76:; 
  L_77:; L.ReturnValue = L.CallFunc_Conv_NameToString_ReturnValue;
  L_104:; 
  L_105:; L.ReturnValue = L.ReturnValue; goto u64_ret;
  L_115:; 
  u64_ret:; u64_set_return(L.ReturnValue);

}

void bp_AnimNotify_GenericEventByTag__Received_Notify(u64_obj *self, const u64_v *args, int argc) {
  (void)args; (void)argc;
  struct { u64_v Animation; u64_v CallFunc_GetOwner_ReturnValue; u64_v MeshComp; u64_v ReturnValue; u64_v _unsupported; } L; memset(&L, 0, sizeof L); (void)L;
  U64_FLOW_DECL; (void)flow_n;
  if (argc > 0) L.MeshComp = args[0];
  if (argc > 1) L.Animation = args[1];
  L_0:; 
  L_1:; 
  L_2:; L.CallFunc_GetOwner_ReturnValue = ({ u64_v _o = L.MeshComp; U64_CALL(u64_asobj(_o), "GetOwner"); });
  L_44:; 
  L_45:; U64_CALL(self, "SendGameplayEventToActor", L.CallFunc_GetOwner_ReturnValue, (*u64_ivar(self, "EventTag")), u64_int(0) /* struct const */);
  L_207:; 
  L_208:; 
  L_209:; L.ReturnValue = u64_bool(0);
  L_220:; 
  L_221:; L.ReturnValue = L.ReturnValue; goto u64_ret;
  L_231:; 
  u64_ret:; u64_set_return(L.ReturnValue);

}
