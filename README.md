# WHOSIN

> **Who’s in? Let’s play.**

Whosin is a browser-based, real-time multiplayer game designed for spontaneous group play.

One person creates a game, receives a short room code, and shares it with the group. Everyone joins from their own phone, tablet, or laptop using a browser.

No account.  
No app download.  
No installation.

The same public Whosin URL can be used again and again for new games.

---

# 1. Product Vision

Whosin should make starting a multiplayer game almost frictionless.

The ideal experience is:

1. Open Whosin.
2. Tap **Create Game**.
3. Receive a short room code.
4. Share the code with friends.
5. Everyone joins from their own device.
6. The host starts the game.
7. Players interact in real time.
8. Everyone sees synchronized results.
9. The game ends with a clear result.
10. The group can immediately play again.

The product should feel closer to joining a conversation than installing and configuring a game.

---

# 2. Core Principles

## Zero Friction

No account creation, passwords, downloads, or app installation.

## Real-Time

The game state must remain synchronized across every connected device.

## Room-Based

Every game exists inside a temporary room identified by a short human-readable code.

## Host-Controlled

One player acts as the host and controls game progression.

## Replayable

Finishing a game should naturally lead into another game.

## Device Agnostic

The experience must work well on:

- iPhone
- Android phones
- Tablets
- Windows laptops
- Mac laptops
- Desktop browsers

## Mobile First

Most players will probably hold their phones while playing together in the same physical space.

## Simple to Understand

Players should be able to join and begin playing without technical knowledge or lengthy instructions.

---

# 3. Working Brand

## Name

**Whosin**

## Brand Idea

The name comes from:

> **Who’s in?**

It represents the social action required to start a game: getting people into the room.

## Primary Tagline

**Who’s in? Let’s play.**

## Brand Vocabulary

Use natural language throughout the interface:

- Create Game
- Join Game
- Room Code
- Players
- Host
- Start Game
- Next Round
- Play Again
- Leave Game

Avoid unnecessary technical terminology in the player experience.

---

# 4. MVP Definition

The first release must provide a complete playable multiplayer experience.

## Required MVP Functionality

### Landing Page

Players can:

- Create a game
- Join an existing game

### Create Game

The creator becomes the host.

The server generates a unique room code.

Example:

```text
WHOSIN

Create a Game

Room Code:
7K4P
```

### Join Game

A player enters:

- Room code
- Display name

Example:

```text
JOIN GAME

Room Code
[ 7 K 4 P ]

Your Name
[ Colin ]

[ JOIN ]
```

### Lobby

Players see:

- Room code
- Player list
- Host indicator
- Start button for host
- Waiting state for other players

### Game

The game engine controls:

- Current round
- Timer
- Player interaction
- Submitted answers/actions
- Round completion
- Scores
- Progression

### Results

Players see:

- Round result
- Current scores
- Final result where applicable
- Next-round control

### Replay

The host can start another game without requiring everyone to leave and reconnect.

---

# 5. Game Design

Whosin consists of a reusable multiplayer room platform and one or more game modes.

The architecture must separate the room system from the game engine so additional games can be introduced later.

## Game Engine Requirements

A game mode should define:

```text
Game Configuration
        ↓
Rounds
        ↓
Player Actions
        ↓
Validation
        ↓
Scoring
        ↓
Round Result
        ↓
Next Round
        ↓
Final Result
```

The room system should not need to understand the individual rules of every game.

---

# 6. Room Model

Every active game has a room.

Example:

```text
Room
├── Code: 7K4P
├── Host: player_01
├── Status: LOBBY
├── Players
│   ├── player_01
│   ├── player_02
│   ├── player_03
│   └── player_04
├── Game State
└── Created At
```

## Room States

```text
LOBBY
  ↓
STARTING
  ↓
PLAYING
  ↓
ROUND_RESULT
  ↓
PLAYING
  ↓
FINAL_RESULT
  ↓
REPLAY / CLOSED
```

---

# 7. Room Codes

Room codes should be:

- Short
- Easy to read
- Easy to communicate verbally
- Case-insensitive
- Difficult to accidentally confuse

Recommended initial format:

```text
4 characters
A-Z + 2-9
```

Exclude visually confusing characters such as:

```text
0
O
I
1
```

