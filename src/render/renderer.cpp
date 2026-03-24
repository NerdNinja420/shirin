#include "render/renderer.h"

#include <cstdlib>

Texture Texture::New(Renderer &r, int w, int h) {
    Texture t;
    t.w      = w;
    t.h      = h;
    t.handle = SDL_CreateTexture(r.handle,
                                 SDL_PIXELFORMAT_RGBA8888,
                                 SDL_TEXTUREACCESS_TARGET,
                                 w,
                                 h);
    if (!t.handle) {
        SDL_Log("Texture::New: failed to create %dx%d texture: %s", w, h, SDL_GetError());
        SDL_Quit();
        exit(1);
    }
    return t;
}

void Texture::destroy() {
    SDL_DestroyTexture(handle);
    handle = nullptr;
}

void Renderer::set_color(Color c) {
    SDL_SetRenderDrawColor(handle, c.r, c.g, c.b, 255);
}

void Renderer::fill_rect(float x, float y, float w, float h) {
    SDL_FRect rect = {x, y, w, h};
    SDL_RenderFillRect(handle, &rect);
}

void Renderer::draw_line(float x0, float y0, float x1, float y1) {
    SDL_RenderLine(handle, x0, y0, x1, y1);
}

void Renderer::clear() {
    SDL_RenderClear(handle);
}

void Renderer::blit(Texture &tex, float x, float y, float w, float h) {
    SDL_FRect dst = {x, y, w, h};
    SDL_RenderTexture(handle, tex.handle, nullptr, &dst);
}

void Renderer::set_target(Texture &tex) {
    SDL_SetRenderTarget(handle, tex.handle);
}

void Renderer::reset_target() {
    SDL_SetRenderTarget(handle, nullptr);
}

void Renderer::present() {
    SDL_RenderPresent(handle);
}
