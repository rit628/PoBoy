#pragma once
#include "GraphicsConstants.hpp"
#include <SDL3/SDL.h>
#include <SDL3/SDL_video.h>
#include <array>

class SoftwareRenderer {
    public:
        SoftwareRenderer();
        ~SoftwareRenderer();
        void updateWindow(SDL_Window* window);
        void renderFrame(std::array<uint8_t, Graphics::FRAMEBUFFER_SIZE>& framebuffer);

    private:
        SDL_Window* renderWindow;
        SDL_Surface* windowSurface;
        SDL_Palette* palette;
        std::array<SDL_Color, 4> paletteColors;
};