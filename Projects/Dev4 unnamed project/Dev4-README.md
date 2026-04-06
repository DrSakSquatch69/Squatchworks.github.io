# Vulkan ECS Engine — Solo Prototype

> **Role:** Solo Developer  
> **Language:** C++  
> **Graphics API:** Vulkan  
> **Architecture:** Entity Component System (EnTT)  
> **Type:** 3D Game Engine Prototype / FPS Foundation  

---

## About

This is the solo foundation project built as a prerequisite to Arachnivasion. Each student on the team independently implemented the same Vulkan/EnTT engine base — player movement, enemy entities, level loading, and the CCL signal system — before the team merged their work into the collaborative Arachnivasion project.

Building this solo first meant every team member had deep ownership of the underlying architecture. The codebase demonstrates direct Vulkan API usage, modular ECS design, and 3D game loop fundamentals without any engine abstraction layer.

---

## Features

- **Custom Vulkan Renderer** — direct Vulkan API rendering with buffer management, GPU instance updates, and draw pipeline
- **ECS Architecture** — EnTT-based with CCL (Connect Component Logic) pattern for signal-driven component updates
- **Player System** — keyboard-driven first-person movement with configurable speed and entity management
- **Enemy Entities** — enemy spawning and management via `SpawnHelpers`
- **Game Manager** — centralized game state tracking player/enemy visibility flags and entity collections
- **Model Manager** — 3D model loading and entity creation from model assets
- **Level Loading** — custom level component system for environment geometry
- **Entity Collections** — named grouping system for managing logical sets of game objects
- **Modular Architecture** — organized into APP, DRAW, GAME, UTIL, and CCL modules

---

## Architecture

| Module | Key Files | Responsibility |
|--------|-----------|---------------|
| `APP` | `Window.hpp` | Window creation and OS message loop |
| `DRAW` | `VulkanRenderer`, `VulkanBuffers`, `DrawComponents`, `LevelComponents` | Vulkan rendering pipeline, GPU buffers, level geometry |
| `GAME` | `GameManager`, `player`, `ModelManager`, `GameComponents`, `SpawnHelpers` | Game logic, player control, entity management |
| `UTIL` | `Utilities`, `GameConfig` | Shared utilities and configuration |
| `CCL` | `CCL.h/.cpp` | Signal-driven component update binding system |

The CCL pattern enables each component definition to register its own update systems via EnTT signals — component updates only fire when explicitly needed, keeping the loop efficient and the architecture data-driven.

---

## Relationship to Arachnivasion

This project was the individual prerequisite to the Arachnivasion team project. After each student completed their solo version, the team combined and extended this foundation — adding spiders, shooting mechanics, wave progression, and the live online leaderboard — to create Arachnivasion.

---

## Build

Requires CMake and the Vulkan SDK installed.

```
cmake -S ./ -B ./build
```

Open the generated `.sln` in Visual Studio and build.

---

*Built by Jacob Blackburn — Full Sail University, 2025*
