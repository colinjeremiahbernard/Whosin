#pragma once

#include "../models/Room.h"

#include <string>

class RoomService
{
public:
    std::string createRoom();

    bool addPlayer(
        const std::string &roomCode,
        const std::string &playerName,
        Player &player,
        int &playerCount);

    bool getRoom(
        const std::string &roomCode,
        Room &room);
};