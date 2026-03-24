/// @file render/renderer.h
/// @brief SDL3 drawing abstraction layer.
///
/// This header includes <SDL3/SDL.h> and exposes SDL_Renderer* / SDL_Texture*
/// directly as struct members.  Domain layers (world/, math/, input/) are
/// insulated from SDL by not including this header — they never need to.
/// All actual SDL draw calls remain in src/render/renderer.cpp.

#pragma once

#include <SDL3/SDL.h>

#include "world/color.h"

/// @brief Owning wrapper around an SDL_Texture render-target.
struct Renderer; // forward declaration — defined below; needed by Texture::New

/// @brief Owning wrapper around an SDL_Texture render-target.
struct Texture {
    SDL_Texture *handle; ///< Underlying SDL texture; nullptr after destroy()
    int          w;      ///< Texture width in pixels
    int          h;      ///< Texture height in pixels

    /// @brief Allocate a new TEXTUREACCESS_TARGET texture.
    ///        Calls SDL_Log and exit(1) on failure.
    ///
    /// @param r  Renderer that will own the underlying SDL_Texture.
    /// @param w  Width in pixels.
    /// @param h  Height in pixels.
    static Texture New(Renderer &r, int w, int h);

    /// @brief Destroy the underlying SDL_Texture and null the handle.
    void destroy();
};

/// @brief Thin wrapper around SDL_Renderer that exposes a color/geometry drawing
///        API usable by domain code without calling SDL functions directly.
struct Renderer {
    SDL_Renderer *handle; ///< Underlying SDL renderer

    /// @brief Set the active draw color for subsequent fill/line/clear calls.
    /// @param c  RGB color; alpha is always 255.
    void set_color(Color c);

    /// @brief Fill a rectangle.
    /// @param x  Left edge in pixels.
    /// @param y  Top edge in pixels.
    /// @param w  Width in pixels.
    /// @param h  Height in pixels.
    void fill_rect(float x, float y, float w, float h);

    /// @brief Draw a line from (x0, y0) to (x1, y1) in pixels.
    void draw_line(float x0, float y0, float x1, float y1);

    /// @brief Clear the current render target using the active draw color.
    void clear();

    /// @brief Composite @p tex onto the current render target at pixel rect (x, y, w, h).
    ///
    /// @param tex  Source texture to blit.
    /// @param x    Destination left edge in pixels.
    /// @param y    Destination top edge in pixels.
    /// @param w    Destination width in pixels.
    /// @param h    Destination height in pixels.
    void blit(Texture &tex, float x, float y, float w, float h);

    /// @brief Redirect subsequent draw calls into @p tex.
    /// @param tex  Target texture to render into.
    void set_target(Texture &tex);

    /// @brief Restore the render target to the window framebuffer.
    void reset_target();

    /// @brief Flip the window's back buffer to the screen.
    void present();
};
