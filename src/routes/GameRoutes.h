
#pragma once

#include <crow.h>
#include "../services/RoomService.h"

inline void registerGameRoutes(
    crow::SimpleApp &app,
    RoomService &roomService)
{
  CROW_ROUTE(app, "/api/rooms/<string>/start")
      .methods(crow::HTTPMethod::POST)([&roomService](
                                           const crow::request &request,
                                           std::string roomCode)
                                       {
        const auto body = crow::json::load(request.body);

        if (!body || !body.has("playerId") ||
            body["playerId"].t() != crow::json::type::String)
        {
            return crow::response(
                400,
                R"({"error":"Player ID is required"})"
            );
        }

        const std::string playerId = body["playerId"].s();

        if (playerId.empty())
        {
            return crow::response(
                400,
                R"({"error":"Player ID cannot be empty"})"
            );
        }

        const auto result =
            roomService.startGame(roomCode, playerId);

        switch (result)
        {
            case StartGameResult::RoomNotFound:
                return crow::response(
                    404,
                    R"({"error":"Room not found"})"
                );

            case StartGameResult::NotHost:
                return crow::response(
                    403,
                    R"({"error":"Only the host can start the game"})"
                );

            case StartGameResult::AlreadyStarted:
                return crow::response(
                    409,
                    R"({"error":"Game has already started"})"
                );

            case StartGameResult::Success:
            {
                crow::json::wvalue response;
                response["roomCode"] = roomCode;
                response["status"] = "playing";

                return crow::response{response};
            }
        }

        return crow::response(500); });
}
