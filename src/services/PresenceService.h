#pragma once

#include "GameService.h"
#include <chrono>
#include <cstdint>
#include <mutex>
#include <set>
#include <unordered_map>

class PresenceService
{
public:
    using Clock = std::chrono::steady_clock;
    using Time = Clock::time_point;
    using SocketId = std::uintptr_t;
    PresenceService(RoomService &rooms, GameService &games)
        : rooms_(rooms), games_(games) {}
    void track(const std::string &code, const std::string &id, Time now = Clock::now());
    bool watch(SocketId socket, const std::string &code, const std::string &id,
               Time now = Clock::now());
    bool leave(const std::string &code, const std::string &id);
    void touch(SocketId socket, Time now = Clock::now());
    void disconnect(SocketId socket, Time now = Clock::now());
    std::vector<std::string> sweep(Time now = Clock::now());

private:
    struct PlayerSession
    {
        std::string code, playerId;
        std::set<SocketId> sockets;
        Time disconnectedAt;
    };
    struct Link { std::string key; Time lastSeen; };
    void detach(SocketId socket, Time now);
    RoomService &rooms_;
    GameService &games_;
    std::mutex mutex_;
    std::unordered_map<std::string, PlayerSession> players_;
    std::unordered_map<SocketId, Link> links_;
};
