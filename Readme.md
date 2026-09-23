# WHOSIN

> **Who’s in? Let’s play.**

Whosin is a browser-based, real-time multiplayer game platform designed for spontaneous group play.

One person creates a game, receives a short room code, and shares it with the group. Everyone joins from their own phone, tablet, or laptop using a browser. No account. No app download. No installation.

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

### Zero friction

No account creation, passwords, downloads, or app installation.

### Real-time

The game state must remain synchronized across every connected device.

### Room-based

Every game exists inside a temporary room identified by a short human-readable code.

### Host-controlled

One player acts as the host and controls game progression.

### Replayable

Finishing a game should naturally lead into another game.

### Device agnostic

The experience must work well on:

- iPhone
- Android phones
- Tablets
- Windows laptops
- Mac laptops
- Desktop browsers

### Mobile first

Most players will probably hold their phones while playing together in the same physical space.

### Simple to understand

Players should be able to join and begin playing without instructions from the host.

---

# 3. Working Brand

## Name

**Whosin**

## Brand idea

The name comes from:

> **Who’s in?**

It represents the social action required to start a game: getting people into the room.

## Primary tagline

**Who’s in? Let’s play.**

## Brand vocabulary

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

Avoid unnecessarily technical terminology such as:

- Session ID
- WebSocket connection
- Authentication token
- Lobby instance

Those concepts belong in the architecture, not the player experience.

---

# 4. MVP Definition

The first release must provide a complete playable multiplayer experience.

## Required MVP functionality

### Landing page

Players can:

- Create a game
- Join an existing game

### Create game

The creator becomes the host.

The server generates a unique room code.

Example:

```text
WHOSIN

Create a Game

Room code:
7K4P
```

### Join game

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
- Winner/leader information where applicable
- Next-round control

### Replay

The host can start another game without requiring everyone to leave and reconnect.

---

# 5. Game Design

Whosin is initially a **game platform shell plus a defined multiplayer game mode**.

The architecture must separate the game engine from the room system so additional game modes can be introduced later.

## Game engine requirements

A game mode should be able to define:

```text
Game configuration
    ↓
Rounds
    ↓
Player actions
    ↓
Validation
    ↓
Scoring
    ↓
Round result
    ↓
Next round
    ↓
Final result
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

## Room states

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

Example:

```text
7K4P
M8RX
T6QD
```

Room codes must be unique among currently active rooms.

They do not need to be globally permanent.

---

# 8. Player Identity

Whosin does not require accounts in the MVP.

A player receives a temporary identity when joining a room.

Example:

```text
playerId = random UUID
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

Recommended behavior:

> Automatically promote the longest-connected remaining player.

The new host receives the host controls.

---

# 10. Real-Time Architecture

Whosin requires server-authoritative real-time communication.

Recommended architecture:

```text
                  ┌───────────────────┐
                  │   Web Browser     │
                  │                   │
                  │ React Application │
                  └─────────┬─────────┘
                            │
                       HTTPS / WSS
                            │
                  ┌─────────▼─────────┐
                  │   Application     │
                  │      Server       │
                  │                   │
                  │ Room Manager      │
                  │ Game Engine       │
                  │ Event Manager     │
                  └───────┬─┬─────────┘
                          │ │
                 ┌────────┘ └────────┐
                 │                   │
        ┌────────▼────────┐  ┌───────▼────────┐
        │     Redis       │  │   PostgreSQL   │
        │                 │  │                │
        │ Live room state │  │ Persistent     │
        │ Pub/Sub         │  │ data           │
        └─────────────────┘  └────────────────┘
```

---

# 11. Recommended Technology Stack

## Frontend

**Next.js + React + TypeScript**

Reasons:

- Strong component architecture
- Excellent responsive-web support
- TypeScript across the application
- Easy deployment
- Good performance
- Suitable for a public consumer-facing URL

## Styling

**Tailwind CSS**

Used for:

- Responsive layouts
- Design consistency
- Mobile-first development
- Rapid UI iteration

## Real-time communication

**WebSockets**

