# Veggie Town Massacre

> **Team:** Algorithm Architects  
> **Role:** Gameplay Programmer (Team Project)  
> **Engine:** Unity (C#) | URP  
> **Type:** 3D Action / Shooter  

---

## About

Veggie Town Massacre is a 3D action game where players fight waves of sentient vegetable enemies terrorizing a town. Built as a team project by Algorithm Architects, the game features multiple enemy types with unique AI behaviors, a gun and ammo system, power-ups, a day/night cycle, environmental effects, and a boss fight against the Daikon King.

---

## Features

- **Multiple Enemy Types** — Carrot, Daikon, and Toxic enemies each with distinct AI behaviors and attack patterns
- **Boss Fight** — Daikon King boss encounter with unique mechanics
- **Enemy Factory System** — wave-based spawning with `EnemyFactory` managing enemy creation and difficulty scaling
- **Gun System** — modular gun stats, ammo pickups, and destroyable bullet physics
- **Power-Up System** — collectible power-ups that modify player abilities
- **Day/Night Cycle** — dynamic lighting system with `DayNightCycle` controller affecting skybox and environment
- **Weather Effects** — rain controller with particle-based weather system
- **Camera System** — custom `CameraController` for third-person gameplay
- **Audio System** — menu music manager, player sound effects, spatial audio
- **Loading Screen** — scene transition system with loading screen prefab
- **Damage Interface** — `IDamage` interface for consistent damage handling across all entities
- **Tutorial System** — tutorial scene introducing game mechanics to new players
- **Bounce Pads** — environmental bounce mechanics for traversal

---

## Architecture

The project follows a component-based Unity architecture with clear separation between player systems, enemy AI, environmental effects, and game management.

| System | Key Scripts |
|--------|------------|
| Player | `PlayerController`, `PlayerSoundManager`, `gun`, `gunStats` |
| Enemies | `EnemyAI`, `CarrotAI`, `DaikonAI`, `DaikonKing`, `ToxicEnemy`, `EnemyFactory` |
| Combat | `IDamage`, `damage`, `DestroyableBullet` |
| Environment | `DayNightCycle`, `RainController`, `SkyboxController`, `BouncePad` |
| Game Flow | `gameManager`, `MainManager`, `LoadingScreen`, `ButtonFns` |
| Pickups | `pickUp`, `PowerupPickup`, `Ammo Pickup` |

---

## Team

Algorithm Architects — Jacob, Anthony, Chelsea, Jeff, Xavier

---

## Build

Open in Unity (URP project). Load the Main Game Scene and press Play.

---

*Built by Algorithm Architects — Full Sail University, 2025*
