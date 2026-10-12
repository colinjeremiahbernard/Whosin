# Whosin

**Who’s in? Let’s play.**

Play online: https://whosin-f948.onrender.com

Whosin is a browser-based multiplayer trivia game. Create a room, share its
six-character code, and join from phones or computers. No accounts or installs.

## How to play

1. Create a room with your display name.
2. Friends enter the room code and their names (up to 12 players).
3. The host starts a five-round game.
4. Each question has a server-controlled 20-second timer. Correct answers earn
   one point; unanswered questions earn zero.
5. The host advances after the round results and reveals the final scoreboard.
6. Play Again resets scores and selects five questions excluding the previous game.

The question bank contains 20 questions. Players have 30 seconds to reconnect
after the server detects a disconnect. A departed host is replaced by the first
remaining player. Leaving immediately removes the player; empty rooms disappear.

## Implementation

- C++20 and Crow: HTTP, WebSockets, room state and authoritative scoring.
- HTML, CSS and JavaScript: responsive browser client, no frontend framework.
- CMake: builds the server.
- Docker: Linux deployment with pinned Crow v1.3.2.
- Render: one running service, HTTPS and WSS supplied by the hosting platform.

All active state is in memory. Restarts and deployments clear rooms. Multiple
server instances are unsupported. PostgreSQL and Redis are not required or
implemented in this MVP.

## Windows development

Install Crow through your existing vcpkg setup, then configure CMake with that
toolchain. For an already configured checkout:

```powershell
cmake --build build
.\build\Debug\whosin.exe
```

Run from the repository root and open http://localhost:8080.
`PORT` overrides 8080; invalid values cause startup to fail clearly.

## Docker and hosting

```sh
docker build -t whosin .
docker run --rm -p 8080:8080 whosin
```

See [deployment instructions](docs/DEPLOYMENT.md) and the
[operations guide](docs/OPERATIONS.md). `/health` returns `{"status":"ok"}`
when the process is responding. Free hosting can sleep; allow time for startup.

## Launch safeguards

- Private 32-character session tokens authorize room reads and player actions;
  visible player IDs alone do not grant access. Tokens stay in per-tab storage.
- HTTP request budgets, bounded rate-limit bookkeeping and 4 KiB body validation.
- Maximum 100 simultaneous rooms, 12 players per room and 512 WebSocket connections.
- WebSocket frames capped at 1 KiB and messages limited to 20 per second per socket.
- Server validates host permissions, round generation, answer choices and deadlines.
- Security headers restrict script sources, framing, and browser device permissions.
- Four worker threads and a non-root Docker process.
- Aggregate room, request and HTTP error counts logged once per minute without names or tokens.

Request limits use the directly connected peer address, never an untrusted
forwarded header. Behind a proxy, multiple visitors can share a budget; see the
operations guide. These safeguards do not replace upstream DDoS protection.

## Verification

`tests/trivia_replay.cpp` checks scoring, replays and stale submissions.
`tests/security_limits.cpp` checks rate limits and socket boundaries.
`tests/room_limits.cpp` checks room capacity and private-token authorization.
`tests/launch_smoke.py` runs HTTP and multiplayer checks against a local binary.
The WebSocket smoke test additionally requires Python's `websockets`.

```sh
g++ -std=c++20 tests/trivia_replay.cpp src/game/TriviaGame.cpp -o trivia-test
./trivia-test
g++ -std=c++20 -pthread tests/security_limits.cpp -o limits-test
./limits-test
g++ -std=c++20 -pthread tests/room_limits.cpp src/services/RoomService.cpp src/services/IdGenerator.cpp -o room-test
./room-test
python tests/launch_smoke.py /absolute/path/to/whosin
```

## Scope and next improvements

The playable MVP is deployed and cross-device play has been verified. Launch
hardening must be built and deployed before its safeguards apply to the live site.
This is suitable for a small-group release, not a claim of unlimited capacity.
Large-scale load testing, external uptime alerts, durable game history and
horizontal scaling remain separate operational or future work.

Names, scores and session identities are temporary. Whosin does not request
email, location, camera or microphone access. Hosting-provider logs are governed
by the provider's retention policy. Do not use display names to share personal data.
