# Arachnivasion

> **Role:** Gameplay Programmer (Team Project — Jacob's Branch: `Jacobs_DevBranch`)  
> **Language:** C++ | Python  
> **Graphics API:** Vulkan  
> **Architecture:** Entity Component System (EnTT)  
> **Type:** 3D Wave Shooter with Live Online Leaderboard  

---

## About

Arachnivasion is a Space Invaders reimagining built from the ground up using Vulkan and the EnTT Entity Component System — no engine, no abstraction layer. The team chose to reinterpret the classic arcade formula in 3D with spider enemies, a player-controlled turret, and a live global leaderboard backed by a deployed Python/Flask API with PostgreSQL.

This was a capstone team project where each member first built the shared engine foundation solo (see: Vulkan ECS Engine Solo Prototype), then merged into a collaborative repo to build the full game across multiple development sprints.

---

## Jacob's Contributions

- Implemented **player shooting mechanics** — firing logic, bullet entity spawning via `SpawnHelpers`, and projectile lifetime management
- Worked on **enemy behavior and wave logic** — spider enemy spawning, movement patterns, and difficulty scaling across waves
- Built and deployed the **online leaderboard system** end-to-end:
  - In-game C++ HTTP client for submitting and retrieving scores
  - Python/Flask REST API (`GET /highscores`, `POST /submit`)
  - PostgreSQL database hosted on Supabase
  - Server deployed and live on Render
- Contributed to **collision detection** between player bullets and enemy entities
- Collaborated across 4 development sprints using Git branching (`Jacobs_DevBranch`) with regular merges into the team main branch

---

## Full Feature Set

- **Custom Vulkan Renderer** — built directly on the Vulkan graphics API with no engine abstraction
- **ECS Architecture** — EnTT-based with CCL signal-driven component updates
- **Player Controller** — input handling and player entity management
- **Enemy Spawning** — `SpawnHelpers` system for wave-based enemy creation and scaling
- **Bullet System** — player projectiles with collision detection and deferred entity destruction
- **Live Leaderboard** — in-game HTTP client submitting scores to a deployed server; top 10 global scores displayed post-game
- **Online Leaderboard API** — Python/Flask REST server with PostgreSQL persistence
  - `GET /highscores` — retrieve top 10 scores sorted by score
  - `POST /submit` — submit score with player initials
- **3D Level Loading** — custom level loader for environment geometry
- **CMake Build System** — cross-platform build configuration

---

## Architecture

### Game (C++ / Vulkan)

The codebase follows a modular ECS pattern where each folder is a "module" containing components and the systems that operate on them.

| Module | Responsibility |
|--------|---------------|
| `GAME` | Game logic, player, spawn helpers, highscore networking |
| `DRAW` | Vulkan renderer, draw components, buffers, level geometry |
| `UTIL` | Shared utilities |
| `APP` | Window and OS management |
| `CCL` | Connect Component Logic — signal-driven system binding component updates |

### Leaderboard Server (Python)

| File | Responsibility |
|------|---------------|
| `server.py` | Flask REST API with PostgreSQL via psycopg |
| `highscores.json` | Local fallback data |
| `requirements.txt` | Python dependencies |

**Deployed on:** Render (web service) + Supabase (PostgreSQL)

---

## Build

### Game
Requires CMake and the Vulkan SDK.

```
cmake -S ./ -B ./build
```

Open the generated `.sln` in the `build/` folder with Visual Studio.

### Leaderboard Server
```
pip install -r requirements.txt
export DATABASE_URL=<your_postgres_url>
python server.py
```

---

*Built by Team Purple — Full Sail University, 2025*  
*Team: Jacob Blackburn, [teammates]*