Examples:

```text
7K4P
M8RX
T6QD
```

Room codes only need to be unique among active rooms.

---

# 8. Player Identity

Whosin does not require accounts in the MVP.

A player receives a temporary identity when joining a room.

Example:

```text
playerId = UUID
```

The player also has:

```text
displayName
```

The identity exists only for the relevant game session.

No password or permanent profile is required.

---

# 9. Host

The player who creates a room becomes the host.

The host can:

- Start the game
- Advance the game when appropriate
- Start another round/game
- End the room

The server, rather than the client, must enforce host permissions.

If the host disconnects, the system should support host reassignment.

Recommended initial behavior:

> Automatically promote the longest-connected remaining player.

---

# 10. Technology Stack

Whosin will use a **C++-centered backend architecture**.

## Frontend

The browser client will initially use:

- HTML
- CSS
- JavaScript

The frontend should remain lightweight and framework-independent during the initial build.

A frontend framework can be introduced later if the application complexity justifies it.

## Backend

**C++20**

The C++ server will contain:

- HTTP handling
- WebSocket connections
- Room management
- Player management
- Game state
- Game rules
- Validation
- Scoring
- Host permissions
- Timers
- Reconnection handling

## Build System

**CMake**

CMake will manage:

- C++ compilation
- Dependencies
- Development builds
- Test builds
- Production builds

## Real-Time Communication

**WebSocket**

WebSockets provide persistent, bidirectional communication between:

```text
Browser ↔ C++ Game Server
```

## Database

**PostgreSQL**

PostgreSQL will be used for durable data such as:

- Game metadata
- Completed game records
- Scores where persistence is required
- Future analytics
- Future account functionality

## Ephemeral State

The initial development version should keep active room state in the C++ server.

**Redis is not required for the first local implementation.**

Redis can be introduced later for:

- Distributed room state
- Pub/Sub
- Multi-server synchronization
- Horizontal scaling
- Room expiration

This prevents unnecessary infrastructure from being introduced before it is needed.

---

# 11. High-Level Architecture

```text
                         INTERNET
                            │
                            ▼
                   ┌──────────────────┐
                   │     Browser      │
                   │                  │
                   │ HTML             │
                   │ CSS              │
                   │ JavaScript       │
                   └────────┬─────────┘
                            │
                         HTTPS
                            │
                         WSS
                            │
                   ┌────────▼─────────┐
                   │   C++20 Server   │
                   │                  │
                   │ HTTP Server      │
                   │ WebSocket Server │
                   │ Room Manager     │
                   │ Game Engine      │
                   │ Player Manager   │
                   │ Validation       │
                   │ Scoring          │
                   └────────┬─────────┘
                            │
                     ┌──────▼──────┐
                     │ PostgreSQL  │
                     │             │
                     │ Durable     │
                     │ Game Data   │
                     └─────────────┘
```

Future scaling:

```text
                         INTERNET
                            │
                            ▼
                     Load Balancer
                            │
              ┌─────────────┼─────────────┐
              │             │             │
              ▼             ▼             ▼
         C++ Server A  C++ Server B  C++ Server C
              │             │             │
              └─────────────┼─────────────┘
                            │
                           Redis
                            │
                       PostgreSQL
```

---

# 12. Server Authority

The client must never be trusted to determine important game outcomes.

For example, the browser may send:

```text
SUBMIT_ANSWER
```

The server determines:

```text
Is the player in the room?
Is the round active?
Has the player already submitted?
Is the answer valid?
What score should be awarded?
Has the round completed?
```

The server then updates the game state and broadcasts the result.

This protects against:

- Score manipulation
- Duplicate submissions
- Invalid actions
- Client-side timer manipulation
- Host permission bypasses

---

# 13. WebSocket Architecture

The WebSocket connection is the primary real-time channel.

## Client → Server

Example events:

```text
room.create
room.join
room.leave

game.start
game.action
game.next
game.replay
game.end
```

## Server → Client

Example events:

```text
room.created
room.updated

player.joined
player.left

game.started
game.state
game.actionAccepted
game.roundComplete
game.finished

host.changed

error
```

The protocol should use structured JSON initially.

Example:

```json
{
  "type": "room.join",
  "payload": {
    "roomCode": "7K4P",
    "displayName": "Colin"
  }
}
```

