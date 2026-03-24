/// @file world/player.h
/// @brief Player position, orientation, input handling, and FOV computation.
///
/// Player owns no SDL or render resources.  Input is received as the
/// platform-agnostic Input struct (see input/input.h) rather than raw SDL
/// keyboard state, so this header has no SDL dependency.

#pragma once

#include "input/input.h"
#include "math/vec2.h"
#include "world/scene.h"

/// @brief Player state.
struct Player {
    Vec2  position;  ///< World-space position in grid coordinates (float)
    float direction; ///< Facing angle in radians (0 = right, π/2 = down)

    /// @brief Spawn a player at the given world position facing the given angle.
    static Player New(Vec2 position, float direction);

    /// @brief Unit vector pointing in the current facing direction.
    Vec2 direction_vec() const;

    /// @brief World-space centre of the camera plane (one unit ahead of the player).
    Vec2 camera_center() const;

    /// @brief Compute the two endpoints of the camera plane.
    ///
    /// Construction:
    ///   p  = position + direction_vec * CAMERA_PLANE_DIST
    ///   r1 = p - (p - position).rot90().normalize()
    ///   r2 = p + (p - position).rot90().normalize()
    ///
    /// Because direction_vec has unit length and CAMERA_PLANE_DIST = 1,
    /// (p - position) is already a unit vector, so rot90().normalize() is
    /// just rot90().  The camera plane extends exactly ±1 unit, giving
    /// a 90-degree horizontal field of view (tan(45°) = 1).
    ///
    /// @param[out] r1  Left endpoint of the camera plane.
    /// @param[out] r2  Right endpoint of the camera plane.
    void get_fov_range(Vec2 &r1, Vec2 &r2) const;

    /// @brief Apply movement from an abstracted Input snapshot.
    ///
    /// Reads input.forward/backward for forward/backward movement and
    /// input.strafe_left/strafe_right for lateral movement.
    /// Mouse rotation is handled by the caller before this call:
    ///   player.direction += input.mouse_dx * Constants::MOUSE_SENSITIVITY
    ///
    /// @param scene  Used for wall-collision detection.
    /// @param input  Aggregated keyboard and mouse state for this frame.
    /// @return true if the player's state changed this frame.
    bool handle_input(const Scene &scene, const Input &input);

  private:
    /// @brief Attempt to move the player by @p delta; cancels the axis that
    ///        would enter a wall (axis-separated collision response).
    ///
    /// @param delta  Desired displacement in world space.
    /// @param scene  Used for wall-collision detection.
    void try_move(Vec2 delta, const Scene &scene);
};
