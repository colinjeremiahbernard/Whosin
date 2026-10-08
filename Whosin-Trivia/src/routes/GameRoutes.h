#pragma once

#include <crow.h>
#include "../game/GameView.h"
#include "../services/GameService.h"
#include "../services/RoomUpdates.h"
#include <cmath>

inline bool gameString(const crow::json::rvalue &body, const char *key)
{
    return body && body.has(key) &&
           body[key].t() == crow::json::type::String &&
           !std::string(body[key].s()).empty();
}

inline bool gameInteger(const crow::json::rvalue &body, const char *key,
                        double minimum, double maximum)
{
    if (!body || !body.has(key) || body[key].t() != crow::json::type::Number)
        return false;
    const double value = body[key].d();
    return std::isfinite(value) && value >= minimum && value <= maximum &&
           std::floor(value) == value;
}

inline crow::response actionResponse(const GameActionResult &result,
                                     RoomUpdates &updates, const std::string &code)
{
    crow::json::wvalue data;
    if (result.status == 200)
    {
        updates.publish(code);
        data["ok"] = true;
    }
    else data["error"] = result.error;
    crow::response response{data};
    response.code = result.status;
    return response;
}

inline void registerGameRoutes(crow::SimpleApp &app,
                              GameService &games, RoomUpdates &updates)
{
    CROW_ROUTE(app, "/api/rooms/<string>/game").methods(crow::HTTPMethod::GET)(
        [&games](std::string code)
    {
        TriviaGame game;
        if (!games.get(code, game))
            return crow::response(404, R"({"error":"Game not found"})");
        crow::response response{gameView(game)};
        response.add_header("Cache-Control", "no-store");
        return response;
    });

    CROW_ROUTE(app, "/api/rooms/<string>/start").methods(crow::HTTPMethod::POST)(
        [&games, &updates](const crow::request &request, std::string code)
    {
        const auto body = crow::json::load(request.body);
        if (!gameString(body, "playerId"))
            return crow::response(400, R"({"error":"Player ID is required"})");
        return actionResponse(games.start(code, body["playerId"].s()), updates, code);
    });

    CROW_ROUTE(app, "/api/rooms/<string>/answer").methods(crow::HTTPMethod::POST)(
        [&games, &updates](const crow::request &request, std::string code)
    {
        const auto body = crow::json::load(request.body);
        if (!gameString(body, "playerId") || !gameInteger(body, "answer", 0, 3) ||
            !gameInteger(body, "round", 1, 5) ||
            !gameInteger(body, "generation", 1, 4294967295.0))
            return crow::response(400, R"({"error":"Invalid answer submission"})");
        return actionResponse(games.answer(code, body["playerId"].s(),
            static_cast<int>(body["answer"].d()), static_cast<int>(body["round"].d()),
            static_cast<unsigned int>(body["generation"].d())), updates, code);
    });

    CROW_ROUTE(app, "/api/rooms/<string>/next").methods(crow::HTTPMethod::POST)(
        [&games, &updates](const crow::request &request, std::string code)
    {
        const auto body = crow::json::load(request.body);
        if (!gameString(body, "playerId") || !gameInteger(body, "round", 1, 5) ||
            !gameInteger(body, "generation", 1, 4294967295.0))
            return crow::response(400, R"({"error":"Invalid next-round request"})");
        return actionResponse(games.next(code, body["playerId"].s(),
            static_cast<int>(body["round"].d()),
            static_cast<unsigned int>(body["generation"].d())), updates, code);
    });

    CROW_ROUTE(app, "/api/rooms/<string>/replay").methods(crow::HTTPMethod::POST)(
        [&games, &updates](const crow::request &request, std::string code)
    {
        const auto body = crow::json::load(request.body);
        if (!gameString(body, "playerId") ||
            !gameInteger(body, "generation", 1, 4294967295.0))
            return crow::response(400, R"({"error":"Invalid replay request"})");
        return actionResponse(games.replay(code, body["playerId"].s(),
            static_cast<unsigned int>(body["generation"].d())), updates, code);
    });
}
