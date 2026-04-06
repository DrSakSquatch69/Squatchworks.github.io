# Untitled FPS — Vulkan ECS Game

> **Role:** Solo Developer  
> **Language:** C++  
> **Graphics API:** Vulkan  
> **Architecture:** Entity Component System (EnTT)  
> **Type:** 3D First-Person Shooter  

---

## About

A 3D first-person shooter built from scratch using Vulkan and the EnTT Entity Component System. The project features a custom Vulkan rendering pipeline, player movement, enemy entities, model loading, and a modular ECS architecture where each component manages its own update logic through signal-driven systems.

---

## Features

- **Custom Vulkan Renderer** — direct Vulkan API rendering with buffer management, GPU instance updates, and draw pipeline
- **ECS Architecture** — EnTT-based with the CCL (Connect Component Logic) pattern for signal-driven component updates
- **Player System** — input-driven player movement with configurable speed and entity management
- **Game Manager** — centralized game state with player/enemy visibility flags and entity collection management
- **Model Manager** — 3D model loading and entity creation from model assets
- **Level Loading** — custom level component system for environment geometry
- **Entity Collections** — named entity grouping system for managing logical groups of game objects
- **Modular Codebase** — organized into APP, DRAW, GAME, and UTIL modules

---

## Architecture

| Module | Key Files | Responsibility |
|--------|-----------|---------------|
| `APP` | `Window.hpp` | Window creation and management |
| `DRAW` | `VulkanRenderer`, `VulkanBuffers`, `DrawComponents`, `LevelComponents` | Vulkan rendering pipeline, GPU buffers, level geometry |
| `GAME` | `GameManager`, `player`, `ModelManager`, `GameComponents`, `SpawnHelpers` | Game logic, player control, entity management |
| `UTIL` | `Utilities`, `GameConfig` | Shared utilities and configuration |
| `CCL` | `CCL.h/.cpp` | Signal-driven component update system |

The CCL pattern enables each component definition file to register its own update systems via EnTT signals, making the architecture efficient and modular — component updates are only triggered when explicitly needed.

---

## Build

Requires CMake and the Vulkan SDK installed.

```
cmake -S ./ -B ./build
```

Open the generated `.sln` in Visual Studio and build.

---

*Built by Jacob Blackburn — Full Sail University, 2025*
