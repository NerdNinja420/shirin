#include "objects/player.h"
#include "objects/scene.h"
#include "render/game.h"
#include "utils/logger.h"

#include <SDL3/SDL.h>
#include <algorithm>

int main(void) {
    Player player = Player::New(Vec2::New(0.0f, 0.0f), 0.0f);

    Game game = Game::New("Shirin — C++ / SDL3 raycaster",
                          player,
                          {Scenes::THE_KEEP, Scenes::THE_DEPTHS, Scenes::THE_CRYPT});

    uint64_t prev = SDL_GetTicks();
    uint64_t fps_acc_ms = 0;
    int fps_frames = 0;

    while (true) {
        uint64_t now = SDL_GetTicks();
        float dt = std::min((float)(now - prev) * 0.001f, 0.1f);
        prev = now;

        Input input = game.poll_events();
        if (input.quit) break;

        game.handle_input(input, dt);

        game.raycaster.render(game.current_scene(),
                              game.player,
                              game.renderer,
                              game.width,
                              game.height);
        game.current_scene().render_minimap(game.renderer, game.player, game.width, game.height);

        game.renderer.present();

        fps_acc_ms += (uint64_t)(dt * 1000.0f);
        fps_frames++;
        if (fps_acc_ms >= 1000) {
            logger::info("FPS: %d", fps_frames);
            fps_acc_ms = 0;
            fps_frames = 0;
        }
    }

    game.destroy();
    return 0;
}
