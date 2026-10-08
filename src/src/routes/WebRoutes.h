#pragma once

#include <crow.h>
#include "../services/RoomUpdates.h"
#include <string>

inline void registerWebRoutes(crow::SimpleApp &app, RoomUpdates &updates)
{
    CROW_ROUTE(app, "/")([]()
    {
        crow::response response;
        response.set_static_file_info("static/index.html");
        response.add_header("Cache-Control", "no-store");
        return response;
    });

    CROW_WEBSOCKET_ROUTE(app, "/ws")
        .onopen([](crow::websocket::connection &connection)
        {
            connection.send_text(R"({"type":"connected"})");
        })
        .onclose([&updates](crow::websocket::connection &connection,
                           const std::string &, auto...)
        {
            updates.remove(connection);
        })
        .onmessage([&updates](crow::websocket::connection &connection,
                             const std::string &message, bool isBinary)
        {
            if (isBinary || message.size() > 1024)
            {
                connection.close("Invalid message");
                return;
            }
            const auto body = crow::json::load(message);
            if (!body || !body.has("type") ||
                body["type"].t() != crow::json::type::String) return;
            const std::string type = body["type"].s();
            if (type == "ping")
            {
                connection.send_text(R"({"type":"pong"})");
                return;
            }
            if (type == "unwatch")
            {
                updates.remove(connection);
                return;
            }
            if (type == "watch" && body.has("roomCode") &&
                body["roomCode"].t() == crow::json::type::String)
            {
                const std::string code = body["roomCode"].s();
                if (code.size() == 6 && code.find_first_not_of(
                    "ABCDEFGHJKLMNPQRSTUVWXYZ23456789") == std::string::npos)
                {
                    updates.watch(connection, code);
                    return;
                }
            }
            connection.send_text(
                R"({"type":"error","message":"Invalid room subscription"})");
        });
}
