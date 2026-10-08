#pragma once

#include <array>
#include <string>

struct TriviaQuestion
{
    std::string text;
    std::array<std::string, 4> options;
    int correct;
};

inline const std::array<TriviaQuestion, 5> triviaQuestions{{
    {"Which planet is known as the Red Planet?",
     {"Venus", "Mars", "Jupiter", "Mercury"}, 1},
    {"How many sides does a hexagon have?",
     {"Five", "Seven", "Eight", "Six"}, 3},
    {"What is the capital of Guyana?",
     {"Georgetown", "Linden", "New Amsterdam", "Bartica"}, 0},
    {"Which ocean is the largest?",
     {"Atlantic", "Indian", "Pacific", "Arctic"}, 2},
    {"Which instrument typically has 88 keys?",
     {"Guitar", "Piano", "Violin", "Trumpet"}, 1}
}};
