#include "../src/game/TriviaGame.h"
#include <cassert>
#include <set>

int main()
{
    for (int trial = 0; trial < 100; ++trial)
    {
        TriviaGame game{"host", {{"host", "Colin"}, {"guest", "Guest"}}};
        const auto initial = game.questions;
        assert(std::set<std::size_t>(initial.begin(), initial.end()).size() == 5);
        for (int round = 1; round <= 5; ++round)
        {
            const auto correct = triviaQuestions[game.questions[game.round]].correct;
            assert(game.answer("host", correct, round, 1).status == 200);
            assert(game.answer("guest", (correct + 1) % 4, round, 1).status == 200);
            assert(game.next("host", round, 1).status == 200);
        }
        assert(game.phase == TriviaPhase::Finished);
        assert(game.scores.at("host") == 5);
        assert(game.scores.at("guest") == 0);
        assert(game.replay("guest", 1).status == 403);
        assert(game.replay("host", 1).status == 200);
        assert(game.generation == 2 && game.round == 0);
        assert(game.scores.at("host") == 0);
        assert(std::set<std::size_t>(game.questions.begin(), game.questions.end()).size() == 5);
        for (auto id : game.questions)
        {
            assert(id < triviaQuestions.size());
            assert(std::find(initial.begin(), initial.end(), id) == initial.end());
        }
        assert(game.answer("host", 0, 1, 1).status == 409);
        const auto snapshot = game.questions;
        auto copied = game;
        assert(copied.questions == snapshot);
    }
    TriviaGame timed{"host", {{"host", "Colin"}}};
    assert(timed.expire(timed.deadline));
    assert(timed.phase == TriviaPhase::Result && timed.scores.at("host") == 0);
}
