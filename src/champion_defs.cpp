#include "champion_defs.h"
#include "champions/darius.h"
#include "champions/garen.h"
#include <map>

static std::map<std::string, ChampionDef> defs;

// Load an image, resize it to a square, and cut it into a circle (transparent corners).
static Texture2D load_circle_texture(const std::string& path, int diameter) {
    Image img = LoadImage(path.c_str());
    if (img.data == nullptr) return Texture2D{};   // draw() falls back to a placeholder

    ImageResize(&img, diameter, diameter);
    ImageFormat(&img, PIXELFORMAT_UNCOMPRESSED_R8G8B8A8);

    Color* px = (Color*)img.data;
    float r = diameter / 2.0f;
    for (int y = 0; y < diameter; y++) {
        for (int x = 0; x < diameter; x++) {
            float dx = x + 0.5f - r, dy = y + 0.5f - r;
            if (dx * dx + dy * dy > r * r) px[y * diameter + x].a = 0;
        }
    }

    Texture2D tex = LoadTextureFromImage(img);
    UnloadImage(img);
    return tex;
}

static void add(ChampionDef def) {
    def.sprite = load_circle_texture(def.sprite_path, (int)(def.radius * 2));
    std::string name = def.name;
    defs[name] = std::move(def);
}

// ---- ADD NEW CHAMPIONS HERE: one block + one image in assets/champions/ ----
// (stats below are approximate placeholders, tune them however you like)
// Abilities go in slots P, Q, W, E, R. Empty slots are skipped, e.g.:
//     darius.abilities[Slot::Q] = make_ability<DariusQ>();
void load_champion_defs() {
    ChampionDef darius{.name = "Darius", .sprite_path = "assets/champions/darius.png", .radius = 60,
                       .health = 650, .health_regen = 10, .movement_speed = 340, .armor = 39,
                       .magic_resist = 32, .attack_damage = 64, .ability_power = 0};
    darius.abilities[Slot::P] = make_ability<DariusPassive>();
    add(std::move(darius));

    ChampionDef garen{.name = "Garen", .sprite_path = "assets/champions/garen.png", .radius = 60,
                      .health = 690, .health_regen = 8, .movement_speed = 340, .armor = 36,
                      .magic_resist = 32, .attack_damage = 66, .ability_power = 0};
    garen.abilities[Slot::P] = make_ability<GarenPassive>();
    garen.abilities[Slot::Q] = make_ability<GarenQ>();
    add(std::move(garen));
}

const ChampionDef& get_def(const std::string& name) {
    return defs.at(name);   // throws if the name isn't registered
}

void unload_champion_defs() {
    for (auto& [name, def] : defs) {
        if (def.sprite.id != 0) UnloadTexture(def.sprite);
    }
    defs.clear();
}