The exact event names and payload schemas may evolve during implementation.

---

# 14. Game State

A representative server-side state:

```cpp
GameState {
    RoomCode roomCode;
    GameStatus status;

    PlayerId hostId;

    std::vector<Player> players;

    int currentRound;
    int totalRounds;

    Timestamp roundStartedAt;
    Timestamp roundEndsAt;

    std::unordered_map<PlayerId, int> scores;

    GameData gameData;
};
```

The actual implementation should use strongly typed C++ structures rather than loosely typed data wherever practical.

---

# 15. Timer Architecture

Timers must be server-authoritative.

The server maintains the authoritative round deadline.

Example:

```text
roundEndsAt = 12:35:20.000
```

The client receives the deadline and calculates the remaining display time.

The server determines when the round has actually ended.

This prevents clients from manipulating or drifting away from the official timer.

---

# 16. Disconnect Handling

Players will inevitably lose connectivity.

Whosin should distinguish between:

## Temporary Disconnect

Keep the player in the room for a short grace period.

If the player reconnects, restore their session.

## Permanent Departure

Remove the player after the grace period.

## Host Disconnect

Transfer host authority according to the host reassignment rule.

## Empty Room

If no players remain, the room should eventually expire.

---

# 17. Room Expiration

Rooms are temporary by default.

Example policy:

```text
Active room:
expires after configurable inactivity period

Empty room:
expires after shorter period

Completed game:
may be persisted as historical data
```

The initial implementation may use C++ timers for room cleanup.

Redis TTLs can be introduced when distributed deployment is required.

---

# 18. Database Model

Initial PostgreSQL schema:

```text
games
-------------------------
id
room_code
status
game_type
created_at
started_at
ended_at


game_players
-------------------------
id
game_id
player_id
display_name
joined_at
final_score


game_rounds
-------------------------
id
game_id
round_number
started_at
ended_at
round_data
result_data
```

The schema should remain flexible enough to support multiple game modes.

---

# 19. HTTP API

The C++ server may expose a small HTTP API.

Initial endpoints:

```text
GET  /health
POST /rooms
GET  /rooms/:code
```

Real-time gameplay actions should primarily use WebSockets.

The REST API should not become a replacement for the real-time protocol.

---

# 20. Frontend Structure

The initial frontend should be simple:

```text
apps/
└── web/
    ├── index.html
    ├── join.html
    ├── room.html
    ├── css/
    │   └── styles.css
    ├── js/
    │   ├── app.js
    │   ├── websocket.js
    │   ├── room.js
    │   └── game.js
    └── assets/
```

This structure can evolve into a frontend framework later if necessary.

---

# 21. C++ Server Structure

Recommended initial structure:

```text
server/
├── CMakeLists.txt
├── include/
│   ├── server/
│   ├── room/
│   ├── player/
│   ├── game/
│   └── protocol/
│
├── src/
│   ├── main.cpp
│   ├── server/
│   ├── room/
│   ├── player/
│   ├── game/
│   └── protocol/
│
└── tests/
```

The exact library-specific structure will be determined after selecting the C++ networking stack.

---

# 22. C++ Dependency Strategy

Dependencies should be introduced deliberately.

The server requires libraries for:

- HTTP
- WebSockets
- JSON
- PostgreSQL connectivity
- Testing

Candidate libraries may include established C++ ecosystem solutions such as:

- Boost.Asio / Boost.Beast
- uWebSockets
- Crow
- Drogon
- nlohmann/json
- libpq / PostgreSQL client libraries

The final selection should be made during the server-foundation phase based on:

- C++20 compatibility
- Windows support
- WebSocket support
- Documentation
- Maintenance
- Performance
- Build complexity
- License compatibility

We should **not install every candidate library**.

---

# 23. Security

Even without accounts, the application requires security controls.

Required:

- HTTPS
- Secure WebSockets
- Server-side validation
- Input length limits
- Display-name sanitization
- Rate limiting
- Room-code validation
- Host permission validation
- Payload size limits
- Abuse prevention
- CORS configuration
- Security headers

Player input must always be considered untrusted.

---

# 24. Display Names

Display names should have:

- Maximum length
- Minimum valid length
- Character validation
- Sanitization

