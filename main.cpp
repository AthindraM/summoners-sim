#include "raylib.h"
#include "raymath.h"
#include <algorithm>
#include <cmath>
#include <random>

std::mt19937 rng{std::random_device{}()};
float rand_range(float lo, float hi) {
  return std::uniform_real_distribution<float>(lo, hi)(rng);
}

class Champion {
public:
  // stats
  int health{0};
  int movement_speed{400}; // pixels per second
  int armor{0};
  int magic_resist{0};
  int attack_damage{0};
  int ability_power{0};

  // physics
  Vector2 pos{0, 0};
  Vector2 vel{0, 0};
  float radius{25.0f};
  Color color{WHITE};

  void set_direction(Vector2 dir) {
    vel = Vector2Scale(Vector2Normalize(dir), (float)movement_speed);
  }

  void enforce_speed() {
    vel = Vector2Scale(Vector2Normalize(vel), (float)movement_speed);
  }
};

bool resolve_collision(Champion &a, Champion &b, float restitution) {
  Vector2 delta = Vector2Subtract(b.pos, a.pos);
  float dist = Vector2Length(delta);
  float min_dist = a.radius + b.radius;
  if (dist >= min_dist)
    return false;

  Vector2 n =
      (dist > 0.0001f) ? Vector2Scale(delta, 1.0f / dist) : Vector2{1, 0};

  float half_push = (min_dist - dist) * 0.5f;
  a.pos = Vector2Subtract(a.pos, Vector2Scale(n, half_push));
  b.pos = Vector2Add(b.pos, Vector2Scale(n, half_push));

  float vel_along_n = Vector2DotProduct(Vector2Subtract(b.vel, a.vel), n);
  if (vel_along_n > 0)
    return true;

  Vector2 impulse = Vector2Scale(n, -(1.0f + restitution) * vel_along_n * 0.5f);
  a.vel = Vector2Subtract(a.vel, impulse);
  b.vel = Vector2Add(b.vel, impulse);
  return true;
}

void bounce_off_walls(Champion &c, int w, int h) {
  if (c.pos.x - c.radius < 0) {
    c.pos.x = c.radius;
    c.vel.x = std::fabs(c.vel.x);
  }
  if (c.pos.x + c.radius > w) {
    c.pos.x = w - c.radius;
    c.vel.x = -std::fabs(c.vel.x);
  }
  if (c.pos.y - c.radius < 0) {
    c.pos.y = c.radius;
    c.vel.y = std::fabs(c.vel.y);
  }
  if (c.pos.y + c.radius > h) {
    c.pos.y = h - c.radius;
    c.vel.y = -std::fabs(c.vel.y);
  }
}

int main() {
  const int W = 900, H = 600;
  const float DT = 1.0f / 120.0f;
  const float RESTITUTION = 1.0f;

  InitWindow(W, H, "Summoner's Sim");
  SetTargetFPS(60);

  Champion a;
  a.color = BLUE;

  Champion b;
  b.color = RED;

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
    DrawCircleV(a.pos, a.radius, a.color);
    DrawCircleV(b.pos, b.radius, b.color);
    EndDrawing();
  }

  CloseWindow();
  return 0;
}
