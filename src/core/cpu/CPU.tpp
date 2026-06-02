#pragma once
#include "CPU.hpp"
#include <cstddef>
#include <cstdint>
#include <utility>

inline uint8_t CPU::getCarry() {
    using enum REGISTER_FLAG;
    return (F & std::to_underlying(C)) >> 4; // 1 if C is set else 0
}

inline uint8_t CPU::getHalfCarry() {
    using enum REGISTER_FLAG;
    return (F & std::to_underlying(H)) >> 5; // 1 if H is set else 0
}

inline void CPU::setZero(uint8_t result) {
    using enum REGISTER_FLAG;
    (result == 0) ? F.set(Z) : F.clear(Z);
}

inline uint8_t CPU::addAndSetFlags(uint8_t a, uint8_t b, uint8_t carry) {
    using enum REGISTER_FLAG;
    uint8_t result = a + b + carry;

    auto& flags = F;
    setZero(result);
    flags.clear(N);

    // set half carry (H): mask upper 4 bits and check if exceeds lower 4
    if (((a & 0x0F) + (b & 0x0F) + carry) > 0x0F) {
        flags.set(H);
    }
    else {
        flags.clear(H);
    }

    // set carry (C): check if greater than uint8_t max
    if ((static_cast<uint16_t>(a) + static_cast<uint16_t>(b) + carry) > 0xFF) {
        flags.set(C);
    }
    else {
        flags.clear(C);
    }
    
    return result;
}

inline uint16_t CPU::addAndSetFlags(uint16_t a, uint16_t b) {
    using enum REGISTER_FLAG;
    uint16_t result = a + b;

    auto& flags = F;
    flags.clear(N);

    // set half carry (H): mask upper 4 bits and check if exceeds lower 12
    if (((a & 0x0FFF) + (b & 0x0FFF)) > 0x0FFF) {
        flags.set(H);
    }
    else {
        flags.clear(H);
    }

    // set carry (C): check if greater than uint16_t max
    if ((static_cast<uint32_t>(a) + static_cast<uint32_t>(b)) > 0xFFFF) {
        flags.set(C);
    }
    else {
        flags.clear(C);
    }
    
    return result;
}

inline uint8_t CPU::subtractAndSetFlags(uint8_t a, uint8_t b, uint8_t carry) {
    using enum REGISTER_FLAG;
    uint8_t result = static_cast<int16_t>(a) - static_cast<int16_t>(b) - carry;

    auto& flags = F;
    setZero(result);
    flags.set(N);

    // set half carry (H): mask upper 4 bits and check if negative (borrow from bit 4)
    if ((static_cast<int8_t>(a & 0x0F) - static_cast<int8_t>(b & 0x0F) - carry) < 0) {
        flags.set(H);
    }
    else {
        flags.clear(H);
    }

    // set carry (C): check if result is negative (borrow required)
    if ((static_cast<int16_t>(a) - static_cast<int16_t>(b) - carry) < 0) {
        flags.set(C);
    }
    else {
        flags.clear(C);
    }

    return result;
}

template<size_t N>
inline void CPU::load(Register<N> auto& target, RegisterValue<N> value) {
    target = value;
}

inline void CPU::loadIndirect(uint16_t address, uint8_t value) {
    mmu.write(address, value);
}

inline void CPU::loadIndirect(Register<8> auto& target, uint16_t address) {
    target = mmu.read(address);
}

inline void CPU::loadHiIndirect(Register<8> auto& target, uint8_t address) {
    target = mmu.read(static_cast<uint16_t>(address) + 0xFF00);
}

inline void CPU::loadHiIndirect(uint16_t address, uint8_t value) {
    mmu.write(static_cast<uint16_t>(address) + 0xFF00, value);
}

inline void CPU::loadIncrement(Register<8> auto& target, Register<16> auto& address) {
    target = mmu.read(address++);
}

inline void CPU::loadIncrement(Register<16> auto& address, uint8_t value) {
    mmu.write(address++, value);
}

inline void CPU::loadDecrement(Register<8> auto& target, Register<16> auto& address) {
    target = mmu.read(address--);
}

inline void CPU::loadDecrement(Register<16> auto& address, uint8_t value) {
    mmu.write(address--, value);
}

