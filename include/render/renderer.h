#ifndef RENDERER_H_
#define RENDERER_H_

#include <SDL3/SDL.h>

#include "utils/color.h"

struct Renderer {
    uint32_t *pixels; // heap-allocated CPU framebuffer, width × height
    int width;
    int height;
    SDL_Renderer *sdl_renderer;
    SDL_Texture *upload_tex; // STREAMING texture used only inside present()
    uint32_t draw_color;     // packed ARGB8888, written by set_color()

    static Renderer New(SDL_Window *win, int w, int h);
    void destroy();

    void set_color(Color c);
    void clear();
    void fill_rect(float x, float y, float w, float h);
    void draw_rect(float x, float y, float w, float h); // outline
    void draw_line(float x0, float y0, float x1, float y1);
    void draw_circle(float cx, float cy, float radius); // outline
    void fill_circle(float cx, float cy, float radius); // filled
    // Upload pixels to SDL, blit to window, flip.
    void present();

  private:
    void plot(int x, int y);
};

#endif
