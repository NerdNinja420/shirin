#include "objects/player.h"
#include "objects/scene.h"
#include "render/game.h"
#include "utils/constants.h"

int main(void) {
    Player player = Player::New(Vec2::New(7.5f, 5.5f), 3.14159265f * 1.25f);
    Scene scene = Scene::New();

    Game game = Game::New("Shirin — C++ / SDL3 raycaster",
                          Constants::WIN_WIDTH,
                          Constants::WIN_HEIGHT,
                          player,
                          scene);

    while (true) {
        uint64_t t0 = game.begin_frame();
        Input input = game.poll_events();
        if (input.quit) break;

        game.handle_input(input);

        game.raycaster.render(game.scene, game.renderer);
        game.scene.render_minimap(game.renderer);

        game.renderer.present();
        game.end_frame(t0);
    }

    game.destroy();
    return 0;
}
