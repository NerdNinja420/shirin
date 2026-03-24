#ifndef RAYCASTER_H_
#define RAYCASTER_H_

#include "math/vec2.h"
#include "render/renderer.h"
#include "objects/player.h"
#include "objects/scene.h"

struct Raycaster {
    Texture texture;

    static Raycaster New(Renderer &r);
    void destroy();
    void render(const Player &player, const Scene &scene, Renderer &r);

  private:
    // Returns INFINITY_VAL if dp == 0 (ray parallel to axis).
    float delta_t(float p, float dp) const;

    // Snaps p to the nearest grid boundary in direction delta, then advances
    // by 1 ULP via nextafterf() — avoids the fixed-epsilon precision trap.
    float snap(float p, float delta) const;

    Vec2 ray_step(Vec2 p1, Vec2 p2) const;

    // Walk starts from p1 (player), not p2. p2 sets direction only.
    // Starting from p1 prevents walls between player and camera plane
    // being skipped (fixes see-through artifact at FOV edges).
    bool cast_ray(Vec2 p1, Vec2 p2, const Scene &scene, Vec2 &hit) const;
};

#endif
