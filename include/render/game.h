#ifndef GAME_H_
#define GAME_H_

#include <SDL3/SDL.h>
#include <cstdint>

#include "utils/input.h"
#include "render/renderer.h"

struct Game {
    SDL_Window *window;
    Renderer    renderer;

    // Calls exit(1) on any initialisation failure.
    static Game New(const char *title, int width, int height);
    void destroy();

    // Returns the tick count to pass to end_frame().
    uint64_t begin_frame();
    Input    poll_events();
    // Presents the frame, then sleeps for the remainder of the frame budget.
    void end_frame(uint64_t frame_start_ticks);
};

#endif
