#include "objects/scene.h"
#include "objects/player.h"
#include "render/renderer.h"
#include "utils/constants.h"

#include <algorithm>

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

static Vec2 world_to_minimap(Vec2 world_pos, int cell_size, int h_gab, int v_gab) {
    Vec2 px;
    px.x = world_pos.x * (float)cell_size + (float)h_gab;
    px.y = world_pos.y * (float)cell_size + (float)v_gab;
    return px;
}

Scene Scene::New() {
    Scene s;
    s.cols = SCENE_COLS;
    s.rows = SCENE_ROWS;

    for (int row = 0; row < SCENE_ROWS; row++) {
        for (int col = 0; col < SCENE_COLS; col++) {
            s.map[row][col] = DEFAULT_MAP[row][col];
        }
    }

    int gab_px   = (int)(Constants::GAB_FACTOR * Constants::MINIMAP_W);
    s.cell_size  = std::min((Constants::MINIMAP_W - 2 * gab_px) / s.cols,
                            (Constants::MINIMAP_H - 2 * gab_px) / s.rows);
    s.h_gab      = (Constants::MINIMAP_W - s.cell_size * s.cols) / 2;
    s.v_gab      = (Constants::MINIMAP_H - s.cell_size * s.rows) / 2;
    s.player     = nullptr;

    return s;
}

int Scene::cell_at(int col, int row) const {
    if (!in_bounds(col, row)) return 1;
    return map[row][col];
}

bool Scene::in_bounds(int col, int row) const {
    return col >= 0 && col < cols && row >= 0 && row < rows;
}

bool Scene::is_wall(int col, int row) const {
    return cell_at(col, row) >= 1;
}

void Scene::render_minimap(Renderer &r) const {
    r.set_color(Color::BASE);
    r.fill_rect(0.0f, 0.0f, (float)Constants::MINIMAP_W, (float)Constants::MINIMAP_H);

    r.set_color(Color::RED);

    for (int i = 0; i <= rows; i++) {
        float y  = (float)(i * cell_size + v_gab);
        float x0 = (float)h_gab;
        float x1 = (float)(h_gab + cell_size * cols);
        r.draw_line(x0, y, x1, y);
    }

    for (int i = 0; i <= cols; i++) {
        float x  = (float)(i * cell_size + h_gab);
        float y0 = (float)v_gab;
        float y1 = (float)(v_gab + cell_size * rows);
        r.draw_line(x, y0, x, y1);
    }

    r.set_color(Color::SURFACE2);

    for (int row = 0; row < rows; row++) {
        for (int col = 0; col < cols; col++) {
            if (map[row][col] >= 1) {
                float cx = (float)(col * cell_size + h_gab + 1);
                float cy = (float)(row * cell_size + v_gab + 1);
                float cw = (float)(cell_size - 1);
                float ch = (float)(cell_size - 1);
                r.fill_rect(cx, cy, cw, ch);
            }
        }
    }

    Vec2 r1, r2;
    player->get_fov_range(r1, r2);

    Vec2 px_player     = world_to_minimap(player->position,       cell_size, h_gab, v_gab);
    Vec2 px_r1         = world_to_minimap(r1,                     cell_size, h_gab, v_gab);
    Vec2 px_r2         = world_to_minimap(r2,                     cell_size, h_gab, v_gab);
    Vec2 px_cam_center = world_to_minimap(player->camera_center(), cell_size, h_gab, v_gab);

    r.set_color(Color::FLAMINGO);
    r.fill_circle(px_player.x, px_player.y, Constants::PLAYER_DOT_RADIUS);

    r.set_color(Color::SKY);
    r.draw_line(px_cam_center.x, px_cam_center.y, px_r1.x, px_r1.y);
    r.draw_line(px_cam_center.x, px_cam_center.y, px_r2.x, px_r2.y);
    r.draw_line(px_player.x,     px_player.y,     px_r1.x, px_r1.y);
    r.draw_line(px_player.x,     px_player.y,     px_r2.x, px_r2.y);
}
