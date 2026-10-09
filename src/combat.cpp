#include "combat.h"
#include <algorithm>
#include <cmath>

void apply_hit(Champion& attacker, Champion& defender) {
    float dmg = attacker.attack_damage * 100.0f / (100.0f + defender.armor);
    defender.health = std::max(0, defender.health - (int)std::lround(dmg));
}

void tick_combat(Champion& a, Champion& b, bool colliding, float dt) {
    a.attack_timer = std::max(0.0f, a.attack_timer - dt);
    b.attack_timer = std::max(0.0f, b.attack_timer - dt);
    if (!colliding) return;

    // both hits are decided before either result matters, so a trade can kill both
    bool a_hits = a.attack_timer <= 0.0f;
    bool b_hits = b.attack_timer <= 0.0f;
    if (a_hits) { apply_hit(a, b); a.attack_timer = HIT_COOLDOWN; }
    if (b_hits) { apply_hit(b, a); b.attack_timer = HIT_COOLDOWN; }
}
