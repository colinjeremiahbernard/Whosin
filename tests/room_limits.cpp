#include "../src/services/RoomService.h"
#include <cassert>

int main()
{
    RoomService rooms;
    Player host;
    const auto code = rooms.createRoom("Host", host);
    assert(rooms.authenticate(code, host.id, host.token));
    assert(!rooms.authenticate(code, host.id, ""));
    assert(!rooms.authenticate(code, host.id, std::string(32, 'x')));
    int count = 0;
    for (int i = 0; i < 11; ++i)
    {
        Player guest;
        assert(rooms.addPlayer(code, "Guest", guest, count) == JoinRoomResult::Success);
        assert(rooms.authenticate(code, guest.id, guest.token));
        assert(!rooms.authenticate(code, host.id, guest.token));
    }
    Player extra;
    assert(count == 12);
    assert(rooms.addPlayer(code, "Extra", extra, count) == JoinRoomResult::RoomFull);
    for (int i = 1; i < 100; ++i) assert(!rooms.createRoom("Host", extra).empty());
    assert(rooms.roomCount() == 100);
    assert(rooms.createRoom("Overflow", extra).empty());
    Room snapshot;
    assert(rooms.removePlayer(code, host.id, snapshot));
    assert(!rooms.authenticate(code, host.id, host.token));
    assert(rooms.addPlayer(code, "Replacement", extra, count) == JoinRoomResult::Success);
}
