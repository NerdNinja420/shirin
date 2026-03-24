#include "render/raycaster.h"
#include "utils/constants.h"

#include <algorithm>
#include <cmath>

Raycaster Raycaster::New(Renderer &r) {
    Raycaster rc;
    rc.texture = Texture::New(r, Constants::WIN_WIDTH, Constants::WIN_HEIGHT);
    return rc;
}

void Raycaster::destroy() {
    texture.destroy();
}

float Raycaster::delta_t(float p, float dp) const {
    if (dp > 0.0f) return (ceilf(p) - p) / dp;
    if (dp < 0.0f) return (floorf(p) - p) / dp;
    return Constants::INFINITY_VAL;
}

float Raycaster::snap(float p, float delta) const {
    // nextafterf(x, y) returns the next representable float after x toward y.
    // This always advances by exactly 1 ULP — the smallest possible step at
    // any coordinate magnitude — so the ray is never stuck on a boundary
    // regardless of what value p has.
    if (delta > 0.0f) {
        float boundary = ceilf(p);
        return nextafterf(boundary, boundary + 1.0f);
    }
    if (delta < 0.0f) {
        float boundary = floorf(p);
        return nextafterf(boundary, boundary - 1.0f);
    }
    // delta == 0: nudge in the positive direction.
    return nextafterf(p, p + 1.0f);
}

Vec2 Raycaster::ray_step(Vec2 p1, Vec2 p2) const {
    // Direction from the previous boundary to the current position.
    Vec2 d = p2 - p1;

    // Time to reach the next x-boundary vs the next y-boundary from p2.
    float tx = delta_t(p2.x, d.x);
    float ty = delta_t(p2.y, d.y);

    Vec2 next;
    if (tx < ty) {
        // x-boundary is closer — snap x, advance y proportionally.
        next.x = snap(p2.x, d.x);
        next.y = p2.y + d.y * tx;
    } else {
        // y-boundary is closer (or equal) — snap y, advance x proportionally.
        next.x = p2.x + d.x * ty;
        next.y = snap(p2.y, d.y);
    }

    return next;
}

bool Raycaster::cast_ray(Vec2 p1, Vec2 p2, const Scene &scene, Vec2 &hit) const {
    // p2 defines the ray direction only (camera-plane point).  The DDA walk
    // starts from p1 (player position) so that any wall between p1 and the
    // camera plane is never skipped — which was the root cause of the
    // "see-through at FOV edges" bug when the camera plane overlapped a wall.
    Vec2 ray_dir = p2 - p1;
    Vec2 prev    = p1 - ray_dir;      // virtual point one step behind the player
    Vec2 curr    = ray_step(prev, p1); // first grid boundary stepping from p1

    while (scene.in_bounds((int)curr.x, (int)curr.y)) {
        // Use floor to map the snapped (possibly fractional) position to the
        // cell that the ray is currently inside.
        int cell_col = (int)floorf(curr.x);
        int cell_row = (int)floorf(curr.y);

        if (scene.cell_at(cell_col, cell_row) >= 1) {
            hit = curr;
            return true;
        }

        Vec2 next = ray_step(prev, curr);
        prev      = curr;
        curr      = next;
    }

    return false;
}

// t = 0 → full wall color,  t = 1 → fully fogged (matches ceiling).
static Color apply_fog(Color c, float perp_dist) {
    float t = perp_dist / Constants::FOG_DISTANCE;
    if (t > 1.0f) t = 1.0f;
    Color fog = Color::MANTLE;
    return Color::New(
        (uint8_t)((float)c.r * (1.0f - t) + (float)fog.r * t),
        (uint8_t)((float)c.g * (1.0f - t) + (float)fog.g * t),
        (uint8_t)((float)c.b * (1.0f - t) + (float)fog.b * t));
}

void Raycaster::render(const Player &player, const Scene &scene, Renderer &r) {
    r.set_target(texture);

    // Ceiling — top half of the frame.
    r.set_color(Color::MANTLE);
    r.fill_rect(0.0f, 0.0f,
                (float)Constants::WIN_WIDTH,
                (float)Constants::WIN_HEIGHT * 0.5f);

    // Floor — bottom half of the frame.
    r.set_color(Color::SURFACE0);
    r.fill_rect(0.0f, (float)Constants::WIN_HEIGHT * 0.5f,
                (float)Constants::WIN_WIDTH,
                (float)Constants::WIN_HEIGHT * 0.5f);

    Vec2 r1, r2;
    player.get_fov_range(r1, r2);

    // Facing direction unit vector, used for perpendicular-distance projection.
    Vec2 dir = player.direction_vec();

    // Width of one ray's pixel strip (may be fractional when WIN_WIDTH % RES != 0).
    float strip_w = (float)Constants::WIN_WIDTH / (float)Constants::RES;

    for (int x = 0; x < Constants::RES; x++) {
        // Linear interpolation across the camera plane: t ∈ [0, 1).
        float t         = (float)x / (float)Constants::RES;
        Vec2  cam_point = r1 + (r2 - r1) * t;

        Vec2 hit;
        if (!cast_ray(player.position, cam_point, scene, hit)) {
            // Ray left the map without hitting a wall — leave the cell BASE-coloured.
            continue;
        }

        // Perpendicular distance: project the hit vector onto the facing direction.
        // This avoids the fisheye effect that Euclidean distance would produce.
        Vec2  hit_vec   = hit - player.position;
        float perp_dist = dir.dot(hit_vec);

        if (perp_dist <= 0.0f) continue; // numerical guard: hit is behind the player

        // Wall-strip height in pixels.
        float height = (float)Constants::WIN_HEIGHT / perp_dist;

        // Vertical extents of the strip, centred on the horizon.
        float y_top    = ((float)Constants::WIN_HEIGHT - height) * 0.5f;
        float y_bottom = ((float)Constants::WIN_HEIGHT + height) * 0.5f;

        // Look up the wall color and apply distance fog.
        int   cell_col = (int)floorf(hit.x);
        int   cell_row = (int)floorf(hit.y);
        Color color    = apply_fog(Color::from_cell(scene.cell_at(cell_col, cell_row)),
                                   perp_dist);

        r.set_color(color);

        // Draw the strip slightly wider (+1) to prevent hairline gaps between
        // adjacent strips due to floating-point rounding.
        r.fill_rect((float)x * strip_w, y_top, strip_w + 1.0f, y_bottom - y_top);
    }

    r.reset_target();
}
