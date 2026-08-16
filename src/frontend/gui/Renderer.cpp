#include "Renderer.hpp"
#include "GraphicsConstants.hpp"
#include <SDL3/SDL_pixels.h>
#include <SDL3/SDL_render.h>
#include <SDL3/SDL_stdinc.h>
#include <SDL3/SDL_surface.h>
#include <SDL3/SDL_video.h>
#include <cstddef>
#include <cstdint>
#include <iostream>
#include <print>
#include <array>

Renderer::Renderer(SDL_Window* renderWindow) {

    this->renderWindow = renderWindow;
    renderer = SDL_CreateRenderer(renderWindow, NULL);
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
    renderTexture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_XRGB8888, SDL_TEXTUREACCESS_STREAMING, Graphics::LCD_WIDTH, Graphics::LCD_HEIGHT);
    SDL_SetTextureScaleMode(renderTexture, SDL_SCALEMODE_PIXELART);
    SDL_SetRenderLogicalPresentation(renderer, Graphics::LCD_WIDTH, Graphics::LCD_HEIGHT, SDL_LOGICAL_PRESENTATION_INTEGER_SCALE);
    SDL_SetRenderVSync(renderer, SDL_RENDERER_VSYNC_ADAPTIVE);

    if (!renderer || !renderTexture) {
        std::println(std::cerr, "Renderer failed to initialize: {}", SDL_GetError());
        exit(EXIT_FAILURE);
    }

}

Renderer::~Renderer() {
    SDL_DestroySurface(conversionSurface);
    SDL_DestroySurface(sourceSurface);
    SDL_DestroyPalette(palette);
    SDL_DestroyTexture(renderTexture);
    SDL_DestroyRenderer(renderer);
}

template<bool Enabled>
void Renderer::setVsync() {
    if constexpr (Enabled) {
        SDL_SetRenderVSync(renderer, SDL_RENDERER_VSYNC_ADAPTIVE);
    }
    else {
        SDL_SetRenderVSync(renderer, SDL_RENDERER_VSYNC_DISABLED);
    }
}

void Renderer::setNativePixelFormat(MODEL gbModel) {
    if (gbModel == MODEL::CGB) {
        sourceSurface = SDL_CreateSurface(Graphics::LCD_WIDTH, Graphics::LCD_HEIGHT, SDL_PIXELFORMAT_XBGR1555);
    }
    else {
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
    }
    conversionSurface = SDL_ConvertSurface(sourceSurface, SDL_PIXELFORMAT_XRGB8888);
}

void Renderer::clearFrame(uint8_t r, uint8_t g, uint8_t b, uint8_t a) {
    SDL_SetRenderDrawColor(renderer, r, g, b, a);
    SDL_RenderClear(renderer);
    SDL_RenderPresent(renderer);
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
}

void Renderer::renderFrame(std::span<const uint8_t> framebuffer) {
    SDL_memcpy(sourceSurface->pixels, framebuffer.data(), framebuffer.size());
    SDL_BlitSurface(sourceSurface, NULL, conversionSurface, NULL);
    SDL_UpdateTexture(renderTexture, NULL, conversionSurface->pixels, conversionSurface->pitch); // SDL says this is slow but it works fine
    SDL_RenderClear(renderer);
    SDL_RenderTexture(renderer, renderTexture, NULL, NULL);
    SDL_RenderPresent(renderer);
}

template void Renderer::setVsync<true>();
template void Renderer::setVsync<false>();