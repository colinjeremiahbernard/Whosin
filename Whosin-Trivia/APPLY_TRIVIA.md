# Whosin: first playable trivia game

## Install

1. Stop the running server with Ctrl+C.
2. Extract this ZIP into a temporary folder.
3. Copy CMakeLists.txt, src/, and static/ into your Whosin project root.
4. Merge the folders and replace matching files. Keep other existing files.
5. From the Whosin root, run:

```powershell
cmake --build build
.\build\Debug\whosin.exe
```

The CMake change adds GameService.cpp and TriviaGame.cpp to the build.
If automatic regeneration does not occur, run `cmake -S . -B build` first.

Open http://localhost:8080 and refresh both browser windows.
The versioned script links load the updated JavaScript.
Create a new room after restarting the server.

## Play

1. Create a room in the normal browser window.
2. Join from an incognito window with another name.
3. The host clicks Start Game.
4. Each player chooses one of four answers.
5. After everyone submits, all players see the correct answer and scores.
6. The host clicks Next Round, repeating until round five.
7. The host clicks Show Final Results to reveal the winner or tied winners.
8. Click Play Again to reset scores and replay with the same players.

Correct answers earn one point. Wrong answers earn zero.
There are five fixed questions in src/game/Questions.h.
Replay uses the same questions in the same order for this first version.

## Files

The archive contains complete files, not patches.
It does not replace your README, .gitignore, model headers, IdGenerator,
or editor configuration.

main.cpp now registers routes only. Room routes move into RoomRoutes.h.
TriviaGame contains rules, GameService coordinates rooms and games,
and GameView serializes the public game state.
The correct answer and submitted choices are withheld until the round ends.
Scores are awarded only when everyone has submitted.
Round and game identifiers reject delayed requests from earlier rounds.

## Validation

- Native C++ engine and room integration tests passed with C++20.
- Concurrent submissions score correctly once.
- Duplicate answers, outsiders, invalid choices, and stale requests are rejected.
- Only the host can advance rounds or start a replay.
- All five rounds, final scores, ties, and score resets are covered.
- Two simulated clients passed the frontend logic smoke test with API fixtures.
- JavaScript syntax checks passed.
- Every source file is strictly under 200 lines.

The full Crow server was not compiled in this environment, and visual browser
testing was unavailable. Your Windows build and two-window playthrough remain
the final integration check.

## Current limits

Rounds wait for every player to answer. There is no answer timer or automatic
handling for players who leave mid-round yet; a disconnected player can resume
by refreshing the same tab while the server is still running.
Restarting the server clears rooms, games, and scores.
Session authentication hardening, room cleanup, and production deployment
remain later work.
