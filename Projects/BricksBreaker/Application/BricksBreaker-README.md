# Bricks Breaker

> **Role:** Solo Developer  
> **Language:** C++  
> **Framework:** Windows Console (Win32 API)  
> **Type:** Console Game  

---

## About

A classic brick breaker game rendered entirely in the Windows console using ASCII/extended character set (CP437). The player controls a paddle to bounce a ball into a grid of bricks, clearing them for points. Built from scratch with custom collision detection, object-oriented game architecture, and a frame-rate controlled game loop.

---

## Features

- **Console Rendering** — uses Windows console output with extended character set for drawing paddles, bricks, and the ball
- **Paddle & Ball Physics** — ball bounces off walls, paddle, and bricks with velocity-based movement
- **Collision Detection** — custom `Contains()` checks for ball-to-brick and ball-to-paddle collisions
- **Brick Grid** — dynamically generated brick layout with colored bricks
- **Frame Rate Control** — adjustable frame timing using `std::chrono` (speed up/slow down with arrow keys)
- **Game Reset** — reset ball and full game state for replayability
- **OOP Architecture** — `BaseObject` inheritance hierarchy for all game entities

---

## Architecture

| Class | Responsibility |
|-------|---------------|
| `Game` | Game loop, update/render cycle, collision logic, brick management |
| `Ball` | Ball entity with velocity and movement updates |
| `Box` | Paddle and brick entity with width, height, and hit detection |
| `BaseObject` | Base class for position, drawing, and shared behavior |
| `Console` | Console rendering utilities |

---

## Controls

| Key | Action |
|-----|--------|
| Left / Right | Move paddle |
| Up Arrow | Increase game speed |
| Down Arrow | Decrease game speed |

---

## Build

Open `Bricks.sln` in Visual Studio and build. No external dependencies — uses only Win32 console APIs and the C++ standard library.

---

*Built by Jacob Blackburn — Full Sail University, 2024*
