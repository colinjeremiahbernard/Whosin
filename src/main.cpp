#include <crow.h>
#include "services/RoomService.h"
#include "services/GameService.h"
#include "services/RoomUpdates.h"
#include "services/PresenceService.h"
#include "routes/RoomRoutes.h"
#include "routes/GameRoutes.h"
#include "routes/WebRoutes.h"
#include <iostream>

int main()
{
    RoomService rooms;
    GameService games(rooms);
    RoomUpdates updates;
    PresenceService presence(rooms, games);
    crow::SimpleApp app;

    registerRoomRoutes(app, rooms, updates, presence);
    registerGameRoutes(app, games, updates);
    registerWebRoutes(app, updates, presence);
    app.tick(std::chrono::seconds(1), [&presence, &updates]()
    {
        for (const auto &code : presence.sweep()) updates.publish(code);
    });

    std::cout << "Whosin starting on http://localhost:8080\n";
    app.port(8080).multithreaded().run();
    return 0;
}

