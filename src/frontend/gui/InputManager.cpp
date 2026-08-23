#include "InputManager.hpp"
#include "GUI.hpp"
#include "FlagOps.hpp"
#include "InterruptConstants.hpp"
#include <SDL3/SDL_events.h>
#include <SDL3/SDL_gamepad.h>
#include <SDL3/SDL_joystick.h>
#include <SDL3/SDL_keycode.h>
#include <cstdint>

InputManager::InputManager(GUI& gui) : gui(gui) {
    int count = 0;
    SDL_JoystickID* gamepads = SDL_GetGamepads(&count);
    if (gamepads) {
        for (int i = 0; i < count; i++) {
            addGamepad(gamepads[i]);
        }
        SDL_free(gamepads);
    }
}

InputManager::~InputManager() {
    for (auto&& [id, gamepad] : activeGamepads) {
        SDL_CloseGamepad(gamepad);
    }
    activeGamepads.clear();
}

void InputManager::handleInput(SDL_Event* event) {
    switch (event->type) {
        case SDL_EVENT_KEY_DOWN: return handleKeyPress<true>(event->key);
        case SDL_EVENT_KEY_UP: return handleKeyPress<false>(event->key);
        case SDL_EVENT_GAMEPAD_BUTTON_DOWN: return handleButtonPress<true>(event->gbutton);
        case SDL_EVENT_GAMEPAD_BUTTON_UP: return handleButtonPress<false>(event->gbutton);
        case SDL_EVENT_GAMEPAD_ADDED: return addGamepad(event->gdevice.which);
        case SDL_EVENT_GAMEPAD_REMOVED: return removeGamepad(event->gdevice.which);
    }
}

uint8_t InputManager::readInput() {
    return currentInput;
}

template<bool Down>
void InputManager::handleKeyPress(SDL_KeyboardEvent& keyPress) {
    using enum Interrupts::JOYPAD_INPUT;
    auto modifyFlag = [this](Interrupts::JOYPAD_INPUT inputFlag) {
        if constexpr (Down) setFlags(currentInput, inputFlag);
        else clearFlags(currentInput, inputFlag);
    };
    
    switch (keyPress.key) {
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

        case SDLK_TAB:
            gui.updateSpeed<Down>();
        break;
    }
}

template<bool Down>
void InputManager::handleButtonPress(SDL_GamepadButtonEvent& buttonPress) {
    using enum Interrupts::JOYPAD_INPUT;
    auto modifyFlag = [this](Interrupts::JOYPAD_INPUT inputFlag) {
        if constexpr (Down) setFlags(currentInput, inputFlag);
        else clearFlags(currentInput, inputFlag);
    };
    
    switch (buttonPress.button) {
        case SDL_GAMEPAD_BUTTON_DPAD_DOWN:
            modifyFlag(DPAD_DOWN);
        break;

        case SDL_GAMEPAD_BUTTON_DPAD_UP:
            modifyFlag(DPAD_UP);
        break;

        case SDL_GAMEPAD_BUTTON_DPAD_LEFT:
            modifyFlag(DPAD_LEFT);
        break;
        
        case SDL_GAMEPAD_BUTTON_DPAD_RIGHT:
            modifyFlag(DPAD_RIGHT);
        break;

        case SDL_GAMEPAD_BUTTON_START:
            modifyFlag(START);
        break;

        case SDL_GAMEPAD_BUTTON_BACK:
            modifyFlag(SELECT);
        break;

        case SDL_GAMEPAD_BUTTON_EAST:
            modifyFlag(A);
        break;

        case SDL_GAMEPAD_BUTTON_SOUTH:
        case SDL_GAMEPAD_BUTTON_NORTH:
            modifyFlag(B);
        break;
    }
}

void InputManager::addGamepad(SDL_JoystickID id) {
    activeGamepads.emplace(id, SDL_OpenGamepad(id));
}

void InputManager::removeGamepad(SDL_JoystickID id) {
    auto removed = activeGamepads.extract(id).mapped();
    SDL_CloseGamepad(removed);
}