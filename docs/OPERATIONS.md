# Whosin operations

## Release checklist

1. Build on Windows and commit/push the tested change to `main`.
2. Wait for Render to show the new deployment as Live.
3. Check `/health`, create/join, a complete game and replay from two devices.
4. Refresh both devices after updates. The session-token update requires rejoining;
   pre-update sessions have no private token. Redeployments clear active rooms.

## Monitoring

Render's health check must be `/health`. Its service logs contain a line every
minute: `Whosin metrics: rooms=... requests=... errors=...`.
Request and error counts cover that minute; rooms is the current count.
Health traffic is counted but exempt from rate limits. Metrics are process-local.

Watch for repeated crashes, build errors, sustained HTTP errors, or room counts
near 100. A health response confirms process availability, not gameplay correctness.
Configure an external uptime monitor for the public `/health` URL if you want
notifications. No external alerting account has been provisioned by this change.
Free-hosting sleep can trigger alerts and may be shortened by monitoring traffic;
choose a monitoring policy that matches your hosting plan.

## Limits

The HTTP budget is per directly connected peer per minute:
600 total requests, 120 POST requests, and 20 room creations.
Limits return HTTP 429 with `Retry-After: 60`; body validation returns 413.
The limiter stores at most 2048 peers and discards expired entries.
Behind Render's proxy a peer may represent several users, so budgets may be
shared. Never blindly trust user-supplied `X-Forwarded-For`. A larger release
should configure trusted-proxy identity or upstream rate limiting explicitly.

Each room permits 12 players; the server permits 100 rooms and 512 sockets.
WebSocket messages above 1 KiB or over 20 messages per second close the connection.
POST bodies are checked after Crow receives them; this is application validation,
not a transport-level upload limit. Use a reverse proxy for transport limits.

## Incident response

- Inspect Render logs and deployment status first.
- If a release causes failures, redeploy the previous working commit via Render.
- Restarting clears rooms and resolves process-local state; players must rejoin.
- For abuse, use hosting/network controls as well as the application limits.
- Never paste session tokens into logs, issues or screenshots.

## Known boundaries

Use one server instance. There is no database, durable metrics store, external
alerting integration or automated load-test result. Display names are temporary;
provider request logs may be retained separately. Review capacity and proxy
configuration before promoting the game to a large public audience.
