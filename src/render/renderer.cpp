#include "render/renderer.h"

#include <algorithm>
#include <cmath>
#include <cstdlib>

Renderer Renderer::New(SDL_Renderer *sdl_r, int w, int h) {
    Renderer r;
    r.sdl_renderer = sdl_r;
    r.width = w;
    r.height = h;
    r.draw_color = 0xFF000000u;
    r.pixels = new uint32_t[w * h];

    r.upload_tex =
        SDL_CreateTexture(sdl_r, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STREAMING, w, h);
    if (!r.upload_tex) {
        SDL_Log("Renderer::New: SDL_CreateTexture failed: %s", SDL_GetError());
        SDL_Quit();
        exit(1);
    }

    return r;
}

void Renderer::destroy() {
    delete[] this->pixels;
    this->pixels = nullptr;
    SDL_DestroyTexture(this->upload_tex);
    this->upload_tex = nullptr;
    SDL_DestroyRenderer(this->sdl_renderer);
    this->sdl_renderer = nullptr;
}

void Renderer::set_color(Color c) {
    this->draw_color = (0xFFu << 24) | ((uint32_t)c.r << 16) | ((uint32_t)c.g << 8) | c.b;
}

void Renderer::clear() {
    std::fill(this->pixels, this->pixels + this->width * this->height, this->draw_color);
}

void Renderer::plot(int x, int y) {
    if (x < 0 || x >= this->width || y < 0 || y >= this->height) return;
    this->pixels[y * this->width + x] = this->draw_color;
}

void Renderer::fill_rect(float x, float y, float w, float h) {
    int x0 = (int)x, y0 = (int)y;
    int x1 = (int)(x + w), y1 = (int)(y + h);
    if (x0 < 0) x0 = 0;
    if (y0 < 0) y0 = 0;
    if (x1 > this->width) x1 = this->width;
    if (y1 > this->height) y1 = this->height;
    for (int row = y0; row < y1; row++) {
        for (int col = x0; col < x1; col++) {
            this->pixels[row * this->width + col] = this->draw_color;
        }
    }
}

void Renderer::draw_rect(float x, float y, float w, float h) {
    this->draw_line(x, y, x + w, y);
    this->draw_line(x + w, y, x + w, y + h);
    this->draw_line(x + w, y + h, x, y + h);
    this->draw_line(x, y + h, x, y);
}

void Renderer::draw_line(float x0, float y0, float x1, float y1) {
    int ix0 = (int)x0, iy0 = (int)y0;
    int ix1 = (int)x1, iy1 = (int)y1;

    int dx = abs(ix1 - ix0);
    int dy = abs(iy1 - iy0);
    int sx = ix0 < ix1 ? 1 : -1;
    int sy = iy0 < iy1 ? 1 : -1;
    int err = dx - dy;

    while (true) {
        this->plot(ix0, iy0);
        if (ix0 == ix1 && iy0 == iy1) break;
        int e2 = 2 * err;
        if (e2 > -dy) {
            err -= dy;
            ix0 += sx;
        }
        if (e2 < dx) {
            err += dx;
            iy0 += sy;
        }
    }
}

void Renderer::draw_circle(float cx, float cy, float radius) {
    int x = (int)radius;
    int y = 0;
    int err = 0;
    int icx = (int)cx, icy = (int)cy;

    while (x >= y) {
        this->plot(icx + x, icy + y);
        this->plot(icx + y, icy + x);
        this->plot(icx - y, icy + x);
        this->plot(icx - x, icy + y);
        this->plot(icx - x, icy - y);
        this->plot(icx - y, icy - x);
        this->plot(icx + y, icy - x);
        this->plot(icx + x, icy - y);
        if (err <= 0) {
            y++;
            err += 2 * y + 1;
        }
        if (err > 0) {
            x--;
            err -= 2 * x + 1;
        }
    }
}

void Renderer::fill_circle(float cx, float cy, float radius) {
    for (float dy = -radius; dy <= radius; dy += 1.0f) {
        float dx = sqrtf(radius * radius - dy * dy);
        this->draw_line(cx - dx, cy + dy, cx + dx, cy + dy);
    }
}

void Renderer::present() {
    SDL_UpdateTexture(this->upload_tex, nullptr, this->pixels, this->width * (int)sizeof(uint32_t));
    SDL_RenderTexture(this->sdl_renderer, this->upload_tex, nullptr, nullptr);
    SDL_RenderPresent(this->sdl_renderer);
}
