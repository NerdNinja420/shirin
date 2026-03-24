/// @file world/player.cpp
/// @brief Player factory, FOV maths, and input / movement implementation.
///
/// No SDL includes — input is received as the platform-agnostic Input struct
/// produced by Game::poll_events().

#include "world/player.h"
#include "world/constants.h"

// --- Factory ---

Player Player::New(Vec2 position, float direction) {
    Player p;
    p.position  = position;
    p.direction = direction;
    return p;
}

// --- Direction helpers ---

Vec2 Player::direction_vec() const {
    return Vec2::from_angle(direction);
}

Vec2 Player::camera_center() const {
    return position + direction_vec() * Constants::CAMERA_PLANE_DIST;
}

// --- FOV computation ---

void Player::get_fov_range(Vec2 &r1, Vec2 &r2) const {
    // One unit ahead of the player along the facing direction.
    Vec2 p = camera_center();

    // (p - position) is already a unit vector (CAMERA_PLANE_DIST == 1), so
    // rot90() alone produces a unit perpendicular — normalize() is a no-op but
    // kept explicit for clarity.
    Vec2 perp = (p - position).rot90().normalize();

    r1 = p - perp;
    r2 = p + perp;
}

// --- Movement ---

void Player::try_move(Vec2 delta, const Scene &scene) {
    float m = Constants::COLLISION_MARGIN;

    // Test all four corners of the player's bounding box.
    // Using a margin prevents the player from pressing so close to a wall
    // that the camera plane ends up inside it (the "see-through" artifact).
    auto blocked = [&](Vec2 pos) -> bool {
        return scene.is_wall((int)(pos.x + m), (int)(pos.y + m)) ||
               scene.is_wall((int)(pos.x - m), (int)(pos.y + m)) ||
               scene.is_wall((int)(pos.x + m), (int)(pos.y - m)) ||
               scene.is_wall((int)(pos.x - m), (int)(pos.y - m));
    };

    Vec2 next = position + delta;
    if (!blocked(next)) {
        position = next;
        return;
    }

    // Axis-separated slide: try each axis independently so the player
    // can glide along walls instead of stopping dead.
    Vec2 next_x;
    next_x.x = position.x + delta.x;
    next_x.y = position.y;
    if (!blocked(next_x)) {
        position = next_x;
        return;
    }

    Vec2 next_y;
    next_y.x = position.x;
    next_y.y = position.y + delta.y;
    if (!blocked(next_y)) {
        position = next_y;
    }
}

bool Player::handle_input(const Scene &scene, const Input &input) {
    bool moved = false;

    if (input.forward) {
        try_move(direction_vec() * Constants::MOVE_STEP, scene);
        moved = true;
    }
    if (input.backward) {
        try_move(direction_vec() * -Constants::MOVE_STEP, scene);
        moved = true;
    }
    // A/D strafe perpendicular to the facing direction.
    // rot90() of direction_vec() gives the left vector (-sin θ, cos θ).
    if (input.strafe_left) {
        try_move(-direction_vec().rot90() * Constants::MOVE_STEP, scene);
        moved = true;
    }
    if (input.strafe_right) {
        try_move(direction_vec().rot90() * Constants::MOVE_STEP, scene);
        moved = true;
    }

    return moved;
}
