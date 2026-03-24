/// @file math/vec2.h
/// @brief 2D floating-point vector — pure math type with no hidden state.
///
/// The original `Coordinate` class stored a class-level
/// `_map_ref` global used for pixel-space conversions, creating fragile init-order
/// dependencies. Vec2 is a plain value type; any context-dependent conversions
/// (e.g. world-to-minimap-pixel) are free functions that take explicit parameters.

#pragma once

/// @brief 2D floating-point vector.
struct Vec2 {
    float x;
    float y;

    /// @brief Construct from explicit components.
    static Vec2 New(float x, float y);

    /// @brief Construct a unit vector pointing in the direction of @p angle (radians).
    ///        Result is (cos(angle), sin(angle)).
    static Vec2 from_angle(float angle);

    /// @brief Zero vector.
    static Vec2 zero();

    // --- Arithmetic operators ---

    Vec2 operator+(Vec2 other) const;
    Vec2 operator-(Vec2 other) const;
    Vec2 operator*(float scalar) const;
    Vec2 operator/(float scalar) const;

    /// @brief Unary negation.
    Vec2 operator-() const;

    bool operator==(Vec2 other) const;

    // --- Math operations ---

    /// @brief Euclidean length: sqrt(x² + y²).
    float length() const;

    /// @brief Return a unit vector in the same direction.
    ///        Behaviour is undefined if length() == 0.
    Vec2 normalize() const;

    /// @brief Dot product: x*other.x + y*other.y.
    float dot(Vec2 other) const;

    /// @brief 90-degree CCW rotation: (x, y) → (−y, x).
    Vec2 rot90() const;
};

// --- Free conversion helpers (no global state) ---

/// @brief Convert a grid-space coordinate to a pixel offset on the minimap surface.
/// @param grid_coord  Fractional grid index along one axis.
/// @param gab         Pixel gap between the minimap border and the first cell edge.
/// @param cell_size   Pixel size of one cell on the minimap.
/// @return            Pixel coordinate on the minimap texture.
float grid_to_pixel(float grid_coord, float gab, float cell_size);

/// @brief Inverse of grid_to_pixel: pixel offset → grid coordinate.
float pixel_to_grid(float pixel, float gab, float cell_size);
