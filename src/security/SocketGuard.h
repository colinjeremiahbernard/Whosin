#pragma once
#include <chrono>
#include <cstdint>
#include <mutex>
#include <unordered_map>

class SocketGuard
{
public:
    using Clock = std::chrono::steady_clock;
    bool open(std::uintptr_t id)
    {
        std::lock_guard<std::mutex> lock(mutex_);
        if (sockets_.size() >= 512) return false;
        sockets_[id] = {Clock::now(), 0};
        return true;
    }
    bool message(std::uintptr_t id)
    {
        std::lock_guard<std::mutex> lock(mutex_);
        auto found = sockets_.find(id);
        if (found == sockets_.end()) return false;
        auto &entry = found->second;
        const auto now = Clock::now();
        if (now - entry.started >= std::chrono::seconds(1)) entry = {now, 0};
        return ++entry.count <= 20;
    }
    void close(std::uintptr_t id)
    {
        std::lock_guard<std::mutex> lock(mutex_);
        sockets_.erase(id);
    }
private:
    struct Entry { Clock::time_point started; int count; };
    std::mutex mutex_;
    std::unordered_map<std::uintptr_t, Entry> sockets_;
};
