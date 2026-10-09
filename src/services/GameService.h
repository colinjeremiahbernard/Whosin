#pragma once

#include "RoomService.h"
#include "../game/TriviaGame.h"
#include <mutex>
#include <unordered_map>

class GameService
{
public:
    explicit GameService(RoomService &rooms) : rooms_(rooms) {}
    GameActionResult start(const std::string &code, const std::string &playerId);
    bool get(const std::string &code, TriviaGame &game);
    bool leave(const std::string &code, const std::string &playerId);
    GameActionResult answer(const std::string &code, const std::string &playerId,
                            int choice, int round, unsigned int generation);
    GameActionResult next(const std::string &code, const std::string &playerId,
                          int round, unsigned int generation);
    GameActionResult replay(const std::string &code, const std::string &playerId,
                            unsigned int generation);

private:
    RoomService &rooms_;
    std::mutex mutex_;
    std::unordered_map<std::string, TriviaGame> games_;
};

