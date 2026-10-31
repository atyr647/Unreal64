// One frame of the game: controller state -> the ported hero input (axes and ability IDs) -> world simulation.
// The input IDs are the project's EGDAbilityInputID values for each ability.
#include "u64_gas_core.h"
#include "u64_game.h"

static const char *const IDS[U64_BTN_COUNT] = {
  "EGDAbilityInputID::Jump", "EGDAbilityInputID::Sprint", "EGDAbilityInputID::Ability3",     // A, B, Z
  "EGDAbilityInputID::Ability2", "EGDAbilityInputID::Ability1", "EGDAbilityInputID::Ability5", "EGDAbilityInputID::Ability4",  // L, R, C-down, C-up
};
static u64_pad prev;

void u64_game_update(float dt, const u64_pad *pad)
{
  u64_obj *pawn = u64_player_pawn();
  if (pawn) {
    u64_input_axis(pawn, "MoveForward", pad->forward);
    u64_input_axis(pawn, "MoveRight", pad->right);
    u64_input_axis(pawn, "TurnRate", pad->turn);
    u64_input_axis(pawn, "LookUpRate", pad->look);
    if (pad->turn_delta != 0) u64_input_axis(pawn, "Turn", pad->turn_delta);
    for (int i = 0; i < U64_BTN_COUNT; i++) {
      if (pad->btn[i] && !prev.btn[i]) u64_input_press(pawn, IDS[i]);
      if (!pad->btn[i] && prev.btn[i]) u64_input_release_id(pawn, IDS[i]);
    }
  }
  prev = *pad; prev.turn_delta = 0;
  u64_world_tick(dt);
}
