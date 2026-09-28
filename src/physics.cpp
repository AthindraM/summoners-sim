#include "physics.h"
#include <cmath>

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
