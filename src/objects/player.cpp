#include "objects/player.h"
#include "utils/constants.h"

Player Player::New(Vec2 position, float direction) {
    Player p;
    p.position = position;
    p.direction = direction;
    return p;
}

Vec2 Player::direction_vec() const {
    return Vec2::from_angle(this->direction);
}

Vec2 Player::camera_center() const {
    return this->position + this->direction_vec() * Constants::CAMERA_PLANE_DIST;
}

void Player::get_fov_range(Vec2 &r1, Vec2 &r2) const {
    Vec2 p = this->camera_center();
    Vec2 perp = (p - this->position).rot90().normalize();
    r1 = p - perp;
    r2 = p + perp;
}
