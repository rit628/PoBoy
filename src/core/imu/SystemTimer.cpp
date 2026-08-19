#include "SystemTimer.hpp"
#include "IMU.hpp"
#include "MemoryConstants.hpp"
#include <cstdint>

using namespace Interrupts;

SystemTimer::SystemTimer(IMU& imu) : imu(imu)
{
    initialize();
}

void SystemTimer::initialize() {
    prevTimaBit = 0;
    reloadTima = false;
    timaReloaded = false;

    systemCounter = 0;
    timerCounter = 0;
    timerModulo = 0;
    
    timerEnabled = true;
    selectedClock = 0;
}

template<uint16_t Register>
uint8_t SystemTimer::readIO() {
    using namespace Memory;
    if constexpr (Register == DIV)  return (systemCounter >> 6) & 0xFF;
    if constexpr (Register == TIMA) return timerCounter;
    if constexpr (Register == TMA)  return timerModulo;
}

template<>
uint8_t SystemTimer::readIO<Memory::TAC>() {
    return 0xF8 | (timerEnabled << 2) | (selectedClock & 0b011);
}

template<uint16_t Register>
void SystemTimer::writeIO(uint8_t value [[ maybe_unused ]]) {
    using namespace Memory;
    if constexpr (Register == DIV)  return void(systemCounter = 0);
}

template<>
void SystemTimer::writeIO<Memory::TMA>(uint8_t value) {
    timerModulo = value;
    if (timaReloaded) timerCounter = timerModulo;
}

template<>
void SystemTimer::writeIO<Memory::TIMA>(uint8_t value) {
    if (timaReloaded) return;
    timerCounter = value;
    reloadTima = false;
}

template<>
void SystemTimer::writeIO<Memory::TAC>(uint8_t value) {
    timerEnabled = value & 0b100;
    selectedClock = value & 0b011;
}

void SystemTimer::initHLE() {
    using namespace Memory;

    systemCounter = 0xAB00; // DIV

    writeIO<TIMA>(0x00);
    writeIO<TMA>(0x00);
    writeIO<TAC>(0xF8);  
}

void SystemTimer::tick() {
    uint16_t overflowBit = timerClocks.at(selectedClock) >> 1;
    bool currTimaBit = bool(++systemCounter & overflowBit) && timerEnabled;
    bool timaTick = prevTimaBit > currTimaBit; // tick on falling edge
    prevTimaBit = currTimaBit;
    timaReloaded = false;

    // TIMA reload on following m-cycle
    if (reloadTima) {
        reloadTima = false;
        timaReloaded = true;
        timerCounter = timerModulo;
        imu.triggerInterrupt(INTERRUPT_FLAG::TIMER);
    }

    // start reload from TIMA overflow
    if (timaTick && ++timerCounter == 0) reloadTima = true;
}

template uint8_t SystemTimer::readIO<Memory::DIV>();
template uint8_t SystemTimer::readIO<Memory::TIMA>();
template uint8_t SystemTimer::readIO<Memory::TMA>();

template void SystemTimer::writeIO<Memory::DIV>(uint8_t);