#ifndef CONSTANTS_H_
#define CONSTANTS_H_

namespace Constants {

    constexpr int WIN_WIDTH = 1280;
    constexpr int WIN_HEIGHT = 720;
    constexpr int FPS = 60;
    // Number of vertical ray columns — higher = sharper, slower.
    constexpr int RES = 5000;

    constexpr float GAB_FACTOR = 0.02f;
    constexpr float INFINITY_VAL = 1e30f;
    // Camera plane distance of 1 + half-width of 1 → 90° FOV (tan 45° = 1).
    constexpr float CAMERA_PLANE_DIST = 1.0f;
    constexpr float MOVE_STEP = 0.1f;
    constexpr float TURN_STEP = 3.14159265f * 0.02f;
    constexpr float MOUSE_SENSITIVITY = 0.002f;
    constexpr float COLLISION_MARGIN = 0.2f;
    constexpr float FOG_DISTANCE = 10.0f;
    constexpr float PLAYER_DOT_RADIUS = 5.0f;

    constexpr int MINIMAP_W = WIN_WIDTH / 3;
    constexpr int MINIMAP_H = WIN_HEIGHT / 3;

} // namespace Constants

#endif
