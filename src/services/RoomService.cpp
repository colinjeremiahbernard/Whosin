#include "RoomService.h"
#include "IdGenerator.h"

#include <mutex>
#include <unordered_map>

namespace
{
  std::unordered_map<std::string, Room> rooms;
  std::mutex roomsMutex;
}

std::string RoomService::createRoom()
{
  std::lock_guard<std::mutex> lock(roomsMutex);

  std::string code;

  do
  {
    code = IdGenerator::roomCode();
  } while (rooms.contains(code));

  Room room;
  room.code = code;

  rooms.emplace(code, std::move(room));

  return code;
}

bool RoomService::addPlayer(
    const std::string &roomCode,
    const std::string &playerName,
    Player &player,
    int &playerCount)
{
  std::lock_guard<std::mutex> lock(roomsMutex);

  auto roomIterator = rooms.find(roomCode);

  if (roomIterator == rooms.end())
  {
    return false;
  }

  Room &room = roomIterator->second;

  player = Player{
      IdGenerator::playerId(),
      playerName};

  room.players.push_back(player);

  playerCount = static_cast<int>(room.players.size());

  return true;
}

bool RoomService::getRoom(
    const std::string &roomCode,
    Room &room)
{
  std::lock_guard<std::mutex> lock(roomsMutex);

  auto roomIterator = rooms.find(roomCode);

  if (roomIterator == rooms.end())
  {
    return false;
  }

  room = roomIterator->second;

  return true;
}