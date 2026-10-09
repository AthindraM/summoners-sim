#pragma once
#include "ability.h"
#include "champion.h"

// Passive (Perseverance): after 8 seconds of not taking damage, gain an additional
// 1.5% max HP of health regen (per second). The bonus is added to the champion's
// health_regen stat, so it shows in the stats panel, and removed when he takes damage.
class GarenPassive : public Ability {
    static constexpr float DELAY = 8.0f;        // seconds without damage before it kicks in
    static constexpr float REGEN_PCT = 0.015f;  // fraction of max HP, as HP per second
    float since_damage = 0.0f;
    float bonus = 0.0f;                         // amount currently added to health_regen (0 = inactive)

public:
    GarenPassive() { name = "Perseverance"; }

    void on_update(const AbilityContext& ctx, float dt) override {
        since_damage += dt;
        if (bonus == 0.0f && since_damage >= DELAY) {
            bonus = ctx.self.def->health * REGEN_PCT;
            ctx.self.health_regen += bonus;
        }
    }

    void on_damage_taken(const AbilityContext& ctx, int dmg) override {
        if (dmg <= 0) return;
        since_damage = 0.0f;
        ctx.self.health_regen -= bonus;
        bonus = 0.0f;
    }
};
