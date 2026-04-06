# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this
repository.

## Commands

```bash
make compile        # Build (incremental; dependency tracking via -MMD -MP)
make run            # Build and run
make clean          # Remove build/
make F=scene        # Compile only src/objects/scene.cpp (single-file syntax check)
```

```bash
meson setup build   # Configure (once)
ninja -C build      # Build
```

No tests exist — verify changes by running the game.

## Architecture

A C++23 / SDL3 raycasting engine. Design goal: minimal C++ (structs + static factory methods,
no initializer-list constructors, no RAII classes), targeting C23 style with operator overloading
where useful.

### Layer structure

```
include/math/     src/math/       Pure math — Vec2. No SDL, no game state.
include/objects/  src/objects/    Game domain — Color, Scene, Player. No SDL.
include/utils/                    Constants, Input, logger. No SDL.
include/render/   src/render/     SDL3 + rendering — Renderer, Game, Raycaster. SDL3 confined here.
```

**SDL isolation rule**: `#include <SDL3/SDL.h>` appears in exactly two `.cpp` files:
`src/render/game.cpp` and `src/render/renderer.cpp`. No header includes SDL directly.

### Rendering pipeline (each frame)

```
Game::poll_events()           → SDL event queue → Input struct
Game::handle_input(input, dt) → mouse look + WASD movement (delta-time scaled) + portal check
Raycaster::render(...)        → ceiling/floor halves + wall strips directly into SDL renderer
Scene::render_minimap(...)    → top-down overlay drawn into SDL renderer
Renderer::present()           → SDL_RenderPresent
logger::info("FPS: %d", n)    → printed to stdout once per second
```

No FPS cap. Window opens at 90 % × 80 % of the primary display.

### Renderer

`Renderer` owns only `SDL_Renderer *sdl_renderer` — no CPU pixel buffer, no streaming texture.
All drawing uses SDL3 primitives directly:

| Method | SDL3 call |
|---|---|
| `set_color(Color)` | `SDL_SetRenderDrawColor` |
| `clear()` | `SDL_RenderClear` |
| `fill_rect / draw_rect` | `SDL_RenderFillRect / SDL_RenderRect` |
| `draw_line` | `SDL_RenderLine` |
| `plot` (private) | `SDL_RenderPoint` |
| `fill_column_aa` | three `SDL_RenderFillRect` calls (top blend, body, bottom blend) |
| `present()` | `SDL_RenderPresent` |

Window size is queried with `SDL_GetDisplayBounds(SDL_GetPrimaryDisplay(), &bounds)` in
`Game::New` and stored in `Game.width` / `Game.height`. These are threaded into
`Raycaster::render` and `Scene::render_minimap` at call time.

### DDA raycasting (Raycaster)

Key design choices:

- **Ray starts from the player**, not the camera-plane point. `cast_ray(p1, p2)` extracts
  direction from `p2-p1` but walks from `p1`. Prevents walls between player and camera plane
  from being skipped ("see-through at FOV edges").
- **`snap()` uses `nextafterf()`** rather than a fixed epsilon. Fixed epsilons fail at larger
  coordinates (IEEE 754 ULP issue); `nextafterf` always advances exactly 1 ULP.
- **Perpendicular distance** = `dot(hit - player.position, direction_vec())`. Avoids fisheye.
- **NS/EW lighting**: EW hits (ray crossed a vertical grid line, `hit.x ≈ integer`) → full
  brightness; NS hits → `color.half()` (each channel `>> 1`).
- **Analytical edge AA**: `fill_column_aa` blends the top pixel (ceiling→wall) and bottom pixel
  (wall→floor) by sub-pixel coverage fraction of `y_top` / `y_bottom`.

### Camera plane / FOV

`Player::get_fov_range(r1, r2)` builds a 90° FOV:
```
p    = position + direction_vec * CAMERA_PLANE_DIST   (= 1.0)
perp = p.rot90()                                      (unit length)
r1   = p - perp
r2   = p + perp
```
Raycaster lerps `t ∈ [0,1)` across `[r1, r2]` for each of the `RES` vertical columns.

### Scene

`Scene` stores `cols`, `rows`, `spawn`, and `const int *map` (flat 1D row-major).
`cell_size`, `h_gab`, `v_gab` are **not** stored — minimap layout is computed locally inside
`render_minimap(Renderer&, const Player&, int win_w, int win_h)` every frame.

`Scene::New` is `constexpr`; the three instances are `inline constexpr` in `include/objects/scene.h`:
```cpp
inline constexpr Scene THE_KEEP   = Scene::New(A, 32, 15, Vec2::New(7.5f, 5.5f));
inline constexpr Scene THE_DEPTHS = Scene::New(B, 64, 31, Vec2::New(2.5f, 2.5f));
inline constexpr Scene THE_CRYPT  = Scene::New(C, 24, 20, Vec2::New(2.5f, 2.5f));
```

Cell values: `0` = floor, `1–4` = wall types (color via `Color::from_cell`), `5` = portal
(passable; triggers scene transition in `Game::check_portal`).

Maps use flat 1D arrays (not `int[R][C]`) so they decay to `const int*` without a
`reinterpret_cast`, keeping `Scene::New` usable in `constexpr` context.

### Collision

`Game::try_move(delta)` uses a 4-corner AABB (± `COLLISION_MARGIN` = 0.2 world units).
Response is axis-separated: full move → X-only → Y-only, giving smooth wall-sliding.

### Color

`Color` (`include/utils/color.h`) stores `uint8_t r, g, b`. Palette entries are `static const`
members (`Color::MANTLE`, `Color::RED`, etc. — Catppuccin Mocha). Key methods:
- `Color::from_cell(int)` — maps cell value to wall color
- `Color::half()` — each channel `>> 1` (NS face shading)
- `Color::lerp(Color, float)` — linear interpolation (fog, AA blending)

### Key constants (`include/utils/constants.h`)

| Constant | Value | Purpose |
|---|---|---|
| `RES` | 5000 | Ray columns (higher = sharper, slower) |
| `MOVE_SPEED` | 6.0 | Player speed in world units/second |
| `MOUSE_SENSITIVITY` | 0.002 | Radians per pixel of horizontal mouse movement |
| `FOG_DISTANCE` | 10.0 | World units at which walls are fully fogged |
| `COLLISION_MARGIN` | 0.2 | Player AABB half-width |
| `CAMERA_PLANE_DIST` | 1.0 | Distance to camera plane (sets 90° FOV) |

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
- **No heap allocation** (`new`/`delete`). SDL resources are raw pointers managed by `destroy()`.
- **`this->` on every member access** — project-wide convention, no exceptions.
- **`inline constexpr`** for header-defined data that must be ODR-safe across TUs.
