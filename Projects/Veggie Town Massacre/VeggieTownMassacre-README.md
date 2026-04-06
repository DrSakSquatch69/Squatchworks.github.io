# Veggie Town Massacre

> **Team:** Algorithm Architects  
> **Role:** Gameplay Programmer & Level Designer (Team Project)  
> **Engine:** Unity (C#) | URP  
> **Type:** 3D Action / Wave Shooter  

---

## About

Veggie Town Massacre is a 3D wave-based action shooter where players fight off hordes of sentient vegetable enemies terrorizing a town. Built across multiple sprints by Team Algorithm Architects, the game features distinct enemy AI types, a modular weapon system, environmental effects, a boss encounter, and a fully released installer build. Jacob served as the team's primary enemy AI programmer, level designer, and QA lead.

---

## Jacob's Contributions

### Pre-Production
- **Full level design and layout** — designed and built the game world, placed all environmental objects, and established the play space used throughout the entire game
- Defined product backlog items for the Beet, Turnip, and Orange enemy types

### Alpha Sprint
- Implemented **Orange enemy unique AI** — custom behavior distinct from base enemy logic
- Implemented **Beet enemy AI** and **Turnip enemy AI**
- Built the **Poison Gas Cloud** system — AI behavior, visual effects, and hazard logic
- Added **menu music** and **button click audio feedback** for the main menu
- Implemented **cross-level persistence** — player health, progress, and pickups correctly carry over between levels

### Beta Sprint
- Finalized and polished **Beet, Turnip, Orange, and Tomato** enemy behaviors
- Performed **asset cleanup and optimization** — removed unused assets to reduce project size
- Verified **TRC (Technical Requirements Checklist)** compliance
- Created the **game installer** for final release distribution
- Implemented **gameplay balance changes** across enemy stats and spawn rates
- **Stylized and refined the UI** across menus and HUD elements

### QA & Bug Fixes
- Fixed **main menu button scaling** issues at various resolutions
- Fixed **button hitbox misalignment** on the main menu
- Resolved **overlapping audio** when clicking main menu buttons rapidly
- Fixed **Credits button** causing unexpected game scene loads
- Fixed multiple **Poison Gas cloud bugs**
- Resolved **game crashes** triggered by quitting from the pause menu

---

## Full Feature Set

- **Multiple Enemy Types** — Carrot, Daikon, Beet, Turnip, Orange, Tomato, and Toxic enemies with distinct AI
- **Boss Fight** — Daikon King with unique mechanics
- **Enemy Factory System** — wave-based spawning with difficulty scaling
- **Gun System** — modular stats, ammo pickups, and destroyable bullet physics
- **Power-Up System** — collectible pickups modifying player abilities
- **Day/Night Cycle** — dynamic lighting affecting skybox and environment
- **Weather System** — particle-based rain controller
- **Poison Gas Hazards** — environmental hazard zones with visual feedback
- **Cross-Level Persistence** — player state preserved across scene transitions
- **Audio System** — spatial audio, menu music, player SFX
- **Installer Build** — packaged for end-user distribution

---

## Architecture

| System | Key Scripts |
|--------|------------|
| Player | `PlayerController`, `PlayerSoundManager`, `gun`, `gunStats` |
| Enemies | `EnemyAI`, `CarrotAI`, `DaikonAI`, `BeetAI`, `TurnipAI`, `OrangeAI`, `ToxicEnemy`, `DaikonKing`, `EnemyFactory` |
| Combat | `IDamage`, `damage`, `DestroyableBullet` |
| Environment | `DayNightCycle`, `RainController`, `SkyboxController`, `BouncePad`, `GasCloud` |
| Game Flow | `gameManager`, `MainManager`, `LoadingScreen`, `ButtonFns` |
| Pickups | `pickUp`, `PowerupPickup`, `Ammo Pickup` |

---

## Team

Algorithm Architects — Jacob Blackburn, Anthony, Chelsea, Jeff, Xavier

---

## Build

Open in Unity (URP project). Load the Main Game Scene and press Play. Or run the packaged installer for the release build.

---

*Built by Algorithm Architects — Full Sail University, 2025*