template<size_t N>
inline void CPU::add(Register<N> auto& target, RegisterValue<N> value) {
    target = addAndSetFlags(target, value);
}

inline void CPU::addIndirect(Register<8> auto& target, uint16_t address) {
    auto value = mmu.read(address);
    add<8>(target, value);
}

inline void CPU::adc(Register<8> auto& target, uint8_t value) {
    auto carry = getCarry();
    target = addAndSetFlags(target, value, carry);
}

inline void CPU::adcIndirect(Register<8> auto& target, uint16_t address) {
    auto value = mmu.read(address);
    adc(target, value);
}

inline void CPU::sub(Register<8> auto& target, uint8_t value) {
    target = subtractAndSetFlags(target, value);
}

inline void CPU::subIndirect(Register<8> auto& target, uint16_t address) {
    auto value = mmu.read(address);
    sub(target, value);
}

inline void CPU::sbc(Register<8> auto& target, uint8_t value) {
    auto carry = getCarry();
    target = subtractAndSetFlags(target, value, carry);
}

inline void CPU::sbcIndirect(Register<8> auto& target, uint16_t address) {
    auto value = mmu.read(address);
    sbc(target, value);
}

inline void CPU::compare(uint8_t lhs, uint8_t rhs) {
    subtractAndSetFlags(lhs, rhs); // subtract and discard result
}

inline void CPU::compareIndirect(uint8_t lhs, uint16_t address) {
    auto rhs = mmu.read(address);
    compare(lhs, rhs);
}

inline void CPU::decrement(Integer<8> auto& target) {
    using enum REGISTER_FLAG;
    bool carrySet = getCarry();
    auto& flags = F;
    target = subtractAndSetFlags(target, 1);
    (carrySet) ? flags.set(C) : flags.clear(C); // ensure C remains unmodified
}

inline void CPU::decrement(Register<16> auto& target) {
    target--;
}

inline void CPU::decrementIndirect(uint16_t address) {
    auto value = mmu.read(address);
    decrement(value);
    mmu.write(address, value);
}

inline void CPU::increment(Integer<8> auto& target) {
    using enum REGISTER_FLAG;
    bool carrySet = getCarry();
    auto& flags = F;
    target = addAndSetFlags(target, 1);
    (carrySet) ? flags.set(C) : flags.clear(C); // ensure C remains unmodified
}

inline void CPU::increment(Register<16> auto& target) {
    target++;
}

inline void CPU::incrementIndirect(uint16_t address) {
    auto value = mmu.read(address);
    increment(value);
    mmu.write(address, value);
}

inline void CPU::bitAnd(Register<8> auto& lhs, uint8_t rhs) {
    using enum REGISTER_FLAG;
    lhs &= rhs;
    auto& flags = F;
    setZero(lhs);
    flags.clear(N);
    flags.set(H);
    flags.clear(C);
}

inline void CPU::bitAndIndirect(Register<8> auto& lhs, uint16_t address) {
    auto rhs = mmu.read(address);
    bitAnd(lhs, rhs);
}

inline void CPU::bitNot(Register<8> auto& target) {
    using enum REGISTER_FLAG;
    target = ~target;
    auto& flags = F;
    flags.set(N);
    flags.set(H);
}

inline void CPU::bitOr(Register<8> auto& lhs, uint8_t rhs) {
    using enum REGISTER_FLAG;
    lhs |= rhs;
    auto& flags = F;
    setZero(lhs);
    flags.clear(N);
    flags.clear(H);
    flags.clear(C);
}

inline void CPU::bitOrIndirect(Register<8> auto& lhs, uint16_t address) {
    auto rhs = mmu.read(address);
    bitOr(lhs, rhs);
}

inline void CPU::bitXor(Register<8> auto& lhs, uint8_t rhs) {
    using enum REGISTER_FLAG;
    lhs ^= rhs;
    auto& flags = F;
    setZero(lhs);
    flags.clear(N);
    flags.clear(H);
    flags.clear(C);
}

inline void CPU::bitXorIndirect(Register<8> auto& lhs, uint16_t address) {
    auto rhs = mmu.read(address);
    bitXor(lhs, rhs);
}

inline void CPU::bitTest(uint8_t bitIndex, uint8_t target) {
    using enum REGISTER_FLAG;
    auto& flags = F;
    uint8_t test = 1 << bitIndex;
    (test & target) ? flags.clear(Z) : flags.set(Z);
    flags.clear(N);
    flags.set(H);
}

