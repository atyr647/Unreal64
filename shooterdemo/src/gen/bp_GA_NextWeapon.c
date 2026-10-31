#include "u64_rt.h"

void bp_GA_NextWeapon__ExecuteUbergraph_GA_NextWeapon(u64_obj *self, const u64_v *args, int argc) {
  (void)args; (void)argc;
  struct { u64_v ReturnValue; u64_v _unsupported; } L; memset(&L, 0, sizeof L); (void)L;
  U64_FLOW_DECL; (void)flow_n;
  if (argc > 0) (*u64_pframe(self, "EntryPoint")) = args[0];
  L_0:; u64_dispatch: switch (u64_as_int((*u64_pframe(self, "EntryPoint")))) { case 0: goto L_0; case 10: goto L_10; case 11: goto L_11; case 12: goto L_12; case 32: goto L_32; case 33: goto L_33; case 69: goto L_69; case 98: goto L_98; case 99: goto L_99; case 113: goto L_113; case 114: goto L_114; case 115: goto L_115; case 134: goto L_134; case 135: goto L_135; case 136: goto L_136; case 172: goto L_172; case 173: goto L_173; case 174: goto L_174; case 221: goto L_221; case 222: goto L_222; case 223: goto L_223; case 237: goto L_237; case 238: goto L_238; case 240: goto L_240; }
  L_10:; 
  L_11:; 
  L_12:; (*u64_pframe(self, "CallFunc_GetAvatarActorFromActorInfo_ReturnValue")) = U64_CALL(self, "GetAvatarActorFromActorInfo");
  L_32:; 
  L_33:; (*u64_pframe(self, "K2Node_DynamicCast_AsGSHero_Character")) = u64_cast((*u64_pframe(self, "CallFunc_GetAvatarActorFromActorInfo_ReturnValue")), "GSHeroCharacter");
  L_69:; (*u64_pframe(self, "K2Node_DynamicCast_bSuccess")) = (*u64_pframe(self, "K2Node_DynamicCast_AsGSHero_Character"));
  L_98:; 
  L_99:; if (!u64_true((*u64_pframe(self, "K2Node_DynamicCast_bSuccess")))) goto L_238;
  L_113:; 
  L_114:; 
  L_115:; (*u64_ivar(self, "OwningHero")) = (*u64_pframe(self, "K2Node_DynamicCast_AsGSHero_Character"));
  L_134:; 
  L_135:; 
  L_136:; ({ u64_v _o = (*u64_ivar(self, "OwningHero")); U64_CALL(u64_asobj(_o), "NextWeapon"); });
  L_172:; 
  L_173:; 
  L_174:; (*u64_pframe(self, "CallFunc_BP_ApplyGameplayEffectToOwner_ReturnValue")) = U64_CALL(self, "BP_ApplyGameplayEffectToOwner", u64_objv(u64_find("GE_WeaponIsChangingDelayReplicationTag_C")), u64_int(1), u64_int(1));
  L_221:; 
  L_222:; 
  L_223:; U64_CALL(self, "K2_EndAbility");
  L_237:; 
  L_238:; goto u64_ret;
  L_240:; 
  u64_ret:; u64_set_return(L.ReturnValue);

}

void bp_GA_NextWeapon__K2_ActivateAbility(u64_obj *self, const u64_v *args, int argc) {
  (void)args; (void)argc;
  struct { u64_v ReturnValue; u64_v _unsupported; } L; memset(&L, 0, sizeof L); (void)L;
  U64_FLOW_DECL; (void)flow_n;
  L_0:; 
  L_1:; 
  L_2:; 
  L_3:; U64_CALL(self, "ExecuteUbergraph_GA_NextWeapon", u64_int(10));
  L_18:; 
  L_19:; goto u64_ret;
  L_21:; 
  u64_ret:; u64_set_return(L.ReturnValue);

}

void bp_GA_NextWeapon__K2_CanActivateAbility(u64_obj *self, const u64_v *args, int argc) {
  (void)args; (void)argc;
  struct { u64_v ActorInfo; u64_v CallFunc_GetNumWeapons_ReturnValue; u64_v CallFunc_Greater_IntInt_ReturnValue; u64_v K2Node_DynamicCast_AsGSHero_Character; u64_v K2Node_DynamicCast_bSuccess; u64_v RelevantTags; u64_v ReturnValue; u64_v _unsupported; } L; memset(&L, 0, sizeof L); (void)L;
  U64_FLOW_DECL; (void)flow_n;
  if (argc > 0) L.ActorInfo = args[0];
  if (argc > 1) L.RelevantTags = args[1].t == U64_REF ? *args[1].u.r : args[1];
  L_0:; 
  L_1:; 
  L_2:; 
  L_3:; L.K2Node_DynamicCast_AsGSHero_Character = u64_cast((*u64_member(&L.ActorInfo, "AvatarActor")), "GSHeroCharacter");
  L_48:; L.K2Node_DynamicCast_bSuccess = L.K2Node_DynamicCast_AsGSHero_Character;
  L_77:; 
  L_78:; if (!u64_true(L.K2Node_DynamicCast_bSuccess)) goto L_317;
  L_92:; 
  L_93:; L.CallFunc_GetNumWeapons_ReturnValue = ({ u64_v _o = L.K2Node_DynamicCast_AsGSHero_Character; U64_CALL(u64_asobj(_o), "GetNumWeapons"); });
  L_143:; L.CallFunc_Greater_IntInt_ReturnValue = U64_CALL(self, "Greater_IntInt", L.CallFunc_GetNumWeapons_ReturnValue, u64_int(1));
  L_177:; 
  L_178:; 
  L_179:; if (!u64_true(L.CallFunc_Greater_IntInt_ReturnValue)) goto L_258;
  L_193:; 
  L_194:; 
  L_195:; L.RelevantTags = u64_int(0) /* struct const */;
  L_241:; L.ReturnValue = u64_bool(1);
  L_252:; 
  L_253:; goto L_317;
  L_258:; 
  L_259:; L.RelevantTags = u64_int(0) /* struct const */;
  L_305:; L.ReturnValue = u64_bool(0);
  L_316:; 
  L_317:; L.ReturnValue = L.ReturnValue; goto u64_ret;
  L_327:; 
  u64_ret:; u64_set_return(L.ReturnValue);
  if (argc > 1) u64_out(args[1], L.RelevantTags);
}
