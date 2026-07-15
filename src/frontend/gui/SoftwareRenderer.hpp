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

        static constexpr uint32_t SDL_EVENT_RENDER_FRAME = SDL_EVENT_USER + 1;

    private:
        SDL_Window* renderWindow = nullptr;
        SDL_Surface* windowSurface = nullptr;
        SDL_Palette* palette = nullptr;
        std::array<SDL_Color, 4> paletteColors;
};