#include "render/game.h"
#include "utils/constants.h"

#include <cmath>
#include <cstdio>
#include <cstdlib>

Game Game::New(const char *title, int width, int height, Player player) {
    if (!SDL_Init(SDL_INIT_VIDEO)) {
        fprintf(stderr, "SDL_Init failed: %s\n", SDL_GetError());
        exit(1);
    }

    SDL_Window *win = SDL_CreateWindow(title, width, height, 0);
    if (!win) {
        fprintf(stderr, "SDL_CreateWindow failed: %s\n", SDL_GetError());
        SDL_Quit();
        exit(1);
    }

    SDL_Renderer *rend = SDL_CreateRenderer(win, nullptr);
    if (!rend) {
        fprintf(stderr, "SDL_CreateRenderer failed: %s\n", SDL_GetError());
        SDL_DestroyWindow(win);
        SDL_Quit();
        exit(1);
    }

    SDL_SetWindowRelativeMouseMode(win, true);

    Game g;
    g.window = win;
    g.active = 0;
    g.scenes[0] = Scene::New((const int *)SceneData::A,
                             SceneData::A_COLS,
                             SceneData::A_ROWS,
                             Vec2::New(7.5f, 5.5f));
    g.scenes[1] = Scene::New((const int *)SceneData::B,
                             SceneData::B_COLS,
                             SceneData::B_ROWS,
                             Vec2::New(2.5f, 2.5f));
    g.scenes[2] = Scene::New((const int *)SceneData::C,
                             SceneData::C_COLS,
                             SceneData::C_ROWS,
                             Vec2::New(2.5f, 2.5f));
    g.player = player;
    g.scenes[0].enter(g.player); // set position to scene A spawn
    g.raycaster = Raycaster::New();
    g.renderer = Renderer::New(rend, width, height);
    return g;
}

void Game::destroy() {
    this->renderer.destroy();
    SDL_DestroyWindow(this->window);
    SDL_Quit();
    this->window = nullptr;
}

uint64_t Game::begin_frame() {
    return SDL_GetTicks();
}

void Game::try_move(Vec2 delta) {
    float m = Constants::COLLISION_MARGIN;

    auto blocked = [&](Vec2 pos) -> bool {
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

void Game::check_portal() {
    int col = (int)floorf(this->player.position.x);
    int row = (int)floorf(this->player.position.y);
    if (this->current_scene().cell_at(col, row) == 5) {
        this->active = (this->active + 1) % 3;
        this->current_scene().enter(this->player);
    }
}

void Game::handle_input(const Input &input) {
    this->player.direction += input.mouse_dx * Constants::MOUSE_SENSITIVITY;
    if (input.forward) this->try_move(this->player.direction_vec() * Constants::MOVE_STEP);
    if (input.backward) this->try_move(this->player.direction_vec() * -Constants::MOVE_STEP);
    if (input.strafe_left)
        this->try_move(-this->player.direction_vec().rot90() * Constants::MOVE_STEP);
    if (input.strafe_right)
        this->try_move(this->player.direction_vec().rot90() * Constants::MOVE_STEP);
    this->check_portal();
}

void Game::end_frame(uint64_t frame_start_ticks) {
    uint64_t frame_ms = 1000 / Constants::FPS;
    uint64_t elapsed = SDL_GetTicks() - frame_start_ticks;
    if (elapsed < frame_ms) {
        SDL_Delay((uint32_t)(frame_ms - elapsed));
    }
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
