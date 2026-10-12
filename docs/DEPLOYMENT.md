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

## Deploying to Render (Free Tier)

Render supports Docker web services on its Free tier with automatic HTTPS and WebSocket support:

### Option A: 1-Click Blueprint (Recommended)
1. Push `render.yaml` to your GitHub repository:
   ```sh
   git add render.yaml docs/DEPLOYMENT.md
   git commit -m "Add Render Blueprint configuration"
   git push origin main
   ```
2. Log in to [Render](https://dashboard.render.com).
3. Click **New +** -> **Blueprint**.
4. Select your `Whosin` repository.
5. Render will detect `render.yaml`, set up the free Docker web service, configure the `/health` check path, and start the initial build.

### Option B: Manual Web Service Setup
1. Log in to [Render](https://dashboard.render.com).
2. Click **New +** -> **Web Service**.
3. Connect your `Whosin` GitHub repository (`https://github.com/colinjeremiahbernard/Whosin`).
4. Fill in the following settings:
   - **Name**: `whosin` (or your preferred service name)
   - **Language / Environment**: `Docker`
   - **Region**: Select closest to your players (e.g., Oregon, Ohio, Frankfurt)
   - **Branch**: `main`
   - **Instance Type / Plan**: **Free** ($0/month)
5. Expand **Advanced**:
   - **Health Check Path**: `/health`
   - **Auto-Deploy**: `Yes`
6. Click **Create Web Service**.

### Free Tier Notes & Best Practices
- **Spin Down on Idle**: Render's free instances spin down after 15 minutes of inactivity. When someone visits your URL, it will take ~30-50 seconds to wake up (cold start).
- **In-Memory State**: Whosin stores room and player state in memory. When the instance spins down or restarts, active rooms are reset.
- **Keep-Alive (Optional)**: If you are hosting a game session and want to prevent spindown, you can configure a free external ping monitor (like UptimeRobot) to ping `https://<your-service>.onrender.com/health` every 10 minutes.
- **WebSocket & Port**: Render automatically exposes port `10000` via the `$PORT` environment variable and handles secure WSS (`/ws`) forwarding automatically.

## Windows development

Run from the repository root so the server can find `static/`:

```powershell
cmake --build build
$env:PORT = "8080"
.\build\Debug\whosin.exe
```

Remove the override with `Remove-Item Env:PORT`.
