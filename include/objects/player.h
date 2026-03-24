#ifndef PLAYER_H_
#define PLAYER_H_

#include "math/vec2.h"
#include "objects/scene.h"
#include "utils/input.h"

struct Player {
    Vec2  position;
    float direction; // radians; 0 = right, π/2 = down

    static Player New(Vec2 position, float direction);

    Vec2 direction_vec() const;
    Vec2 camera_center() const;

    // Computes the left (r1) and right (r2) endpoints of the camera plane.
    //   p    = position + direction_vec * CAMERA_PLANE_DIST
    //   perp = (p - position).rot90()   // unit length since CAMERA_PLANE_DIST == 1
    //   r1   = p - perp,   r2 = p + perp
    // The ±1 half-width gives a 90° FOV (tan 45° = 1).
    void get_fov_range(Vec2 &r1, Vec2 &r2) const;

    // Mouse rotation is applied by the caller before this:
    //   player.direction += input.mouse_dx * MOUSE_SENSITIVITY
    bool handle_input(const Scene &scene, const Input &input);

  private:
    // Tries delta; if blocked, retries X-only then Y-only (wall sliding).
    void try_move(Vec2 delta, const Scene &scene);
};

#endif
