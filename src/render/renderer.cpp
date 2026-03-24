#include "render/renderer.h"

#include <algorithm>
#include <cmath>
#include <cstdlib>

Renderer Renderer::New(SDL_Renderer *sdl_r, int w, int h) {
    Renderer r;
    r.sdl_renderer = sdl_r;
    r.width        = w;
    r.height       = h;
    r.draw_color   = 0xFF000000u;
    r.pixels       = new uint32_t[w * h];

    r.upload_tex = SDL_CreateTexture(
        sdl_r, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STREAMING, w, h);
    if (!r.upload_tex) {
        SDL_Log("Renderer::New: SDL_CreateTexture failed: %s", SDL_GetError());
        SDL_Quit();
        exit(1);
    }

    return r;
}

void Renderer::destroy() {
    delete[] pixels;
    pixels = nullptr;
    SDL_DestroyTexture(upload_tex);
    upload_tex = nullptr;
    SDL_DestroyRenderer(sdl_renderer);
    sdl_renderer = nullptr;
}

void Renderer::set_color(Color c) {
    draw_color = (0xFFu << 24) | ((uint32_t)c.r << 16) | ((uint32_t)c.g << 8) | c.b;
}

void Renderer::clear() {
    std::fill(pixels, pixels + width * height, draw_color);
}

void Renderer::plot(int x, int y) {
    if (x < 0 || x >= width || y < 0 || y >= height) return;
    pixels[y * width + x] = draw_color;
}

void Renderer::fill_rect(float x, float y, float w, float h) {
    int x0 = (int)x,       y0 = (int)y;
    int x1 = (int)(x + w), y1 = (int)(y + h);
    if (x0 < 0)      x0 = 0;
    if (y0 < 0)      y0 = 0;
    if (x1 > width)  x1 = width;
    if (y1 > height) y1 = height;
    for (int row = y0; row < y1; row++) {
        for (int col = x0; col < x1; col++) {
            pixels[row * width + col] = draw_color;
        }
    }
}

void Renderer::draw_rect(float x, float y, float w, float h) {
    draw_line(x,     y,     x + w, y);
    draw_line(x + w, y,     x + w, y + h);
    draw_line(x + w, y + h, x,     y + h);
    draw_line(x,     y + h, x,     y);
}

void Renderer::draw_line(float x0, float y0, float x1, float y1) {
    int ix0 = (int)x0, iy0 = (int)y0;
    int ix1 = (int)x1, iy1 = (int)y1;

    int dx  = abs(ix1 - ix0);
    int dy  = abs(iy1 - iy0);
    int sx  = ix0 < ix1 ? 1 : -1;
    int sy  = iy0 < iy1 ? 1 : -1;
    int err = dx - dy;

    while (true) {
        plot(ix0, iy0);
        if (ix0 == ix1 && iy0 == iy1) break;
        int e2 = 2 * err;
        if (e2 > -dy) { err -= dy; ix0 += sx; }
        if (e2 <  dx) { err += dx; iy0 += sy; }
    }
}

void Renderer::draw_circle(float cx, float cy, float radius) {
    int x   = (int)radius;
    int y   = 0;
    int err = 0;
    int icx = (int)cx, icy = (int)cy;

    while (x >= y) {
        plot(icx + x, icy + y); plot(icx + y, icy + x);
        plot(icx - y, icy + x); plot(icx - x, icy + y);
        plot(icx - x, icy - y); plot(icx - y, icy - x);
        plot(icx + y, icy - x); plot(icx + x, icy - y);
        if (err <= 0) { y++;   err += 2 * y + 1; }
        if (err >  0) { x--;   err -= 2 * x + 1; }
    }
}

void Renderer::fill_circle(float cx, float cy, float radius) {
    for (float dy = -radius; dy <= radius; dy += 1.0f) {
        float dx = sqrtf(radius * radius - dy * dy);
        draw_line(cx - dx, cy + dy, cx + dx, cy + dy);
    }
}

void Renderer::present() {
    SDL_UpdateTexture(upload_tex, nullptr, pixels, width * (int)sizeof(uint32_t));
    SDL_RenderTexture(sdl_renderer, upload_tex, nullptr, nullptr);
    SDL_RenderPresent(sdl_renderer);
}
