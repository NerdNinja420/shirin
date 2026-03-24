/// @file render/raycaster.h
/// @brief DDA raycasting engine — renders the 3D first-person view.
///
/// Algorithm overview (per vertical column):
///   1. Interpolate a point on the camera plane between r1 and r2.
///   2. Walk a ray from the player position towards that point using the
///      DDA (Digital Differential Analyser) grid-stepping algorithm.
///   3. Compute the perpendicular distance (dot product with facing direction)
///      to avoid the fisheye distortion that Euclidean distance would cause.
///   4. Derive wall-strip height from WIN_HEIGHT / perp_dist.
///   5. Draw the strip via Renderer::fill_rect into a render-target Texture.
///
/// Design notes:
///   - render() is called exactly once per frame (no double-render bug).
///   - The surface clear happens here, not inside input handling.
///   - The texture is written before being composited onto the window.

#pragma once

#include "math/vec2.h"
#include "render/renderer.h"
#include "world/player.h"
#include "world/scene.h"

/// @brief Owns the render-target Texture for the 3D view and performs DDA raycasting.
struct Raycaster {
    Texture texture; ///< Owning render-target texture, WIN_WIDTH × WIN_HEIGHT

    /// @brief Create the render-target texture.
    ///        Delegates to Texture::New; calls SDL_Log and exit(1) on failure.
    /// @param r  Renderer used to allocate the texture.
    static Raycaster New(Renderer &r);

    /// @brief Destroy the owned Texture.
    void destroy();

    /// @brief Render one full frame of the 3D view into texture.
    ///
    /// Fills the ceiling (top half) and floor (bottom half), then draws one
    /// vertical strip per ray column with distance fog applied.
    ///
    /// @param player  Current player position and facing direction.
    /// @param scene   World map used for wall queries.
    /// @param r       Renderer used for all draw calls.
    void render(const Player &player, const Scene &scene, Renderer &r);

  private:
    /// @brief Time from position @p p to the next integer grid boundary along
    ///        an axis, travelling at rate @p dp per unit time.
    ///        Returns INFINITY_VAL if dp == 0 (ray is parallel to that axis).
    float delta_t(float p, float dp) const;

    /// @brief Snap coordinate @p p to the nearest integer boundary in the
    ///        direction of @p delta, then advance by exactly 1 ULP via
    ///        nextafterf() so the result is unambiguously inside the next cell.
    ///        Using nextafterf avoids the float-precision trap where a fixed
    ///        epsilon (e.g. 1e-6f) can be a no-op at larger coordinates.
    float snap(float p, float delta) const;

    /// @brief Advance the ray from @p p1 toward @p p2 by exactly one grid-cell
    ///        boundary crossing (the smaller of the x- and y-intercept times).
    Vec2 ray_step(Vec2 p1, Vec2 p2) const;

    /// @brief Walk the ray until it hits a wall or exits the map.
    ///
    /// The DDA walk starts from @p p1 (the player position), not from @p p2.
    /// @p p2 is used only to determine the ray direction (p2 - p1).  Starting
    /// from p1 ensures walls between the player and the camera plane are never
    /// skipped, fixing the "see-through at FOV edges" artifact.
    ///
    /// @param p1         Ray origin (player position).
    /// @param p2         Camera-plane point — defines direction only, not start.
    /// @param scene      Map used for wall queries.
    /// @param[out] hit   World-space position of the wall hit (valid only when true).
    /// @return true if a wall was hit before leaving the map.
    bool cast_ray(Vec2 p1, Vec2 p2, const Scene &scene, Vec2 &hit) const;
};
