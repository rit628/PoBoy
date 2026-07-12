#include "SystemTimer.hpp"
#include "IMU.hpp"
#include "MemoryConstants.hpp"
#include <cstdint>

using namespace Interrupts;

SystemTimer::SystemTimer(IMU& imu) : imu(imu) {}

template<uint16_t Register>
uint8_t SystemTimer::readIO() {
    using namespace Memory;
    if constexpr (Register == DIV)  return systemCounter >> 8; // DIV is upper byte of system counter
    if constexpr (Register == TIMA) return timerCounter;
    if constexpr (Register == TMA)  return timerModulo;
}

template<>
uint8_t SystemTimer::readIO<Memory::TAC>() {
    return 0xF8 | (timerEnabled << 2) | (selectedClock & 0b011);
}

template<uint16_t Register>
void SystemTimer::writeIO(uint8_t value) {
    using namespace Memory;
    if constexpr (Register == DIV)  return void(systemCounter = 0);
    if constexpr (Register == TIMA) return void(timerCounter = value);
    if constexpr (Register == TMA)  return void(timerModulo = value);
}

template<>
void SystemTimer::writeIO<Memory::TAC>(uint8_t value) {
    timerEnabled = value & 0b100;
    selectedClock = value & 0b011;
}

void SystemTimer::tick() {
    uint16_t overflowBit = timerClocks.at(selectedClock) >> 1;
    bool currTimaBit = bool(++systemCounter & overflowBit) && timerEnabled;
    bool timaTick = prevTimaBit > currTimaBit; // tick on falling edge
    prevTimaBit = currTimaBit;

    // start reload from TIMA overflow
    if (timaTick && ++timerCounter == 0) timaReloadTCycle = 0;
    // TIMA takes 4 cycles to reload
    if (timaReloadTCycle != 4 && ++timaReloadTCycle == 4) {
        timerCounter = timerModulo;
        imu.triggerInterrupt(INTERRUPT_FLAG::TIMER);
    }
}

template uint8_t SystemTimer::readIO<Memory::DIV>();
template uint8_t SystemTimer::readIO<Memory::TIMA>();
template uint8_t SystemTimer::readIO<Memory::TMA>();

template void SystemTimer::writeIO<Memory::DIV>(uint8_t);
template void SystemTimer::writeIO<Memory::TIMA>(uint8_t);
template void SystemTimer::writeIO<Memory::TMA>(uint8_t);