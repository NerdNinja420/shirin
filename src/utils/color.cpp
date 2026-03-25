#include "utils/color.h"

Color Color::New(uint8_t r, uint8_t g, uint8_t b) {
    Color c;
    c.r = r;
    c.g = g;
    c.b = b;
    return c;
}

Color Color::from_cell(int cell_value) {
    switch (cell_value) {
        case 1:
            return Color::RED;
        case 2:
            return Color::PINK;
        case 3:
            return Color::MAROON;
        case 4:
            return Color::SKY;
        case 5:
            return Color::PEACH;
        default:
            return Color::BASE;
    }
}

Color Color::half() const {
    return Color::New(this->r >> 1, this->g >> 1, this->b >> 1);
}

const Color Color::ROSEWATER = Color::New(245, 224, 220);
const Color Color::FLAMINGO = Color::New(242, 205, 205);
const Color Color::PINK = Color::New(245, 194, 231);
const Color Color::MAUVE = Color::New(203, 166, 247);
const Color Color::RED = Color::New(243, 139, 168);
const Color Color::MAROON = Color::New(235, 160, 172);
const Color Color::SKY = Color::New(137, 220, 235);
const Color Color::SURFACE2 = Color::New(88, 91, 112);
const Color Color::SURFACE1 = Color::New(69, 71, 90);
const Color Color::SURFACE0 = Color::New(49, 50, 68);
const Color Color::BASE = Color::New(30, 30, 46);
const Color Color::MANTLE = Color::New(24, 24, 37);
const Color Color::CRUST = Color::New(17, 17, 27);
const Color Color::PEACH = Color::New(250, 179, 135);
