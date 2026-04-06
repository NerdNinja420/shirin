#ifndef RAYCASTER_H_
#define RAYCASTER_H_

#include "math/vec2.h"
#include "objects/scene.h"
#include "render/renderer.h"

struct Raycaster {
    static Raycaster New();
    void render(const Scene &scene, const Player &player, Renderer &r, int win_w, int win_h);

  private:
    // Returns INFINITY_VAL if dp == 0 (ray parallel to axis).
    float delta_t(float p, float dp) const;

    // Snaps p to the nearest grid boundary in direction delta, then advances
    // by 1 ULP via nextafterf() — avoids the fixed-epsilon precision trap.
    float snap(float p, float delta) const;
    Vec2 ray_step(Vec2 p1, Vec2 p2) const;
    bool cast_ray(Vec2 p1, Vec2 p2, const Scene &scene, Vec2 &hit) const;
};

#endif
