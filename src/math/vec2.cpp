/// @file math/vec2.cpp
/// @brief Vec2 method implementations.

#include "math/vec2.h"

#include <cmath>

// --- Factories ---

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

// --- Arithmetic operators ---

Vec2 Vec2::operator+(Vec2 other) const {
    Vec2 r;
    r.x = x + other.x;
    r.y = y + other.y;
    return r;
}

Vec2 Vec2::operator-(Vec2 other) const {
    Vec2 r;
    r.x = x - other.x;
    r.y = y - other.y;
    return r;
}

Vec2 Vec2::operator*(float scalar) const {
    Vec2 r;
    r.x = x * scalar;
    r.y = y * scalar;
    return r;
}

Vec2 Vec2::operator/(float scalar) const {
    Vec2 r;
    r.x = x / scalar;
    r.y = y / scalar;
    return r;
}

Vec2 Vec2::operator-() const {
    Vec2 r;
    r.x = -x;
    r.y = -y;
    return r;
}

bool Vec2::operator==(Vec2 other) const {
    return x == other.x && y == other.y;
}

// --- Math operations ---

float Vec2::length() const {
    return sqrtf(x * x + y * y);
}

Vec2 Vec2::normalize() const {
    return (*this) / length();
}

float Vec2::dot(Vec2 other) const {
    return x * other.x + y * other.y;
}

Vec2 Vec2::rot90() const {
    Vec2 r;
    r.x = -y;
    r.y = x;
    return r;
}

// --- Free conversion helpers ---

float grid_to_pixel(float grid_coord, float gab, float cell_size) {
    return grid_coord * cell_size + gab;
}

float pixel_to_grid(float pixel, float gab, float cell_size) {
    return (pixel - gab) / cell_size;
}
