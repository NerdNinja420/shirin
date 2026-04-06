#include "render/game.h"
#include "utils/constants.h"
#include "utils/logger.h"

#include <cmath>
#include <fstream>
#include <functional>
#include <stdexcept>

Game Game::New(const char *title, Player player, std::vector<Scene> scenes) {
    if (!SDL_Init(SDL_INIT_VIDEO)) {
        throw std::runtime_error(std::string("SDL_Init failed: ") + SDL_GetError());
    }

    SDL_Rect bounds;
    SDL_GetDisplayBounds(SDL_GetPrimaryDisplay(), &bounds);
    int width = (int)(bounds.w * 0.9f);
    int height = (int)(bounds.h * 0.8f);

    SDL_Window *win = SDL_CreateWindow(title, width, height, 0);
    if (!win) {
        SDL_Quit();
        throw std::runtime_error(std::string("SDL_CreateWindow failed: ") + SDL_GetError());
    }

    SDL_SetWindowRelativeMouseMode(win, true);

    Game g;
    g.window = win;
    g.width = width;
    g.height = height;
    g.active = 0;
    g.scenes = scenes;
    g.player = player;

    // Load last active scene from save file if present.
    {
        std::ifstream f("save.dat");
        int saved = 0;
        if (f >> saved && saved >= 0 && saved < (int)g.scenes.size()) g.active = saved;
    }

    g.scenes[g.active].enter(g.player);
    g.raycaster = Raycaster::New();
    g.renderer = Renderer::New(win);
    return g;
}

void Game::destroy() {
    // Persist active scene so the next run resumes from the same scene.
    {
        std::ofstream f("save.dat");
        if (f.is_open()) f << this->active;
    }
    this->renderer.destroy();
    SDL_DestroyWindow(this->window);
    SDL_Quit();
    this->window = nullptr;
}

void Game::try_move(Vec2 delta) {
    float m = Constants::COLLISION_MARGIN;

    // [&] means that the lambda captures the scopes variables by reference
    std::function<bool(Vec2)> blocked = [&](Vec2 pos) -> bool {
        return this->current_scene().is_wall((int)(pos.x + m), (int)(pos.y + m)) ||
               this->current_scene().is_wall((int)(pos.x - m), (int)(pos.y + m)) ||
               this->current_scene().is_wall((int)(pos.x + m), (int)(pos.y - m)) ||
               this->current_scene().is_wall((int)(pos.x - m), (int)(pos.y - m));
    };

    Vec2 next = this->player.position + delta;
    if (!blocked(next)) {
        this->player.position = next;
        return;
    }

    Vec2 next_x = {this->player.position.x + delta.x, this->player.position.y};
    if (!blocked(next_x)) {
        this->player.position = next_x;
        return;
    }

    Vec2 next_y = {this->player.position.x, this->player.position.y + delta.y};
    if (!blocked(next_y)) {
        this->player.position = next_y;
    }
}

// TODO: Add support for portal to specific scenes.
// TODO: Add support for several portals in one scene.
void Game::check_portal() {
    int col = (int)floorf(this->player.position.x);
    int row = (int)floorf(this->player.position.y);
    if (this->current_scene().cell_at(col, row) == 5) {
        this->active = (this->active + 1) % (int)this->scenes.size();
        this->current_scene().enter(this->player);
    }
}

void Game::handle_input(const Input &input, float dt) {
    float m = Constants::MOVE_SPEED;

    this->player.direction += input.mouse_dx * Constants::MOUSE_SENSITIVITY;
    if (input.forward) this->try_move(this->player.direction_vec() * m * dt);
    if (input.backward) this->try_move(this->player.direction_vec() * -m * dt);
    if (input.strafe_left) this->try_move(-this->player.direction_vec().rot90() * m * dt);
    if (input.strafe_right) this->try_move(this->player.direction_vec().rot90() * m * dt);
    this->check_portal();
}

Input Game::poll_events() {
    Input input;
    input.forward = false;
    input.backward = false;
    input.strafe_left = false;
    input.strafe_right = false;
    input.mouse_dx = 0.0f;
    input.quit = false;

    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_EVENT_QUIT) {
            input.quit = true;
        }
        if (event.type == SDL_EVENT_KEY_DOWN && event.key.scancode == SDL_SCANCODE_ESCAPE) {
            input.quit = true;
        }
        if (event.type == SDL_EVENT_MOUSE_MOTION) {
            input.mouse_dx += event.motion.xrel;
        }
    }

    const bool *keys = SDL_GetKeyboardState(nullptr);
    input.forward = keys[SDL_SCANCODE_W];
    input.backward = keys[SDL_SCANCODE_S];
    input.strafe_left = keys[SDL_SCANCODE_A];
    input.strafe_right = keys[SDL_SCANCODE_D];

    return input;
}
