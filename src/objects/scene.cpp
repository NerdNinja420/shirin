#include "objects/scene.h"
#include "objects/player.h"
#include "render/renderer.h"
#include "utils/constants.h"

#include <algorithm>

static Vec2 world_to_minimap(Vec2 world_pos, int cell_size, int h_gab, int v_gab) {
    Vec2 px;
    px.x = world_pos.x * (float)cell_size + (float)h_gab;
    px.y = world_pos.y * (float)cell_size + (float)v_gab;
    return px;
}

int Scene::cell_at(int col, int row) const {
    if (!this->in_bounds(col, row)) return 1;
    return this->map[row * this->cols + col];
}

bool Scene::in_bounds(int col, int row) const {
    return col >= 0 && col < this->cols && row >= 0 && row < this->rows;
}

bool Scene::is_wall(int col, int row) const {
    int c = this->cell_at(col, row);
    return c >= 1 && c != 5;
}

void Scene::enter(Player &player) const {
    player.position = this->spawn;
}

void Scene::render_minimap(Renderer &r, const Player &player, int win_w, int win_h) const {
    int minimap_w = win_w / 3;
    int minimap_h = win_h / 3;
    int gab_px = (int)(Constants::GAB_FACTOR * minimap_w);
    int cell_size =
        std::min((minimap_w - 2 * gab_px) / this->cols, (minimap_h - 2 * gab_px) / this->rows);
    int h_gab = (minimap_w - cell_size * this->cols) / 2;
    int v_gab = (minimap_h - cell_size * this->rows) / 2;

    r.set_color(Color::BASE);
    r.fill_rect(0.0f, 0.0f, (float)minimap_w, (float)minimap_h);

    r.set_color(Color::RED);

    for (int i = 0; i <= this->rows; i++) {
        float y = (float)(i * cell_size + v_gab);
        float x0 = (float)h_gab;
        float x1 = (float)(h_gab + cell_size * this->cols);
        r.draw_line(x0, y, x1, y);
    }

    for (int i = 0; i <= this->cols; i++) {
        float x = (float)(i * cell_size + h_gab);
        float y0 = (float)v_gab;
        float y1 = (float)(v_gab + cell_size * this->rows);
        r.draw_line(x, y0, x, y1);
    }

    for (int row = 0; row < this->rows; row++) {
        for (int col = 0; col < this->cols; col++) {
            int cell = this->map[row * this->cols + col];
            if (cell == 5) {
                r.set_color(Color::PEACH);
                r.fill_rect((float)(col * cell_size + h_gab + 1),
                            (float)(row * cell_size + v_gab + 1),
                            (float)(cell_size - 1),
                            (float)(cell_size - 1));
            } else if (cell >= 1) {
                r.set_color(Color::SURFACE2);
                r.fill_rect((float)(col * cell_size + h_gab + 1),
                            (float)(row * cell_size + v_gab + 1),
                            (float)(cell_size - 1),
                            (float)(cell_size - 1));
            }
        }
    }

    Vec2 r1, r2;
    player.get_fov_range(r1, r2);

    Vec2 px_player = world_to_minimap(player.position, cell_size, h_gab, v_gab);
    Vec2 px_r1 = world_to_minimap(r1, cell_size, h_gab, v_gab);
    Vec2 px_r2 = world_to_minimap(r2, cell_size, h_gab, v_gab);
    Vec2 px_cam_center = world_to_minimap(player.camera_center(), cell_size, h_gab, v_gab);

    r.set_color(Color::FLAMINGO);
    r.fill_circle(px_player.x, px_player.y, Constants::PLAYER_DOT_RADIUS);

    r.set_color(Color::SKY);
    r.draw_line(px_cam_center.x, px_cam_center.y, px_r1.x, px_r1.y);
    r.draw_line(px_cam_center.x, px_cam_center.y, px_r2.x, px_r2.y);
    r.draw_line(px_player.x, px_player.y, px_r1.x, px_r1.y);
    r.draw_line(px_player.x, px_player.y, px_r2.x, px_r2.y);
}
