#ifndef VEC2_H_
#define VEC2_H_

struct Vec2 {
    float x;
    float y;

    static Vec2 New(float x, float y);
    // Returns (cos(angle), sin(angle)).
    static Vec2 from_angle(float angle);
    static Vec2 zero();

    Vec2 operator+(Vec2 other) const;
    Vec2 operator-(Vec2 other) const;
    Vec2 operator*(float scalar) const;
    Vec2 operator/(float scalar) const;
    Vec2 operator-() const;
    bool operator==(Vec2 other) const;

    float length() const;
    // Undefined if length() == 0.
    Vec2  normalize() const;
    float dot(Vec2 other) const;
    // 90° CCW: (x, y) → (−y, x).
    Vec2  rot90() const;
};

// World-grid coordinate to minimap pixel.
// gab: pixel gap between minimap border and first cell edge.
// cell_size: pixel size of one grid cell on the minimap.
float grid_to_pixel(float grid_coord, float gab, float cell_size);
float pixel_to_grid(float pixel,      float gab, float cell_size);

#endif
