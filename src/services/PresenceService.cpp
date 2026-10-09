#include "PresenceService.h"
#include <algorithm>

void PresenceService::track(const std::string &code, const std::string &id, Time now)
{
    std::lock_guard<std::mutex> lock(mutex_);
    players_.try_emplace(code + ":" + id, PlayerSession{code, id, {}, now});
}

bool PresenceService::watch(
    SocketId socket, const std::string &code, const std::string &id, Time now)
{
    std::lock_guard<std::mutex> lock(mutex_);
    Room room;
    if (!rooms_.getRoom(code, room) ||
        std::none_of(room.players.begin(), room.players.end(),
                     [&](const Player &player) { return player.id == id; })) return false;
    const auto key = code + ":" + id;
    const auto existing = links_.find(socket);
    if (existing != links_.end() && existing->second.key == key)
    {
        existing->second.lastSeen = now;
        return true;
    }
    detach(socket, now);
    auto [player, inserted] = players_.try_emplace(key, PlayerSession{code, id, {}, now});
    player->second.sockets.insert(socket);
    links_[socket] = Link{key, now};
    return true;
}

void PresenceService::touch(SocketId socket, Time now)
{
    std::lock_guard<std::mutex> lock(mutex_);
    const auto found = links_.find(socket);
    if (found != links_.end()) found->second.lastSeen = now;
}

void PresenceService::detach(SocketId socket, Time now)
{
    const auto link = links_.find(socket);
    if (link == links_.end()) return;
    const auto player = players_.find(link->second.key);
    if (player != players_.end())
    {
        player->second.sockets.erase(socket);
        if (player->second.sockets.empty()) player->second.disconnectedAt = now;
    }
    links_.erase(link);
}

void PresenceService::disconnect(SocketId socket, Time now)
{
    std::lock_guard<std::mutex> lock(mutex_);
    detach(socket, now);
}

std::vector<std::string> PresenceService::sweep(Time now)
{
    std::lock_guard<std::mutex> lock(mutex_);
    std::vector<SocketId> stale;
    for (const auto &[socket, link] : links_)
        if (now - link.lastSeen >= std::chrono::seconds(45)) stale.push_back(socket);
    for (const auto socket : stale) detach(socket, now);

    std::set<std::string> changed;
    for (auto player = players_.begin(); player != players_.end();)
    {
        const auto &session = player->second;
        if (session.sockets.empty() &&
            now - session.disconnectedAt >= std::chrono::seconds(30))
        {
            if (games_.leave(session.code, session.playerId)) changed.insert(session.code);
            player = players_.erase(player);
        }
        else ++player;
    }
    return {changed.begin(), changed.end()};
}
