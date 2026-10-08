
#include <crow.h>
#include "services/RoomService.h"
#include "routes/GameRoutes.h"

#include <iostream>
#include <string>
#include <utility>
#include <vector>

int main()
{
    crow::SimpleApp app;
    RoomService roomService;

    registerGameRoutes(app, roomService);

    CROW_ROUTE(app, "/")
    ([]()
     { return "Whosin server is running!"; });

    CROW_ROUTE(app, "/api/rooms")
        .methods(crow::HTTPMethod::POST)([&roomService](const crow::request &request)
                                         {
        const auto body = crow::json::load(request.body);

        if (!body || !body.has("name") ||
            body["name"].t() != crow::json::type::String)
        {
            return crow::response(
                400, R"({"error":"Host name is required"})");
        }

        const std::string name = body["name"].s();

        if (name.empty())
        {
            return crow::response(
                400, R"({"error":"Host name cannot be empty"})");
        }

        Player host;
        const std::string code =
            roomService.createRoom(name, host);

        crow::json::wvalue response;
        response["roomCode"] = code;
        response["playerId"] = host.id;
        response["playerName"] = host.name;
        response["isHost"] = true;

        return crow::response{response}; });

    CROW_ROUTE(app, "/api/rooms/<string>")
        .methods(crow::HTTPMethod::GET)([&roomService](std::string code)
                                        {
        Room room;

        if (!roomService.getRoom(code, room))
        {
            return crow::response(
                404, R"({"error":"Room not found"})");
        }

        crow::json::wvalue response;
        response["roomCode"] = room.code;
        response["hostId"] = room.hostId;
        response["playerCount"] =
            static_cast<int>(room.players.size());

        switch (room.status)
        {
            case GameStatus::Waiting:
                response["status"] = "waiting";
                break;

            case GameStatus::Playing:
                response["status"] = "playing";
                break;

            case GameStatus::Finished:
                response["status"] = "finished";
                break;
        }

        std::vector<crow::json::wvalue> players;

        for (const Player& player : room.players)
        {
            crow::json::wvalue item;
            item["id"] = player.id;
            item["name"] = player.name;
            players.push_back(std::move(item));
        }

        response["players"] = std::move(players);

        return crow::response{response}; });

    CROW_ROUTE(app, "/api/rooms/<string>/join")
        .methods(crow::HTTPMethod::POST)([&roomService](
                                             const crow::request &request,
                                             std::string code)
                                         {
        const auto body = crow::json::load(request.body);

        if (!body || !body.has("name") ||
            body["name"].t() != crow::json::type::String)
        {
            return crow::response(
                400, R"({"error":"Player name is required"})");
        }

        const std::string name = body["name"].s();

        if (name.empty())
        {
            return crow::response(
                400, R"({"error":"Player name cannot be empty"})");
        }

        Player player;
        int count = 0;

        const auto result =
            roomService.addPlayer(code, name, player, count);

        if (result == JoinRoomResult::RoomNotFound)
        {
            return crow::response(
                404, R"({"error":"Room not found"})");
        }

        if (result == JoinRoomResult::GameAlreadyStarted)
        {
            return crow::response(
                409, R"({"error":"Game has already started"})");
        }

        crow::json::wvalue response;
        response["roomCode"] = code;
        response["playerId"] = player.id;
        response["playerName"] = player.name;
        response["playerCount"] = count;
        response["isHost"] = false;

        return crow::response{response}; });

    std::cout
        << "Whosin server starting on http://localhost:8080\n";

    app.port(8080).multithreaded().run();
    return 0;
}
