#ifndef SCENE_H_
#define SCENE_H_

constexpr int SCENE_COLS = 32;
constexpr int SCENE_ROWS = 15;

struct Scene {
    int cols;
    int rows;
    int cell_size;
    int h_gab;
    int v_gab;
    int map[SCENE_ROWS][SCENE_COLS];

    static Scene New();

    // Out-of-bounds coordinates return 1 (solid wall) to stop the DDA ray.
    int  cell_at(int col, int row) const;
    bool in_bounds(int col, int row) const;
    bool is_wall(int col, int row) const;
};

#endif
