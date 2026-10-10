#pragma once
#include <algorithm>
#include <cmath>
#include "ability.h"
#include "champion.h"
#include "combat.h"

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

    bool active() const override { return bonus != 0.0f; }

    void on_update(const AbilityContext& ctx, float dt) override {
        since_damage += dt;
        cooldown_timer = std::max(0.0f, DELAY - since_damage);   // seconds until it kicks in (shown in the panel)
        if (bonus == 0.0f && since_damage >= DELAY) {
            bonus = ctx.self.def->health * REGEN_PCT;
            ctx.self.health_regen += bonus;
        }
    }

    void on_damage_taken(const AbilityContext& ctx, int dmg) override {
        if (dmg <= 0) return;
        since_damage = 0.0f;
        cooldown_timer = DELAY;
        ctx.self.health_regen -= bonus;
        bonus = 0.0f;
    }
};

// Q (Decisive Strike): cast automatically whenever it's off cooldown. Gain 35% bonus movement
// speed for 1.4 seconds, and the next basic attack within 4.5 seconds deals an extra 30 (+50% AD)
// physical damage. The 8 second cooldown only starts counting once the empowerment ends (the
// attack lands or the 4.5 seconds run out). The speed bonus is added to the movement_speed
// stat, so it shows in the panel.
// (Not implemented yet: the lunge, the 1.5s silence, cleansing slows, resetting the attack timer.)
class GarenQ : public Ability {
    static constexpr float MS_BONUS_PCT = 0.35f;     // of his base movement speed
    static constexpr float MS_DURATION = 1.4f;       // seconds
    static constexpr float EMPOWER_WINDOW = 4.5f;    // seconds to land the empowered attack
    static constexpr float BONUS_DAMAGE = 30.0f;
    static constexpr float BONUS_AD_RATIO = 0.50f;   // of his total attack damage

    float speed_timer = 0.0f;
    float empower_timer = 0.0f;
    int speed_bonus = 0;                             // amount currently added to movement_speed

    void end_speed(Champion& self) {
        self.movement_speed -= speed_bonus;
        speed_bonus = 0;
        speed_timer = 0.0f;
    }

public:
    GarenQ() {
        name = "Decisive Strike";
        cooldown = 8.0f;
        range = 1e9f;   // no target needed: it fires whenever it's off cooldown
    }

    bool active() const override { return speed_timer > 0.0f || empower_timer > 0.0f; }

    void cast(const AbilityContext& ctx) override {
        if (speed_bonus != 0) end_speed(ctx.self);
        speed_bonus = (int)std::lround(ctx.self.def->movement_speed * MS_BONUS_PCT);
        ctx.self.movement_speed += speed_bonus;
        speed_timer = MS_DURATION;
        empower_timer = EMPOWER_WINDOW;
    }

    void on_update(const AbilityContext& ctx, float dt) override {
        if (speed_timer > 0.0f) {
            speed_timer -= dt;
            if (speed_timer <= 0.0f) end_speed(ctx.self);
        }
        empower_timer = std::max(0.0f, empower_timer - dt);
        // The framework starts the cooldown at the cast; hold it full until the empowerment ends.
        if (empower_timer > 0.0f) cooldown_timer = cooldown;
    }

    void on_basic_hit(const AbilityContext& ctx, int /*dmg*/) override {
        if (empower_timer <= 0.0f) return;
        empower_timer = 0.0f;   // the empowerment is used up by this attack
        deal_damage(ctx.self, ctx.enemy, BONUS_DAMAGE + BONUS_AD_RATIO * ctx.self.attack_damage,
                    DamageType::Physical);
    }
};
