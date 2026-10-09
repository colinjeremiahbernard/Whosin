
#include "RoomService.h"
#include "IdGenerator.h"

#include <mutex>
#include <algorithm>
#include <unordered_map>
#include <utility>

namespace
{
  std::unordered_map<std::string, Room> rooms;
  std::mutex roomsMutex;
}

std::string RoomService::createRoom(
    const std::string &hostName,
    Player &host)
{
  std::lock_guard<std::mutex> lock(roomsMutex);

  std::string code;
  do
  {
    code = IdGenerator::roomCode();
  } while (rooms.contains(code));

  host = Player{IdGenerator::playerId(), hostName};

  Room room;
  room.code = code;
  room.hostId = host.id;
  room.players.push_back(host);

  rooms.emplace(code, std::move(room));
  return code;
}

JoinRoomResult RoomService::addPlayer(
    const std::string &roomCode,
    const std::string &playerName,
    Player &player,
    int &playerCount)
{
  std::lock_guard<std::mutex> lock(roomsMutex);

  auto it = rooms.find(roomCode);

  if (it == rooms.end())
    return JoinRoomResult::RoomNotFound;

  Room &room = it->second;

  if (room.status != GameStatus::Waiting)
    return JoinRoomResult::GameAlreadyStarted;

  std::string id;
  bool duplicate;

  do
  {
    id = IdGenerator::playerId();
    duplicate = false;

    for (const Player &existing : room.players)
    {
      if (existing.id == id)
      {
        duplicate = true;
        break;
      }
    }
  } while (duplicate);

  player = Player{id, playerName};
  room.players.push_back(player);

  playerCount = static_cast<int>(room.players.size());

  return JoinRoomResult::Success;
}

bool RoomService::getRoom(
    const std::string &roomCode,
    Room &room)
{
  std::lock_guard<std::mutex> lock(roomsMutex);

  auto it = rooms.find(roomCode);

  if (it == rooms.end())
    return false;

  room = it->second;
  return true;
}

StartGameResult RoomService::startGame(
    const std::string &roomCode,
    const std::string &playerId)
{
  std::lock_guard<std::mutex> lock(roomsMutex);

  auto it = rooms.find(roomCode);

  if (it == rooms.end())
    return StartGameResult::RoomNotFound;

  Room &room = it->second;

  if (room.hostId != playerId)
    return StartGameResult::NotHost;

  if (room.status != GameStatus::Waiting)
    return StartGameResult::AlreadyStarted;

  room.status = GameStatus::Playing;

  return StartGameResult::Success;
}

void RoomService::setStatus(const std::string &roomCode, GameStatus status)
{
  std::lock_guard<std::mutex> lock(roomsMutex);
  auto it = rooms.find(roomCode);
  if (it != rooms.end()) it->second.status = status;
}

bool RoomService::removePlayer(
    const std::string &code, const std::string &playerId, Room &snapshot)
{
  std::lock_guard<std::mutex> lock(roomsMutex);
  auto found = rooms.find(code);
  if (found == rooms.end()) return false;
  auto &room = found->second;
  const auto previous = room.players.size();
  std::erase_if(room.players, [&](const Player &p) { return p.id == playerId; });
  if (room.players.size() == previous) return false;
  if (room.hostId == playerId)
    room.hostId = room.players.empty() ? "" : room.players.front().id;
  snapshot = room;
  if (room.players.empty()) rooms.erase(found);
  return true;
}


