#include "SoftwareRenderer.hpp"
#include "GraphicsConstants.hpp"
#include <SDL3/SDL_pixels.h>
#include <SDL3/SDL_stdinc.h>
#include <SDL3/SDL_surface.h>
#include <cstddef>
#include <cstdint>
#include <stdexcept>

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

uint8_t SoftwareRenderer::getMaxGameScale(size_t width, size_t height) {
    uint8_t renderScaleX = width / Graphics::LCD_WIDTH;
    uint8_t renderScaleY = height / Graphics::LCD_HEIGHT;
    return std::min(renderScaleX, renderScaleY);
}

void SoftwareRenderer::updateWindow(SDL_Window* window) {
    renderWindow = window;
    windowSurface = SDL_GetWindowSurface(renderWindow);
    
    uint8_t renderScale = getMaxGameScale(windowSurface->w, windowSurface->h);
    gameScreen.w = Graphics::LCD_WIDTH * renderScale;
    gameScreen.h = Graphics::LCD_HEIGHT * renderScale;
    gameScreen.x = (windowSurface->w - gameScreen.w) / 2;
    gameScreen.y = (windowSurface->h - gameScreen.h) / 2;
}

void SoftwareRenderer::renderFrame(std::array<uint8_t, Graphics::FRAMEBUFFER_SIZE>& framebuffer) {
    static constexpr uint8_t PITCH = Graphics::LCD_WIDTH / Graphics::PIXELS_PER_BYTE;
    auto* renderSurface = SDL_CreateSurfaceFrom(Graphics::LCD_WIDTH, Graphics::LCD_HEIGHT, SDL_PIXELFORMAT_INDEX2LSB, framebuffer.data(), PITCH);
    SDL_SetSurfacePalette(renderSurface, palette);
    SDL_FillSurfaceRect(windowSurface, NULL, SDL_MapSurfaceRGB(windowSurface, 0, 0, 0));
    SDL_BlitSurfaceScaled(renderSurface, NULL, windowSurface, &gameScreen, SDL_SCALEMODE_PIXELART);
    SDL_UpdateWindowSurface(renderWindow);
    SDL_DestroySurface(renderSurface);
}