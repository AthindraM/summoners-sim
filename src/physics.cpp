#include "physics.h"
#include <cmath>

// All champions have equal mass, so each one moves half the overlap
// and takes half the impulse.
bool resolve_collision(Champion& a, Champion& b, float restitution) {
    Vector2 delta = Vector2Subtract(b.pos, a.pos);
    float dist = Vector2Length(delta);
    float min_dist = a.radius + b.radius;
    if (dist >= min_dist) return false;  // not touching

    Vector2 n = (dist > 0.0001f) ? Vector2Scale(delta, 1.0f / dist) : Vector2{1, 0};

    // separate by penetration depth, half each
    float half_push = (min_dist - dist) * 0.5f;
    a.pos = Vector2Subtract(a.pos, Vector2Scale(n, half_push));
    b.pos = Vector2Add(b.pos, Vector2Scale(n, half_push));

    // impulse only if moving toward each other
    float vel_along_n = Vector2DotProduct(Vector2Subtract(b.vel, a.vel), n);
    if (vel_along_n > 0) return true;  // overlapping but already separating

    Vector2 impulse = Vector2Scale(n, -(1.0f + restitution) * vel_along_n * 0.5f);
    a.vel = Vector2Subtract(a.vel, impulse);
    b.vel = Vector2Add(b.vel, impulse);
    return true;
}

void bounce_off_walls(Champion& c, Rectangle arena) {
    float left = arena.x, right = arena.x + arena.width;
    float top = arena.y, bottom = arena.y + arena.height;
    if (c.pos.x - c.radius < left)   { c.pos.x = left + c.radius;   c.vel.x =  std::fabs(c.vel.x); }
    if (c.pos.x + c.radius > right)  { c.pos.x = right - c.radius;  c.vel.x = -std::fabs(c.vel.x); }
    if (c.pos.y - c.radius < top)    { c.pos.y = top + c.radius;    c.vel.y =  std::fabs(c.vel.y); }
    if (c.pos.y + c.radius > bottom) { c.pos.y = bottom - c.radius; c.vel.y = -std::fabs(c.vel.y); }
}
