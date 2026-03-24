#ifndef MINIMAP_H_
#define MINIMAP_H_

#include "math/vec2.h"
#include "render/renderer.h"
#include "objects/player.h"
#include "objects/scene.h"

struct Minimap {
    Texture texture;

    static Minimap New(Renderer &r);
    void destroy();
    void render(const Player &player, const Scene &scene, Renderer &r);

  private:
    // Converts world-space grid position to minimap pixel coordinates.
    // Uses scene.cell_size, h_gab, v_gab — no global state required.
    Vec2 world_to_minimap(Vec2 world_pos, const Scene &scene) const;
};

#endif
