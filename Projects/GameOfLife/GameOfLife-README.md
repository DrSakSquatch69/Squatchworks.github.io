# Conway's Game of Life

> **Role:** Solo Developer  
> **Language:** C++ (98.7%)  
> **Framework:** wxWidgets / Windows Forms — Visual Studio  
> **Type:** Desktop Application  

---

## About

A full implementation of Conway's Game of Life — the classic cellular automaton — built from scratch as a student project. The application features a real-time simulation grid with play/pause/step controls, customizable settings, and file-based persistence.

---

## Features

- **Real-Time Simulation** — play, pause, and step through generations with adjustable speed
- **Interactive Grid** — click to toggle cells alive or dead; drag to paint patterns
- **Neighbor Count Display** — optional overlay showing each cell's live neighbor count
- **Customizable Colors** — configurable colors for living cells, dead cells, and grid lines via settings dialog
- **Finite & Infinite Modes** — toggle between a bounded grid and a toroidal (wrapping) universe
- **Grid Options** — show/hide grid lines and 10x10 section grid overlay
- **Settings Persistence** — save/load settings to binary file so preferences carry across sessions
- **Generation & Cell Counter** — live HUD tracking generation count and living cell population

---

## Architecture

| Class | Responsibility |
|-------|---------------|
| `App` | wxWidgets application entry point |
| `MainWindow` | Primary window, toolbar, menu system, game loop |
| `DrawingPanel` | Grid rendering, cell interaction, mouse input |
| `Settings` | Configuration state with binary save/load |
| `SettingsDialog` | UI dialog for modifying settings |

---

## Tech

- **C++** with wxWidgets for cross-platform GUI
- Binary file I/O for settings persistence
- Custom rendering pipeline for grid and cell visualization
- Event-driven architecture for user interaction

---

## Build

Open `Game Of Life.sln` in Visual Studio and build. Requires wxWidgets library to be configured in the project include/library paths.

---

*Built by Jacob Blackburn — Full Sail University, 2024*
