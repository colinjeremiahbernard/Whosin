#pragma once

#include <array>
#include <string>

struct TriviaQuestion
{
    std::string text;
    std::array<std::string, 4> options;
    int correct;
};

inline const std::array<TriviaQuestion, 20> triviaQuestions{{
    {"Which planet is known as the Red Planet?",
     {"Venus", "Mars", "Jupiter", "Mercury"}, 1},
    {"How many sides does a hexagon have?",
     {"Five", "Seven", "Eight", "Six"}, 3},
    {"What is the capital of Guyana?",
     {"Georgetown", "Linden", "New Amsterdam", "Bartica"}, 0},
    {"Which ocean is the largest?",
     {"Atlantic", "Indian", "Pacific", "Arctic"}, 2},
    {"Which instrument typically has 88 keys?",
     {"Guitar", "Piano", "Violin", "Trumpet"}, 1},
    {"What is the chemical symbol for gold?", {"Ag", "Fe", "Au", "Cu"}, 2},
    {"How many minutes are in two hours?", {"60", "90", "100", "120"}, 3},
    {"Which animal is a mammal?", {"Dolphin", "Shark", "Trout", "Octopus"}, 0},
    {"Which continent contains Brazil?", {"Asia", "South America", "Europe", "Africa"}, 1},
    {"What is 9 multiplied by 7?", {"56", "72", "63", "81"}, 2},
    {"Which gas do plants absorb during photosynthesis?", {"Oxygen", "Helium", "Hydrogen", "Carbon dioxide"}, 3},
    {"Which shape has three sides?", {"Triangle", "Square", "Pentagon", "Circle"}, 0},
    {"Which planet is closest to the Sun?", {"Earth", "Mercury", "Saturn", "Neptune"}, 1},
    {"How many players are on the field for one football team at the start of a match?", {"Nine", "Ten", "Eleven", "Twelve"}, 2},
    {"Which language is primarily spoken in Brazil?", {"Spanish", "French", "English", "Portuguese"}, 3},
    {"What is the largest living land animal?", {"African elephant", "Giraffe", "Hippopotamus", "Rhinoceros"}, 0},
    {"How many degrees are in a right angle?", {"45", "90", "180", "360"}, 1},
    {"Which organ pumps blood around the body?", {"Liver", "Lung", "Heart", "Kidney"}, 2},
    {"Which month comes immediately after September?", {"August", "November", "December", "October"}, 3},
    {"Which is the smallest prime number?", {"2", "1", "3", "0"}, 0}
}};
