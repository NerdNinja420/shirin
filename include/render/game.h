#ifndef GAME_H_
#define GAME_H_

#include <SDL3/SDL.h>
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
    int active;        // index of the current scene
    int width, height; // actual window dimensions, set from display in New()

    Scene &current_scene() {
        return this->scenes[this->active];
    }
    const Scene &current_scene() const {
        return this->scenes[this->active];
    }

    // Calls exit(1) on any initialisation failure.
    // Window size is 90% × 80% of the primary display.
    // scenes[0].enter() sets player position to the first scene's spawn.
    static Game New(const char *title, Player player, std::vector<Scene> scenes);
    void destroy();

    Input poll_events();
    // Applies mouse rotation, WASD movement, then checks for portal transition.
    void handle_input(const Input &input, float dt);

  private:
    // Tries delta; if blocked, retries X-only then Y-only (wall sliding).
    void try_move(Vec2 delta);
    // If player is standing on a portal cell (5), advance to the next scene.
    void check_portal();
};

#endif
