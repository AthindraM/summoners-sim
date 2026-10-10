#pragma once
#include "champion.h"

const float HIT_COOLDOWN = 0.5f;   // seconds between hits from the same champion

enum class DamageType { Physical, Magic, True };

// Deals damage after the target's resistance (armor for Physical, magic_resist for Magic,
// none for True), using the League formula 100 / (100 + resist). Fires the target's
// on_damage_taken hooks and returns the damage actually dealt.
int deal_damage(Champion& source, Champion& target, float raw, DamageType type);

// A basic hit from a collision: physical damage from attack_damage, then on_basic_hit hooks.
void apply_hit(Champion& attacker, Champion& defender);

// Call once per physics step. If the champions are colliding, each one
// that is off cooldown (and allowed to attack) hits the other.
void tick_combat(Champion& a, Champion& b, bool colliding, float dt);
