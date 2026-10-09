
#pragma once

#include "../models/Room.h"

#include <string>

enum class StartGameResult
{
    Success,
    RoomNotFound,
    NotHost,
    AlreadyStarted
};

enum class JoinRoomResult
{
    Success,
    RoomNotFound,
    GameAlreadyStarted
};

class RoomService
{
public:
    bool removePlayer(const std::string &code, const std::string &playerId, Room &room);

    void setStatus(const std::string &roomCode, GameStatus status);

    std::string createRoom(
        const std::string &hostName,
        Player &host);

    JoinRoomResult addPlayer(
        const std::string &roomCode,
        const std::string &playerName,
        Player &player,
        int &playerCount);

    bool getRoom(
        const std::string &roomCode,
        Room &room);

    StartGameResult startGame(
        const std::string &roomCode,
        const std::string &playerId);
};


