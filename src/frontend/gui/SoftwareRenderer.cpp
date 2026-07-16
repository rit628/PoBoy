#include "SoftwareRenderer.hpp"
#include "GraphicsConstants.hpp"
#include <SDL3/SDL_pixels.h>
#include <SDL3/SDL_stdinc.h>
#include <SDL3/SDL_surface.h>
#include <cstddef>
#include <cstdint>
#include <stdexcept>
#include <tuple>

SoftwareRenderer::SoftwareRenderer() {
    using namespace std::string_literals;
    palette = SDL_CreatePalette(4);
    paletteColors = {{
        {230, 230, 230, 255},
        {184, 184, 184, 255},
        {120, 120, 120, 255},
        {40, 40, 40, 255}
    }};
    SDL_SetPaletteColors(palette, paletteColors.data(), 0, 4);

    if (!palette) {
        throw std::runtime_error("Software renderer failed to initialize: "s + SDL_GetError());
    }
}

SoftwareRenderer::~SoftwareRenderer() {
    SDL_DestroyPalette(palette);
}

void SoftwareRenderer::updateWindow(SDL_Window* window) {
    renderWindow = window;
    windowSurface = SDL_GetWindowSurface(renderWindow);
    
    uint8_t renderScaleX = windowSurface->w / Graphics::LCD_WIDTH;
    uint8_t renderScaleY = windowSurface->h / Graphics::LCD_HEIGHT;
    uint8_t renderScale = std::min(renderScaleX, renderScaleY);

    gameScreen.w = Graphics::LCD_WIDTH * renderScale;
    gameScreen.h = Graphics::LCD_HEIGHT * renderScale;
    gameScreen.x = (windowSurface->w - gameScreen.w) / 2;
    gameScreen.y = (windowSurface->h - gameScreen.h) / 2;
}

void SoftwareRenderer::renderFrame(std::array<uint8_t, Graphics::FRAMEBUFFER_SIZE>& framebuffer) {
    if (renderWindow == nullptr) return;
    using CallbackArgType = std::tuple<decltype(this), decltype(framebuffer)>;
    CallbackArgType callbackArgs = {this, framebuffer};
    SDL_Event renderWake;
    SDL_zero(renderWake);
    renderWake.type = SDL_EVENT_RENDER_FRAME;
    SDL_PushEvent(&renderWake);
    SDL_RunOnMainThread([](void* callbackArgs) -> void {
        auto&& [self, framebuffer] = *static_cast<CallbackArgType*>(callbackArgs);
        auto* renderSurface = SDL_CreateSurfaceFrom(Graphics::LCD_WIDTH, Graphics::LCD_HEIGHT, SDL_PIXELFORMAT_INDEX2LSB, framebuffer.data(), Graphics::LCD_WIDTH / Graphics::PIXELS_PER_BYTE);
        SDL_SetSurfacePalette(renderSurface, self->palette);
        SDL_FillSurfaceRect(self->windowSurface, NULL, SDL_MapSurfaceRGB(self->windowSurface, 0, 0, 0));
        SDL_BlitSurfaceScaled(renderSurface, NULL, self->windowSurface, &self->gameScreen, SDL_SCALEMODE_PIXELART);
        SDL_UpdateWindowSurface(self->renderWindow);
        SDL_DestroySurface(renderSurface);
    }, &callbackArgs, true);
}