The browser must never inject raw player input into HTML.

---

# 25. Anti-Cheating

Whosin is intended primarily for social play.

The server must nevertheless control:

- Scores
- Timers
- Round completion
- Valid actions
- Player membership
- Host permissions

The client should submit actions, not results.

Bad:

```text
{
    "score": 500
}
```

Good:

```text
{
    "answer": "X"
}
```

The C++ game engine calculates the score.

---

# 26. Error Handling

Player-facing errors should be understandable.

Instead of:

```text
ROOM_NOT_FOUND
```

show:

> **We couldn't find that room.**  
> Check the code and try again.

Examples:

> **That room is full.**

> **The game has already started.**

> **You've been disconnected. Reconnecting…**

> **The host has left. Finding a new host…**

Technical details should be logged server-side.

---

# 27. Logging

The C++ server should provide structured logging for:

- Server startup
- Server shutdown
- Room creation
- Room destruction
- Player joins
- Player leaves
- Host changes
- Game start
- Game completion
- Errors
- WebSocket failures

Do not log unnecessary personal information.

---

# 28. Observability

Production monitoring should eventually include:

```text
active_rooms
active_players
games_started
games_completed
average_players_per_game
average_game_duration
disconnect_rate
reconnect_rate
```

A health endpoint should report whether the server is operational.

---

# 29. Privacy

MVP should collect as little personal information as possible.

No account is required.

Player names should be considered temporary game-session data.

The system should have a clear privacy policy before public launch.

Do not collect:

- Contacts
- Location
- Phone numbers
- Email addresses

unless a future feature explicitly requires them.

---

# 30. Responsive UX

The application must be mobile-first.

## Mobile

The interface should:

- Use large touch targets
- Avoid tiny text
- Minimize typing
- Work in portrait orientation
- Remain usable on smaller screens
- Provide clear visual game states

## Desktop

Desktop layouts can provide:

- Larger player lists
- More whitespace
- Larger game presentation
- Additional host controls

The gameplay rules remain identical.

---

# 31. Accessibility

Whosin should target WCAG 2.2 AA principles where practical.

Requirements include:

- Keyboard navigation
- Visible focus states
- Sufficient text contrast
- Semantic HTML
- Accessible buttons
- Screen-reader-friendly status changes
- Reduced-motion consideration
- Avoiding color as the sole communication method

---

# 32. Testing Strategy

## Unit Tests

Test:

- Game rules
- Scoring
- Room-code generation
- State transitions
- Validation
- Timer logic

## Integration Tests

Test:

- Room creation
- Joining
- Leaving
- Host transfer
- Game start
- Game progression
- Reconnection

## End-to-End Tests

Simulate multiple browser clients.

Example:

```text
Browser A → creates room

Browser B → joins

Browser C → joins

Browser A → starts game

B/C → perform actions

Server → calculates result

All browsers → receive identical state
```

## Load Testing

Eventually test:

- Many simultaneous rooms
- Multiple players per room
- WebSocket connections
- Reconnection storms
- Room expiration

---

# 33. Repository Structure

The project will evolve toward:

```text
Whosin/
│
├── README.md
├── .gitignore
├── CMakeLists.txt
│
├── apps/
│   └── web/
│
├── server/
│   ├── include/
│   ├── src/
│   └── tests/
│
├── packages/
│   ├── protocol/
│   └── game-engine/
│
├── database/
│   ├── migrations/
│   └── seeds/
│
├── tests/
│   ├── integration/
│   └── e2e/
│
└── docs/
```

The initial implementation does not need every directory immediately.

Directories should be created as the corresponding subsystem is introduced.

---

# 34. Git Strategy

The repository should remain clean and incremental.

Each meaningful stage should produce a working commit.

Examples:

```text
Initial project structure
Add web client foundation
Add C++ server foundation
Add room management
Add WebSocket protocol
Add multiplayer lobby
Add game engine
Add scoring
Add replay
Add production configuration
```

Avoid large commits containing unrelated changes.

---

# 35. Development Phases

## Phase 1 — Project Foundation

Build:

- Repository structure
- CMake
- C++ project
- Basic web client
- Development documentation

Deliverable:

> The project builds successfully.

---

## Phase 2 — C++ Server

