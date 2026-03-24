#ifndef GAME_H_
#define GAME_H_

#include <SDL3/SDL.h>
#include <cstdint>

#include "objects/player.h"
#include "objects/scene.h"
#include "render/raycaster.h"
#include "render/renderer.h"
#include "utils/input.h"

struct Game {
    SDL_Window *window;
    Renderer renderer;
    Scene scene;
    Player player;
    Raycaster raycaster;

    // Calls exit(1) on any initialisation failure.
    static Game New(const char *title, int width, int height, Player player, Scene scene);
    void destroy();

    // Returns the tick count to pass to end_frame().
    uint64_t begin_frame();
    Input poll_events();
    // Applies mouse rotation and WASD movement with collision.
    void handle_input(const Input &input);
    // Sleeps for the remainder of the frame budget. Does NOT call present().
    void end_frame(uint64_t frame_start_ticks);

  private:
    // Tries delta; if blocked, retries X-only then Y-only (wall sliding).
    void try_move(Vec2 delta);
};

#endif
