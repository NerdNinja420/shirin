#include "math/vec2.h"
#include "render/game.h"
#include "render/minimap.h"
#include "render/raycaster.h"
#include "utils/constants.h"
#include "objects/player.h"
#include "objects/scene.h"

int main(void) {
    Game game = Game::New("Shirin — C++ / SDL3 raycaster",
                          Constants::WIN_WIDTH,
                          Constants::WIN_HEIGHT);

    Scene     scene   = Scene::New();
    Player    player  = Player::New(Vec2::New(7.5f, 5.5f), 3.14159265f * 1.25f);
    Raycaster rc      = Raycaster::New(game.renderer);
    Minimap   minimap = Minimap::New(game.renderer);

    while (true) {
        uint64_t t0    = game.begin_frame();
        Input    input = game.poll_events();

        if (input.quit) break;

        player.direction += input.mouse_dx * Constants::MOUSE_SENSITIVITY;
        player.handle_input(scene, input);

        rc.render(player, scene, game.renderer);
        minimap.render(player, scene, game.renderer);

        game.renderer.clear();
        game.renderer.blit(rc.texture,
                           0.0f, 0.0f,
                           (float)Constants::WIN_WIDTH,
                           (float)Constants::WIN_HEIGHT);
        game.renderer.blit(minimap.texture,
                           0.0f, 0.0f,
                           (float)Constants::MINIMAP_W,
                           (float)Constants::MINIMAP_H);

        game.end_frame(t0);
    }

    rc.destroy();
    minimap.destroy();
    game.destroy();
    return 0;
}
