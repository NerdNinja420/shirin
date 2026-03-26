#ifndef RENDERER_H_
#define RENDERER_H_

#include <SDL3/SDL.h>

#include "utils/color.h"

struct Renderer {
    SDL_Renderer *sdl_renderer;

    static Renderer New(SDL_Window *win);
    void destroy();

    void set_color(Color c);
    void clear();
    void draw_line(float x0, float y0, float x1, float y1);
    void draw_rect(float x, float y, float w, float h); // outline
    void fill_rect(float x, float y, float w, float h);
    // Wall strip with sub-pixel edge blending against ceiling and floor colors.
    void fill_column_aa(float x,
                        float w,
                        float y_top,
                        float y_bottom,
                        Color wall,
                        Color ceiling,
                        Color floor);
    void draw_circle(float cx, float cy, float radius); // outline
    void fill_circle(float cx, float cy, float radius); // filled
    void present();

  private:
    void plot(int x, int y);
};

#endif
