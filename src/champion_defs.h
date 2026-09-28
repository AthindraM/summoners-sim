#pragma once
#include "champion.h"
#include <string>

void load_champion_defs();
const ChampionDef &get_def(const std::string &name);
void unload_champion_defs();
