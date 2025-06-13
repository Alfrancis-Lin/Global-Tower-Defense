# Global Tower Defense

**Global Tower Defense** is an immersive and feature-rich Tower Defense game built in **C++** using the **Allegro5** game development library. Created as a final project for a school assignment, this game offers a compelling blend of strategy, action, multiplayer fun, and system design. With procedurally generated maps, an upgrade system, and a custom player data system in JSON, it’s made with an eye toward scalability and future development.

---

## Game Features

### Gameplay & Mechanics

- **4 Unique Stages**:
  1. **Basic Stage** – Introduces core gameplay mechanics.
  2. **Advanced Enemies Stage** – New enemy types are introduced.
  3. **Multiplayer Stage** – One player places towers, while the other summons enemies using keys (`Y`, `U`, `I`, `O`).
  4. **Procedural Maze Stage** – Randomly generated maps to spice up replayability.

- **Sophisticated Enemies**:
  - Standard walkers
  - Splitting enemies that divide on death
  - Planes that drop foot soldiers
  - Bombers that attach and destroy turrets

- **Diverse Turrets**:
  - Damage dealers
  - Status effect appliers (e.g., slow)
  - Coin farming turrets for resource generation

- **Upgrade System**:
  - Level up your turrets to enhance damage, fire rate, and effects
  - Fascinating sprites and bullets on upgrade

- **Obstacles & Environment**:
  - Rocks and logs dynamically placed to add challenge
  - Turrets must shoot through obstacles to hit targets

- **Revival Minigame**:
  - Play a Chrome Dino-style game after death
  - Survive to 100 points to revive and restart.

---

## Project Structure & Architecture

- Allegro5 for graphics, audio, and event handling
- CMake and `Makefile` support for streamlined building
- Player data management using JSON [nlohmann/json](https://github.com/nlohmann/json)
- Local HTTP leaderboard site** using Python and HTML (optional but fun!)

---

## Save System and JSON Integration

Player progression (name, level, experience) is saved as `.json` files. This structure is designed for future integration with external databases, such as Firebase or MySQL, making it extensible for online functionality.

---

## Local Leaderboard Website

Upon victory, players can enter their name to be listed on a locally hosted website that displays the leaderboard. The backend is powered by Python's built-in HTTP server, while the frontend is rendered in HTML.

---

## Setup & Installation

### Requirements

- C++ compiler (e.g., `g++`)
- [Allegro5](https://liballeg.org/)
- CMake (or use the provided Makefile)
- [nlohmann/json](https://github.com/nlohmann/json/releases/tag/v3.12.0) The header file will do ```json.hpp```

### Build Instructions

#### Option 1: Using Makefile

zsh (Sorry, our developer only can guarantee for zsh)
```
git clone https://github.com/Alfrancis-Lin/Global-Tower-Defense.git
cd Global-Tower-Defense
make
````

# Credits
Please refer to [CREDITS.md](CREDITS.md)
