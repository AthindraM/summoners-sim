#include "raylib.h"
#include "raymath.h"
#include <algorithm>
#include <cmath>
#include <random>
#include "champion.h"
#include "champion_defs.h"
#include "physics.h"
#include "ui.h"

std::mt19937 rng{std::random_device{}()};   // use a fixed seed like rng{42} to replay a run
float rand_range(float lo, float hi) {
    return std::uniform_real_distribution<float>(lo, hi)(rng);
}

int main() {
    const int W = 1100, H = 700;
    const float ARENA_SIZE = 600.0f;
    const Rectangle arena{(W - ARENA_SIZE) / 2.0f, (H - ARENA_SIZE) / 2.0f, ARENA_SIZE, ARENA_SIZE};
    const int PANEL_W = 220;
    const float DT = 1.0f / 120.0f;   // fixed physics step
    const float RESTITUTION = 1.0f;   // fully elastic

    InitWindow(W, H, "Summoner's Sim");
    SetTargetFPS(60);
    ChangeDirectory(GetApplicationDirectory());   // so "assets/..." paths resolve
    load_champion_defs();

    Champion a(get_def("Darius"));
    Champion b(get_def("Garen"));

    // a spawns in the left half of the arena, b in the right half, so they never start overlapping
    float mid_x = arena.x + arena.width / 2.0f;
    a.pos = {rand_range(arena.x + a.radius, mid_x - a.radius),
             rand_range(arena.y + a.radius, arena.y + arena.height - a.radius)};
    b.pos = {rand_range(mid_x + b.radius, arena.x + arena.width - b.radius),
             rand_range(arena.y + b.radius, arena.y + arena.height - b.radius)};

    float angle_a = rand_range(0, 2 * PI);
    float angle_b = rand_range(0, 2 * PI);
    a.set_direction({cosf(angle_a), sinf(angle_a)});
    b.set_direction({cosf(angle_b), sinf(angle_b)});

    float accumulator = 0.0f;

    while (!WindowShouldClose()) {
        accumulator += std::min(GetFrameTime(), 0.25f);

        while (accumulator >= DT) {
            for (Champion* c : {&a, &b}) {
                c->pos = Vector2Add(c->pos, Vector2Scale(c->vel, DT));
                bounce_off_walls(*c, arena);
            }

            if (resolve_collision(a, b, RESTITUTION)) {
                a.vel = Vector2Rotate(a.vel, rand_range(-0.05f, 0.05f));  // about +-3 degrees
                b.vel = Vector2Rotate(b.vel, rand_range(-0.05f, 0.05f));
            }

            a.enforce_speed();
            b.enforce_speed();
            accumulator -= DT;
        }

        BeginDrawing();
        ClearBackground(BLACK);
        DrawRectangleLinesEx(arena, 3, RAYWHITE);
        a.draw();
        b.draw();
        draw_stats_panel(a, 20, (int)arena.y, PANEL_W);
        draw_stats_panel(b, W - PANEL_W - 20, (int)arena.y, PANEL_W);
        EndDrawing();
    }

    unload_champion_defs();
    CloseWindow();
    return 0;
}
