#include "ui.h"
#include <algorithm>
#include <cmath>
#include <cstdio>

void draw_stats_panel(const Champion &c, int x, int y, int width) {
  DrawText(TextFormat("%s  Lv %d", c.def->name.c_str(), c.level), x, y, 30,
           RAYWHITE);
  y += 44;

  DrawText(TextFormat("HP  %d / %d", c.health, c.max_health), x, y, 20,
           RAYWHITE);
  y += 28;
  // HP bar: green = current HP, white = shield (shown on the bar but not in the
  // HP number). The bar's scale is max HP, or HP + shield when that is bigger.
  // Small ticks mark every 100 HP and large ticks every 1000 HP (a kind of tick
  // is skipped if it would be crowded).
  const int bar_h = 14;
  float total = (float)std::max(c.max_health, c.health + c.shield);
  float scale = total > 0.0f ? width / total : 0.0f;
  int hp_w = (int)(c.health * scale);
  int hp_shield_w = (int)((c.health + c.shield) * scale);
  DrawRectangle(x, y, width, bar_h, DARKGRAY);
  DrawRectangle(x, y, hp_w, bar_h, GREEN);
  if (c.shield > 0)
    DrawRectangle(x + hp_w, y, hp_shield_w - hp_w, bar_h, WHITE);
  auto draw_ticks = [&](int step, int tick_h) {
    if (step * scale < 3.0f)
      return;
    for (int hp = step; hp < total; hp += step) {
      if (step == 100 && hp % 1000 == 0)
        continue; // the large tick covers it
      int tx = x + (int)(hp * scale);
      DrawLine(tx, y + bar_h - tick_h, tx, y + bar_h, Color{0, 0, 0, 160});
    }
  };
  draw_ticks(100, 5);
  draw_ticks(1000, bar_h);
  y += 34;

  const int line = 26;
  DrawText(TextFormat("Health regen:   %g /s", c.health_regen), x, y, 20,
           RAYWHITE);
  y += line;
  DrawText(TextFormat("Attack damage:  %d", c.attack_damage), x, y, 20,
           RAYWHITE);
  y += line;
  DrawText(TextFormat("Ability power:  %d", c.ability_power), x, y, 20,
           RAYWHITE);
  y += line;
  DrawText(TextFormat("Armor:          %d", c.armor), x, y, 20, RAYWHITE);
  y += line;
  DrawText(TextFormat("Magic resist:   %d", c.magic_resist), x, y, 20,
           RAYWHITE);
  y += line;
  DrawText(
      TextFormat("Move speed:     %d", (int)std::lround(c.current_speed())), x,
      y, 20, RAYWHITE);
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
