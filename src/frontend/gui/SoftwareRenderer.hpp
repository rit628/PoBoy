#pragma once
#include "GraphicsConstants.hpp"
#include <SDL3/SDL.h>
#include <SDL3/SDL_rect.h>
#include <SDL3/SDL_video.h>
#include <array>
#include <cstdint>

class SoftwareRenderer {
    public:
        SoftwareRenderer();
        ~SoftwareRenderer();
        uint8_t getMaxGameScale(size_t width, size_t height);
        void updateWindow(SDL_Window* window);
        void renderFrame(std::array<uint8_t, Graphics::FRAMEBUFFER_SIZE>& framebuffer);

    private:
        SDL_Window* renderWindow = nullptr;
        SDL_Surface* windowSurface = nullptr;
        SDL_Rect gameScreen{};
        SDL_Palette* palette = nullptr;
        std::array<SDL_Color, 4> paletteColors;
};