#include "champion_defs.h"
#include <map>

static std::map<std::string, ChampionDef> defs;

static Texture2D load_circle_texture(const std::string &path, int diameter) {
  Image img = LoadImage(path.c_str());
  if (img.data == nullptr)
    return Texture2D{};

  ImageResize(&img, diameter, diameter);
  ImageFormat(&img, PIXELFORMAT_UNCOMPRESSED_R8G8B8A8);

  Color *px = (Color *)img.data;
  float r = diameter / 2.0f;
  for (int y = 0; y < diameter; y++) {
    for (int x = 0; x < diameter; x++) {
      float dx = x + 0.5f - r, dy = y + 0.5f - r;
      if (dx * dx + dy * dy > r * r)
        px[y * diameter + x].a = 0;
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

void load_champion_defs() {
  add({.name = "Darius",
       .sprite_path = "assets/champions/Darius_0.jpg",
       .radius = 30,
       .health = 650,
       .movement_speed = 340,
       .armor = 39,
       .magic_resist = 32,
       .attack_damage = 64,
       .ability_power = 0});

  add({.name = "Garen",
       .sprite_path = "assets/champions/Garen_0.jpg",
       .radius = 30,
       .health = 690,
       .movement_speed = 340,
       .armor = 36,
       .magic_resist = 32,
       .attack_damage = 66,
       .ability_power = 0});
}

const ChampionDef &get_def(const std::string &name) { return defs.at(name); }

void unload_champion_defs() {
  for (auto &[name, def] : defs) {
    if (def.sprite.id != 0)
      UnloadTexture(def.sprite);
  }
  defs.clear();
}
