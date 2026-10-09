"use strict";

(() => {
  const { api, message } = window.whosin;
  let context = null;
  let game = null;
  let loading = false;
  let pending = false;
  let busy = false;

  function render() {
    if (game && context) window.whosinGameView(game, context, busy, act);
  }

  function path(active) {
    return `/api/rooms/${encodeURIComponent(active.room.roomCode)}`;
  }

  async function load() {
    if (!context) return;
    if (loading) { pending = true; return; }
    loading = true;
    do {
      pending = false;
      const active = context;
      try {
        const state = await api(`${path(active)}/game`);
        if (!context || active.room.roomCode !== context.room.roomCode) continue;
        game = state;
        window.whosinTimer.update(state);
        render();
      } catch (error) {
        if (context && active.room.roomCode === context.room.roomCode)
          message(error.message || "Cannot load the game. Refresh to try again.");
      }
    } while (pending && context);
    loading = false;
  }

  async function act(action, extra = {}) {
    if (!context || !game || busy) return;
    const active = context;
    const payload = {
      playerId: active.playerId,
      round: game.round,
      generation: game.generation,
      ...extra
    };
    busy = true;
    message("");
    render();
    try {
      await api(`${path(active)}/${action}`, payload);
      window.dispatchEvent(new Event("whosin.refreshRoom"));
      await load();
    } catch (error) {
      message(error.message || "Cannot send your action. Try again.");
      await load();
    } finally {
      busy = false;
      render();
    }
  }

  window.addEventListener("whosin.roomState", (event) => {
    const next = event.detail;
    if (!next || next.room.status === "waiting") {
      window.whosinTimer.update(null);
      context = null;
      game = null;
      document.getElementById("game-panel").hidden = true;
      return;
    }
    context = next;
    load();
  });
})();
