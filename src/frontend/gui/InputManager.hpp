#pragma once
#include <SDL3/SDL_events.h>
#include <cstdint>

class InputManager {
    public:
        void handleInput(SDL_Event* event);
        uint8_t readInput();

    private:
        template<bool Down>
        void handleKeypress(SDL_KeyboardEvent& keypress);

        uint8_t currentInput = 0;
};