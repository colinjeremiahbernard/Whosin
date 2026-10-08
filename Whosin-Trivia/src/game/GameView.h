#pragma once

#include <crow.h>
#include "TriviaGame.h"
#include <utility>
#include <vector>

inline crow::json::wvalue gameView(const TriviaGame &game)
{
    crow::json::wvalue data;
    data["round"] = static_cast<int>(game.round + 1);
    data["totalRounds"] = static_cast<int>(triviaQuestions.size());
    data["generation"] = game.generation;
    data["hostId"] = game.hostId;
    data["phase"] = game.phase == TriviaPhase::Question ? "question" :
                    game.phase == TriviaPhase::Result ? "result" : "finished";
    data["submittedCount"] = static_cast<int>(game.answers.size());
    data["playerCount"] = static_cast<int>(game.players.size());

    const auto &question = triviaQuestions[game.round];
    data["question"] = question.text;
    std::vector<crow::json::wvalue> options;
    for (const auto &option : question.options) options.emplace_back(option);
    data["options"] = std::move(options);

    // Never send the correct answer or other players' choices during a question.
    if (game.phase != TriviaPhase::Question)
        data["correctAnswer"] = question.correct;

    std::vector<crow::json::wvalue> players;
    for (const auto &player : game.players)
    {
        crow::json::wvalue item;
        item["id"] = player.id;
        item["name"] = player.name;
        item["score"] = game.scores.at(player.id);
        item["submitted"] = game.answers.contains(player.id);
        if (game.phase != TriviaPhase::Question && game.answers.contains(player.id))
            item["answer"] = game.answers.at(player.id);
        players.push_back(std::move(item));
    }
    data["players"] = std::move(players);
    return data;
}
