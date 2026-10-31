#pragma once
// Controller state for one frame (platform independent: the N64 ROM fills it from the joypad, tests from a script).
enum { U64_BTN_JUMP, U64_BTN_SPRINT, U64_BTN_DASH, U64_BTN_ADS, U64_BTN_FIRE, U64_BTN_METEOR, U64_BTN_ARMOR, U64_BTN_COUNT };
typedef struct { float forward, right, turn, look, turn_delta; int btn[U64_BTN_COUNT]; } u64_pad;
void u64_game_update(float dt, const u64_pad *pad);
