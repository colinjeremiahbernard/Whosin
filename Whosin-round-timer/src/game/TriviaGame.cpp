#include "TriviaGame.h"
#include <utility>
#include <algorithm>

TriviaGame::TriviaGame(std::string host, std::vector<Player> members)
    : hostId(std::move(host)), players(std::move(members))
{
    for (const auto &player : players) scores[player.id] = 0;
}

GameActionResult TriviaGame::answer(
    const std::string &playerId, int choice,
    int expectedRound, unsigned int expectedGame)
{
    if (!scores.contains(playerId)) return {403, "Player is not in this game"};
    if (expectedGame != generation || expectedRound != static_cast<int>(round + 1))
        return {409, "The game has moved on. Try again"};
    if (phase != TriviaPhase::Question || Clock::now() >= deadline)
        return {409, "This round has ended"};
    if (choice < 0 || choice >= 4) return {400, "Choose a valid answer"};
    if (answers.contains(playerId)) return {409, "You have already answered"};

    answers.emplace(playerId, choice);

    finishRoundIfReady();
    return {};
}

void TriviaGame::finishRoundIfReady()
{
    if (phase != TriviaPhase::Question || players.empty() ||
        answers.size() != players.size()) return;
    finishRound();
}

void TriviaGame::resetDeadline()
{
    deadline = Clock::now() + std::chrono::seconds(20);
}

bool TriviaGame::expire(Clock::time_point now)
{
    if (phase != TriviaPhase::Question || now < deadline) return false;
    finishRound();
    return true;
}

void TriviaGame::finishRound()
{
    if (phase != TriviaPhase::Question) return;
    for (const auto &[id, submitted] : answers)
        if (submitted == triviaQuestions[round].correct) ++scores.at(id);
    phase = TriviaPhase::Result;
}

void TriviaGame::leave(const std::string &playerId, const std::string &newHost)
{
    std::erase_if(players, [&](const Player &p) { return p.id == playerId; });
    answers.erase(playerId);
    scores.erase(playerId);
    hostId = newHost;
    finishRoundIfReady();
}

GameActionResult TriviaGame::next(
    const std::string &playerId,
    int expectedRound, unsigned int expectedGame)
{
    if (playerId != hostId) return {403, "Only the host can advance the game"};
    if (expectedGame != generation || expectedRound != static_cast<int>(round + 1))
        return {409, "The game has moved on. Try again"};
    if (phase != TriviaPhase::Result) return {409, "Wait for the round results"};

    if (round + 1 == triviaQuestions.size())
        phase = TriviaPhase::Finished;
    else
    {
        ++round;
        answers.clear();
        phase = TriviaPhase::Question;
        resetDeadline();
    }
    return {};
}

GameActionResult TriviaGame::replay(
    const std::string &playerId, unsigned int expectedGame)
{
    if (playerId != hostId) return {403, "Only the host can start another game"};
    if (expectedGame != generation) return {409, "The game has moved on. Try again"};
    if (phase != TriviaPhase::Finished) return {409, "Finish this game first"};

    ++generation;
    round = 0;
    answers.clear();
    for (auto &[id, score] : scores) score = 0;
    phase = TriviaPhase::Question;
    resetDeadline();
    return {};
}