Build:

- HTTP server
- Health endpoint
- Basic server startup/shutdown
- Configuration
- Logging

Deliverable:

> The C++ server runs locally and responds to an HTTP health request.

---

## Phase 3 — Web Client

Build:

- Whosin landing page
- Create Game interface
- Join Game interface
- Responsive layout

Deliverable:

> The browser presents the basic Whosin experience.

---

## Phase 4 — WebSockets

Build:

- WebSocket server
- Client connection
- Connection lifecycle
- JSON event protocol
- Error handling

Deliverable:

> Browser and C++ server communicate in real time.

---

## Phase 5 — Rooms

Build:

- Room creation
- Room codes
- Room joining
- Player identity
- Lobby
- Host
- Player list

Deliverable:

> Multiple browsers can enter the same room and see synchronized players.

---

## Phase 6 — Multiplayer Game Engine

Build:

- Game abstraction
- Game state
- Rounds
- Timers
- Player actions
- Validation
- Scoring

Deliverable:

> Multiple players can complete a full game.

---

## Phase 7 — Results and Replay

Build:

- Round results
- Final results
- Play Again
- Room reset
- Score handling

Deliverable:

> A group can immediately play another game.

---

## Phase 8 — Resilience

Build:

- Reconnection
- Disconnect handling
- Host transfer
- Room expiration
- Invalid-action handling

Deliverable:

> Normal connection failures do not destroy the game.

---

## Phase 9 — PostgreSQL

Build:

- Database connection
- Migrations
- Game persistence
- Historical results where appropriate

Deliverable:

> Durable game information can be stored safely.

---

## Phase 10 — Production Hardening

Build:

- HTTPS
- Secure WebSockets
- Rate limiting
- Security headers
- Monitoring
- Error tracking
- Load testing
- Deployment configuration

Deliverable:

> Whosin is ready for controlled public use.

---

# 36. Definition of Done — MVP

## Entry

- [ ] Public URL loads
- [ ] Mobile layout works
- [ ] Desktop layout works
- [ ] Create Game works
- [ ] Join Game works

## Rooms

- [ ] Unique room code generated
- [ ] Players can join
- [ ] Players see one another
- [ ] Host is identified
- [ ] Host can start
- [ ] Invalid codes are handled
- [ ] Full/closed rooms are handled

## Game

- [ ] Game state is server-authoritative
- [ ] Players can perform the required game action
- [ ] Server validates actions
- [ ] Timer is synchronized
- [ ] Scores/results synchronize
- [ ] All clients receive state changes

## Resilience

- [ ] Player disconnect/reconnect works
- [ ] Host departure is handled
- [ ] Empty rooms expire
- [ ] Invalid actions cannot corrupt state

## Replay

- [ ] Game reaches a final result
- [ ] Host can initiate another game
- [ ] Players can continue without manually creating a new room

## Production

- [ ] HTTPS enabled
- [ ] WebSockets secured
- [ ] Rate limiting enabled
- [ ] Errors monitored
- [ ] Health endpoint available
- [ ] Production environment variables secured

---

# 37. Future Features

These should not block MVP development.

Potential future capabilities:

## Multiple Game Modes

```text
Whosin: Trivia
Whosin: Vote
Whosin: Bluff
Whosin: Word
Whosin: Party
```

## Custom Games

Hosts could eventually create their own question sets.

## Persistent Profiles

Optional accounts could allow:

- Statistics
- Achievements
- Friends
- Game history

## QR Joining

Players could scan a QR code instead of typing the room code.

## Share Links

Example:

```text
whosin.example/j/7K4P
```

## TV / Shared-Screen Mode

One screen displays the main game while each player uses their phone as a controller.

## Game Packs

The platform could eventually support themed content packs.

## Private Rooms

Optional room passwords or invitation links could be introduced.

---

# 38. Product Principles for Future Development

Every feature should be evaluated against four questions.

### Does it make joining easier?

If not, it needs a strong reason to exist.

### Does it make playing together more fun?

Whosin is fundamentally social.

### Does it preserve the no-login/no-install promise?

The default experience should remain frictionless.

### Does it make another round easy?

The product should naturally encourage:

> **“One more game?”**

---

# 39. Launch Experience

The ideal first screen:

