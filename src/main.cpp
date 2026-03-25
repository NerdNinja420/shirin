#include "objects/player.h"
#include "objects/scene.h"
#include "render/game.h"
#include "utils/constants.h"

int main(void) {
    Player player = Player::New(Vec2::New(0.0f, 0.0f), 0.0f);

    // clang-format off
    Game game = Game::New("Shirin — C++ / SDL3 raycaster",
                          Constants::WIN_WIDTH, Constants::WIN_HEIGHT,
                          player,
                          {
                              Scene::New((const int *)SceneData::A, SceneData::A_COLS, SceneData::A_ROWS, Vec2::New(7.5f, 5.5f)),
                              Scene::New((const int *)SceneData::B, SceneData::B_COLS, SceneData::B_ROWS, Vec2::New(2.5f, 2.5f)),
                              Scene::New((const int *)SceneData::C, SceneData::C_COLS, SceneData::C_ROWS, Vec2::New(2.5f, 2.5f)),
                          });
    // clang-format on

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
