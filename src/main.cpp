/// @file main.cpp
/// @brief Application entry point and main game loop.
///
/// This file contains no SDL includes — all platform code is encapsulated in
/// the Game, Renderer, and Texture classes in src/render/.
///
/// Controls:
///   Mouse X  — rotate view (relative mode, cursor captured)
///   W / S    — move forward / backward
///   A / D    — strafe left / right
///   Escape   — quit
///
/// Frame order:
///   1. begin_frame()      — record frame-start tick
///   2. poll_events()      — quit flag, mouse delta, movement keys → Input
///   3. Apply mouse delta  — player.direction += mouse_dx * MOUSE_SENSITIVITY
///   4. handle_input()     — movement with wall-collision response
///   5. raycaster.render() — write 3D view into raycaster.texture
///   6. minimap.render()   — write overlay into minimap.texture
///   7. Composite          — clear → blit raycaster → blit minimap
///   8. end_frame()        — SDL_RenderPresent + frame-cap sleep

#include "math/vec2.h"
#include "render/game.h"
#include "render/minimap.h"
#include "render/raycaster.h"
#include "world/constants.h"
#include "world/player.h"
#include "world/scene.h"

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

        // Mouse look: horizontal delta rotates the player view.
        player.direction += input.mouse_dx * Constants::MOUSE_SENSITIVITY;

        // Movement: forward/backward and strafe, with collision.
        player.handle_input(scene, input);

        // Render both views into their respective textures.
        rc.render(player, scene, game.renderer);
        minimap.render(player, scene, game.renderer);

        // Composite: clear the window, blit both textures, then present + sleep.
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
