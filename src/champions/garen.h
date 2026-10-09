#pragma once
#include "ability.h"
#include "champion.h"

// Passive (Perseverance): after 8 seconds of not taking damage, regenerate an
// additional 1.5% max HP per second.
class GarenPassive : public Ability {
    static constexpr float DELAY = 8.0f;        // seconds without damage before it kicks in
    static constexpr float REGEN_PCT = 0.015f;  // fraction of max HP, per second
    float since_damage = 0.0f;

public:
    GarenPassive() { name = "Perseverance"; }

    void on_update(const AbilityContext& ctx, float dt) override {
        since_damage += dt;
        if (since_damage >= DELAY) ctx.self.heal(ctx.self.def->health * REGEN_PCT * dt);
    }

    void on_damage_taken(const AbilityContext& /*ctx*/, int dmg) override {
        if (dmg > 0) since_damage = 0.0f;
    }
};
