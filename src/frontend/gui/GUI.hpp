#pragma once
#include "AudioStreamer.hpp"
#include "GraphicsConstants.hpp"
#include "InputManager.hpp"
#include "SoftwareRenderer.hpp"
#include <SDL3/SDL.h>
#include <array>
#include <cstdint>

template<typename RendererType = SoftwareRenderer>
class GUI {
    public:
        GUI();
        ~GUI();
        void updateWindow();
        void handleInput(SDL_Event* event);

        void renderFrame(std::array<uint8_t, Graphics::FRAMEBUFFER_SIZE>& framebuffer);
        void queueAudioData(std::span<const float> data);
        uint8_t readInput();

        static constexpr uint32_t SDL_EVENT_RENDER_FRAME = RendererType::SDL_EVENT_RENDER_FRAME;

    private:
        SDL_Window* gameWindow;
        RendererType renderer;
        AudioStreamer audioStreamer;
        InputManager inputManager;
};