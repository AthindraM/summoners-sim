#pragma once
#include "champion.h"

const float HIT_COOLDOWN = 0.5f;   // seconds between hits from the same champion

// Physical damage, League-style: armor reduces damage by 100 / (100 + armor).
void apply_hit(Champion& attacker, Champion& defender);

// Call once per physics step. If the champions are colliding, each one
// that is off cooldown hits the other.
void tick_combat(Champion& a, Champion& b, bool colliding, float dt);
