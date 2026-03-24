#ifndef VEC2_H_
#define VEC2_H_

struct Vec2 {
    float x;
    float y;

    static Vec2 New(float x, float y);
    static Vec2 from_angle(float angle); // Returns (cos(angle), sin(angle)).
    static Vec2 zero();

    Vec2 operator+(Vec2 other) const;
    Vec2 operator-(Vec2 other) const;
    Vec2 operator*(float scalar) const;
    Vec2 operator/(float scalar) const;
    Vec2 operator-() const;
    bool operator==(Vec2 other) const;

    float length() const;
    Vec2 normalize() const; // Undefined if length() == 0.
    float dot(Vec2 other) const;
    Vec2 rot90() const; // 90^\circ CCW: (x, y) → (−y, x).
};

// World-grid coordinate to minimap pixel.
// @param gab: pixel gap between minimap border and first cell edge.
// @param cell_size: pixel size of one grid cell on the minimap.
float grid_to_pixel(float grid_coord, float gab, float cell_size);
float pixel_to_grid(float pixel, float gab, float cell_size);

#endif
