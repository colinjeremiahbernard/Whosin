#pragma once

#include "Questions.h"
#include <algorithm>
#include <random>
#include <vector>

using QuestionSelection = std::array<std::size_t, 5>;
static_assert(triviaQuestions.size() >= 10);

inline QuestionSelection selectQuestions(const QuestionSelection &previous = {
    triviaQuestions.size(), triviaQuestions.size(), triviaQuestions.size(),
    triviaQuestions.size(), triviaQuestions.size()})
{
    std::vector<std::size_t> available;
    for (std::size_t i = 0; i < triviaQuestions.size(); ++i)
        if (std::find(previous.begin(), previous.end(), i) == previous.end())
            available.push_back(i);
    thread_local std::mt19937 random{std::random_device{}()};
    std::shuffle(available.begin(), available.end(), random);
    QuestionSelection selection;
    std::copy_n(available.begin(), selection.size(), selection.begin());
    return selection;
}
