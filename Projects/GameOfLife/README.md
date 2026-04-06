# Conway's Game of Life

> **Role:** Solo Developer  
> **Language:** C++ (98.7%)  
> **Framework:** wxWidgets  
> **IDE:** Visual Studio  
> **Type:** Desktop Application / Cellular Automaton Simulation  

---

## About

A full implementation of Conway's Game of Life — the classic zero-player cellular automaton — built from scratch as a solo project. The application features a real-time simulation grid with interactive controls, customizable visuals, and binary file persistence for settings. This project demonstrated event-driven GUI programming, custom rendering pipelines, and binary I/O — all implemented without a game engine.

---

## Features

- **Real-Time Simulation** — play, pause, and step through generations at adjustable speed
- **Interactive Grid** — click to toggle individual cells alive or dead; drag to paint patterns
- **Finite & Toroidal Modes** — toggle between a bounded grid and a wrapping (infinite-edge) universe
- **Neighbor Count Overlay** — optional display showing each cell's live neighbor count in real time
- **Customizable Colors** — user-configurable living cell, dead cell, and grid line colors via settings dialog
- **Grid Display Options** — toggle grid lines and 10×10 section overlay independently
- **Settings Persistence** — settings saved and loaded from binary file so preferences carry across sessions
- **Live HUD** — generation counter and living cell population updated every tick
- **Custom Rendering Pipeline** — wxWidgets `DrawingPanel` with full control over cell and grid rendering

---

## Architecture

| Class | Responsibility |
|-------|---------------|
| `App` | wxWidgets application entry point |
| `MainWindow` | Primary window, toolbar, menu system, simulation game loop |
| `DrawingPanel` | Grid rendering, cell painting, mouse input handling |
| `Settings` | Configuration state — colors, grid options, speed, mode |
| `SettingsDialog` | UI dialog for editing and applying settings |

---

## Tech

- **C++** with wxWidgets for cross-platform desktop GUI
- Binary file I/O for persistent settings
- Custom paint event rendering for grid and cell visualization
- Event-driven architecture — mouse, timer, and menu events

---

## Build

Open `Game Of Life.sln` in Visual Studio and build. Requires wxWidgets configured in the project include and library paths.

---

*Built by Jacob Blackburn — Full Sail University, 2024*
