#ifndef PLAYER_H_
#define PLAYER_H_

#include "math/vec2.h"

struct Player {
    Vec2 position;
    float direction; // radians; 0 = right, π/2 = down

    static Player New(Vec2 position, float direction);
    Vec2 direction_vec() const;
    Vec2 camera_center() const;
    // p = position + direction_vec * CAMERA_PLANE_DIST
    // perp = (p - position).rot90()  // unit since CAMERA_PLANE_DIST == 1
    // r1 = p - perp,  r2 = p + perp  → 90° FOV (tan 45° = 1)
    void get_fov_range(Vec2 &r1, Vec2 &r2) const;
};

#endif
