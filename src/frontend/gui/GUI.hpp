#pragma once
#include "AudioStreamer.hpp"
#include "InputManager.hpp"
#include "SoftwareRenderer.hpp"
#include <SDL3/SDL.h>
#include <cstdint>
#include <string>

template<typename RendererType = SoftwareRenderer>
class GUI {
    public:
        GUI();
        ~GUI();
        SDL_Window* getWindow();
        void updateWindow(std::string windowName = "");
        void renderInterface();
        void handleInput(SDL_Event* event);

        void renderFrame(std::span<const uint8_t> framebuffer);
        void queueAudioData(std::span<const float> data);
        uint8_t readInput();

    private:
        SDL_Window* gameWindow;
        RendererType renderer;
        AudioStreamer audioStreamer;
        InputManager inputManager;
};