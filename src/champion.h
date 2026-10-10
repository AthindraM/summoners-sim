#pragma once
#include "ability.h"
#include "raylib.h"
#include "raymath.h"
#include <algorithm>
#include <array>
#include <cmath>
#include <memory>
#include <string>

inline constexpr int MAX_LEVEL = 18;

// How much of its growth a stat has at a given level. Currently level squared.
// (League's own curve is (n-1) * (0.7025 + 0.0175 * (n-1)); swap it in here to
// change every stat.)
inline float level_factor(int level) {
  return (level - 1) * (0.7025 + 0.0175 * (level - 1));
}

inline int stat_at_level(int base, float growth, int level) {
  return (int)std::lround(base + growth * level_factor(level));
}

// Static data for one champion. One of these exists per champion and is shared
// by every instance of that champion in a battle.
struct ChampionDef {
  std::string name;
  std::string sprite_path;
  float radius{25.0f};
  int health{0};
  float health_regen{0};   // HP per second
  int movement_speed{100}; // pixels per second
  int armor{0};
  int magic_resist{0};
  int attack_damage{0};
  int ability_power{0};
  // Growth per level: a stat at level n is base + growth x level_factor(n), so
  // the plain stats above are the base values and these are what each level
  // adds on top.
  float health_growth{0};
  float health_regen_growth{0};
  float armor_growth{0};
  float magic_resist_growth{0};
  float attack_damage_growth{0};
  std::array<AbilityFactory, Slot::Count>
      abilities{};    // indexed by Slot; empty slots are skipped
  Texture2D sprite{}; // filled in by load_champion_defs()
};

// A champion actually fighting in a battle. Stats are copied from the def so
// they can change mid-fight (damage, buffs) without touching the def.
class Champion {
public:
  const ChampionDef *def;
  int level; // 1 to MAX_LEVEL; sets the stats below
  int max_health;
  int health;
  float health_regen; // HP per second; abilities can add to it
  int movement_speed;
  int armor;
  int magic_resist;
  int attack_damage;      // includes any bonus AD from abilities
  int base_attack_damage; // attack damage at this level, before any bonus AD
  int ability_power;
  float radius;
  Vector2 pos{0, 0};
  Vector2 vel{0, 0};
  float attack_timer{0.0f}; // seconds until this champion can land another hit
  bool can_attack{
      true}; // false while casting something that blocks basic attacks
  float regen_buffer{0.0f}; // fractional HP carried between steps
  std::array<std::unique_ptr<Ability>, Slot::Count>
      abilities; // indexed by Slot

  explicit Champion(const ChampionDef &d, int lvl = 1)
      : def(&d), level(std::clamp(lvl, 1, MAX_LEVEL)),
        max_health(stat_at_level(d.health, d.health_growth, level)),
        health(max_health),
        health_regen(d.health_regen +
                     d.health_regen_growth * level_factor(level)),
        movement_speed(d.movement_speed),
        armor(stat_at_level(d.armor, d.armor_growth, level)),
        magic_resist(
            stat_at_level(d.magic_resist, d.magic_resist_growth, level)),
        attack_damage(
            stat_at_level(d.attack_damage, d.attack_damage_growth, level)),
        base_attack_damage(attack_damage), ability_power(d.ability_power),
        radius(d.radius) {
    for (int i = 0; i < Slot::Count; i++) {
      if (!d.abilities[i])
        continue;
      abilities[i] = d.abilities[i]();
      abilities[i]->slot = i;
    }
  }

  // Heal by a (possibly fractional) amount; the fractional part carries over
  // between steps.
  void heal(float amount) {
    if (health <= 0)
      return; // the dead don't heal
    regen_buffer += amount;
    int whole = (int)regen_buffer;
    regen_buffer -= whole;
    health = std::min(health + whole, max_health);
  }

  // Base regeneration: health_regen HP per second.
  void regen(float dt) { heal(health_regen * dt); }

  // point the champion in a direction; speed always comes from movement_speed
  void set_direction(Vector2 dir) {
    vel = Vector2Scale(Vector2Normalize(dir), (float)movement_speed);
  }

  // keep speed locked to movement_speed (collisions only change direction)
  void enforce_speed() {
    vel = Vector2Scale(Vector2Normalize(vel), (float)movement_speed);
  }

  void draw() const {
    if (def->sprite.id == 0) { // sprite missing: magenta placeholder
      DrawCircleV(pos, radius, MAGENTA);
      return;
    }
    DrawTextureV(def->sprite, {pos.x - radius, pos.y - radius}, WHITE);
  }
};
