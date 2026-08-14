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
    rSB = 0;
    rSC = 0;
}

template<uint16_t Register>
uint8_t IMU::readIO() {
    using namespace Memory;
    if constexpr (Register == IF) return interruptFlags | 0xE0; // upper 3 bits of IF are always 1
    if constexpr (Register == IE) return interruptEnable;

    if constexpr (DIV <= Register && Register <= TAC) return timer.readIO<Register>();

    if constexpr (Register == P1) return joypad.readIO<Register>();

    if constexpr (Register == SB) return rSB;
    if constexpr (Register == SC) return rSC | 0x7E;    // mask is 0x7C in cgb mode
}

template<uint16_t Register>
void IMU::writeIO(uint8_t value) {
    using namespace Memory;
    if constexpr (Register == IF) return void(interruptFlags = value);
    if constexpr (Register == IE) return void(interruptEnable = value);

    if constexpr (DIV <= Register && Register <= TAC) return timer.writeIO<Register>(value);

    if constexpr (Register == P1) return joypad.writeIO<Register>(value);

    if constexpr (Register == SB) return void(rSB = value);
    if constexpr (Register == SC) return void(rSC = value);
}

void IMU::initHLE() {
    using namespace Memory;

    writeIO<IF>(0xE1);
    writeIO<IE>(0x00);

    timer.initHLE();
    joypad.initHLE();
    writeIO<SB>(0x00);
    writeIO<SC>(0x7E);
}

void IMU::tick() {
    timer.tick();
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
template uint8_t IMU::readIO<Memory::SB>();
template uint8_t IMU::readIO<Memory::SC>();

template void IMU::writeIO<Memory::IF>(uint8_t);
template void IMU::writeIO<Memory::IE>(uint8_t);
template void IMU::writeIO<Memory::DIV>(uint8_t);
template void IMU::writeIO<Memory::TIMA>(uint8_t);
template void IMU::writeIO<Memory::TMA>(uint8_t);
template void IMU::writeIO<Memory::TAC>(uint8_t);
template void IMU::writeIO<Memory::P1>(uint8_t);
template void IMU::writeIO<Memory::SB>(uint8_t);
template void IMU::writeIO<Memory::SC>(uint8_t);