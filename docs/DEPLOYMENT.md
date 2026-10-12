# Run Whosin on a host

Whosin now supports `PORT` (default: 8080) and `GET /health`.
Invalid ports fail at startup with a readable error.

## Docker

From the repository root:

```sh
docker build -t whosin .
docker run --rm -p 8080:8080 whosin
```

Open http://localhost:8080. The container builds the C++ server and includes
the browser files. It runs as an unprivileged user.

To select another internal port:

```sh
docker run --rm -e PORT=9000 -p 9000:9000 whosin
```

## Hosting requirements

Choose a host that supports long-running Docker containers and WebSockets.
Configure its build context as the repository root and use `Dockerfile`.
Set its health-check path to `/health` and route traffic to the `PORT` value.
Use one running instance: room and game state are held in memory.
Server restarts clear all rooms; persistent storage is not implemented.

The hosting service or reverse proxy must provide HTTPS and forward WebSocket
upgrade requests to `/ws`. The browser automatically uses WSS on HTTPS pages.
Do not expose the server directly over plain HTTP for public play.

This configuration prepares deployment; it does not provision hosting or a
public URL. Rate limiting, monitoring, and public-launch security checks remain.

## Windows development

Run from the repository root so the server can find `static/`:

```powershell
cmake --build build
$env:PORT = "8080"
.\build\Debug\whosin.exe
```

Remove the override with `Remove-Item Env:PORT`.
