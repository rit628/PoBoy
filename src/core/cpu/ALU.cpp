#include "ALU.hpp"
#include "Register.hpp"
#include <cstddef>
#include <cstdint>
#include <utility>

uint8_t ALU::getCarry() {
    using enum REGISTER_FLAG;
    return (rf.F & C) >> 3; // 1 if C is set else 0
}

uint8_t ALU::getHalfCarry() {
    using enum REGISTER_FLAG;
    return (rf.F & H) >> 2; // 1 if H is set else 0
}

void ALU::setCarry(uint8_t a, uint8_t b, uint8_t carry) {
    using enum REGISTER_FLAG;
    // check if greater than uint8_t max
    if ((static_cast<uint16_t>(a) + static_cast<uint16_t>(b) + carry) > 0xFF) {
        rf.F |= C;
    }
    else {
        rf.F &= ~C;
    }
}

void ALU::setHalfCarry(uint8_t a, uint8_t b, uint8_t carry) {
    using enum REGISTER_FLAG;
    // mask upper 4 bits and check if exceeds lower 4
    if (((a & 0x0F) + (b & 0x0F) + carry) > 0x0F) {
        rf.F |= H;
    }
    else {
        rf.F &= ~H;
    }
}

void ALU::setCarry(uint16_t a, uint16_t b) {
    using enum REGISTER_FLAG;
    // check if greater than uint16_t max
    if ((static_cast<uint32_t>(a) + static_cast<uint32_t>(b)) > 0xFFFF) {
        rf.F |= C;
    }
    else {
        rf.F &= ~C;
    }
}

void ALU::setHalfCarry(uint16_t a, uint16_t b) {
    using enum REGISTER_FLAG;
    // mask upper 4 bits and check if exceeds lower 12
    if (((a & 0x0FFF) + (b & 0x0FFF)) > 0x0FFF) {
        rf.F |= H;
    }
    else {
        rf.F &= ~H;
    }
}

void ALU::setZero(uint8_t result) {
    using enum REGISTER_FLAG;
    rf.F &= (result == 0) ? std::to_underlying(Z) : ~Z;
}
