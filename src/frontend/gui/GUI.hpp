#pragma once
#include "AudioStreamer.hpp"
#include "InputManager.hpp"
#include "Renderer.hpp"
#include <SDL3/SDL.h>
#include <cstdint>
#include <string>

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
        Renderer renderer;
        AudioStreamer audioStreamer;
        InputManager inputManager;
};