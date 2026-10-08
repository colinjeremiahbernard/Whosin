"use strict";

(() => {
  let socket = null;
  let retryTimer = null;
  let heartbeatTimer = null;
  let watchedRoom = null;
  let lastReply = 0;
  let stopped = false;
  const status = document.getElementById("connection-status");
  function setStatus(message) { status.textContent = message; }
  function subscribe() {
    if (socket?.readyState !== WebSocket.OPEN) return;
    socket.send(JSON.stringify(watchedRoom
      ? { type: "watch", roomCode: watchedRoom } : { type: "unwatch" }));
  }
  window.whosinLive = {
    watch(roomCode) { watchedRoom = roomCode || null; subscribe(); }
  };
  function stopHeartbeat() {
    clearInterval(heartbeatTimer);
    heartbeatTimer = null;
  }
  function connect() {
    if (stopped) return;
    setStatus("Connecting…");
    const protocol = location.protocol === "https:" ? "wss:" : "ws:";
    const connection = new WebSocket(`${protocol}//${location.host}/ws`);
    socket = connection;
    connection.addEventListener("open", () => {
      if (socket !== connection) return;
      lastReply = Date.now();
      subscribe();
      stopHeartbeat();
      heartbeatTimer = setInterval(() => {
        if (connection.readyState !== WebSocket.OPEN) return;
        if (Date.now() - lastReply > 45000) {
          setStatus("Reconnecting…");
          connection.close();
          return;
        }
        connection.send(JSON.stringify({ type: "ping" }));
      }, 15000);
    });
    connection.addEventListener("message", (event) => {
      if (socket !== connection) return;
      try {
        const message = JSON.parse(event.data);
        if (message.type === "connected" || message.type === "pong") {
          lastReply = Date.now();
          setStatus("Connected");
        }
        if (message.type === "room.updated" && watchedRoom)
          window.dispatchEvent(new Event("whosin.roomUpdated"));
        if (message.type === "error") console.warn("Whosin:", message.message);
      } catch { console.warn("Whosin received an invalid server message."); }
    });
    connection.addEventListener("close", () => {
      if (socket !== connection) return;
      stopHeartbeat();
      socket = null;
      if (stopped) return;
      setStatus("Reconnecting…");
      clearTimeout(retryTimer);
      retryTimer = setTimeout(connect, 2000);
    });
    connection.addEventListener("error", () => {
      if (socket === connection) setStatus("Connection interrupted");
      connection.close();
    });
  }
  window.addEventListener("pagehide", () => {
    stopped = true;
    clearTimeout(retryTimer);
    stopHeartbeat();
    if (socket) socket.close();
  });
  window.addEventListener("pageshow", (event) => {
    if (!event.persisted) return;
    stopped = false;
    connect();
  });
  connect();
})();
