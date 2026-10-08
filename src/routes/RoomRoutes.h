#pragma once

#include <crow.h>
#include "../services/RoomService.h"
#include "../services/RoomUpdates.h"
#include <utility>
#include <vector>

inline bool validName(const std::string &name)
{
    return !name.empty() && name.size() <= 120 &&
           name.find_first_not_of(" \t\r\n") != std::string::npos;
}

inline void registerRoomRoutes(crow::SimpleApp &app,
                              RoomService &rooms, RoomUpdates &updates)
{
    CROW_ROUTE(app, "/api/rooms").methods(crow::HTTPMethod::POST)(
        [&rooms](const crow::request &request)
    {
        const auto body = crow::json::load(request.body);
        if (!body || !body.has("name") ||
            body["name"].t() != crow::json::type::String)
            return crow::response(400, R"({"error":"Host name is required"})");
        const std::string name = body["name"].s();
        if (!validName(name))
            return crow::response(400, R"({"error":"Enter a valid host name"})");
        Player host;
        const auto code = rooms.createRoom(name, host);
        crow::json::wvalue data;
        data["roomCode"] = code;
        data["playerId"] = host.id;
        data["playerName"] = host.name;
        data["isHost"] = true;
        return crow::response{data};
    });

    CROW_ROUTE(app, "/api/rooms/<string>").methods(crow::HTTPMethod::GET)(
        [&rooms](std::string code)
    {
        Room room;
        if (!rooms.getRoom(code, room))
            return crow::response(404, R"({"error":"Room not found"})");
        crow::json::wvalue data;
        data["roomCode"] = room.code;
        data["hostId"] = room.hostId;
        data["playerCount"] = static_cast<int>(room.players.size());
        data["status"] = room.status == GameStatus::Waiting ? "waiting" :
                         room.status == GameStatus::Playing ? "playing" : "finished";
        std::vector<crow::json::wvalue> players;
        for (const auto &player : room.players)
        {
            crow::json::wvalue item;
            item["id"] = player.id;
            item["name"] = player.name;
            players.push_back(std::move(item));
        }
        data["players"] = std::move(players);
        crow::response response{data};
        response.add_header("Cache-Control", "no-store");
        return response;
    });

    CROW_ROUTE(app, "/api/rooms/<string>/join").methods(crow::HTTPMethod::POST)(
        [&rooms, &updates](const crow::request &request, std::string code)
    {
        const auto body = crow::json::load(request.body);
        if (!body || !body.has("name") ||
            body["name"].t() != crow::json::type::String)
            return crow::response(400, R"({"error":"Player name is required"})");
        const std::string name = body["name"].s();
        if (!validName(name))
            return crow::response(400, R"({"error":"Enter a valid player name"})");
        Player player;
        int count = 0;
        const auto result = rooms.addPlayer(code, name, player, count);
        if (result == JoinRoomResult::RoomNotFound)
            return crow::response(404, R"({"error":"Room not found"})");
        if (result == JoinRoomResult::GameAlreadyStarted)
            return crow::response(409, R"({"error":"Game has already started"})");
        updates.publish(code);
        crow::json::wvalue data;
        data["roomCode"] = code;
        data["playerId"] = player.id;
        data["playerName"] = player.name;
        data["playerCount"] = count;
        data["isHost"] = false;
        return crow::response{data};
    });
}
