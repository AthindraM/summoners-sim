#include "ui.h"
#include <cstdio>

void draw_stats_panel(const Champion& c, int x, int y, int width) {
    DrawText(c.def->name.c_str(), x, y, 30, RAYWHITE);
    y += 44;

    DrawText(TextFormat("HP  %d / %d", c.health, c.def->health), x, y, 20, RAYWHITE);
    y += 28;
    float frac = c.def->health > 0 ? Clamp((float)c.health / c.def->health, 0.0f, 1.0f) : 0.0f;
    DrawRectangle(x, y, width, 14, DARKGRAY);
    DrawRectangle(x, y, (int)(width * frac), 14, GREEN);
    y += 34;

    const int line = 26;
    DrawText(TextFormat("Health regen:   %g /s", c.health_regen), x, y, 20, RAYWHITE); y += line;
    DrawText(TextFormat("Attack damage:  %d", c.attack_damage), x, y, 20, RAYWHITE); y += line;
    DrawText(TextFormat("Ability power:  %d", c.ability_power), x, y, 20, RAYWHITE); y += line;
    DrawText(TextFormat("Armor:          %d", c.armor), x, y, 20, RAYWHITE);         y += line;
    DrawText(TextFormat("Magic resist:   %d", c.magic_resist), x, y, 20, RAYWHITE);  y += line;
    DrawText(TextFormat("Move speed:     %d", c.movement_speed), x, y, 20, RAYWHITE);
    y += line + 14;

    // P / Q / W / E / R: name and cooldown (grayed while on cooldown)
    for (int i = 0; i < Slot::Count; i++) {
        const Ability* ab = c.abilities[i].get();
        if (!ab) {
            DrawText(TextFormat("%c  -", "PQWER"[i]), x, y, 18, GRAY);
        } else {
            char status[16] = "";
            if (i != Slot::P) {
                if (ab->cooldown_timer > 0.0f) snprintf(status, sizeof status, "%.1fs", ab->cooldown_timer);
                else snprintf(status, sizeof status, "ready");
            }
            Color col = ab->cooldown_timer > 0.0f ? GRAY : RAYWHITE;
            DrawText(TextFormat("%c  %s  %s", "PQWER"[i], ab->name.c_str(), status), x, y, 18, col);
        }
        y += 24;
    }
}
