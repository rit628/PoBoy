#include "SystemTimer.hpp"
#include "IMU.hpp"
#include <cstdint>

using namespace Interrupts;

SystemTimer::SystemTimer(IMU& imu) : imu(imu) {}

void SystemTimer::tick(uint8_t tCycles) {
    for (uint8_t i = 0; i < tCycles; i++) {
        tick();
    }
}

void SystemTimer::tick() {
    uint16_t overflowBit = timerClocks.at(selectedClock) >> 1;
    bool nextTimaBit = bool(++systemCounter & overflowBit) && timerEnabled;
    bool timaTick = currTimaBit > nextTimaBit; // tick on falling edge
    currTimaBit = nextTimaBit;

    // start reload from TIMA overflow
    if (timaTick && ++timerCounter == 0) timaReloadTCycle = 0;
    // TIMA takes 4 cycles to reload
    if (timaReloadTCycle != 4 && ++timaReloadTCycle == 4) {
        timerCounter = timerModulo;
        imu.writeIF(INTERRUPT_FLAG::TIMER);
    }
}

uint8_t SystemTimer::readDIV() {
    return systemCounter & ~(dividerClock - 1); // DIV is upper byte of system counter
}

void SystemTimer::writeDIV(uint8_t) {
    systemCounter = 0;
}

uint8_t SystemTimer::readTIMA() {
    return timerCounter;
}

void SystemTimer::writeTIMA(uint8_t value) {
    timerCounter = value;
}

uint8_t SystemTimer::readTMA() {
    return timerModulo;
}

void SystemTimer::writeTMA(uint8_t value) {
    timerModulo = value;
}

uint8_t SystemTimer::readTAC() {
    return (timerEnabled << 2) | (selectedClock & 0b011);
}

void SystemTimer::writeTAC(uint8_t value) {
    timerEnabled = value & 0b100;
    selectedClock = value & 0b011;
}