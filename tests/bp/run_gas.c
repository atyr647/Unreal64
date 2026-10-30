// Scenario tests of ported engine behaviour using the project's own compiled Blueprints.
#include "u64_rt.h"
#include <stdio.h>
#include <stdarg.h>
const u64_bpfn *u64_bp_registry(void); int u64_bp_registry_count(void);
static int failures, quiet = 1, verbose = 1;
static int bp_calls;
static void logf_(const char *f, ...)
{
  va_list a; va_start(a, f);
  if (!strncmp(f, "[bp]", 4) && !strcmp(va_arg(a, const char *), "GA_Dash_BP")) bp_calls++;   // only the ability under test
  va_end(a);
  if (quiet) return;
  va_start(a, f); vprintf(f, a); va_end(a); putchar('\n');
}
#define CHECK(c) do { if (!(c)) { printf("FAIL line %d: %s\n", __LINE__, #c); failures++; } else if (verbose) printf("ok   %s\n", #c); } while (0)
static void run(float secs) { for (float t = 0; t < secs; t += 0.05f) u64_world_tick(0.05f); }
static u64_v *iv(u64_obj *o, const char *n) { return u64_ivar(o, n); }

int u64_run_scenarios(int verbose_)
{
  verbose = verbose_; failures = 0;
  u64_bp_table = u64_bp_registry(); u64_bp_count = u64_bp_registry_count(); u64_log = logf_;
  u64_obj *hero = u64_new("Hero", "BP_HeroCharacter");

  // 1. Sprint ability: StartSprinting on activate, input release ends it and StopSprinting runs.
  u64_obj *sprint = u64_new("SprintAbility", "GA_Sprint_BP");
  *iv(sprint, "Avatar") = u64_objv(hero);
  *iv(hero, "Acceleration") = u64_vec(1, 0, 0);
  u64_call(sprint, "K2_ActivateAbility", 0, 0);
  run(0.3f);
  CHECK(u64_true(*iv(hero, "bSprinting")));
  CHECK(!u64_true(*iv(sprint, "Ended")));
  u64_input_release(sprint);
  run(0.3f);
  CHECK(u64_true(*iv(sprint, "Ended")));
  CHECK(!u64_true(*iv(hero, "bSprinting")));

  // 2. Damage: 40 damage vs 100 armor mitigates to 20 (GDDamageExecCalculation); lethal damage destroys the actor.
  u64_obj *dummy = u64_new("Dummy", "BP_Minion");
  u64_v asc = u64_call(dummy, "GetAbilitySystemComponent", 1, (u64_v[]){u64_objv(dummy)});
  CHECK(asc.t == U64_OBJ);
  *iv(asc.u.o, "Armor") = u64_float(100);
  u64_v spec = u64_call(asc.u.o, "MakeOutgoingSpec", 2, (u64_v[]){u64_cls("GE_Damage_C"), u64_float(1)});
  u64_call(hero, "AssignTagSetByCallerMagnitude", 3, (u64_v[]){spec, u64_name("Data.Damage"), u64_float(40)});
  u64_call(asc.u.o, "BP_ApplyGameplayEffectSpecToSelf", 1, &spec);
  CHECK(iv(asc.u.o, "Health")->u.f > 79.9f && iv(asc.u.o, "Health")->u.f < 80.1f);
  *iv(asc.u.o, "Armor") = u64_float(0);
  *iv(spec.u.o, "sbc:Data.Damage") = u64_float(500);
  u64_call(asc.u.o, "BP_ApplyGameplayEffectSpecToSelf", 1, &spec);
  CHECK(!u64_obj_alive(dummy));

  // 3. Ability tasks via Dash: the root-motion task finishes at 0.1 s and ends the ability; the montage task
  //    (scheduled for 1.0 s) must then be cancelled and never reach the Blueprint.
  u64_obj *dash = u64_new("DashAbility", "GA_Dash_BP");
  *iv(dash, "Avatar") = u64_objv(hero);
  u64_call(dash, "K2_ActivateAbility", 0, 0);
  run(0.05f);
  CHECK(!u64_true(*iv(dash, "Ended")));
  run(0.3f);
  CHECK(u64_true(*iv(dash, "Ended")));
  bp_calls = 0; run(2.0f);
  CHECK(bp_calls == 0);

  // 4. Out-parameters: BreakVector writes through references.
  u64_v x = {0}, y = {0}, z = {0};
  u64_call(hero, "BreakVector", 4, (u64_v[]){u64_vec(1, 2, 3), u64_ref(&x), u64_ref(&y), u64_ref(&z)});
  CHECK(x.u.f == 1 && y.u.f == 2 && z.u.f == 3);

  // 5. Text formatting.
  u64_v t = u64_call(hero, "Format", 3, (u64_v[]){u64_str("{0}/{1}"), u64_int(42), u64_int(100)});
  CHECK(!strcmp(t.u.s, "42/100"));

  printf("scenarios: %s (%d failure%s)\n", failures ? "FAILED" : "ALL PASSED", failures, failures == 1 ? "" : "s");
  return failures;
}

#ifndef U64_NO_MAIN
int main(void) { return u64_run_scenarios(1) != 0; }
#endif
