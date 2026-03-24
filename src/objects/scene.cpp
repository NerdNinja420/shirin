#include "objects/scene.h"
#include "utils/constants.h"

#include <algorithm> // std::min

// clang-format off
static const int DEFAULT_MAP[SCENE_ROWS][SCENE_COLS] = {
    {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
    {1, 0, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 4, 0, 0, 0, 4, 0, 0, 0, 0, 0, 0, 0, 1},
    {1, 0, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
    {1, 0, 0, 0, 0, 0, 0, 0, 3, 3, 3, 3, 0, 0, 4, 0, 0, 4, 0, 4, 0, 0, 0, 4, 0, 1, 0, 0, 0, 0, 0, 1},
    {1, 0, 0, 0, 0, 2, 0, 0, 3, 0, 0, 3, 0, 0, 4, 0, 0, 4, 0, 4, 0, 0, 0, 4, 0, 1, 0, 0, 0, 0, 0, 1},
    {1, 0, 0, 0, 0, 2, 0, 0, 3, 0, 0, 3, 0, 0, 4, 0, 0, 4, 0, 4, 0, 0, 0, 4, 0, 1, 0, 0, 0, 0, 0, 1},
    {1, 0, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 0, 4, 0, 0, 4, 0, 4, 0, 0, 0, 4, 0, 1, 2, 2, 0, 2, 2, 1},
    {1, 0, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 0, 4, 0, 0, 0, 0, 4, 0, 0, 0, 4, 0, 1, 0, 0, 0, 0, 0, 1},
    {1, 2, 0, 2, 2, 2, 0, 0, 3, 0, 0, 3, 0, 0, 4, 0, 0, 0, 0, 4, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1},
    {1, 0, 0, 0, 0, 4, 0, 0, 3, 3, 3, 3, 0, 0, 4, 4, 4, 4, 0, 4, 4, 4, 4, 4, 0, 1, 0, 0, 0, 0, 0, 1},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 1},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1},
    {1, 0, 0, 0, 0, 4, 0, 0, 3, 0, 3, 3, 3, 2, 2, 2, 0, 4, 0, 0, 0, 0, 0, 1, 0, 1, 0, 0, 0, 0, 0, 1},
    {1, 0, 0, 0, 0, 4, 0, 0, 3, 0, 0, 0, 0, 0, 0, 0, 0, 4, 0, 0, 0, 0, 0, 1, 0, 1, 0, 0, 3, 0, 0, 1},
    {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
};
// clang-format on

Scene Scene::New() {
    Scene s;
    s.cols = SCENE_COLS;
    s.rows = SCENE_ROWS;

    // Compute minimap cell size and gap offsets:
    //   gab_px    = int(GAB_FACTOR * MINIMAP_W)
    //   cell_size = min((MINIMAP_W - 2*gab) / cols, (MINIMAP_H - 2*gab) / rows)
    //   h_gab     = (MINIMAP_W - cell_size * cols) / 2
    //   v_gab     = (MINIMAP_H - cell_size * rows) / 2
    int gab_px  = (int)(Constants::GAB_FACTOR * Constants::MINIMAP_W);
    s.cell_size = std::min((Constants::MINIMAP_W - 2 * gab_px) / s.cols,
                           (Constants::MINIMAP_H - 2 * gab_px) / s.rows);
    s.h_gab     = (Constants::MINIMAP_W - s.cell_size * s.cols) / 2;
    s.v_gab     = (Constants::MINIMAP_H - s.cell_size * s.rows) / 2;

    for (int row = 0; row < SCENE_ROWS; row++) {
        for (int col = 0; col < SCENE_COLS; col++) {
            s.map[row][col] = DEFAULT_MAP[row][col];
        }
    }

    return s;
}

int Scene::cell_at(int col, int row) const {
    // Out-of-bounds treated as a wall so the DDA never escapes the map.
    if (!in_bounds(col, row)) return 1;
    return map[row][col];
}

bool Scene::in_bounds(int col, int row) const {
    return col >= 0 && col < cols && row >= 0 && row < rows;
}

bool Scene::is_wall(int col, int row) const {
    return cell_at(col, row) >= 1;
}
