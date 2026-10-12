#include <crow.h>
#include "services/RoomService.h"
#include "services/GameService.h"
#include "services/RoomUpdates.h"
#include "services/PresenceService.h"
#include "routes/RoomRoutes.h"
#include "routes/GameRoutes.h"
#include "routes/WebRoutes.h"
#include <iostream>
#include "config/ServerConfig.h"

int main()
{
    unsigned short port;
    try { port = serverPort(); }
    catch (const std::exception &error)
    {
        std::cerr << error.what() << "\n";
        return 1;
    }
    RoomService rooms;
    GameService games(rooms);
    RoomUpdates updates;
    PresenceService presence(rooms, games);
    WhosinApp app;
    app.get_middleware<RequestGuard>().rooms = &rooms;

    registerRoomRoutes(app, rooms, updates, presence);
    registerGameRoutes(app, games, updates);
    registerWebRoutes(app, updates, presence, rooms);
    auto lastReport = std::chrono::steady_clock::now();
    app.tick(std::chrono::seconds(1), [&presence, &games, &updates, &rooms, &app, &lastReport]()
    {
        for (const auto &code : presence.sweep()) updates.publish(code);
        for (const auto &code : games.sweep()) updates.publish(code);
        const auto now = std::chrono::steady_clock::now();
        if (now - lastReport >= std::chrono::minutes(1))
        {
            auto &guard = app.get_middleware<RequestGuard>();
            std::clog << "Whosin metrics: rooms=" << rooms.roomCount()
                      << " requests=" << guard.requests.exchange(0)
                      << " errors=" << guard.errors.exchange(0) << "\n";
            lastReport = now;
        }
    });

    CROW_ROUTE(app, "/health")([]()
    {
        crow::json::wvalue data;
        data["status"] = "ok";
        crow::response response{data};
        response.add_header("Cache-Control", "no-store");
        return response;
    });
    std::cout << "Whosin starting on port " << port << "\n";
    app.bindaddr("0.0.0.0").port(port).concurrency(4).run();
    return 0;
}
