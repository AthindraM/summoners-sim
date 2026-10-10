#include "ui.h"
#include <cstdio>

void draw_stats_panel(const Champion &c, int x, int y, int width) {
  DrawText(TextFormat("%s  Lv %d", c.def->name.c_str(), c.level), x, y, 30,
           RAYWHITE);
  y += 44;

  DrawText(TextFormat("HP  %d / %d", c.health, c.max_health), x, y, 20,
           RAYWHITE);
  y += 28;
  float frac = c.max_health > 0
                   ? Clamp((float)c.health / c.max_health, 0.0f, 1.0f)
                   : 0.0f;
  DrawRectangle(x, y, width, 14, DARKGRAY);
  DrawRectangle(x, y, (int)(width * frac), 14, GREEN);
  y += 34;

  const int line = 26;
  DrawText(TextFormat("HP Regen:   %g/s", c.health_regen), x, y, 20, RAYWHITE);
  y += line;
  DrawText(TextFormat("AD:  %d", c.attack_damage), x, y, 20, RAYWHITE);
  y += line;
  DrawText(TextFormat("AP:  %d", c.ability_power), x, y, 20, RAYWHITE);
  y += line;
  DrawText(TextFormat("Armor:          %d", c.armor), x, y, 20, RAYWHITE);
  y += line;
  DrawText(TextFormat("Magic Resist:   %d", c.magic_resist), x, y, 20,
           RAYWHITE);
  y += line;
  DrawText(TextFormat("Move Speed:     %d", c.movement_speed), x, y, 20,
           RAYWHITE);
  y += line + 14;

  // P / Q / W / E / R: name and cooldown (gray while on cooldown, yellow while
  // active)
  for (int i = 0; i < Slot::Count; i++) {
    const Ability *ab = c.abilities[i].get();
    if (!ab) {
      DrawText(TextFormat("%c  -", "PQWER"[i]), x, y, 18, GRAY);
    } else {
      char status[16] = "";
      if (ab->cooldown_timer > 0.0f)
        snprintf(status, sizeof status, "%.1fs", ab->cooldown_timer);
      else if (i != Slot::P)
        snprintf(status, sizeof status, "ready");
      Color col =
          ab->active() ? YELLOW : (ab->cooldown_timer > 0.0f ? GRAY : RAYWHITE);
      DrawText(TextFormat("%c  %s  %s", "PQWER"[i], ab->name.c_str(), status),
               x, y, 18, col);
    }
    y += 24;
  }
}
