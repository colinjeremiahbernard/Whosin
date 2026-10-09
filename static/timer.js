"use strict";

(() => {
  const label = document.getElementById("round-timer");
  let timer = null;
  let deadline = 0;
  let refreshed = false;

  function tick() {
    const remaining = Math.max(0, deadline - performance.now());
    label.textContent = `Time remaining: ${Math.ceil(remaining / 1000)} seconds`;
    if (remaining > 0) return;
    document.querySelectorAll("#answer-options button").forEach((button) => {
      button.disabled = true;
    });
    label.textContent = "Time is up. Waiting for results…";
    if (!refreshed) {
      refreshed = true;
      window.dispatchEvent(new Event("whosin.refreshRoom"));
    }
  }

  window.whosinTimer = {
    update(game) {
      clearInterval(timer);
      timer = null;
      label.hidden = !game || game.phase !== "question";
      if (label.hidden) return;
      deadline = performance.now() + Math.max(0, game.remainingMs || 0);
      refreshed = false;
      tick();
      timer = setInterval(tick, 100);
    }
  };
})();
