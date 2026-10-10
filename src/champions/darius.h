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
    bool active() const override { return might_active; }

    // Add a stack of Hemorrhage (or refresh at max); also used by abilities like Q.
    void add_stack(Champion& self) {
        stacks = std::min(stacks + 1, MAX_STACKS);
        stack_timer = STACK_DURATION;
        if (stacks == MAX_STACKS && !might_active) {
            self.attack_damage += MIGHT_BONUS_AD;   // shows up in the AD stat
            might_active = true;
        }
    }

    void on_basic_hit(const AbilityContext& ctx, int dmg) override {
        if (dmg <= 0) return;
        add_stack(ctx.self);
    }

    void on_update(const AbilityContext& ctx, float dt) override {
        orbit_angle += ORBIT_SPEED * dt;
        if (stacks == 0) return;

        tick_timer += dt;
        while (tick_timer >= TICK) {
            tick_timer -= TICK;
            float bonus_ad = (float)(ctx.self.attack_damage - ctx.self.base_attack_damage);
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

// Q (Decimate): cast automatically whenever it's off cooldown, whether or not the enemy is in
// reach (so it can miss). For the 1 second cast Darius can't basic attack (collisions still
// happen, so the enemy can still hit him), and he hefts his axe for 0.75 seconds, then swings
// it around himself.
//   - Hit by the outer ring (the blade): 50 (+100% AD) physical damage, applies a Hemorrhage
//     stack, and heals Darius for 17% of his missing health (17% per champion hit, max 51%).
//   - Hit only by the inner circle (the handle): 35% of that damage, no stack, no heal.
// The radii are in pixels, scaled down from the game's 460 / 240 units to fit the arena.
// (Mana cost is ignored; there is no mana.)
class DariusQ : public Ability {
    static constexpr float WINDUP = 0.75f;           // seconds until the swing
    static constexpr float CAST_DURATION = 1.0f;     // seconds he can't basic attack
    static constexpr float OUTER_RADIUS = 175.0f;    // pixels
    static constexpr float INNER_RADIUS = 120.0f;    // pixels
    static constexpr float BASE_DAMAGE = 50.0f;
    static constexpr float AD_RATIO = 1.00f;         // of his total attack damage
    static constexpr float INNER_DAMAGE_MULT = 0.35f;
    static constexpr float HEAL_PER_TARGET = 0.17f;  // of missing health
    static constexpr float HEAL_MAX = 0.51f;
    static constexpr int   SEGMENTS = 90;            // smoothness of the indicator circles

    bool casting = false;
    bool swung = false;
    float cast_time = 0.0f;

    void swing(const AbilityContext& ctx) {
        Champion& self = ctx.self;
        Champion& enemy = ctx.enemy;
        float d = Vector2Distance(self.pos, enemy.pos);
        // any part of the enemy touching the ring = hit by the blade;
        // entirely inside the inner circle = hit by the handle only
        bool blade = (d + enemy.radius > INNER_RADIUS) && (d - enemy.radius <= OUTER_RADIUS);
        bool handle = !blade && (d + enemy.radius <= INNER_RADIUS);
        if (!blade && !handle) return;

        float raw = BASE_DAMAGE + AD_RATIO * self.attack_damage;
        if (handle) raw *= INNER_DAMAGE_MULT;
        deal_damage(self, enemy, raw, DamageType::Physical);

        if (blade) {
            if (auto* passive = dynamic_cast<DariusPassive*>(self.abilities[Slot::P].get()))
                passive->add_stack(self);
            float missing = (float)(self.max_health - self.health);
            int champions_hit = 1;   // only one enemy exists for now
            self.heal(missing * std::min(HEAL_PER_TARGET * champions_hit, HEAL_MAX));
        }
    }

public:
    DariusQ() {
        name = "Decimate";
        cooldown = 9.0f;
        range = 1e9f;   // no target needed: it fires whenever it's off cooldown
    }

    bool active() const override { return casting; }

    void cast(const AbilityContext& ctx) override {
        casting = true;
        swung = false;
        cast_time = 0.0f;
        ctx.self.can_attack = false;
    }

    void on_update(const AbilityContext& ctx, float dt) override {
        if (!casting) return;
        cast_time += dt;
        if (!swung && cast_time >= WINDUP) {
            swung = true;
            swing(ctx);
        }
        if (cast_time >= CAST_DURATION) {
            casting = false;
            ctx.self.can_attack = true;
        }
    }

    // Faded red ring for the blade (outer edge), lighter red disc for the handle (inner circle).
    // They sit side by side, so the colors never overlap. Both flash brighter right after the swing.
    void on_draw(const AbilityContext& ctx) const override {
        if (!casting) return;
        float boost = swung ? 1.8f : 1.0f;
        Vector2 c = ctx.self.pos;
        DrawRing(c, INNER_RADIUS, OUTER_RADIUS, 0.0f, 360.0f, SEGMENTS,
             Color{160, 25, 25, (unsigned char)(110 * boost)});
    DrawRing(c, 45, INNER_RADIUS, 0.0f, 360.0f, SEGMENTS,
             Color{200, 30, 30, (unsigned char)(60 * boost)});
    }
};
