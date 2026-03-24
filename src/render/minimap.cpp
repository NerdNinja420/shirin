/// @file render/minimap.cpp
/// @brief Minimap overlay rendering implementation.

#include "render/minimap.h"
#include "world/color.h"
#include "world/constants.h"

#include <cmath> // sqrtf

// --- Internal helpers ---

/// @brief Draw a filled circle using horizontal scan lines.
///        All lines are drawn with the renderer's current draw colour.
///
/// @param r       Renderer to draw into.
/// @param cx      Centre x in pixels.
/// @param cy      Centre y in pixels.
/// @param radius  Circle radius in pixels.
static void fill_circle(Renderer &r, float cx, float cy, float radius) {
    for (float dy = -radius; dy <= radius; dy += 1.0f) {
        float dx = sqrtf(radius * radius - dy * dy);
        r.draw_line(cx - dx, cy + dy, cx + dx, cy + dy);
    }
}

// --- Factory / lifecycle ---

Minimap Minimap::New(Renderer &r) {
    Minimap mm;
    mm.texture = Texture::New(r, Constants::MINIMAP_W, Constants::MINIMAP_H);
    return mm;
}

void Minimap::destroy() {
    texture.destroy();
}

// --- Coordinate conversion ---

Vec2 Minimap::world_to_minimap(Vec2 world_pos, const Scene &scene) const {
    Vec2 px;
    px.x = world_pos.x * (float)scene.cell_size + (float)scene.h_gab;
    px.y = world_pos.y * (float)scene.cell_size + (float)scene.v_gab;
    return px;
}

// --- Render ---

void Minimap::render(const Player &player, const Scene &scene, Renderer &r) {
    r.set_target(texture);

    // 1. Clear to background colour.
    r.set_color(Color::BASE);
    r.clear();

    // 2. Draw grid lines.
    r.set_color(Color::RED);

    // Horizontal lines — one per row boundary (rows+1 lines total).
    for (int i = 0; i <= scene.rows; i++) {
        float y  = (float)(i * scene.cell_size + scene.v_gab);
        float x0 = (float)scene.h_gab;
        float x1 = (float)(scene.h_gab + scene.cell_size * scene.cols);
        r.draw_line(x0, y, x1, y);
    }

    // Vertical lines — one per column boundary (cols+1 lines total).
    for (int i = 0; i <= scene.cols; i++) {
        float x  = (float)(i * scene.cell_size + scene.h_gab);
        float y0 = (float)scene.v_gab;
        float y1 = (float)(scene.v_gab + scene.cell_size * scene.rows);
        r.draw_line(x, y0, x, y1);
    }

    // 3. Fill wall cells.
    r.set_color(Color::SURFACE2);

    for (int row = 0; row < scene.rows; row++) {
        for (int col = 0; col < scene.cols; col++) {
            if (scene.map[row][col] >= 1) {
                // Inset by 1 pixel on each side so the grid lines remain visible.
                float cx = (float)(col * scene.cell_size + scene.h_gab + 1);
                float cy = (float)(row * scene.cell_size + scene.v_gab + 1);
                float cw = (float)(scene.cell_size - 1);
                float ch = (float)(scene.cell_size - 1);
                r.fill_rect(cx, cy, cw, ch);
            }
        }
    }

    // Compute player and FOV key points in minimap pixel space.
    Vec2 r1, r2;
    player.get_fov_range(r1, r2);

    Vec2 px_player     = world_to_minimap(player.position, scene);
    Vec2 px_r1         = world_to_minimap(r1, scene);
    Vec2 px_r2         = world_to_minimap(r2, scene);
    Vec2 px_cam_center = world_to_minimap(player.camera_center(), scene);

    // 4. Player dot (filled circle).
    r.set_color(Color::FLAMINGO);
    fill_circle(r, px_player.x, px_player.y, Constants::PLAYER_DOT_RADIUS);

    // 5. FOV trapezoid lines (FOV trapezoid lines).
    r.set_color(Color::SKY);

    // Camera-plane edge to each endpoint.
    r.draw_line(px_cam_center.x, px_cam_center.y, px_r1.x, px_r1.y);
    r.draw_line(px_cam_center.x, px_cam_center.y, px_r2.x, px_r2.y);

    // Player eye to each endpoint.
    r.draw_line(px_player.x, px_player.y, px_r1.x, px_r1.y);
    r.draw_line(px_player.x, px_player.y, px_r2.x, px_r2.y);

    r.reset_target();
}
