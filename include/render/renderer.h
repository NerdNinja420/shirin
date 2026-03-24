#ifndef RENDERER_H_
#define RENDERER_H_

#include <SDL3/SDL.h>

#include "utils/color.h"

struct Renderer;

struct Texture {
    SDL_Texture *handle;
    int          w;
    int          h;

    // Allocates a TEXTUREACCESS_TARGET texture. Calls exit(1) on failure.
    static Texture New(Renderer &r, int w, int h);
    void destroy();
};

struct Renderer {
    SDL_Renderer *handle;

    void set_color(Color c);
    void fill_rect(float x, float y, float w, float h);
    void draw_line(float x0, float y0, float x1, float y1);
    void clear();
    // Blit tex onto the current target at destination rect (x, y, w, h).
    void blit(Texture &tex, float x, float y, float w, float h);
    void set_target(Texture &tex);
    void reset_target();
    void present();
};

#endif
