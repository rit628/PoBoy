#include "SystemTimer.hpp"
#include "IMU.hpp"
#include <cstdint>

SystemTimer::SystemTimer(IMU& imu) : imu(imu) {}

void SystemTimer::tick(uint8_t tCycles) {
    for (uint8_t i = 0; i < tCycles; i++) {
        tick();
    }
}

void SystemTimer::tick() {
    auto prevCounter = systemCounter++;
    auto bitChanged = [&,this](uint16_t clockBit) -> bool {
        return (clockBit & prevCounter) ^ (clockBit & systemCounter);
    };
    divider += bitChanged(dividerClock);
    if (!timerEnabled) return;
    auto timerClock = timerClocks.at(selectedClock);
    /* check for bit change */
    if (bitChanged(timerClock)) {
        if (timerCounter == 0xFF) { // TIMA overflow
            timerCounter = timerModulo;
            imu.writeIF(INTERRUPT_FLAG::TIMER);
        }
        else {
            timerCounter++;
        }
    }
}

uint8_t SystemTimer::readDIV() {
    return divider;
}

void SystemTimer::writeDIV(uint8_t) {
    divider = 0;
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