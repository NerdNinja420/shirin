#include "math/vec2.h"

#include <cmath>

Vec2 Vec2::New(float x, float y) {
    Vec2 v;
    v.x = x;
    v.y = y;
    return v;
}

Vec2 Vec2::from_angle(float angle) {
    Vec2 v;
    v.x = cosf(angle);
    v.y = sinf(angle);
    return v;
}

Vec2 Vec2::zero() {
    Vec2 v;
    v.x = 0.0f;
    v.y = 0.0f;
    return v;
}

Vec2 Vec2::operator+(Vec2 other) const {
    Vec2 r;
    r.x = this->x + other.x;
    r.y = this->y + other.y;
    return r;
}

Vec2 Vec2::operator-(Vec2 other) const {
    Vec2 r;
    r.x = this->x - other.x;
    r.y = this->y - other.y;
    return r;
}

Vec2 Vec2::operator*(float scalar) const {
    Vec2 r;
    r.x = this->x * scalar;
    r.y = this->y * scalar;
    return r;
}

Vec2 Vec2::operator/(float scalar) const {
    Vec2 r;
    r.x = this->x / scalar;
    r.y = this->y / scalar;
    return r;
}

Vec2 Vec2::operator-() const {
    Vec2 r;
    r.x = -this->x;
    r.y = -this->y;
    return r;
}

bool Vec2::operator==(Vec2 other) const {
    return this->x == other.x && this->y == other.y;
}

float Vec2::length() const {
    return sqrtf(this->x * this->x + this->y * this->y);
}

Vec2 Vec2::normalize() const {
    return (*this) / this->length();
}

float Vec2::dot(Vec2 other) const {
    return this->x * other.x + this->y * other.y;
}

Vec2 Vec2::rot90() const {
    Vec2 r;
    r.x = -this->y;
    r.y = this->x;
    return r;
}

float grid_to_pixel(float grid_coord, float gab, float cell_size) {
    return grid_coord * cell_size + gab;
}

float pixel_to_grid(float pixel, float gab, float cell_size) {
    return (pixel - gab) / cell_size;
}
