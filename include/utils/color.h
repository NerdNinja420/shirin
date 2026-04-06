#ifndef COLOR_H_
#define COLOR_H_

#include <cstdint>

class Color {
  public:
    uint8_t r;
    uint8_t g;
    uint8_t b;

    static Color New(uint8_t r, uint8_t g, uint8_t b);

    // Maps cell value to wall color: 1→RED, 2→PINK, 3→MAROON, 4→SKY, 5→PEACH else→BASE.
    static Color from_cell(int cell_value);
    // Returns a new Color with each channel halved (NS wall face darkening).
    Color half() const;
    // Linear interpolate toward other: t=0 → this, t=1 → other.
    Color lerp(Color other, float t) const;

    static const Color ROSEWATER;
    static const Color FLAMINGO;
    static const Color PINK;
    static const Color MAUVE;
    static const Color RED;
    static const Color MAROON;
    static const Color SKY;
    static const Color SURFACE2;
    static const Color SURFACE1;
    static const Color SURFACE0;
    static const Color BASE;
    static const Color MANTLE;
    static const Color CRUST;
    static const Color PEACH;
};

#endif
