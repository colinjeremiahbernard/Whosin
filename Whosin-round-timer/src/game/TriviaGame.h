#pragma once

#include "Questions.h"
#include "../models/Player.h"
#include <string>
#include <chrono>
#include <unordered_map>
#include <vector>

enum class TriviaPhase { Question, Result, Finished };

struct GameActionResult
{
    int status = 200;
    std::string error;
};

struct TriviaGame
{
    std::string hostId;
    std::vector<Player> players;
    std::unordered_map<std::string, int> answers;
    std::unordered_map<std::string, int> scores;
    std::size_t round = 0;
    unsigned int generation = 1;
    TriviaPhase phase = TriviaPhase::Question;

    using Clock = std::chrono::steady_clock;
    Clock::time_point deadline = Clock::now() + std::chrono::seconds(20);
    bool expire(Clock::time_point now = Clock::now());
    void finishRound();
    void resetDeadline();

    TriviaGame() = default;
    TriviaGame(std::string host, std::vector<Player> members);
    void leave(const std::string &playerId, const std::string &newHost);
    void finishRoundIfReady();
    GameActionResult answer(const std::string &playerId, int choice,
                            int expectedRound, unsigned int expectedGame);
    GameActionResult next(const std::string &playerId,
                          int expectedRound, unsigned int expectedGame);
    GameActionResult replay(const std::string &playerId,
                            unsigned int expectedGame);
};
