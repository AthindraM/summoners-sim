#pragma once
#include "raylib.h"
#include "raymath.h"
#include <string>

// Static data for one champion. One of these exists per champion and is shared
// by every instance of that champion in a battle.
struct ChampionDef {
    std::string name;
    std::string sprite_path;
    float radius{25.0f};
    int health{0};
    int movement_speed{100};   // pixels per second
    int armor{0};
    int magic_resist{0};
    int attack_damage{0};
    int ability_power{0};
    Texture2D sprite{};        // filled in by load_champion_defs()
};

// A champion actually fighting in a battle. Stats are copied from the def so
// they can change mid-fight (damage, buffs) without touching the def.
class Champion {
public:
    const ChampionDef* def;
    int health;
    int movement_speed;
    int armor;
    int magic_resist;
    int attack_damage;
    int ability_power;
    float radius;
    Vector2 pos{0, 0};
    Vector2 vel{0, 0};
    float attack_timer{0.0f};   // seconds until this champion can land another hit

    explicit Champion(const ChampionDef& d)
        : def(&d), health(d.health), movement_speed(d.movement_speed), armor(d.armor),
          magic_resist(d.magic_resist), attack_damage(d.attack_damage),
          ability_power(d.ability_power), radius(d.radius) {}

    // point the champion in a direction; speed always comes from movement_speed
    void set_direction(Vector2 dir) {
        vel = Vector2Scale(Vector2Normalize(dir), (float)movement_speed);
    }

    // keep speed locked to movement_speed (collisions only change direction)
    void enforce_speed() {
        vel = Vector2Scale(Vector2Normalize(vel), (float)movement_speed);
    }

    void draw() const {
        if (def->sprite.id == 0) {                 // sprite missing: magenta placeholder
            DrawCircleV(pos, radius, MAGENTA);
            return;
        }
        DrawTextureV(def->sprite, {pos.x - radius, pos.y - radius}, WHITE);
    }
};
