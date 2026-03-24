#include "render/game.h"
#include "utils/constants.h"

#include <cstdio>
#include <cstdlib>

Game Game::New(const char *title, int width, int height, Player player, Scene scene) {
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
    g.window       = win;
    g.player       = player;
    g.scene        = scene;
    g.scene.player = &g.player;
    g.raycaster    = Raycaster::New();
    g.renderer     = Renderer::New(rend, width, height);
    return g;
}

void Game::destroy() {
    renderer.destroy();
    SDL_DestroyWindow(window);
    SDL_Quit();
    window = nullptr;
}

uint64_t Game::begin_frame() {
    return SDL_GetTicks();
}

void Game::handle_input(const Input &input) {
    player.direction += input.mouse_dx * Constants::MOUSE_SENSITIVITY;
    player.handle_input(scene, input);
}

void Game::end_frame(uint64_t frame_start_ticks) {
    uint64_t frame_ms = 1000 / Constants::FPS;
    uint64_t elapsed  = SDL_GetTicks() - frame_start_ticks;
    if (elapsed < frame_ms) {
        SDL_Delay((uint32_t)(frame_ms - elapsed));
    }
}

Input Game::poll_events() {
    Input input;
    input.forward      = false;
    input.backward     = false;
    input.strafe_left  = false;
    input.strafe_right = false;
    input.mouse_dx     = 0.0f;
    input.quit         = false;

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

    const bool *keys   = SDL_GetKeyboardState(nullptr);
    input.forward      = keys[SDL_SCANCODE_W];
    input.backward     = keys[SDL_SCANCODE_S];
    input.strafe_left  = keys[SDL_SCANCODE_A];
    input.strafe_right = keys[SDL_SCANCODE_D];

    return input;
}
