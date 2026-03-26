# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this
repository.

## Commands

```bash
make compile        # Build (incremental; dependency tracking via -MMD -MP)
make run            # Build and run
make clean          # Remove build/
make F=scene        # Compile only src/world/scene.cpp (single-file syntax check)
```

No tests exist — verify changes by running the game.

## Architecture

A C++ / SDL3 raycasting engine. The goal was minimal C++ (structs +
static factory methods, no initializer-list constructors, no RAII classes), targeting C23 style with
operator overloading where useful.

### Layer structure

The project is split into four logical layers, each in its own subdirectory:

```
include/math/     src/math/       Pure math — Vec2. No SDL, no game state.
include/world/    src/world/      Game domain — Color, Constants, Scene, Player. No SDL.
include/input/                    Input snapshot struct (Input). No SDL.
include/render/   src/render/     SDL3 abstraction + rendering — Renderer, Texture, Game,
                                  Raycaster, Minimap. SDL3 is confined here.
```

**SDL isolation rule**: SDL3 headers (`#include <SDL3/SDL.h>`) appear in exactly two files:
`src/render/renderer.cpp` and `src/render/game.cpp`. No other file — especially no header —
includes SDL directly. All other code uses `Renderer`, `Texture`, and `Game` from
`include/render/renderer.h` / `include/render/game.h`.

### Rendering pipeline (each frame)

```
Game::begin_frame()        → record frame-start tick
Game::poll_events()        → SDL events translated to Input struct
player.direction +=        → mouse look applied directly
Player::handle_input()     → movement + AABB collision
Raycaster::render()        → writes 3D view into raycaster.texture
Minimap::render()          → writes overlay into minimap.texture
game.renderer.clear()
game.renderer.blit(...)    → composite both textures onto window
Game::end_frame()          → SDL_RenderPresent + frame-cap sleep
```

Both `Raycaster` and `Minimap` render into their own `Texture` (TEXTUREACCESS_TARGET) by
calling `r.set_target(texture)`, then `r.reset_target()` when done. `main.cpp` composites
them with `game.renderer.blit()`.

### SDL abstraction (Renderer + Texture + Game)

`Renderer` and `Texture` store their SDL handles as `void*` (cast to `SDL_Renderer*` /
`SDL_Texture*` inside `renderer.cpp`). This keeps SDL types completely out of all headers
outside the render layer.

`Game` owns the SDL_Window and Renderer. `Game::poll_events()` translates the full SDL
event queue into a plain `Input` struct — the only place SDL scancodes and event types appear.

### DDA raycasting (Raycaster)

Key design choices that differ from typical tutorials:

- **Ray origin is the player position**, not the camera-plane point. `cast_ray(p1, p2)` extracts
  direction from `p2-p1` but starts walking from `p1`. This prevents walls between the player
  and camera plane from being skipped ("see-through at FOV edges").
- **`snap()` uses `nextafterf()`** rather than a fixed epsilon (`+1e-6f`). Fixed epsilons become
  no-ops at larger coordinate values (IEEE 754 ULP issue); `nextafterf` always advances exactly 1
  ULP.
- **Perpendicular distance** = `dot(hit - player.position, direction_vec())`. Avoids fisheye.

### Camera plane / FOV

`Player::get_fov_range(r1, r2)` builds a 90° FOV:
```
p    = position + direction_vec * CAMERA_PLANE_DIST   (CAMERA_PLANE_DIST = 1.0)
perp = (p - position).rot90()                         (already unit length)
r1   = p - perp
r2   = p + perp
```
The raycaster lerps `t ∈ [0,1)` across `[r1, r2]` for each of the `RES` vertical columns.

### Collision

`Player::try_move(delta, scene)` uses a 4-corner AABB (±`COLLISION_MARGIN` = 0.2 world units).
Collision response is axis-separated: if the full move is blocked, try X-only, then Y-only — this
gives smooth wall-sliding.

### Color

`Color` (in `include/world/color.h`) is a single class that combines the old `struct Color` and
`namespace Colors`. Palette entries are static const members: `Color::MANTLE`, `Color::RED`, etc.
`Color::from_cell(int)` replaces the old free function `wall_color(int)`.

`Renderer::set_color(Color)` handles the SDL conversion internally (reads `c.r, c.g, c.b`
directly) — `Color` itself has no SDL dependency.

### Commit conventions

Use actual emoji characters (not `:name:` syntax):

| # | Emoji | Type | When to use |
|---|-------|------|-------------|
| 1 | 🎨 | `style` | formatting, whitespace changes |
| 2 | ⚙️ | `conf` | change configuration |
| 3 | ✨ | `feat` | add new functionality |
| 4 | 🩹 | `fix` | quick fix or correction |
| 5 | 🐛 | `bug` | fix discovered bug |
| 6 | 🚀 | `perf` | improve performance |
| 7 | ♻️ | `refactor` | restructure code |
| 8 | 🚚 | `move` | move or rename code |
| 9 | 💣 | `breaking` | introduce breaking changes |
| 10 | 🔀 | `merge` | merge branches |
| 11 | 📦 | `deps` | update dependencies |
| 12 | 📚 | `docs` | update documentation |
| 13 | 🧪 | `test` | add or update tests |
| 14 | 🔧 | `chore` | maintenance tasks |
| 15 | 🔒 | `security` | fix security issues |
| 16 | 🛠️ | `env` | development environment |
| 17 | 📝 | `revert` | revert changes |
| 18 | 🎯 | `wip` | work in progress |

### Coding conventions

- **No C++ initializer-list constructors.** All types use static factory methods:
  ```cpp
  TypeName TypeName::New(...) { TypeName t; t.field = val; return t; }
  ```
- **No heap allocation** (no `new`/`delete`). SDL resources are raw pointers stored as `void*`
  in Renderer/Texture/Game; cleanup via `destroy()`.
- Structs own SDL resources when created via their `New()` factories; `Renderer` references passed
  to `Raycaster::New` / `Minimap::New` are non-owning (the `Game` owns the renderer lifetime).

### Key constants to tune

All in `include/world/constants.h`:
- `RES` — number of ray columns (currently 5000; higher = sharper, slower)
- `FOG_DISTANCE` — world units at which walls are fully fogged (currently 10.0)
- `COLLISION_MARGIN` — player AABB half-width (currently 0.2)
- `MOUSE_SENSITIVITY` — radians per pixel of horizontal mouse movement

### Map

`Scene::New()` initialises the 32×15 grid hardcoded in `src/world/scene.cpp`. Cell values:
0 = empty, 1–4 = wall types (colors differ via `Color::from_cell`). Player spawns at
`(7.5, 5.5)` facing `π * 1.25`.
