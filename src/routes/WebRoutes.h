#pragma once

#include <crow.h>
#include "../security/RequestGuard.h"
#include "../services/RoomUpdates.h"
#include "../services/PresenceService.h"
#include <string>
#include <memory>
#include "../security/SocketGuard.h"

inline void registerWebRoutes(WhosinApp &app, RoomUpdates &updates,
                              PresenceService &presence, RoomService &rooms)
{
    CROW_ROUTE(app, "/")([]()
    {
        crow::response response;
        response.set_static_file_info("static/index.html");
        response.add_header("Cache-Control", "no-store");
        return response;
    });

    auto sockets = std::make_shared<SocketGuard>();
    CROW_WEBSOCKET_ROUTE(app, "/ws")
        .max_payload(1024)
        .onopen([sockets](crow::websocket::connection &connection)
        {
            if (!sockets->open(reinterpret_cast<std::uintptr_t>(&connection)))
            {
                connection.close("Server is full");
                return;
            }
            connection.send_text(R"({"type":"connected"})");
        })
        .onclose([&updates, &presence, sockets](crow::websocket::connection &connection,
                           const std::string &, auto...)
        {
            sockets->close(reinterpret_cast<std::uintptr_t>(&connection));
            updates.remove(connection);
            presence.disconnect(reinterpret_cast<PresenceService::SocketId>(&connection));
        })
        .onmessage([&updates, &presence, &rooms, sockets](crow::websocket::connection &connection,
                             const std::string &message, bool isBinary)
        {
            if (!sockets->message(reinterpret_cast<std::uintptr_t>(&connection)) ||
                isBinary || message.size() > 1024)
            {
                connection.close("Invalid message");
                return;
            }
            const auto body = crow::json::load(message);
            if (!body || !body.has("type") ||
                body["type"].t() != crow::json::type::String) return;
            const std::string type = body["type"].s();
            if ((type == "watch" || (type == "ping" && body.has("roomCode"))) &&
                (!body.has("sessionToken") || body["sessionToken"].t() != crow::json::type::String ||
                 !body.has("roomCode") || body["roomCode"].t() != crow::json::type::String ||
                 !body.has("playerId") || body["playerId"].t() != crow::json::type::String ||
                 !rooms.authenticate(body["roomCode"].s(), body["playerId"].s(), body["sessionToken"].s())))
            {
                connection.close("Invalid session");
                return;
            }
            if (type == "ping")
            {
                const auto socket = reinterpret_cast<PresenceService::SocketId>(&connection);
                if (body.has("roomCode") && body.has("playerId") &&
                    body["roomCode"].t() == crow::json::type::String &&
                    body["playerId"].t() == crow::json::type::String)
                {
                    if (!presence.watch(socket, body["roomCode"].s(), body["playerId"].s()))
                    {
                        connection.send_text(R"({"type":"session.expired"})");
                        return;
                    }
                }
                else presence.touch(socket);
                connection.send_text(R"({"type":"pong"})");
                return;
            }
            if (type == "unwatch")
            {
                updates.remove(connection);
                presence.disconnect(reinterpret_cast<PresenceService::SocketId>(&connection));
                return;
            }
            if (type == "watch" && body.has("roomCode") &&
                body["roomCode"].t() == crow::json::type::String &&
                body.has("playerId") && body["playerId"].t() == crow::json::type::String)
            {
                const std::string code = body["roomCode"].s();
                const std::string id = body["playerId"].s();
                if (code.size() == 6 && id.size() == 16 && presence.watch(
                    reinterpret_cast<PresenceService::SocketId>(&connection), code, id))
                {
                    updates.watch(connection, code);
                    return;
                }
                updates.remove(connection);
                connection.send_text(R"({"type":"session.expired"})");
                return;
            }
            connection.send_text(
                R"({"type":"error","message":"Invalid room subscription"})");
        });
}