The preferred implementation can use Socket.IO or an equivalent managed WebSocket infrastructure.

The important architectural requirement is:

> The server is authoritative and clients receive state updates through a real-time connection.

## Backend

**Node.js + TypeScript**

The backend contains:

- Room management
- Player management
- Game state
- Game rules
- Validation
- Scoring
- Host permissions
- Real-time events

## Database

**PostgreSQL**

Used for durable data such as:

- Game metadata
- Completed game records
- Scores where persistence is required
- Analytics
- Future user/account functionality

## Live state

**Redis**

Used for:

- Active rooms
- Temporary game state
- Pub/Sub
- Distributed real-time coordination
- Expiration of abandoned rooms

Not every piece of temporary game state needs to be written to PostgreSQL.

---

# 12. Server Authority

The client must never be trusted to determine important game outcomes.

For example, the client may request:

```text
SUBMIT_ANSWER
```

The server decides:

```text
Is the player in the room?
Is the round active?
Has the player already submitted?
Is the answer valid?
What score should be awarded?
Has the round completed?
```

Then the server broadcasts the resulting state.

This prevents:

- Score manipulation
- Duplicate submissions
- Invalid actions
- Client-side timer cheating
- Host permission bypasses

---

# 13. Event Architecture

The real-time protocol should use explicit events.

Example client → server events:

```text
room:create
room:join
room:leave
game:start
game:action
game:next
game:replay
```

Example server → client events:

```text
room:created
room:updated
player:joined
player:left
game:started
game:state
game:actionAccepted
game:roundComplete
game:finished
host:changed
error
```

The exact protocol can evolve during implementation.

---

# 14. Game State

A representative server state:

```typescript
GameState {
  roomCode: string
  status: GameStatus
  hostId: string
  players: Player[]
  currentRound: number
  totalRounds: number
  roundStartedAt: number
  roundEndsAt: number
  scores: Record<string, number>
  roundData: unknown
}
```

The actual implementation should use strict TypeScript types rather than unrestricted objects wherever possible.

---

# 15. Timer Architecture

Timers must be server-authoritative.

Do not rely on each browser independently counting down from a local timer.

Instead, the server provides:

```text
roundEndsAt
```

Clients calculate the remaining display time using the synchronized server state.

Example:

```text
roundEndsAt = 12:35:20.000

Client:
remaining = roundEndsAt - currentServerAdjustedTime
```

This prevents clients from drifting apart.

When the timer expires, the server determines that the round has ended.

---

# 16. Disconnect Handling

Players will inevitably lose connectivity.

Whosin should distinguish between:

### Temporary disconnect

Keep the player in the room for a short grace period.

If they reconnect, restore their session.

### Permanent departure

Remove the player after the grace period.

### Host disconnect

Transfer host authority according to the host reassignment rule.

### Empty room

If no players remain, the room should eventually expire automatically.

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
can be retained as historical data if persistence is enabled
```

Redis TTLs are appropriate for ephemeral room state.

---

# 18. Database Model

Initial relational model:

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

The schema should remain flexible enough to support additional game modes.

---

# 19. API Boundaries

REST endpoints may be used for:

```text
GET  /health
POST /rooms
GET  /rooms/:code
```

Real-time actions should primarily use WebSockets.

Avoid creating a large REST API for actions that inherently require real-time synchronization.

---

# 20. Frontend Routes

Recommended routes:

```text
/
```

Landing page.

```text
/join
```

Join a room.

```text
/room/[code]
```

Room lobby/game experience.

```text
/room/[code]/results
```

Optional dedicated results route if needed.

The actual URL architecture can be simplified if game state is maintained entirely inside `/room/[code]`.

---

# 21. Responsive UX

The application must be designed mobile-first.

## Mobile priorities

The primary gameplay interface should:

- Fit one-handed use where practical
- Use large touch targets
- Avoid tiny text
- Minimize typing
- Avoid unnecessary navigation
- Work in portrait orientation
- Remain usable on smaller screens

## Desktop

Desktop layouts can take advantage of:

- Larger player lists
- More whitespace
- Larger game
