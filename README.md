# NinjaRunner: Shadow Dash 🥷

![C++](https://img.shields.io/badge/C++-17-blue?logo=c%2B%2B)
![SFML](https://img.shields.io/badge/SFML-2.6-green?logo=sfml)
![Platform](https://img.shields.io/badge/Platform-Windows-lightgrey)
![Build](https://img.shields.io/badge/Build-CMake%20%7C%20MinGW-success)

A high-speed 2D rooftop parkour endless runner built with **C++17** and **SFML 2.6**. Play as a shadow ninja leaping across procedurally generated city skylines — run, jump, wall-grab, dash, and slide past obstacles while collecting shuriken coins and power-ups. Survive as long as you can across day and night cityscapes.

---

## 🎮 Features

- **2 Dynamic Levels**: Rooftop Day (sunlit city) and Rooftop Night (neon-lit metropolis)
- **3 Ninja Types**: Shadow (balanced), Blaze (fast), Storm (tank) — each with unique colors and stats
- **Full Parkour Moveset**: Jump, double-jump, wall grab/slide, dash, slide under obstacles, ledge grab
- **Procedural Audio**: All sound effects and background music generated in real time — no audio files needed
- **Procedural Textures**: All sprites, backgrounds, and UI elements rendered at runtime (90+ unique textures)
- **Flat Design Aesthetic**: Vector-style flat design with corporate palette, clean silhouettes, and modern UI
- **Power-Up System**: Shields, magnets, double points, speed boosts, and extra lives
- **Particle Effects**: Dust trails, power-up halos, screen shake
- **ECS Architecture**: Entity-Component-System design for clean, extensible game logic
- **Parallax Scrolling**: Multi-layer city backgrounds with depth effect
- **Bitmap Font Renderer**: Procedural 8×12 monospace font — no external font files required

---

## 🎬 Gameplay

NinjaRunner is an endless runner with a vertical twist. Your ninja auto-runs across a scrolling rooftop skyline. You must:

- **Jump & Double-Jump** over chimneys, AC units, and satellite dishes
- **Slide** under clotheslines and signs
- **Wall-Grab** on building edges to climb up
- **Dash** through dangerous sections at double speed
- **Collect** shuriken coins to increase your score
- **Grab** power-ups for temporary advantages
- **Survive** — each hit costs a life. Lose all three and it's game over!

The game progressively speeds up over time, making each run harder than the last.

### 🎯 Scoring

| Action | Points |
|--------|--------|
| Shuriken Coin collected | 100 |
| Distance (per unit) | 10× speed |
| Double Points power-up | 2× all income |

---

## 🎮 Controls

| Action | Default Key | In-Game | Menu |
|--------|-------------|---------|------|
| **Jump** | `Space` / `↑` / `W` | ✅ | — |
| **Double Jump** | `Space` (in air) | ✅ | — |
| **Slide** | `↓` / `S` / `Shift` | ✅ | — |
| **Dash** | `X` / `Z` | ✅ | — |
| **Wall Grab** | Hold `Space` near wall | ✅ | — |
| **Pause** | `Esc` / `P` | ✅ | — |
| **Navigate Menu** | `↑↓` / `WS` | — | ✅ |
| **Select** | `Space` / `Enter` | — | ✅ |
| **Cycle Ninja** | Select "Ninja" in menu | — | ✅ |

---

## 🥷 Ninjas

| Ninja | Health | Jump | Dash | Playstyle | Colors |
|-------|--------|------|------|-----------|--------|
| **Shadow** | 3 ❤️ | Standard (650) | 2× speed | Balanced | Navy & Crimson |
| **Blaze** | 2 ❤️ | Higher (700) | 2.5× speed | High risk, high reward | Black & Orange |
| **Storm** | 5 ❤️ | Lower (600) | 1.5× speed | Tank — survives more hits | Blue-Gray & Electric Blue |

> Cycle through ninja types from the main menu by selecting "Select Ninja".

---

## 🌃 Levels & Themes

### Rooftop Day ☀️
- Bright gradient sky (blue to warm horizon), corporate flat-design buildings
- **Palette**: Soft violet, corporate blue, pastel sky blue, coral accents
- Obstacles: chimney, AC unit, satellite dish, clothesline, sign, pigeon
- Parallax: 3-layer city skyline

### Rooftop Night 🌙
- Dark starry sky, crescent moon, pulsing neon signs on buildings
- **Palette**: Deep navy, dark violet, neon accents (pink, cyan, green, amber)
- Obstacles: same as day + neon decorations
- Features: 3-layer glow neon signs on ~30% of buildings

---

## ⚡ Power-Ups

| Power-Up | Effect | Duration |
|----------|--------|----------|
| 🛡️ **Shield** | Absorbs one hit (cyan halo) | Until hit |
| 🧲 **Magnet** | Attracts nearby coins automatically (yellow halo) | 8s |
| 🏆 **Double Points** | All score income is doubled (amber halo) | 8s |
| 💨 **Speed Boost** | Temporary burst of speed (red halo) | 8s |
| ❤️ **Extra Life** | Restores one health point | Instant |

> Active power-ups display a pulsing colored halo around the ninja.

---

## 🏗️ Architecture

```
ninja_runner/
├── include/
│   ├── core/          # Game loop, input, audio, assets, ECS
│   ├── ecs/           # Entity-Component-System framework
│   ├── components/    # Transform, Physics, Collision, Health, etc.
│   ├── entities/      # Player, Obstacle factories
│   ├── systems/       # Render, Physics, Collision, Animation, etc.
│   ├── levels/        # Level definitions and manager
│   └── ui/            # Menu, HUD, Game Over screen
├── src/               # Implementation files (mirrors include/)
├── CMakeLists.txt     # CMake build configuration
└── README.md
```

### ECS Components

| Component | Purpose |
|-----------|---------|
| `TransformComponent` | Position, scale, rotation |
| `PhysicsComponent` | Velocity, gravity, ground/wall state |
| `CollisionComponent` | Hitbox, trigger/static, tag |
| `SpriteComponent` | Texture, color, visibility |
| `AnimationComponent` | Frame-based sprite animation with per-frame textures |
| `HealthComponent` | HP, invulnerability, damage |
| `PlayerComponent` | Ninja type, jump/dash/slide params |

### Systems

| System | Responsibility |
|--------|---------------|
| `RenderSystem` | Draws all visible entities (z-sorted) |
| `PhysicsSystem` | Gravity, movement, ground/wall detection |
| `CollisionSystem` | AABB collision detection + callbacks |
| `AnimationSystem` | Frame timers, sprite texture updates |
| `PlayerControlSystem` | Input → jump/dash/slide/wall-grab |
| `ParticleSystem` | Dust trails, environmental particles |
| `ParallaxSystem` | Multi-layer background scrolling |

---

## 🔧 Building

### Prerequisites

- **Windows** (the game uses Win32 `waveOut` API for audio)
- **CMake** ≥ 3.16
- **MinGW-w64** (GCC with POSIX threads and UCRT)
- **Make** (or MinGW Make)

### Build Steps

```bash
cd ninja_runner
mkdir build && cd build
cmake .. -G "MinGW Makefiles"
mingw32-make -j$(nproc)
```

The executable `NinjaRunner.exe` will be created in `build/`.

### Dependencies

The project references SFML 2.6.2 prebuilt libraries from `../dino_game/sfml/`. Ensure the dino_game project is checked out alongside ninja_runner:

```
/parent/
├── dino_game/
│   └── sfml/        # Shared SFML binaries
└── ninja_runner/    # This project
```

### Build Configuration

The `CMakeLists.txt` is preconfigured for **static linking** with SFML 2.6.2 (`-s` variants). The build:

1. Compiles all C++ sources with `-std=c++17 -Wall -Wextra -Wpedantic`
2. Links SFML Graphics, Window, and System (static)
3. Links Win32 libraries: `winmm`, `opengl32`, `gdi32`, `ws2_32`, `ole32`, `oleaut32`, `uuid`
4. Links `freetype` for font rendering
5. Links `stdc++` for MinGW ABI stability

> **Note:** Audio does NOT use SFML Audio — it uses the Win32 `waveOut` API directly (via `winmm.dll`).

---

## 📁 Project Structure

### Source Layout

```
src/
├── main.cpp                 # Entry point
├── core/
│   ├── Game.cpp             # Main game loop, state management
│   ├── InputManager.cpp     # Keyboard input with action queries
│   ├── AssetManager.cpp     # Procedural texture generation (90+ textures)
│   ├── AudioManager.cpp     # Win32 waveOut audio, procedural sound/music
│   ├── FontRenderer.cpp     # Procedural bitmap font renderer
│   └── KeyBindings.cpp      # Key binding configuration
├── systems/                 # 7 ECS systems
├── entities/                # Factory classes
├── levels/                  # 2 level implementations
└── ui/                      # 3 UI screens
```

### Code Statistics

| Metric | Count |
|--------|-------|
| Source files | ~25 |
| Header files | ~22 |
| Procedural textures | 90+ |
| Procedural sounds | 8 effects + background music |
| Unique entities | 8+ types (player, obstacles, coins, power-ups) |

---

## 🧪 Technical Highlights

### Procedural Audio Pipeline

All audio is generated mathematically at runtime — zero audio file dependencies:

- **Sound effects**: ADSR envelope + sine/square waves + noise mixing
  - `jump` — frequency sweep 400→900 Hz, 200ms
  - `double jump` — sweep 500→1200 Hz, 300ms
  - `coin` — square wave ding at 1200 Hz, 80ms
  - `damage` — white noise + low sweep 300→100 Hz, 200ms
  - `powerup` — ascending arpeggio, 400ms
  - `gameover` — descending minor third, 500ms
- **Music**: Background loop with melody (pentatonic scale), bass line, chords, and percussion

### Procedural Textures

All sprites are pixel-drawn at runtime using `sf::Image`:

- Ninja sprite sheets: 8 run frames + jump + slide pose per ninja type (3 types = 30 textures)
- Obstacles: chimney, AC unit, satellite dish, clothesline, sign, pigeon (2 frames)
- City parallax backgrounds: procedural skyline generator
- UI elements: shuriken coins, power-ups, particles

### Flat Design Aesthetic

Buildings and UI follow a modern flat design style:
- **Corporate palette**: Corporate blue, pastel sky blue, soft violet, soft coral, pastel orange
- **Dark blue outlines**: 2px `#2C3E50` borders on all buildings
- **Minimalist shapes**: Clean rectangles with subtle shadow offsets
- **No noise**: Zero pixel noise or texture artifacts
- **Vector-ready**: All geometry is clean rectangles and circles

### ECS Framework

Lightweight custom ECS with:

- Type-safe component access via `std::type_index`
- `std::unique_ptr` ownership for automatic cleanup
- Template-based `addComponent<T>()` / `getComponent<T>()`
- Collision callback system with string tags

### Font Renderer

Custom procedural bitmap font renderer (`FontRenderer`) that:
- Generates a 96-character ASCII atlas (8×12 px per glyph, 16×6 grid)
- Renders text via vertex arrays for efficiency
- Supports arbitrary font sizes via bilinear scaling
- Works without any external .ttf files

---

## 📜 License

This project is provided for educational and personal use. SFML is distributed under the [zlib/libpng license](https://www.sfml-dev.org/license.php).

---

## 🙌 Contributing

Contributions are welcome! Ideas for improvement:

- **More city biomes**: Industrial district, Shinjuku night, Chinatown
- **New obstacles**: Drone, window cleaner scaffold, open hatch
- **Boss encounters**: Helicopter chase, rival ninja
- **Online leaderboards**: High score server
- **Mobile build**: Touch controls for Android/iOS

---

*Built with 💚 and C++17*
