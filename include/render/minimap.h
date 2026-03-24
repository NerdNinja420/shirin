/// @file render/minimap.h
/// @brief Top-down minimap overlay renderer.
///
/// Renders into a Texture sized MINIMAP_W × MINIMAP_H.
/// The texture is composited over the top-left corner of the window
/// by main.cpp after the raycaster has finished writing its own texture.
///
/// Drawing order (back to front):
///   1. Clear to Color::BASE
///   2. Grid lines        (Color::RED)
///   3. Filled wall cells (Color::SURFACE2)
///   4. Player dot        (Color::FLAMINGO, filled circle)
///   5. FOV trapezoid     (Color::SKY): player→r1, player→r2, cam_center→r1, cam_center→r2

#pragma once

#include "math/vec2.h"
#include "render/renderer.h"
#include "world/player.h"
#include "world/scene.h"

/// @brief Owns the render-target Texture for the minimap overlay.
struct Minimap {
    Texture texture; ///< Owning render-target texture, MINIMAP_W × MINIMAP_H

    /// @brief Create the render-target texture.
    ///        Delegates to Texture::New; calls SDL_Log and exit(1) on failure.
    /// @param r  Renderer used to allocate the texture.
    static Minimap New(Renderer &r);

    /// @brief Destroy the owned Texture.
    void destroy();

    /// @brief Render one frame of the minimap into texture.
    ///
    /// @param player  Current player state (position, direction, FOV endpoints).
    /// @param scene   World map for wall cell data and minimap layout constants.
    /// @param r       Renderer used for all draw calls.
    void render(const Player &player, const Scene &scene, Renderer &r);

  private:
    /// @brief Convert a world-space position to minimap-texture pixel coordinates.
    ///        Uses scene.cell_size and gap offsets — no global state required.
    ///
    /// @param world_pos  Fractional grid position.
    /// @param scene      Provides cell_size, h_gab, v_gab for the conversion.
    /// @return           Pixel coordinate on the minimap texture.
    Vec2 world_to_minimap(Vec2 world_pos, const Scene &scene) const;
};
