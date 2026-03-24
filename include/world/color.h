/// @file world/color.h
/// @brief 24-bit RGB color type with the Catppuccin Mocha palette as static members.
///
/// The former split between struct Color and namespace Colors has been merged
/// into a single class.  Palette entries are static const members, so callers
/// write Color::MANTLE instead of Colors::MANTLE.
///
/// The old free function wall_color() is now Color::from_cell().
/// No SDL types appear in this header.

#pragma once

#include <cstdint>

/// @brief A plain 24-bit RGB color.
///
/// Palette entries are static const members rather than a separate namespace.
/// The Renderer converts a Color to SDL draw calls internally using the public
/// r/g/b fields — no SDL dependency exists in this header.
class Color {
  public:
    uint8_t r; ///< Red channel
    uint8_t g; ///< Green channel
    uint8_t b; ///< Blue channel

    /// @brief Construct a Color from explicit channel values.
    /// @param r  Red channel (0–255).
    /// @param g  Green channel (0–255).
    /// @param b  Blue channel (0–255).
    static Color New(uint8_t r, uint8_t g, uint8_t b);

    /// @brief Map a wall cell integer value to its display color.
    ///
    /// Cell values (as stored in Scene::map):
    ///   1 → RED, 2 → PINK, 3 → MAROON, 4 → SKY
    ///
    /// Returns Color::BASE for any unrecognised value rather than aborting.
    ///
    /// @param cell_value  Integer stored in the scene grid.
    /// @return  Display color for that wall type.
    static Color from_cell(int cell_value);

    // --- Catppuccin Mocha palette ---

    static const Color ROSEWATER; ///< (245, 224, 220)
    static const Color FLAMINGO;  ///< (242, 205, 205) — player dot
    static const Color PINK;      ///< (245, 194, 231) — wall type 2
    static const Color MAUVE;     ///< (203, 166, 247)
    static const Color RED;       ///< (243, 139, 168) — wall type 1 / minimap grid lines
    static const Color MAROON;    ///< (235, 160, 172) — wall type 3
    static const Color SKY;       ///< (137, 220, 235) — wall type 4 / FOV lines
    static const Color SURFACE2;  ///< ( 88,  91, 112) — filled wall cells on minimap
    static const Color SURFACE1;  ///< ( 69,  71,  90)
    static const Color SURFACE0;  ///< ( 49,  50,  68) — floor
    static const Color BASE;      ///< ( 30,  30,  46)
    static const Color MANTLE;    ///< ( 24,  24,  37) — ceiling + fog target
    static const Color CRUST;     ///< ( 17,  17,  27)
};
