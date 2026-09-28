#pragma once
#include "champion.h"

bool resolve_collision(Champion &a, Champion &b, float restitution);
void bounce_off_walls(Champion &c, int w, int h);
