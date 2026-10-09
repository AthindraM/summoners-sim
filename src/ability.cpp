#include "ability.h"
#include "champion.h"
#include <algorithm>

bool Ability::wants_to_cast(const AbilityContext& ctx) const {
    if (cooldown_timer > 0.0f) return false;
    float gap = Vector2Distance(ctx.self.pos, ctx.enemy.pos) - ctx.self.radius - ctx.enemy.radius;
    return gap <= range;
}

void tick_abilities(Champion& self, Champion& enemy, float dt) {
    if (self.health <= 0) return;   // the dead don't cast
    AbilityContext ctx{self, enemy};
    for (auto& ab : self.abilities) {
        if (!ab) continue;          // empty slot
        ab->cooldown_timer = std::max(0.0f, ab->cooldown_timer - dt);
        ab->on_update(ctx, dt);
        if (ab->slot != Slot::P && ab->wants_to_cast(ctx)) {
            ab->cast(ctx);
            ab->cooldown_timer = ab->cooldown;
        }
    }
}

void draw_abilities(Champion& self, Champion& enemy) {
    if (self.health <= 0 || enemy.health <= 0) return;
    AbilityContext ctx{self, enemy};
    for (auto& ab : self.abilities) {
        if (ab) ab->on_draw(ctx);
    }
}
