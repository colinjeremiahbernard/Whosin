#pragma once

#include <crow.h>
#include <mutex>
#include <string>
#include <unordered_map>

class RoomUpdates
{
public:
    void watch(crow::websocket::connection &connection, const std::string &code)
    {
        std::lock_guard<std::mutex> lock(mutex_);
        subscriptions_[&connection] = code;
        connection.send_text(R"({"type":"room.updated"})");
    }

    void remove(crow::websocket::connection &connection)
    {
        std::lock_guard<std::mutex> lock(mutex_);
        subscriptions_.erase(&connection);
    }

    void publish(const std::string &code)
    {
        std::lock_guard<std::mutex> lock(mutex_);
        for (const auto &[connection, room] : subscriptions_)
            if (room == code) connection->send_text(R"({"type":"room.updated"})");
    }

private:
    std::mutex mutex_;
    std::unordered_map<crow::websocket::connection *, std::string> subscriptions_;
};
