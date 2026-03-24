/// @file world/constants.h
/// @brief Compile-time game constants.
///
/// All values are constexpr so they fold into call sites at compile time.
/// Linux is the target platform
/// values since that is the target platform.

#pragma once

namespace Constants {

    constexpr int WIN_WIDTH  = 1280; ///< Window width in pixels
    constexpr int WIN_HEIGHT = 720;  ///< Window height in pixels
    constexpr int FPS        = 60;   ///< Target frames per second

    /// @brief Number of vertical ray columns. Higher values = sharper 3D view.
    constexpr int RES = 5000;

    /// @brief Fractional border gap around the minimap grid, relative to minimap width.
    constexpr float GAB_FACTOR = 0.02f;

    /// @brief Sentinel "infinity" returned when a ray travels parallel to an axis.
    constexpr float INFINITY_VAL = 1e30f;

    /// @brief World-space distance from the player eye to the camera plane.
    ///        Together with a normalized camera-plane half-width of 1.0, this
    ///        produces a 90-degree horizontal FOV (tan(45°) = 1/1).
    constexpr float CAMERA_PLANE_DIST = 1.0f;

    /// @brief World units the player moves per key-held frame.
    constexpr float MOVE_STEP = 0.1f;

    /// @brief Radians the player rotates per key-held frame.
    constexpr float TURN_STEP = 3.14159265f * 0.02f;

    /// @brief Radians per pixel of horizontal mouse movement.
    constexpr float MOUSE_SENSITIVITY = 0.002f;

    /// @brief Half-width of the player's collision bounding box (world units).
    ///        Keeps the player far enough from walls that the camera plane
    ///        cannot overlap them, preventing the see-through artifact.
    constexpr float COLLISION_MARGIN = 0.2f;

    /// @brief World-space distance at which wall color is fully replaced by fog.
    ///        Smaller = thicker fog, larger = clearer view.
    constexpr float FOG_DISTANCE = 10.0f;

    /// @brief Radius (in minimap pixels) of the player indicator dot.
    constexpr float PLAYER_DOT_RADIUS = 5.0f;

    /// Minimap overlay dimensions (one-third of the main window).
    constexpr int MINIMAP_W = WIN_WIDTH / 3;
    constexpr int MINIMAP_H = WIN_HEIGHT / 3;

} // namespace Constants
