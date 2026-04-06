# Bricks Breaker

> **Role:** Solo Developer  
> **Language:** C++  
> **Renderer:** Console (Windows API)  
> **Type:** 2D Arcade Game  

---

## About

Bricks Breaker is a classic arcade-style brick breaking game built entirely from scratch in C++ using a console-based renderer. Every system — the ball physics, brick grid, paddle input, and collision detection — was implemented manually without a game engine. This was an early foundational project focused on core OOP design patterns and real-time game loop architecture.

---

## Features

- **Real-Time Game Loop** — continuous update/render cycle managing all game state
- **Ball Physics** — velocity-based movement with angle reflection off walls, paddle, and bricks
- **Collision Detection** — custom AABB collision between ball, paddle, bricks, and boundaries
- **Brick Grid** — destructible brick layout with hit detection and removal
- **Paddle Control** — keyboard-driven paddle movement with boundary clamping
- **Win/Loss States** — game over on ball loss, win condition on brick clear
- **Console Renderer** — Windows console API rendering using a `Console` output system
- **Object Hierarchy** — `BaseObject` base class extended by `Ball`, `Box` for shared position/rendering logic

---

## Architecture

| Class | Responsibility |
|-------|---------------|
| `Main` | Entry point, game initialization |
| `Game` | Core game loop, state management, collision orchestration |
| `Ball` | Ball entity — position, velocity, movement, reflection |
| `Box` | Brick and paddle entity — position, dimensions, active state |
| `BaseObject` | Shared base — position, size, render properties |
| `Console` | Windows console output — cursor positioning, character rendering |

---

## Build

Open `Bricks.sln` in Visual Studio and build. No external dependencies required.

---

*Built by Jacob Blackburn — Full Sail University, 2024*