inline void CPU::bitTestIndirect(uint8_t bitIndex, uint16_t address) {
    auto target = mmu.read(address);
    bitTest(bitIndex, target);
}

inline void CPU::bitReset(uint8_t bitIndex, Integer<8> auto& target) {
    uint8_t test = 1 << bitIndex;
    target &= ~test; // set bit "test" to 0 in target
}

inline void CPU::bitResetIndirect(uint8_t bitIndex, uint16_t address) {
    auto value = mmu.read(address);
    bitReset(bitIndex, value);
    mmu.write(address, value);
}

inline void CPU::bitSet(uint8_t bitIndex, Integer<8> auto& target) {
    uint8_t test = 1 << bitIndex;
    target |= test; // set bit "test" to 1 in target
}

inline void CPU::bitSetIndirect(uint8_t bitIndex, uint16_t address) {
    auto value = mmu.read(address);
    bitSet(bitIndex, value);
    mmu.write(address, value);
}

inline void CPU::rotateLeft(Integer<8> auto& target) {
    using enum REGISTER_FLAG;
    uint8_t carry = getCarry(); // carry becomes new lsb
    bool msb = target & 0x80;
    target = (target << 1) | carry; // rotate through carry

    auto& flags = F;
    setZero(target);
    flags.clear(N);
    flags.clear(H);
    (msb) ? flags.set(C) : flags.clear(C); // carry = former msb
}

inline void CPU::rotateLeftIndirect(uint16_t address) {
    auto value = mmu.read(address);
    rotateLeft(value);
    mmu.write(address, value);
}

inline void CPU::rotateLeftCircular(Integer<8> auto& target) {
    using enum REGISTER_FLAG;
    uint8_t msb = (target & 0x80) >> 7; // msb becomes new lsb
    target = (target << 1) | msb; 

    auto& flags = F;
    setZero(target);
    flags.clear(N);
    flags.clear(H);
    (msb) ? flags.set(C) : flags.clear(C); // carry = former msb
}

inline void CPU::rotateLeftCircularIndirect(uint16_t address) {
    auto value = mmu.read(address);
    rotateLeftCircular(value);
    mmu.write(address, value);
}

inline void CPU::rotateRight(Integer<8> auto& target) {
    using enum REGISTER_FLAG;
    uint8_t carry = getCarry() << 7; // carry becomes new msb
    bool lsb = target & 0x01;
    target = (target >> 1) | carry; // rotate through carry

    auto& flags = F;
    setZero(target);
    flags.clear(N);
    flags.clear(H);
    (lsb) ? flags.set(C) : flags.clear(C); // carry = former lsb
}

inline void CPU::rotateRightIndirect(uint16_t address) {
    auto value = mmu.read(address);
    rotateRight(value);
    mmu.write(address, value);
}

inline void CPU::rotateRightCircular(Integer<8> auto& target) {
    using enum REGISTER_FLAG;
    uint8_t lsb = (target & 0x01) << 7; // lsb becomes new msb
    target = (target >> 1) | lsb;

    auto& flags = F;
    setZero(target);
    flags.clear(N);
    flags.clear(H);
    (lsb) ? flags.set(C) : flags.clear(C); // carry = former lsb
}

inline void CPU::rotateRightCircularIndirect(uint16_t address) {
    auto value = mmu.read(address);
    rotateRightCircular(value);
    mmu.write(address, value);
}

inline void CPU::shiftLeftArithmetic(Integer<8> auto& target) {
    using enum REGISTER_FLAG;
    bool msb = target & 0x80;
    target <<= 1;
    auto& flags = F;
    setZero(target);
    flags.clear(N);
    flags.clear(H);
    (msb) ? flags.set(C) : flags.clear(C);
}

inline void CPU::shiftLeftArithmeticIndirect(uint16_t address) {
    auto value = mmu.read(address);
    shiftLeftArithmetic(value);
    mmu.write(address, value);
}

inline void CPU::shiftRightArithmetic(Integer<8> auto& target) {
    using enum REGISTER_FLAG;
    bool lsb = target & 0x01;
    uint8_t msb = target & 0x80;
    target = (target >> 1) | msb; // preserve msb
    auto& flags = F;
    setZero(target);
    flags.clear(N);
    flags.clear(H);
    (lsb) ? flags.set(C) : flags.clear(C);
}

