#include "InputManager.hpp"
#include "FlagOps.hpp"
#include "InterruptConstants.hpp"
#include <SDL3/SDL_events.h>
#include <SDL3/SDL_keycode.h>
#include <cstdint>

void InputManager::handleInput(SDL_Event* event) {
    switch (event->type) {
        case SDL_EVENT_KEY_DOWN: return handleKeypress<true>(event->key);
        case SDL_EVENT_KEY_UP: return handleKeypress<false>(event->key);
    }
}

uint8_t InputManager::readInput() {
    return currentInput;
}

template<bool Down>
void InputManager::handleKeypress(SDL_KeyboardEvent& keypress) {
    using enum Interrupts::JOYPAD_INPUT;
    auto modifyFlag = [this](Interrupts::JOYPAD_INPUT inputFlag) {
        if constexpr (Down) setFlags(currentInput, inputFlag);
        else clearFlags(currentInput, inputFlag);
    };
    
    switch (keypress.key) {
        case SDLK_DOWN:
            modifyFlag(DPAD_DOWN);
        break;

        case SDLK_UP:
            modifyFlag(DPAD_UP);
        break;

        case SDLK_LEFT:
            modifyFlag(DPAD_LEFT);
        break;
        
        case SDLK_RIGHT:
            modifyFlag(DPAD_RIGHT);
        break;

        case SDLK_RETURN:
            modifyFlag(START);
        break;

        case SDLK_RSHIFT:
            modifyFlag(SELECT);
        break;

        case SDLK_S:
            modifyFlag(A);
        break;

        case SDLK_A:
            modifyFlag(B);
        break;
    }
}