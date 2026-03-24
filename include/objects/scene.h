#ifndef SCENE_H_
#define SCENE_H_

constexpr int SCENE_COLS = 32;
constexpr int SCENE_ROWS = 15;

struct Player;   // forward declaration — player.h includes only vec2.h, no cycle
struct Renderer; // forward declaration — keeps SDL out of this header

struct Scene {
    int cols;
    int rows;
    int cell_size;  // minimap: pixel size of one grid cell
    int h_gab;      // minimap: left/right pixel margin
    int v_gab;      // minimap: top/bottom pixel margin
    Player *player; // non-owning; linked by Game after construction
    int map[SCENE_ROWS][SCENE_COLS];

    static Scene New();

    // Out-of-bounds coordinates return 1 (solid wall) to stop the DDA ray.
    int cell_at(int col, int row) const;
    bool in_bounds(int col, int row) const;
    bool is_wall(int col, int row) const;

    // Render the top-down minimap overlay into r's framebuffer (top-left corner).
    void render_minimap(Renderer &r) const;
};

#endif
