#pragma once
#include <SDL3/SDL_events.h>
#include <cstdint>

class GUI;

class InputManager {
    public:
        InputManager(GUI& gui);
        void handleInput(SDL_Event* event);
        uint8_t readInput();

    private:
        template<bool Down>
        void handleKeypress(SDL_KeyboardEvent& keypress);

        GUI& gui;
        uint8_t currentInput = 0;
};