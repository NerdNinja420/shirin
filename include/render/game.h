#ifndef GAME_H_
#define GAME_H_

#include <SDL3/SDL.h>
#include <cstdint>
#include <initializer_list>
#include <vector>

#include "objects/player.h"
#include "objects/scene.h"
#include "render/raycaster.h"
#include "render/renderer.h"
#include "utils/input.h"

struct Game {
    SDL_Window *window;
    Renderer renderer;
    Player player;
    Raycaster raycaster;
    std::vector<Scene> scenes;
    int active; // index of the current scene

    Scene &current_scene() {
        return this->scenes[this->active];
    }
    const Scene &current_scene() const {
        return this->scenes[this->active];
    }

    // Calls exit(1) on any initialisation failure.
    // scenes[0].enter() sets player position to the first scene's spawn.
    static Game New(const char *title,
                    int width,
                    int height,
                    Player player,
                    std::initializer_list<Scene> scenes);
    void destroy();

    // Returns the tick count to pass to end_frame().
    uint64_t begin_frame();
    Input poll_events();
    // Applies mouse rotation, WASD movement, then checks for portal transition.
    void handle_input(const Input &input);
    // Sleeps for the remainder of the frame budget. Does NOT call present().
    void end_frame(uint64_t frame_start_ticks);

  private:
    // Tries delta; if blocked, retries X-only then Y-only (wall sliding).
    void try_move(Vec2 delta);
    // If player is standing on a portal cell (5), advance to the next scene.
    void check_portal();
};

#endif
