#include "objects/player.h"
#include "objects/scene.h"
#include "render/game.h"
#include "utils/constants.h"

int main(void) {
    Player player = Player::New(Vec2::New(0.0f, 0.0f), 0.0f);

    Game game = Game::New("Shirin — C++ / SDL3 raycaster",
                          Constants::WIN_WIDTH,
                          Constants::WIN_HEIGHT,
                          player,
                          {Scenes::THE_KEEP, Scenes::THE_DEPTHS, Scenes::THE_CRYPT});

    while (true) {
        uint64_t t0 = game.begin_frame();
        Input input = game.poll_events();
        if (input.quit) break;

        game.handle_input(input);

        game.raycaster.render(game.current_scene(), game.player, game.renderer);
        game.current_scene().render_minimap(game.renderer, game.player);

        game.renderer.present();
        game.end_frame(t0);
    }

    game.destroy();
    return 0;
}
