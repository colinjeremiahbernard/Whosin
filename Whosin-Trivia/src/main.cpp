#include <crow.h>
#include "services/RoomService.h"
#include "services/GameService.h"
#include "services/RoomUpdates.h"
#include "routes/RoomRoutes.h"
#include "routes/GameRoutes.h"
#include "routes/WebRoutes.h"
#include <iostream>

int main()
{
    RoomService rooms;
    GameService games(rooms);
    RoomUpdates updates;
    crow::SimpleApp app;

    registerRoomRoutes(app, rooms, updates);
    registerGameRoutes(app, games, updates);
    registerWebRoutes(app, updates);

    std::cout << "Whosin starting on http://localhost:8080\n";
    app.port(8080).multithreaded().run();
    return 0;
}
