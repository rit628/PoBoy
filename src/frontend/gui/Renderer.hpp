#pragma once
#include <SDL3/SDL_render.h>
#include <SDL3/SDL_video.h>
#include <span>

class Renderer {
    public:
        Renderer();
        ~Renderer();
        uint8_t getMaxGameScale(size_t width, size_t height);
        void updateWindow(SDL_Window* window);
        void renderFrame(std::span<const uint8_t> framebuffer);

    private:
        SDL_Window* renderWindow = nullptr;
        
        SDL_Renderer* renderer = nullptr;
        SDL_Texture* renderTexture = nullptr;
        SDL_Palette* palette = nullptr;
        SDL_Surface* sourceSurface = nullptr;       // for copying index2lsb encoded framebuffer
        SDL_Surface* conversionSurface = nullptr;   // for converting to rgba8888
        SDL_FRect gameScreen{};                     // scaled subset of window for game rendering
};