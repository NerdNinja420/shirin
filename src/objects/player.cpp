#include "objects/player.h"
#include "objects/scene.h"
#include "utils/constants.h"

Player Player::New(Vec2 position, float direction) {
    Player p;
    p.position = position;
    p.direction = direction;
    return p;
}

Vec2 Player::direction_vec() const {
    return Vec2::from_angle(direction);
}

Vec2 Player::camera_center() const {
    return position + direction_vec() * Constants::CAMERA_PLANE_DIST;
}

void Player::get_fov_range(Vec2 &r1, Vec2 &r2) const {
    Vec2 p    = camera_center();
    Vec2 perp = (p - position).rot90().normalize();
    r1 = p - perp;
    r2 = p + perp;
}

void Player::try_move(Vec2 delta, const Scene &scene) {
    float m = Constants::COLLISION_MARGIN;

    auto blocked = [&](Vec2 pos) -> bool {
        return scene.is_wall((int)(pos.x + m), (int)(pos.y + m)) ||
               scene.is_wall((int)(pos.x - m), (int)(pos.y + m)) ||
               scene.is_wall((int)(pos.x + m), (int)(pos.y - m)) ||
               scene.is_wall((int)(pos.x - m), (int)(pos.y - m));
    };

    Vec2 next = position + delta;
    if (!blocked(next)) { position = next; return; }

    Vec2 next_x = {position.x + delta.x, position.y};
    if (!blocked(next_x)) { position = next_x; return; }

    Vec2 next_y = {position.x, position.y + delta.y};
    if (!blocked(next_y)) { position = next_y; }
}

bool Player::handle_input(const Scene &scene, const Input &input) {
    bool moved = false;

    if (input.forward)      { try_move( direction_vec()        * Constants::MOVE_STEP, scene); moved = true; }
    if (input.backward)     { try_move( direction_vec()        * -Constants::MOVE_STEP, scene); moved = true; }
    if (input.strafe_left)  { try_move(-direction_vec().rot90() * Constants::MOVE_STEP, scene); moved = true; }
    if (input.strafe_right) { try_move( direction_vec().rot90() * Constants::MOVE_STEP, scene); moved = true; }

    return moved;
}
