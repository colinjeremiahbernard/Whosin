#pragma once

#include <charconv>
#include <cstdlib>
#include <stdexcept>
#include <string_view>

inline unsigned short serverPort()
{
    const char *value = std::getenv("PORT");
    if (!value) return 8080;
    const std::string_view text{value};
    unsigned int port = 0;
    const auto result = std::from_chars(text.data(), text.data() + text.size(), port);
    if (result.ec != std::errc{} || result.ptr != text.data() + text.size() ||
        port == 0 || port > 65535)
        throw std::runtime_error("PORT must be an integer between 1 and 65535");
    return static_cast<unsigned short>(port);
}
