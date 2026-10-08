
#pragma once

#include "Player.h"

#include <string>
#include <vector>

enum class GameStatus
{
  Waiting,
  Playing,
  Finished
};

struct Room
{
  std::string code;
  std::string hostId;
  std::vector<Player> players;

  GameStatus status = GameStatus::Waiting;
};
