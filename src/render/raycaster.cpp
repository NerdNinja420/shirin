#include "render/raycaster.h"
#include "objects/player.h"
#include "utils/constants.h"

#include <cmath>

Raycaster Raycaster::New() {
    return Raycaster{};
}

float Raycaster::delta_t(float p, float dp) const {
    if (dp > 0.0f) return (ceilf(p) - p) / dp;
    if (dp < 0.0f) return (floorf(p) - p) / dp;
    return Constants::INFINITY_VAL;
}

float Raycaster::snap(float p, float delta) const {
    if (delta > 0.0f) {
        float boundary = ceilf(p);
        return nextafterf(boundary, boundary + 1.0f);
    }
    if (delta < 0.0f) {
        float boundary = floorf(p);
        return nextafterf(boundary, boundary - 1.0f);
    }
    return nextafterf(p, p + 1.0f);
}

Vec2 Raycaster::ray_step(Vec2 p1, Vec2 p2) const {
    Vec2 d = p2 - p1;

    float tx = this->delta_t(p2.x, d.x);
    float ty = this->delta_t(p2.y, d.y);

    Vec2 next;
    if (tx < ty) {
        next.x = this->snap(p2.x, d.x);
        next.y = p2.y + d.y * tx;
    } else {
        next.x = p2.x + d.x * ty;
        next.y = this->snap(p2.y, d.y);
    }

    return next;
}

bool Raycaster::cast_ray(Vec2 p1, Vec2 p2, const Scene &scene, Vec2 &hit) const {
    Vec2 ray_dir = p2 - p1;
    Vec2 prev = p1 - ray_dir;
    Vec2 curr = this->ray_step(prev, p1);

    while (scene.in_bounds((int)curr.x, (int)curr.y)) {
        int cell_col = (int)floorf(curr.x);
        int cell_row = (int)floorf(curr.y);

        if (scene.cell_at(cell_col, cell_row) >= 1 && scene.cell_at(cell_col, cell_row) != 5) {
            hit = curr;
            return true;
        }

        Vec2 next = this->ray_step(prev, curr);
        prev = curr;
        curr = next;
    }

    return false;
}

static Color apply_fog(Color c, float perp_dist) {
    float t = perp_dist / Constants::FOG_DISTANCE;
    if (t > 1.0f) t = 1.0f;
    Color fog = Color::MANTLE;
    return Color::New((uint8_t)((float)c.r * (1.0f - t) + (float)fog.r * t),
                      (uint8_t)((float)c.g * (1.0f - t) + (float)fog.g * t),
                      (uint8_t)((float)c.b * (1.0f - t) + (float)fog.b * t));
}

void Raycaster::render(const Scene &scene, const Player &player, Renderer &r) {
    r.set_color(Color::MANTLE);
    r.fill_rect(0.0f, 0.0f, (float)Constants::WIN_WIDTH, (float)Constants::WIN_HEIGHT * 0.5f);

    r.set_color(Color::SURFACE0);
    r.fill_rect(0.0f,
                (float)Constants::WIN_HEIGHT * 0.5f,
                (float)Constants::WIN_WIDTH,
                (float)Constants::WIN_HEIGHT * 0.5f);

    Vec2 r1, r2;
    player.get_fov_range(r1, r2);

    Vec2 dir = player.direction_vec();
    float strip_w = (float)Constants::WIN_WIDTH / (float)Constants::RES;

    for (int x = 0; x < Constants::RES; x++) {
        float t = (float)x / (float)Constants::RES;
        Vec2 cam_point = r1 + (r2 - r1) * t;

        Vec2 hit;
        if (!this->cast_ray(player.position, cam_point, scene, hit)) continue;

        Vec2 hit_vec = hit - player.position;
        float perp_dist = dir.dot(hit_vec);

        if (perp_dist <= 0.0f) continue;

        float height = (float)Constants::WIN_HEIGHT / perp_dist;
        float y_top = ((float)Constants::WIN_HEIGHT - height) * 0.5f;
        float y_bottom = ((float)Constants::WIN_HEIGHT + height) * 0.5f;

        int cell_col = (int)floorf(hit.x);
        int cell_row = (int)floorf(hit.y);
        Color color = apply_fog(Color::from_cell(scene.cell_at(cell_col, cell_row)), perp_dist);

        r.set_color(color);
        r.fill_rect((float)x * strip_w, y_top, strip_w + 1.0f, y_bottom - y_top);
    }
}
