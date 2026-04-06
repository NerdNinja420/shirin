#ifndef CONSTANTS_H_
#define CONSTANTS_H_

namespace Constants {

    // Number of vertical ray columns — higher = sharper, slower.
    constexpr int RES = 5000;

    constexpr float GAB_FACTOR = 0.02f;
    constexpr float INFINITY_VAL = 1e30f;
    // Camera plane distance of 1 + half-width of 1 → 90° FOV (tan 45° = 1).
    constexpr float CAMERA_PLANE_DIST = 1.0f;
    constexpr float MOVE_SPEED = 6.0f; // units per second (≡ 0.1 × 60 fps)
    constexpr float MOUSE_SENSITIVITY = 0.002f;
    constexpr float COLLISION_MARGIN = 0.2f;
    constexpr float FOG_DISTANCE = 10.0f;
    constexpr float PLAYER_DOT_RADIUS = 5.0f;

} // namespace Constants

#endif
