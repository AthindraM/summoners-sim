#pragma once
#include "champion.h"

// Returns true if the two champions were overlapping this step.
bool resolve_collision(Champion& a, Champion& b, float restitution);
void bounce_off_walls(Champion& c, Rectangle arena);
