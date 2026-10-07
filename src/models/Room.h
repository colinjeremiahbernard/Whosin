#pragma once

#include "Player.h"

#include <string>
#include <vector>

struct Room
{
  std::string code;
  std::vector<Player> players;
};