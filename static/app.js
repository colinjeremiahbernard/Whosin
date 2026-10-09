"use strict";

(() => {
  const element = (id) => document.getElementById(id);
  const { api, message } = window.whosin;
  const sessionKey = "whosin.session";
  let session = null;
  let refreshing = false;
  let pending = false;
  let submitting = false;

  function roomPath() {
    return `/api/rooms/${encodeURIComponent(session.roomCode)}`;
  }

  function saveSession(data) {
    session = { roomCode: data.roomCode, playerId: data.playerId };
    try { sessionStorage.setItem(sessionKey, JSON.stringify(session)); }
    catch { /* The page can work without storage. */ }
    window.whosinLive.watch(session.roomCode, session.playerId);
  }

  function clearSession() {
    session = null;
    window.whosinLive.watch(null);
    try { sessionStorage.removeItem(sessionKey); }
    catch { /* Storage may be unavailable. */ }
    element("entry").hidden = false;
    element("lobby").hidden = true;
    window.dispatchEvent(new CustomEvent("whosin.roomState", { detail: null }));
  }

  function renderRoom(room) {
    element("entry").hidden = true;
    element("lobby").hidden = false;
    element("lobby-code").textContent = room.roomCode;
    element("players").replaceChildren(...room.players.map((player) => {
      const item = document.createElement("li");
      const host = player.id === room.hostId ? " · Host" : "";
      const you = player.id === session.playerId ? " · You" : "";
      item.textContent = `${player.name}${host}${you}`;
      return item;
    }));
    const isHost = session.playerId === room.hostId;
    const waiting = room.status === "waiting";
    element("start-game").hidden = !isHost || !waiting;
    element("game-status").textContent = waiting
      ? (isHost ? "Start when everyone has joined." : "Waiting for the host to start.")
      : (room.status === "finished" ? "Game complete!" : "Trivia is in progress.");
    window.dispatchEvent(new CustomEvent("whosin.roomState", {
      detail: { room, playerId: session.playerId }
    }));
  }

  async function refreshRoom() {
    if (!session) return;
    if (refreshing) { pending = true; return; }
    refreshing = true;
    do {
      pending = false;
      const active = session;
      try {
        const room = await api(roomPath());
        if (active !== session) continue;
        if (!room.players.some((player) => player.id === session.playerId)) {
          clearSession();
          message("Your session has ended. Create or join a room.");
          break;
        }
        renderRoom(room);
      } catch (error) {
        if (active !== session) continue;
        if (error.status === 404) clearSession();
        message(error.status === 404 ? "That room is no longer available."
          : "Cannot load the room. Check your connection or refresh the page.");
      }
    } while (pending && session);
    refreshing = false;
  }

  async function submitAction(button, action) {
    if (submitting) return;
    submitting = true;
    button.disabled = true;
    message("");
    try { await action(); }
    catch (error) { message(error.message || "Cannot reach the server."); }
    finally { submitting = false; button.disabled = false; }
  }

  element("create-form").addEventListener("submit", (event) => {
    event.preventDefault();
    const button = event.currentTarget.querySelector("button");
    submitAction(button, async () => {
      const name = element("host-name").value.trim();
      if (!name) throw new Error("Enter your name.");
      saveSession(await api("/api/rooms", { name }));
      await refreshRoom();
    });
  });

  element("join-form").addEventListener("submit", (event) => {
    event.preventDefault();
    const button = event.currentTarget.querySelector("button");
    submitAction(button, async () => {
      const name = element("player-name").value.trim();
      const code = element("room-code").value.trim().toUpperCase();
      if (!name || !code) throw new Error("Enter your name and room code.");
      saveSession(await api(`/api/rooms/${encodeURIComponent(code)}/join`, { name }));
      await refreshRoom();
    });
  });

  element("start-game").addEventListener("click", (event) => {
    submitAction(event.currentTarget, async () => {
      if (!session) return;
      await api(`${roomPath()}/start`, { playerId: session.playerId });
      await refreshRoom();
    });
  });

  window.addEventListener("whosin.roomUpdated", refreshRoom);
  window.addEventListener("whosin.sessionExpired", () => {
    clearSession();
    message("Your reconnect window ended. Create or join a new room.");
  });
  window.addEventListener("whosin.refreshRoom", refreshRoom);
  try {
    const saved = JSON.parse(sessionStorage.getItem(sessionKey));
    if (saved && typeof saved.roomCode === "string" &&
        typeof saved.playerId === "string") session = saved;
  } catch { clearSession(); }
  if (session) {
    element("entry").hidden = true;
    window.whosinLive.watch(session.roomCode, session.playerId);
    refreshRoom();
  }
})();

