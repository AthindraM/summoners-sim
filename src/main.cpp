#include "champion.h"
#include "champion_defs.h"
#include "physics.h"
#include "raylib.h"
#include "raymath.h"
#include <algorithm>
#include <cmath>
#include <random>

std::mt19937 rng{
    std::random_device{}()}; // use a fixed seed like rng{42} to replay a run
float rand_range(float lo, float hi) {
  return std::uniform_real_distribution<float>(lo, hi)(rng);
}

int main() {
  const int W = 900, H = 600;
  const float DT = 1.0f / 120.0f;
  const float RESTITUTION = 1.0f;

  InitWindow(W, H, "Summoner's Sim");
  SetTargetFPS(60);
  ChangeDirectory(GetApplicationDirectory());
  load_champion_defs();

  Champion a(get_def("Darius"));
  Champion b(get_def("Garen"));

  a.pos = {rand_range(a.radius, W / 2.0f - a.radius),
           rand_range(a.radius, H - a.radius)};
  b.pos = {rand_range(W / 2.0f + b.radius, W - b.radius),
           rand_range(b.radius, H - b.radius)};

  float angle_a = rand_range(0, 2 * PI);
  float angle_b = rand_range(0, 2 * PI);
  a.set_direction({cosf(angle_a), sinf(angle_a)});
  b.set_direction({cosf(angle_b), sinf(angle_b)});

  float accumulator = 0.0f;

  while (!WindowShouldClose()) {
    accumulator += std::min(GetFrameTime(), 0.25f);

    while (accumulator >= DT) {
      for (Champion *c : {&a, &b}) {
        c->pos = Vector2Add(c->pos, Vector2Scale(c->vel, DT));
        bounce_off_walls(*c, W, H);
      }

      if (resolve_collision(a, b, RESTITUTION)) {
        a.vel = Vector2Rotate(a.vel, rand_range(-0.05f, 0.05f));
        b.vel = Vector2Rotate(b.vel, rand_range(-0.05f, 0.05f));
      }

      a.enforce_speed();
      b.enforce_speed();
      accumulator -= DT;
    }

    BeginDrawing();
    ClearBackground(BLACK);
    a.draw();
    b.draw();
    EndDrawing();
  }

  unload_champion_defs();
  CloseWindow();
  return 0;
}
