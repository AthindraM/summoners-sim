#pragma once
#include <algorithm>
#include <cmath>
#include "ability.h"
#include "champion.h"
#include "combat.h"

// Passive (Hemorrhage): every basic hit puts a stack of Hemorrhage on the target for 5 seconds
// (refreshed by each new stack, max 5). Each stack deals 13 (+30% bonus AD) physical damage in
// total over those 5 seconds, reduced by armor. At 5 stacks Darius gains 30 bonus AD (Noxian
// Might) for as long as the stacks last. One small red sphere orbits the target per stack.
class DariusPassive : public Ability {
    static constexpr int   MAX_STACKS = 5;
    static constexpr float STACK_DURATION = 5.0f;        // seconds; every new stack refreshes it
    static constexpr float BLEED_BASE = 13.0f;           // per stack, total over STACK_DURATION
    static constexpr float BLEED_BONUS_AD_RATIO = 0.30f;
    static constexpr int   MIGHT_BONUS_AD = 30;          // gained at max stacks
    static constexpr float TICK = 0.5f;                  // seconds between bleed damage ticks
    static constexpr float ORBIT_SPEED = 3.0f;           // radians per second
    static constexpr float ORBIT_GAP = 14.0f;            // distance from the target's edge
    static constexpr float SPHERE_RADIUS = 7.0f;

    int stacks = 0;
    float stack_timer = 0.0f;     // seconds until the stacks fall off
    float tick_timer = 0.0f;
    bool might_active = false;    // is the 30 bonus AD currently added to attack_damage?
    float orbit_angle = 0.0f;

    void clear(Champion& self) {
        stacks = 0;
        stack_timer = 0.0f;
        tick_timer = 0.0f;
        if (might_active) {
            self.attack_damage -= MIGHT_BONUS_AD;
            might_active = false;
        }
    }

public:
    DariusPassive() { name = "Hemorrhage"; }

    int stack_count() const { return stacks; }

    void on_basic_hit(const AbilityContext& ctx, int dmg) override {
        if (dmg <= 0) return;
        stacks = std::min(stacks + 1, MAX_STACKS);
        stack_timer = STACK_DURATION;
        if (stacks == MAX_STACKS && !might_active) {
            ctx.self.attack_damage += MIGHT_BONUS_AD;   // shows up in the AD stat
            might_active = true;
        }
    }

    void on_update(const AbilityContext& ctx, float dt) override {
        orbit_angle += ORBIT_SPEED * dt;
        if (stacks == 0) return;

        tick_timer += dt;
        while (tick_timer >= TICK) {
            tick_timer -= TICK;
            float bonus_ad = (float)(ctx.self.attack_damage - ctx.self.def->attack_damage);
            float per_stack = BLEED_BASE + BLEED_BONUS_AD_RATIO * bonus_ad;   // total over STACK_DURATION
            deal_damage(ctx.self, ctx.enemy, stacks * per_stack * TICK / STACK_DURATION, DamageType::Physical);
        }

        stack_timer -= dt;
        if (stack_timer <= 0.0f) clear(ctx.self);
    }

    void on_draw(const AbilityContext& ctx) const override {
        const Champion& target = ctx.enemy;
        float orbit_radius = target.radius + ORBIT_GAP;
        for (int i = 0; i < stacks; i++) {
            float angle = orbit_angle + i * (2.0f * PI / MAX_STACKS);
            DrawCircleV({target.pos.x + cosf(angle) * orbit_radius,
                         target.pos.y + sinf(angle) * orbit_radius}, SPHERE_RADIUS, RED);
        }
    }
};
