/// @file world/scene.h
/// @brief World map and derived spatial constants.
///
/// The scene owns the grid data (which cell values occupy which (col, row)
/// positions) and the pixel-layout constants needed to draw the minimap.
/// Cell values: 0 = empty, 1–4 = wall types mapped to colors by Color::from_cell().

#pragma once

/// @brief Dimensions of the default map.
constexpr int SCENE_COLS = 32;
constexpr int SCENE_ROWS = 15;

/// @brief World map and minimap layout constants.
struct Scene {
    int cols;      ///< Number of columns (x-axis cells)
    int rows;      ///< Number of rows    (y-axis cells)
    int cell_size; ///< Minimap: pixel width/height of one cell
    int h_gab;     ///< Minimap: left/right pixel gap between texture edge and grid
    int v_gab;     ///< Minimap: top/bottom pixel gap between texture edge and grid

    /// @brief Raw map data, row-major: map[row][col].
    int map[SCENE_ROWS][SCENE_COLS];

    /// @brief Build the scene from the hardcoded 32×15 default map.
    ///        Minimap cell_size, h_gab, and v_gab are derived from
    ///        Constants::MINIMAP_W / MINIMAP_H automatically.
    static Scene New();

    /// @brief Return the cell value at integer grid position (col, row).
    ///        Out-of-bounds coordinates are treated as solid walls (value 1),
    ///        which prevents the DDA ray from walking off the map.
    /// @param col  Column index (x-axis).
    /// @param row  Row index (y-axis).
    int cell_at(int col, int row) const;

    /// @brief Return true if (col, row) lies within the map boundaries.
    bool in_bounds(int col, int row) const;

    /// @brief Return true if the cell at (col, row) is a wall (value >= 1).
    bool is_wall(int col, int row) const;
};
