#include "u64_rt.h"

void bp_GC_KnockedDown__OnExecute(u64_obj *self, const u64_v *args, int argc) {
  (void)args; (void)argc;
  struct { u64_v CallFunc_BreakGameplayCueParameters_AbilityLevel; u64_v CallFunc_BreakGameplayCueParameters_AggregatedSourceTags; u64_v CallFunc_BreakGameplayCueParameters_AggregatedTargetTags; u64_v CallFunc_BreakGameplayCueParameters_EffectCauser; u64_v CallFunc_BreakGameplayCueParameters_EffectContext; u64_v CallFunc_BreakGameplayCueParameters_GameplayEffectLevel; u64_v CallFunc_BreakGameplayCueParameters_Instigator; u64_v CallFunc_BreakGameplayCueParameters_Location; u64_v CallFunc_BreakGameplayCueParameters_MatchedTagName; u64_v CallFunc_BreakGameplayCueParameters_Normal; u64_v CallFunc_BreakGameplayCueParameters_NormalizedMagnitude; u64_v CallFunc_BreakGameplayCueParameters_OriginalTag; u64_v CallFunc_BreakGameplayCueParameters_PhysicalMaterial; u64_v CallFunc_BreakGameplayCueParameters_RawMagnitude; u64_v CallFunc_BreakGameplayCueParameters_SourceObject; u64_v CallFunc_BreakGameplayCueParameters_TargetAttachComponent; u64_v CallFunc_OnExecute_ReturnValue; u64_v MyTarget; u64_v Parameters; u64_v ReturnValue; u64_v _unsupported; } L; memset(&L, 0, sizeof L); (void)L;
  U64_FLOW_DECL; (void)flow_n;
  if (argc > 0) L.MyTarget = args[0];
  if (argc > 1) L.Parameters = args[1].t == U64_REF ? *args[1].u.r : args[1];
  L_0:; 
  L_1:; 
  L_2:; U64_CALL(self, "BreakGameplayCueParameters", u64_ref(&L.Parameters), u64_ref(&L.CallFunc_BreakGameplayCueParameters_NormalizedMagnitude), u64_ref(&L.CallFunc_BreakGameplayCueParameters_RawMagnitude), u64_ref(&L.CallFunc_BreakGameplayCueParameters_EffectContext), u64_ref(&L.CallFunc_BreakGameplayCueParameters_MatchedTagName), u64_ref(&L.CallFunc_BreakGameplayCueParameters_OriginalTag), u64_ref(&L.CallFunc_BreakGameplayCueParameters_AggregatedSourceTags), u64_ref(&L.CallFunc_BreakGameplayCueParameters_AggregatedTargetTags), u64_ref(&L.CallFunc_BreakGameplayCueParameters_Location), u64_ref(&L.CallFunc_BreakGameplayCueParameters_Normal), u64_ref(&L.CallFunc_BreakGameplayCueParameters_Instigator), u64_ref(&L.CallFunc_BreakGameplayCueParameters_EffectCauser), u64_ref(&L.CallFunc_BreakGameplayCueParameters_SourceObject), u64_ref(&L.CallFunc_BreakGameplayCueParameters_PhysicalMaterial), u64_ref(&L.CallFunc_BreakGameplayCueParameters_GameplayEffectLevel), u64_ref(&L.CallFunc_BreakGameplayCueParameters_AbilityLevel), u64_ref(&L.CallFunc_BreakGameplayCueParameters_TargetAttachComponent));
  L_165:; 
  L_166:; U64_CALL(self, "PlaySoundAtLocation", L.MyTarget, (*u64_ivar(self, "KnockedDownSound")), L.CallFunc_BreakGameplayCueParameters_Location, u64_vec(0.0f, 0.0f, 0.0f), u64_float(1.0f), u64_float(1.0f), u64_float(0.0f), u64_objv(0), u64_objv(0), u64_objv(0));
  L_234:; 
  L_235:; L.CallFunc_OnExecute_ReturnValue = U64_CALL_SUPER("GC_KnockedDown", "OnExecute", L.MyTarget, u64_ref(&L.Parameters));
  L_273:; 
  L_274:; L.ReturnValue = L.CallFunc_OnExecute_ReturnValue;
  L_293:; 
  L_294:; L.ReturnValue = L.ReturnValue; goto u64_ret;
  L_304:; 
  u64_ret:; u64_set_return(L.ReturnValue);
  if (argc > 1) u64_out(args[1], L.Parameters);
}
