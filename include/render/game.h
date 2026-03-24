/// @file render/game.h
/// @brief Top-level SDL3 lifecycle owner: window, renderer, and event translation.

#pragma once

#include <SDL3/SDL.h>
#include <cstdint>

#include "input/input.h"
#include "render/renderer.h"

/// @brief Owns the SDL_Window and Renderer; translates SDL events into Input.
struct Game {
    SDL_Window *window;   ///< Application window
    Renderer    renderer; ///< Owns the underlying SDL_Renderer

    /// @brief Initialise SDL, create window and renderer, and capture the mouse.
    ///
    /// Prints to stderr and calls exit(1) on any initialisation failure.
    ///
    /// @param title   Window caption string.
    /// @param width   Window width in pixels.
    /// @param height  Window height in pixels.
    static Game New(const char *title, int width, int height);

    /// @brief Destroy the renderer, window, and call SDL_Quit.
    void destroy();

    /// @brief Record the start of a frame and return the current tick count.
    ///
    /// Pass the returned value to end_frame() so it can sleep for the
    /// remainder of the frame budget.
    ///
    /// @return  SDL tick count (milliseconds since SDL_Init) at frame start.
    uint64_t begin_frame();

    /// @brief Drain the SDL event queue and return an aggregated Input.
    ///
    /// Translates:
    ///   - SDL_EVENT_QUIT / Escape key   → input.quit = true
    ///   - SDL_EVENT_MOUSE_MOTION.xrel   → accumulated into input.mouse_dx
    ///   - SDL_GetKeyboardState W/S/A/D  → input.forward/backward/strafe_left/strafe_right
    Input poll_events();

    /// @brief Present the frame, then sleep for the remainder of the frame budget.
    ///
    /// Calls renderer.present(), then computes elapsed time since
    /// @p frame_start_ticks and calls SDL_Delay for any remaining budget
    /// (1000 / Constants::FPS milliseconds per frame).
    ///
    /// @param frame_start_ticks  Value returned by begin_frame() at the top of this frame.
    void end_frame(uint64_t frame_start_ticks);
};
