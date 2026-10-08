"use strict";

window.whosinGameView = (game, context, busy, act) => {
  const element = (id) => document.getElementById(id);
  const me = game.players.find((player) => player.id === context.playerId);
  const isHost = game.hostId === context.playerId;
  const question = game.phase === "question";
  const finished = game.phase === "finished";
  element("game-panel").hidden = false;
  element("round-label").textContent = finished ? "FINAL RESULTS"
    : `ROUND ${game.round} OF ${game.totalRounds}`;
  element("question").textContent = finished ? "Thanks for playing!" : game.question;
  element("answer-options").replaceChildren();

  if (!finished) {
    game.options.forEach((option, index) => {
      const button = document.createElement("button");
      button.type = "button";
      button.className = "answer-option";
      button.textContent = `${String.fromCharCode(65 + index)}. ${option}`;
      button.disabled = busy || !question || !me || me.submitted;
      if (!question && index === game.correctAnswer) {
        button.classList.add("correct");
        button.textContent += " — Correct answer";
      }
      button.addEventListener("click", () => act("answer", { answer: index }));
      element("answer-options").append(button);
    });
  }

  const ranked = [...game.players].sort((a, b) => b.score - a.score);
  const best = ranked[0]?.score;
  const winners = ranked.filter((player) => player.score === best);
  if (finished) {
    const names = winners.map((player) => player.name).join(", ");
    element("round-status").textContent = winners.length === 1
      ? `${names} wins with ${best} points!`
      : `Tie: ${names}, with ${best} points each!`;
  } else if (question) {
    const prefix = me?.submitted ? "Answer submitted. " : "Choose one answer. ";
    element("round-status").textContent =
      `${prefix}${game.submittedCount} of ${game.playerCount} players answered.`;
  } else {
    element("round-status").textContent = me?.answer === game.correctAnswer
      ? "Correct! You earned 1 point." : "No point this round. Try the next one!";
  }

  element("score-list").replaceChildren(...ranked.map((player) => {
    const item = document.createElement("li");
    const you = player.id === context.playerId ? " · You" : "";
    const answered = question && player.submitted ? " · Answered" : "";
    item.textContent = `${player.name}${you} — ${player.score} points${answered}`;
    return item;
  }));

  const advance = element("advance-game");
  advance.hidden = !isHost || question;
  advance.disabled = busy;
  advance.textContent = finished ? "Play Again" :
    game.round === game.totalRounds ? "Show Final Results" : "Next Round";
  advance.onclick = () => act(finished ? "replay" : "next");
  element("host-instruction").textContent = !isHost && !question
    ? (finished ? "Waiting for the host to play again." : "Waiting for the host to continue.")
    : "";
};
