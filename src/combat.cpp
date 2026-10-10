#include "combat.h"
#include <algorithm>
#include <cmath>

int deal_damage(Champion& source, Champion& target, float raw, DamageType type) {
    if (target.health <= 0) return 0;

    float multiplier = 1.0f;
    if (type == DamageType::Physical)   multiplier = 100.0f / (100.0f + target.armor);
    else if (type == DamageType::Magic) multiplier = 100.0f / (100.0f + target.magic_resist);

    int dmg = std::min((int)std::lround(raw * multiplier), target.health);
    target.health -= dmg;

    AbilityContext ctx{target, source};   // from the target's point of view
    for (auto& ab : target.abilities) {
        if (ab) ab->on_damage_taken(ctx, dmg);
    }
    return dmg;
}

void apply_hit(Champion& attacker, Champion& defender) {
    int dmg = deal_damage(attacker, defender, (float)attacker.attack_damage, DamageType::Physical);

    AbilityContext ctx{attacker, defender};
    for (auto& ab : attacker.abilities) {
        if (ab) ab->on_basic_hit(ctx, dmg);
    }
}

void tick_combat(Champion& a, Champion& b, bool colliding, float dt) {
    a.attack_timer = std::max(0.0f, a.attack_timer - dt);
    b.attack_timer = std::max(0.0f, b.attack_timer - dt);
    if (!colliding) return;

    // both hits are decided before either result matters, so a trade can kill both
    bool a_hits = a.can_attack && a.attack_timer <= 0.0f;
    bool b_hits = b.can_attack && b.attack_timer <= 0.0f;
    if (a_hits) { apply_hit(a, b); a.attack_timer = HIT_COOLDOWN; }
    if (b_hits) { apply_hit(b, a); b.attack_timer = HIT_COOLDOWN; }
}
