#pragma once
#include <functional>
#include <memory>
#include <string>

class Champion;

// Ability slots: Passive, Q, W, E, R. Usable as array indices: abilities[Slot::Q].
struct Slot { enum Index { P, Q, W, E, R, Count }; };

// Who an ability is acting for and against.
struct AbilityContext {
    Champion& self;
    Champion& enemy;
};

// Base class for every ability. Each fighter gets its own instance, so an ability can
// keep its own state (timers, stacks) in member variables. Override only the hooks it needs.
class Ability {
public:
    std::string name;
    int slot{Slot::P};            // set by Champion when it builds its abilities
    float cooldown{0.0f};         // seconds between casts (Q/W/E/R)
    float cooldown_timer{0.0f};   // seconds until it can be cast again
    float range{0.0f};            // edge-to-edge distance within which it casts

    virtual ~Ability() = default;

    // Q/W/E/R: true when the ability should fire this step (default: off cooldown and in range)
    virtual bool wants_to_cast(const AbilityContext& ctx) const;

    virtual void cast(const AbilityContext& /*ctx*/) {}                        // Q/W/E/R effect
    virtual void on_update(const AbilityContext& /*ctx*/, float /*dt*/) {}     // every physics step
    virtual void on_damage_taken(const AbilityContext& /*ctx*/, int /*dmg*/) {}  // ctx.self took dmg
    virtual void on_basic_hit(const AbilityContext& /*ctx*/, int /*dmg*/) {}     // ctx.self landed a basic hit
};

// A ChampionDef stores one factory per slot; every fight calls it to build a fresh ability.
using AbilityFactory = std::function<std::unique_ptr<Ability>()>;

template <typename T>
AbilityFactory make_ability() {
    return []() -> std::unique_ptr<Ability> { return std::make_unique<T>(); };
}

// Ticks cooldowns, runs on_update, and casts any ability that wants to fire.
void tick_abilities(Champion& self, Champion& enemy, float dt);
