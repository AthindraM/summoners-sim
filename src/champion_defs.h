#pragma once
#include "champion.h"
#include <string>

void load_champion_defs();                       // call once, after InitWindow()
const ChampionDef& get_def(const std::string& name);
void unload_champion_defs();                     // call before CloseWindow()
