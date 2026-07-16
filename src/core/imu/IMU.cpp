#include "IMU.hpp"
#include "FlagOps.hpp"
#include "MemoryConstants.hpp"
#include <cstdint>

using namespace Interrupts;

IMU::IMU(std::function<uint8_t()> readInput)
        : timer(*this)
        , joypad(*this, readInput)
{
    initialize();
}

void IMU::initialize() {
    interruptFlags = 0;
    interruptEnable = 0;
    
    timer.initialize();
    joypad.initialize();
}

template<uint16_t Register>
uint8_t IMU::readIO() {
    using namespace Memory;
    if constexpr (Register == IF) return interruptFlags | 0xE0; // upper 3 bits of IF are always 1
    if constexpr (Register == IE) return interruptEnable;

    if constexpr (DIV <= Register && Register <= TAC) return timer.readIO<Register>();

    if constexpr (Register == P1) return joypad.readIO<Register>();
}

template<uint16_t Register>
void IMU::writeIO(uint8_t value) {
    using namespace Memory;
    if constexpr (Register == IF) return void(interruptFlags = value);
    if constexpr (Register == IE) return void(interruptEnable = value);

    if constexpr (DIV <= Register && Register <= TAC) return timer.writeIO<Register>(value);

    if constexpr (Register == P1) return joypad.writeIO<Register>(value);
}

void IMU::tick() {
    timer.tick();
    joypad.tick();
}

void IMU::triggerInterrupt(INTERRUPT_FLAG flag) {
    setFlags(interruptFlags, flag);
}

void IMU::enableInterrupt(INTERRUPT_FLAG flag) {
    setFlags(interruptEnable, flag);
}

template uint8_t IMU::readIO<Memory::IF>();
template uint8_t IMU::readIO<Memory::IE>();
template uint8_t IMU::readIO<Memory::DIV>();
template uint8_t IMU::readIO<Memory::TIMA>();
template uint8_t IMU::readIO<Memory::TMA>();
template uint8_t IMU::readIO<Memory::TAC>();
template uint8_t IMU::readIO<Memory::P1>();

template void IMU::writeIO<Memory::IF>(uint8_t);
template void IMU::writeIO<Memory::IE>(uint8_t);
template void IMU::writeIO<Memory::DIV>(uint8_t);
template void IMU::writeIO<Memory::TIMA>(uint8_t);
template void IMU::writeIO<Memory::TMA>(uint8_t);
template void IMU::writeIO<Memory::TAC>(uint8_t);
template void IMU::writeIO<Memory::P1>(uint8_t);