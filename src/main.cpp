#include <crow.h>

#include "services/RoomService.h"

#include <iostream>
#include <string>
#include <vector>

int main()
{
  crow::SimpleApp app;
  RoomService roomService;

  CROW_ROUTE(app, "/")
  ([]()
   { return "Whosin server is running!"; });

  CROW_ROUTE(app, "/api/rooms")
      .methods(crow::HTTPMethod::POST)([&roomService]()
                                       {
        crow::json::wvalue response;
        response["roomCode"] = roomService.createRoom();

        return crow::response{response}; });

  CROW_ROUTE(app, "/api/rooms/<string>")
      .methods(crow::HTTPMethod::GET)([&roomService](std::string roomCode)
                                      {
        Room room;

        if (!roomService.getRoom(roomCode, room))
        {
            return crow::response(
                404,
                R"({"error":"Room not found"})"
            );
        }

        crow::json::wvalue response;
        response["roomCode"] = room.code;
        response["playerCount"] =
            static_cast<int>(room.players.size());

        std::vector<crow::json::wvalue> players;

        for (const Player& player : room.players)
        {
            crow::json::wvalue playerJson;
            playerJson["id"] = player.id;
            playerJson["name"] = player.name;

            players.push_back(std::move(playerJson));
        }

        response["players"] = std::move(players);

        return crow::response{response}; });

  CROW_ROUTE(app, "/api/rooms/<string>/join")
      .methods(crow::HTTPMethod::POST)([&roomService](
                                           const crow::request &request,
                                           std::string roomCode)
                                       {
        const auto body = crow::json::load(request.body);

        if (!body || !body.has("name"))
        {
            return crow::response(
                400,
                R"({"error":"Player name is required"})"
            );
        }

        const std::string playerName = body["name"].s();

        if (playerName.empty())
        {
            return crow::response(
                400,
                R"({"error":"Player name cannot be empty"})"
            );
        }

        Player player;
        int playerCount = 0;

        if (!roomService.addPlayer(
                roomCode,
                playerName,
                player,
                playerCount))
        {
            return crow::response(
                404,
                R"({"error":"Room not found"})"
            );
        }

        crow::json::wvalue response;
        response["roomCode"] = roomCode;
        response["playerId"] = player.id;
        response["playerName"] = player.name;
        response["playerCount"] = playerCount;

        return crow::response{response}; });

  std::cout
      << "Whosin server starting on http://localhost:8080\n";

  app.port(8080)
      .multithreaded()
      .run();

  return 0;
}