```text
                         WHOSIN

                  Who's in? Let's play.

              ┌─────────────────────────┐
              │       CREATE GAME        │
              └─────────────────────────┘

                         or

              ┌─────────────────────────┐
              │        JOIN GAME         │
              └─────────────────────────┘
```

After creating:

```text
                 YOUR ROOM IS READY

                       7K4P

                Share this code
                   with friends

                  ● Colin
                  ● Maya
                  ● James
                  ● Sarah

                  [ START GAME ]
```

The experience should communicate the product's core promise without requiring documentation.

---

# 40. Performance Targets

Initial targets:

- Fast initial page load
- Responsive interactions
- Near-immediate room updates
- Minimal unnecessary network traffic
- Fast game-action acknowledgement under normal network conditions
- Graceful behavior on slower mobile connections

The server should be capable of supporting multiple simultaneous rooms without blocking one room's game loop on another.

---

# 41. Browser Support

Target current versions of:

- Chrome
- Safari
- Firefox
- Edge

Primary emphasis:

- iOS Safari
- Android Chrome
- Desktop Chrome
- Desktop Safari
- Desktop Edge

---

# 42. Configuration

Sensitive configuration must never be committed to source control.

Example:

```text
DATABASE_URL=
SERVER_HOST=
SERVER_PORT=
WEBSOCKET_PORT=
LOG_LEVEL=
```

Production secrets must be stored securely.

---

# 43. Deployment Architecture

Initial deployment may use a single C++ server:

```text
Internet
   │
   ▼
HTTPS / WSS
   │
   ▼
C++ Whosin Server
   │
   ▼
PostgreSQL
```

As demand grows:

```text
Internet
   │
   ▼
Load Balancer
   │
   ├── C++ Server
   ├── C++ Server
   └── C++ Server
           │
           ▼
         Redis
           │
           ▼
       PostgreSQL
```

Redis should only become part of the production architecture when horizontal scaling requires shared real-time state or Pub/Sub.

---

# 44. Success Metrics

Important product metrics include:

```text
Games created
Games started
Games completed
Average players per room
Average rounds per game
Replay rate
Join success rate
Connection failure rate
Average session duration
```

A particularly useful metric is:

> **Percentage of completed games that result in another game being started.**

This measures whether the core experience naturally creates another round of play.

---

# 45. Final Architecture Principle

The most important architectural rule is:

> **The browser renders Whosin. The C++ server owns Whosin.**

The browser is responsible for:

- Presentation
- Input
- Interaction
- Displaying synchronized state

The server is responsible for:

- Truth
- Rules
- State
- Timing
- Validation
- Scoring
- Permissions
- Multiplayer synchronization

---

# 46. Build Order

Development will proceed in this order:

```text
1. Git / repository foundation
2. CMake project
3. C++ server foundation
4. Basic HTTP server
5. Web client foundation
6. WebSocket communication
7. Room creation
8. Room joining
9. Lobby
10. Host controls
11. Game engine
12. First playable game
13. Scoring/results
14. Replay
15. Reconnection
16. Error handling
17. PostgreSQL
18. Responsive/mobile polish
19. Accessibility
20. Testing
21. Production deployment
22. Monitoring
23. Public launch
```

We will not build every subsystem simultaneously.

Each phase should produce a working, testable result before the next major subsystem is introduced.

---

# 47. First Build Milestone

The first meaningful multiplayer milestone is:

> **Three people on three different devices can open the public Whosin URL, enter the same room using a four-character code, see one another appear instantly, start a game, play a complete round, receive the same result, and immediately play another round.**

Everything else is secondary to proving this loop.

---

# 48. Project Status

**Product:** Whosin

**Tagline:** Who’s in? Let’s play.

**Stage:** Pre-build specification

**MVP:** Room-based real-time multiplayer web game

**Authentication:** None

**Installation:** None

**Frontend:** HTML + CSS + JavaScript

**Backend:** C++20

**Networking:** HTTP + WebSocket

**Build System:** CMake

**Database:** PostgreSQL

**Distributed State:** Redis when required for scaling

**Primary Devices:** Mobile + desktop browsers

**Core Principle:** Server-authoritative multiplayer state

**Primary Success Condition:** A group can join and start playing with almost zero friction.
