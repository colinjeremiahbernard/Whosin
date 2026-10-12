"""Run against a local Whosin binary, from the repository root."""
import asyncio
import json
import os
import subprocess
import sys
import time
import urllib.error
import urllib.request

BASE = "http://127.0.0.1:19081"


def request(path, body=None, session=None):
    headers = {"Content-Type": "application/json"}
    if session:
        headers.update({"X-Player-Id": session["playerId"],
                        "X-Session-Token": session["sessionToken"]})
    data = json.dumps(body).encode() if body is not None else None
    req = urllib.request.Request(BASE + path, data=data, headers=headers)
    try:
        result = urllib.request.urlopen(req, timeout=5)
    except urllib.error.HTTPError as error:
        result = error
    with result:
        return result.status, json.load(result), result.headers


async def checks():
    import websockets
    status, host, headers = request("/api/rooms", {"name": "Colin"})
    assert status == 200 and len(host["sessionToken"]) == 32
    assert "frame-ancestors 'none'" in headers["Content-Security-Policy"]
    path = "/api/rooms/" + host["roomCode"]
    status, guest, _ = request(path + "/join", {"name": "Guest"})
    assert status == 200 and guest["sessionToken"] != host["sessionToken"]
    assert request(path)[0] == 403
    assert request(path + "/start", {"playerId": host["playerId"]})[0] == 403
    forged = {**guest, "playerId": host["playerId"]}
    assert request(path + "/start", {"playerId": host["playerId"]}, forged)[0] == 403
    assert request(path + "/start", {"playerId": guest["playerId"]}, guest)[0] == 403
    room = request(path, session=host)[1]
    assert all("token" not in p and "sessionToken" not in p for p in room["players"])
    assert request("/api/rooms", {"name": "x" * 5000})[0] == 413
    url = "ws://127.0.0.1:19081/ws"
    async with websockets.connect(url) as hs, websockets.connect(url) as gs:
        for socket, session in [(hs, host), (gs, guest)]:
            assert json.loads(await socket.recv())["type"] == "connected"
            await socket.send(json.dumps({"type": "watch", **session}))
            await socket.send(json.dumps({"type": "ping", **session}))
            # A watch also publishes room.updated; consume until pong.
            while json.loads(await asyncio.wait_for(socket.recv(), 3))["type"] != "pong":
                pass
        assert request(path + "/start", {"playerId": host["playerId"]}, host)[0] == 200
        first_questions = []
        for round_number in range(1, 6):
            state = request(path + "/game", session=host)[1]
            first_questions.append(state["question"])
            assert "correctAnswer" not in state
            for session in [host, guest]:
                assert request(path + "/answer", {"playerId": session["playerId"],
                    "round": round_number, "generation": 1, "answer": 0}, session)[0] == 200
            state = request(path + "/game", session=guest)[1]
            assert state["phase"] == "result" and "correctAnswer" in state
            assert all("token" not in p for p in state["players"])
            assert request(path + "/next", {"playerId": host["playerId"],
                "round": round_number, "generation": 1}, host)[0] == 200
        assert request(path + "/game", session=host)[1]["phase"] == "finished"
        assert request(path + "/replay", {"playerId": host["playerId"],
            "generation": 1}, host)[0] == 200
        replay = request(path + "/game", session=host)[1]
        assert replay["generation"] == 2 and replay["question"] not in first_questions
        assert all(p["score"] == 0 for p in replay["players"])
        assert request(path + "/answer", {"playerId": host["playerId"],
            "round": 1, "generation": 1, "answer": 0}, host)[0] == 409
        # Forged WebSocket identity is rejected without keeping a player present.
        async with websockets.connect(url) as bad:
            await bad.recv()
            await bad.send(json.dumps({"type": "watch", **forged}))
            try:
                await asyncio.wait_for(bad.recv(), 3)
                raise AssertionError("Forged websocket accepted")
            except websockets.exceptions.ConnectionClosed:
                pass
        assert request(path + "/leave", {"playerId": host["playerId"]}, host)[0] == 200
        assert request(path, session=guest)[1]["hostId"] == guest["playerId"]
        assert request(path + "/leave", {"playerId": guest["playerId"]}, guest)[0] == 200
    for _ in range(650):
        status, _, headers = request("/api/rooms/ABC234")
        if status == 429:
            assert headers["Retry-After"] == "60"
            break
    else:
        raise AssertionError("Rate limit missing")
    assert request("/health")[0] == 200


if __name__ == "__main__":
    process = subprocess.Popen([os.path.abspath(sys.argv[1])],
        env={**os.environ, "PORT": "19081"},
        stdout=subprocess.DEVNULL, stderr=subprocess.DEVNULL)
    try:
        for _ in range(50):
            try:
                assert request("/health")[0] == 200
                break
            except OSError:
                time.sleep(0.1)
        asyncio.run(checks())
        print("Launch smoke checks passed")
    finally:
        process.terminate()
        process.wait(timeout=10)
