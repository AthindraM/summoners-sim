#pragma once
#include "champion.h"

const float HIT_COOLDOWN = 0.5f;   // seconds between hits from the same champion

enum class DamageType { Physical, Magic, True };

// Fraction of raw damage that gets through: the resistance formula 100 / (100 + resist)
// (armor for Physical, magic_resist for Magic, none for True), times (1 - damage_reduction).
float damage_multiplier(const Champion& target, DamageType type);

// Deals damage after damage_multiplier. The target's shield absorbs first, then health.
// Fires the target's on_damage_taken hooks and returns the damage actually dealt
// (shield + health, without overkill).
int deal_damage(Champion& source, Champion& target, float raw, DamageType type);

// A basic hit from a collision: physical damage from attack_damage, then on_basic_hit hooks.
void apply_hit(Champion& attacker, Champion& defender);

// Call once per physics step. If the champions are colliding, each one
// that is off cooldown (and allowed to attack) hits the other.
void tick_combat(Champion& a, Champion& b, bool colliding, float dt);