inline void CPU::shiftRightArithmeticIndirect(uint16_t address) {
    auto value = mmu.read(address);
    shiftRightArithmetic(value);
    mmu.write(address, value);
}

inline void CPU::shiftRightLogical(Integer<8> auto& target) {
    using enum REGISTER_FLAG;
    bool lsb = target & 0x01;
    target >>= 1;
    auto& flags = F;
    setZero(target);
    flags.clear(N);
    flags.clear(H);
    (lsb) ? flags.set(C) : flags.clear(C);
}

inline void CPU::shiftRightLogicalIndirect(uint16_t address) {
    auto value = mmu.read(address);
    shiftRightLogical(value);
    mmu.write(address, value);
}

inline void CPU::swap(Integer<8> auto& target) {
    using enum REGISTER_FLAG;
    target = (target << 4) | (target >> 4);
    auto& flags = F;
    setZero(target);
    flags.clear(N);
    flags.clear(H);
    flags.clear(C);
}

inline void CPU::swapIndirect(uint16_t address) {
    auto value = mmu.read(address);
    swap(value);
    mmu.write(address, value);
}

inline void CPU::pop(Register<16> auto& target) {
    auto& sp = SP;
    target.setLo(mmu.read(sp++));
    target.setHi(mmu.read(sp++));
}

inline void CPU::push(Register<16> auto& target) {
    auto& sp = SP;
    mmu.write(--sp, target.hi());
    mmu.write(--sp, target.lo());
}

inline void CPU::call(uint16_t address) {
    push(PC);
    jump(address);
}

inline void CPU::jump(uint16_t address) {
    PC = address;
}

inline void CPU::jumpRelative(int8_t offset) {
    PC += offset;
}

inline void CPU::ret() {
    pop(PC);
}

inline void CPU::reti() {
    ret();
    IME = INTERRUPT_MASTER_FLAG::ENABLE_PENDING; // skip to this state since ret() counts as next instruction
}

inline void CPU::restart(uint8_t address) {
    call(address);
}

inline void CPU::complementCarryFlag() {
    using enum REGISTER_FLAG;
    auto& flags = F;
    flags.clear(N);
    flags.clear(H);
    (flags.test(C)) ? flags.clear(C) : flags.set(C);
}

inline void CPU::setCarryFlag() {
    using enum REGISTER_FLAG;
    auto& flags = F;
    flags.clear(N);
    flags.clear(H);
    flags.set(C);
}

inline void CPU::disableInterrupts() {
    IME = INTERRUPT_MASTER_FLAG::DISABLED;
}

inline void CPU::enableInterrupts() {
    IME = INTERRUPT_MASTER_FLAG::ENABLE_REQUESTED; // signal to enable IME after next instruction
}

inline void CPU::halt() {
    if (IME == INTERRUPT_MASTER_FLAG::ENABLED) {
        // pause execution until interrupt is serviced (call handler)
    }
    else {
        if ((mmu.read(MMU::IE) & mmu.read(MMU::IF)) == 0) { // interrupts are not pending
            // pause execution until interrupt becomes pending, dont call handler
        }
        else { // interrupts are pending
            // continue execution, read next byte twice, dont increment pc (bug emulation)
        }
    }
}

inline void CPU::decimalAdjustAccumulator() {
    using enum REGISTER_FLAG;
    auto& flags = F;
    auto& accumulator = A;
    uint8_t adjustment = 0;
    if (flags.test(N)) {
        if (flags.test(H)) {
            adjustment += 0x06;
        }
        if (flags.test(C)) {
            adjustment += 0x60;
        }
        accumulator -= adjustment;
    }
    else {
        if (flags.test(H) || ((accumulator & 0x0F) > 0x09)) {
            adjustment += 0x06;
        }
        if (flags.test(C) || (accumulator > 0x99)) {
            adjustment += 0x60;
            flags.set(C);
        }
        accumulator += adjustment;
    }
    setZero(accumulator);
    flags.clear(H);
}

inline void CPU::stop() {
    // this one is kind of ridiculous (and unused), just interpret as noop
    // maybe will implement in the future for completeness
}