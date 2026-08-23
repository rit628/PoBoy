#pragma once
#include <SDL3/SDL_events.h>
#include <SDL3/SDL_joystick.h>
#include <cstdint>
#include <unordered_map>

class GUI;

class InputManager {
    public:
        InputManager(GUI& gui);
        ~InputManager();
        void handleInput(SDL_Event* event);
        uint8_t readInput();

    private:
        template<bool Down>
        void handleKeyPress(SDL_KeyboardEvent& keyPress);
        template<bool Down>
        void handleButtonPress(SDL_GamepadButtonEvent& buttonPress);
        void addGamepad(SDL_JoystickID id);
        void removeGamepad(SDL_JoystickID id);

        GUI& gui;
        uint8_t currentInput = 0;
        std::unordered_map<SDL_JoystickID, SDL_Gamepad*> activeGamepads;
};