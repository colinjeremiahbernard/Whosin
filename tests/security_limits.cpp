#include "../src/security/RateLimiter.h"
#include "../src/security/SocketGuard.h"
#include <cassert>

int main()
{
    const auto now = RateLimiter::Clock::now();
    RateLimiter limiter;
    for (int i = 0; i < 20; ++i) assert(limiter.allow("creator", true, true, now));
    assert(!limiter.allow("creator", true, true, now));
    assert(limiter.allow("creator", true, true, now + std::chrono::minutes(1)));
    for (int i = 0; i < 120; ++i) assert(limiter.allow("writer", true, false, now));
    assert(!limiter.allow("writer", true, false, now));
    for (int i = 0; i < 600; ++i) assert(limiter.allow("reader", false, false, now));
    assert(!limiter.allow("reader", false, false, now));
    SocketGuard sockets;
    assert(sockets.open(1));
    for (int i = 0; i < 20; ++i) assert(sockets.message(1));
    assert(!sockets.message(1));
    sockets.close(1);
    assert(!sockets.message(1));
    for (int i = 1; i <= 512; ++i) assert(sockets.open(i));
    assert(!sockets.open(513));
    sockets.close(1);
    assert(sockets.open(513));
}
