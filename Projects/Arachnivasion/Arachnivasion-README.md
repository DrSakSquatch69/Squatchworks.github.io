# Arachnivasion

> **Role:** Gameplay Programmer (Team Project)  
> **Language:** C++ | Python  
> **Graphics API:** Vulkan  
> **Architecture:** Entity Component System (EnTT)  
> **Type:** 3D Game with Online Leaderboard  

---

## About

Arachnivasion is a 3D game built from the ground up using Vulkan and the EnTT Entity Component System. The project features a custom rendering pipeline, player controls, enemy spawning, and a live online leaderboard backed by a Python/Flask API with PostgreSQL.

This was a team capstone project demonstrating low-level graphics programming, ECS architecture, and full-stack integration with a networked highscore system.

---

## Features

- **Custom Vulkan Renderer** — built directly on the Vulkan graphics API with no engine abstraction
- **ECS Architecture** — EnTT-based component system with modular, signal-driven updates (CCL pattern)
- **Player Controller** — input handling and player entity management
- **Enemy Spawning** — `SpawnHelpers` system for creating and managing enemy entities
- **Highscore System** — in-game network client that submits and retrieves scores from a live server
- **Online Leaderboard API** — Python/Flask REST server with PostgreSQL persistence
  - `GET /highscores` — retrieve top 10 scores
  - `POST /submit` — submit new score with initials
- **3D Level Loading** — custom level loader for environment geometry
- **CMake Build System** — cross-platform build configuration

---

## Architecture

### Game (C++ / Vulkan)

The codebase follows a modular ECS pattern where each folder is a "module" containing components and the systems that operate on them.

| Module | Responsibility |
|--------|---------------|
| `GAME` | Game components, player logic, spawn helpers, highscore networking |
| `DRAW` | Vulkan renderer, draw components, buffers, level components |
| `UTIL` | Shared utilities |
| `APP` | Window management |
| `CCL` | Connect Component Logic — signal-driven system for binding component updates |

### Leaderboard Server (Python)

| File | Responsibility |
|------|---------------|
| `server.py` | Flask REST API with PostgreSQL via psycopg |
| `highscores.json` | Local fallback data |
| `requirements.txt` | Python dependencies |

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
