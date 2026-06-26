#include "Joypad.hpp"
#include "FlagOps.hpp"
#include "IMU.hpp"
#include "InterruptConstants.hpp"
#include <cstdint>

using namespace Interrupts;

Joypad::Joypad(IMU& imu, std::function<uint8_t()> readInput)
              : imu(imu), readInput(readInput) {}

void Joypad::tick(uint8_t tCycles) {
    for (uint8_t i = 0; i < tCycles; i++) {
        tick();
    }
}

void Joypad::tick() {
    uint8_t previousJoypadInput = currentJoypadInput;
    readP1();
    uint8_t modifiedInputs = previousJoypadInput ^ currentJoypadInput;
    uint8_t depressedInputs = currentJoypadInput & modifiedInputs;
    if (depressedInputs != modifiedInputs) {    // button was pressed (input flag bit was unset)
        imu.writeIF(INTERRUPT_FLAG::JOYPAD);
    }
}

uint8_t Joypad::readP1() {
    using enum P1_FLAG;
    if (!testFlags(selectedJoypadInput, SELECT_BUTTONS)) {
        currentJoypadInput = (~readInput() >> 4) & 0x0F; // get upper nibble of joypad input (buttons)
    }
    else if (!testFlags(selectedJoypadInput, SELECT_DPAD)) {
        currentJoypadInput = ~readInput() & 0x0F; // get lower nibble of joypad input (dpad)
    }
    else {
        currentJoypadInput = 0x0F;
    }
    return selectedJoypadInput | currentJoypadInput; 
}

void Joypad::writeP1(uint8_t value) {
    using enum P1_FLAG;
    selectedJoypadInput = extractFlags(value, SELECT_BUTTONS, SELECT_DPAD);
}