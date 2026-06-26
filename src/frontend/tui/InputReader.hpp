#pragma once
#include "FlagOps.hpp"
#include "InterruptConstants.hpp"
#include <chrono>
#include <cstdint>
#include <iostream>
#include <future>
#ifdef __linux__
#include <termios.h>
#endif

/* crappy input reader just for testing with tui */
class InputReader {
    public:
        void enableUnbufferedInput();
        void run();
        uint8_t readInput();

    private:
        uint8_t currentInput = 0;
};

inline void InputReader::enableUnbufferedInput() {
    #ifdef __linux__
    struct termios terminalOptions = {};
    tcgetattr(0, &terminalOptions);
    terminalOptions.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(0, TCSANOW, &terminalOptions);
    #endif
}

inline void InputReader::run() {
    enableUnbufferedInput();
    auto pollTimeout = std::chrono::milliseconds(100);
    auto poll = []() {
        char input;
        std::cin.get(input);
        return input;
    };
    auto getInput = std::async(std::launch::async, poll);

    while (true) {
        char input = '\0';
        if (getInput.wait_for(pollTimeout) == std::future_status::ready) {
            input = getInput.get();
            getInput = std::async(std::launch::async, poll);
        }
        currentInput = 0;   // too much work to add overlapping inputs, just clear
        
        using enum Interrupts::JOYPAD_INPUT;
        if (input == 'w') setFlags(currentInput, DPAD_UP);
        if (input == 'a') setFlags(currentInput, DPAD_LEFT);
        if (input == 's') setFlags(currentInput, DPAD_DOWN);
        if (input == 'd') setFlags(currentInput, DPAD_RIGHT);
        if (input == '[') setFlags(currentInput, A);
        if (input == ']') setFlags(currentInput, B);
        if (input == '-') setFlags(currentInput, START);
        if (input == '=') setFlags(currentInput, SELECT);
    }
}

inline uint8_t InputReader::readInput() {
    return currentInput;
}
