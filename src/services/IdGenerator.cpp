#include "IdGenerator.h"

#include <random>

namespace
{
  std::string generate(
      const char *characters,
      std::size_t characterCount,
      std::size_t length)
  {
    static thread_local std::mt19937 generator{
        std::random_device{}()};

    std::uniform_int_distribution<std::size_t> distribution(
        0,
        characterCount - 1);

    std::string value;
    value.reserve(length);

    for (std::size_t i = 0; i < length; ++i)
    {
      value += characters[distribution(generator)];
    }

    return value;
  }
}

std::string IdGenerator::roomCode()
{
  static constexpr char characters[] =
      "ABCDEFGHJKLMNPQRSTUVWXYZ23456789";

  return generate(
      characters,
      sizeof(characters) - 1,
      6);
}

std::string IdGenerator::playerId()
{
  static constexpr char characters[] =
      "abcdefghijklmnopqrstuvwxyz"
      "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
      "0123456789";

  return generate(
      characters,
      sizeof(characters) - 1,
      16);
}