#include "GameService.h"

bool GameService::leave(const std::string &code, const std::string &playerId)
{
    std::lock_guard<std::mutex> lock(mutex_);
    Room room;
    if (!rooms_.removePlayer(code, playerId, room)) return false;
    auto found = games_.find(code);
    if (found != games_.end())
    {
        if (room.players.empty()) games_.erase(found);
        else found->second.leave(playerId, room.hostId);
    }
    return true;
}

GameActionResult GameService::start(
    const std::string &code, const std::string &playerId)
{
    std::lock_guard<std::mutex> lock(mutex_);
    switch (rooms_.startGame(code, playerId))
    {
        case StartGameResult::RoomNotFound: return {404, "Room not found"};
        case StartGameResult::NotHost: return {403, "Only the host can start the game"};
        case StartGameResult::AlreadyStarted: return {409, "Game has already started"};
        case StartGameResult::Success: break;
    }
    Room room;
    rooms_.getRoom(code, room);
    games_.emplace(code, TriviaGame{room.hostId, room.players});
    return {};
}

bool GameService::get(const std::string &code, TriviaGame &game)
{
    std::lock_guard<std::mutex> lock(mutex_);
    const auto found = games_.find(code);
    if (found == games_.end()) return false;
    game = found->second;
    return true;
}

GameActionResult GameService::answer(
    const std::string &code, const std::string &playerId,
    int choice, int round, unsigned int generation)
{
    std::lock_guard<std::mutex> lock(mutex_);
    const auto found = games_.find(code);
    if (found == games_.end()) return {404, "Game not found"};
    return found->second.answer(playerId, choice, round, generation);
}

GameActionResult GameService::next(
    const std::string &code, const std::string &playerId,
    int round, unsigned int generation)
{
    std::lock_guard<std::mutex> lock(mutex_);
    const auto found = games_.find(code);
    if (found == games_.end()) return {404, "Game not found"};
    auto result = found->second.next(playerId, round, generation);
    if (result.status == 200 && found->second.phase == TriviaPhase::Finished)
        rooms_.setStatus(code, GameStatus::Finished);
    return result;
}

GameActionResult GameService::replay(
    const std::string &code, const std::string &playerId,
    unsigned int generation)
{
    std::lock_guard<std::mutex> lock(mutex_);
    const auto found = games_.find(code);
    if (found == games_.end()) return {404, "Game not found"};
    auto result = found->second.replay(playerId, generation);
    if (result.status == 200) rooms_.setStatus(code, GameStatus::Playing);
    return result;
}

