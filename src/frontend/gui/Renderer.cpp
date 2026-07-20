#include "Renderer.hpp"
#include "GraphicsConstants.hpp"
#include <SDL3/SDL_pixels.h>
#include <SDL3/SDL_render.h>
#include <SDL3/SDL_stdinc.h>
#include <SDL3/SDL_surface.h>
#include <SDL3/SDL_video.h>
#include <cstddef>
#include <cstdint>
#include <stdexcept>
#include <array>
#include <string>

Renderer::Renderer() {
    using std::string_literals::operator""s;
    
    palette = SDL_CreatePalette(4);
    static constexpr std::array<SDL_Color, 4> paletteColors = {{
        {230, 230, 230, 255},
        {184, 184, 184, 255},
        {120, 120, 120, 255},
        {40, 40, 40, 255}
    }};
    SDL_SetPaletteColors(palette, paletteColors.data(), 0, 4);

    sourceSurface = SDL_CreateSurface(Graphics::LCD_WIDTH, Graphics::LCD_HEIGHT, SDL_PIXELFORMAT_INDEX2LSB);
    SDL_SetSurfacePalette(sourceSurface, palette);
    conversionSurface = SDL_ConvertSurface(sourceSurface, SDL_PIXELFORMAT_RGBA8888);

    if (!palette || !sourceSurface || !conversionSurface) {
        throw std::runtime_error("Renderer failed to initialize: "s + SDL_GetError());
    }
}

Renderer::~Renderer() {
    SDL_DestroySurface(conversionSurface);
    SDL_DestroySurface(sourceSurface);
    SDL_DestroyPalette(palette);
    SDL_DestroyTexture(renderTexture);
    SDL_DestroyRenderer(renderer);
}

uint8_t Renderer::getMaxGameScale(size_t width, size_t height) {
    uint8_t maxScaleX = width / Graphics::LCD_WIDTH;
    uint8_t maxScaleY = height / Graphics::LCD_HEIGHT;
    return std::min(maxScaleX, maxScaleY);
}

void Renderer::updateWindow(SDL_Window* window) {
    if (!renderWindow) {
        renderWindow = window;
        renderer = SDL_CreateRenderer(renderWindow, NULL);
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
        renderTexture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STREAMING, Graphics::LCD_WIDTH, Graphics::LCD_HEIGHT);
        SDL_SetTextureScaleMode(renderTexture, SDL_SCALEMODE_PIXELART);
    }
    
    /* scale and center game screen */
    int width = 0, height = 0;
    SDL_GetWindowSize(renderWindow, &width, &height);
    uint8_t renderScale = getMaxGameScale(width, height);
    gameScreen.w = Graphics::LCD_WIDTH * renderScale;
    gameScreen.h = Graphics::LCD_HEIGHT * renderScale;
    gameScreen.x = (width - gameScreen.w) / 2;
    gameScreen.y = (height - gameScreen.h) / 2;
}

void Renderer::renderFrame(std::span<const uint8_t> framebuffer) {
    SDL_memcpy(sourceSurface->pixels, framebuffer.data(), framebuffer.size());
    SDL_BlitSurface(sourceSurface, NULL, conversionSurface, NULL);
    SDL_UpdateTexture(renderTexture, NULL, conversionSurface->pixels, conversionSurface->pitch); // SDL says this is slow but it works fine
    SDL_RenderTexture(renderer, renderTexture, NULL, &gameScreen);
    SDL_RenderPresent(renderer);
}