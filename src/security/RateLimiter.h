#pragma once
#include <chrono>
#include <mutex>
#include <string>
#include <unordered_map>

class RateLimiter
{
public:
    using Clock = std::chrono::steady_clock;
    bool allow(const std::string &key, bool write, bool create,
               Clock::time_point now = Clock::now())
    {
        std::lock_guard<std::mutex> lock(mutex_);
        if (now >= cleanup_)
        {
            std::erase_if(entries_, [&](const auto &entry)
            { return now - entry.second.started >= std::chrono::minutes(1); });
            cleanup_ = now + std::chrono::seconds(10);
        }
        auto found = entries_.find(key);
        if (found == entries_.end())
        {
            if (entries_.size() >= 2048) return false;
            found = entries_.emplace(key, Entry{now}).first;
        }
        auto &entry = found->second;
        if (now - entry.started >= std::chrono::minutes(1)) entry = Entry{now};
        if (++entry.total > 600) return false;
        if (write && ++entry.writes > 120) return false;
        return !create || ++entry.creates <= 20;
    }
private:
    struct Entry { Clock::time_point started; int total = 0, writes = 0, creates = 0; };
    std::mutex mutex_;
    std::unordered_map<std::string, Entry> entries_;
    Clock::time_point cleanup_{};
};
