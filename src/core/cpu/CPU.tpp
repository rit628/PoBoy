#pragma once
#include "CPU.hpp"
#include "IO.hpp"
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

    setZero(result);
    F.clear(N);

    // set half carry (H): mask upper 4 bits and check if exceeds lower 4
    (((a & 0x0F) + (b & 0x0F) + carry) > 0x0F) ? F.set(H) : F.clear(H);

    // set carry (C): check if greater than uint8_t max
    ((static_cast<uint16_t>(a) + static_cast<uint16_t>(b) + carry) > 0xFF) ? F.set(C) : F.clear(C);
    
    return result;
}

inline uint16_t CPU::addAndSetFlags(uint16_t a, uint16_t b) {
    using enum REGISTER_FLAG;
    uint16_t result = a + b;

    F.clear(N);

    // set half carry (H): mask upper 4 bits and check if exceeds lower 12
    (((a & 0x0FFF) + (b & 0x0FFF)) > 0x0FFF) ? F.set(H) : F.clear(H);

    // set carry (C): check if greater than uint16_t max
    ((static_cast<uint32_t>(a) + static_cast<uint32_t>(b)) > 0xFFFF) ? F.set(C) : F.clear(C);
    
    return result;
}

inline uint16_t CPU::addAndSetFlags(uint16_t a, int8_t b) {
    // set carry and half carry based on 8 bit unsigned addition
    addAndSetFlags(static_cast<uint8_t>(a & 0x00FF), std::bit_cast<uint8_t, int8_t>(b));
    
    // clear Z and N flags unconditionally
    using enum REGISTER_FLAG;
    F.clear(Z);
    F.clear(N);

    return a + b;
}

inline uint8_t CPU::subtractAndSetFlags(uint8_t a, uint8_t b, uint8_t carry) {
    using enum REGISTER_FLAG;
    uint8_t result = static_cast<int16_t>(a) - static_cast<int16_t>(b) - carry;

    setZero(result);
    F.set(N);

    // set half carry (H): mask upper 4 bits and check if negative (borrow from bit 4)
    ((static_cast<int8_t>(a & 0x0F) - static_cast<int8_t>(b & 0x0F) - carry) < 0) ? F.set(H) : F.clear(H);

    // set carry (C): check if result is negative (borrow required)
    ((static_cast<int16_t>(a) - static_cast<int16_t>(b) - carry) < 0) ? F.set(C) : F.clear(C);

    return result;
}

template<size_t N>
inline void CPU::load(Register<N> auto& target, RegisterValue<N> value) {
    target = value;
}

inline void CPU::loadIndirect(uint16_t address, uint8_t value) {
    mmu.write(address, value);
}

inline void CPU::loadIndirect(uint16_t address, uint16_t value) {
    mmu.write(address, value & 0x00FF);
    mmu.write(address + 1, (value & 0xFF00) >> 8);
}

inline void CPU::loadIndirect(Register<8> auto& target, uint16_t address) {
    target = mmu.read(address);
}

inline void CPU::loadHiIndirect(Register<8> auto& target, uint8_t address) {
    target = mmu.read(static_cast<uint16_t>(address) | 0xFF00);
}

inline void CPU::loadHiIndirect(uint16_t address, uint8_t value) {
    mmu.write(static_cast<uint16_t>(address) | 0xFF00, value);
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

inline void CPU::loadAdjusted(Register<16> auto& target, Register<16> auto& value, int8_t adjust) {
    auto adjustedValue = addAndSetFlags(value, adjust);
    target = adjustedValue;
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

inline void CPU::addRelative(Register<16> auto& target, int8_t value) {
    target = addAndSetFlags(target, value);
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
    target = subtractAndSetFlags(target, 1);
    (carrySet) ? F.set(C) : F.clear(C); // ensure C remains unmodified
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
    target = addAndSetFlags(target, 1);
    (carrySet) ? F.set(C) : F.clear(C); // ensure C remains unmodified
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
    setZero(lhs);
    F.clear(N);
    F.set(H);
    F.clear(C);
}

inline void CPU::bitAndIndirect(Register<8> auto& lhs, uint16_t address) {
    auto rhs = mmu.read(address);
    bitAnd(lhs, rhs);
}

inline void CPU::bitNot(Register<8> auto& target) {
    using enum REGISTER_FLAG;
    target = ~target;
    F.set(N);
    F.set(H);
}

inline void CPU::bitOr(Register<8> auto& lhs, uint8_t rhs) {
    using enum REGISTER_FLAG;
    lhs |= rhs;
    setZero(lhs);
    F.clear(N);
    F.clear(H);
    F.clear(C);
}

inline void CPU::bitOrIndirect(Register<8> auto& lhs, uint16_t address) {
    auto rhs = mmu.read(address);
    bitOr(lhs, rhs);
}

inline void CPU::bitXor(Register<8> auto& lhs, uint8_t rhs) {
    using enum REGISTER_FLAG;
    lhs ^= rhs;
    setZero(lhs);
    F.clear(N);
    F.clear(H);
    F.clear(C);
}

inline void CPU::bitXorIndirect(Register<8> auto& lhs, uint16_t address) {
    auto rhs = mmu.read(address);
    bitXor(lhs, rhs);
}

inline void CPU::bitTest(uint8_t bitIndex, uint8_t target) {
    using enum REGISTER_FLAG;
    uint8_t test = 1 << bitIndex;
    (test & target) ? F.clear(Z) : F.set(Z);
    F.clear(N);
    F.set(H);
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

    setZero(target);
    F.clear(N);
    F.clear(H);
    (msb) ? F.set(C) : F.clear(C); // carry = former msb
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

    setZero(target);
    F.clear(N);
    F.clear(H);
    (msb) ? F.set(C) : F.clear(C); // carry = former msb
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

    setZero(target);
    F.clear(N);
    F.clear(H);
    (lsb) ? F.set(C) : F.clear(C); // carry = former lsb
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

    setZero(target);
    F.clear(N);
    F.clear(H);
    (lsb) ? F.set(C) : F.clear(C); // carry = former lsb
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
    setZero(target);
    F.clear(N);
    F.clear(H);
    (msb) ? F.set(C) : F.clear(C);
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
    setZero(target);
    F.clear(N);
    F.clear(H);
    (lsb) ? F.set(C) : F.clear(C);
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
    setZero(target);
    F.clear(N);
    F.clear(H);
    (lsb) ? F.set(C) : F.clear(C);
}

inline void CPU::shiftRightLogicalIndirect(uint16_t address) {
    auto value = mmu.read(address);
    shiftRightLogical(value);
    mmu.write(address, value);
}

inline void CPU::swap(Integer<8> auto& target) {
    using enum REGISTER_FLAG;
    target = (target << 4) | (target >> 4);
    setZero(target);
    F.clear(N);
    F.clear(H);
    F.clear(C);
}

inline void CPU::swapIndirect(uint16_t address) {
    auto value = mmu.read(address);
    swap(value);
    mmu.write(address, value);
}

inline void CPU::pop(Register<16> auto& target) {
    target.setLo(mmu.read(SP++));
    target.setHi(mmu.read(SP++));
}

inline void CPU::push(Register<16> auto& target) {
    mmu.write(--SP, target.hi());
    mmu.write(--SP, target.lo());
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
    IME = INTERRUPT_MASTER_FLAG::ENABLED; // skip to this state since ret() counts as next instruction
}

inline void CPU::restart(uint8_t address) {
    call(address);
}

inline void CPU::complementCarryFlag() {
    using enum REGISTER_FLAG;
    F.clear(N);
    F.clear(H);
    (F.test(C)) ? F.clear(C) : F.set(C);
}

inline void CPU::setCarryFlag() {
    using enum REGISTER_FLAG;
    F.clear(N);
    F.clear(H);
    F.set(C);
}

inline void CPU::disableInterrupts() {
    IME = INTERRUPT_MASTER_FLAG::DISABLED;
}

inline void CPU::enableInterrupts() {
    IME = INTERRUPT_MASTER_FLAG::ENABLE_PENDING; // signal to enable interrupts after next machine cycle
}

inline void CPU::halt() {
    using enum STATE;
    auto IF = mmu.read(IO::IF);
    auto IE = mmu.read(IO::IE);
    if (IME == INTERRUPT_MASTER_FLAG::ENABLED || !(IF & IE & 0x1F)) {
        state = HALTED;
    }
    else { // IME = 0 and some interrupt is pending => halt bug
        state = BUGGED;
    }
}

inline void CPU::decimalAdjustAccumulator() {
    using enum REGISTER_FLAG;
    auto& accumulator = A;
    uint8_t adjustment = 0;
    if (F.test(N)) {
        if (F.test(H)) {
            adjustment += 0x06;
        }
        if (F.test(C)) {
            adjustment += 0x60;
        }
        accumulator -= adjustment;
    }
    else {
        if (F.test(H) || ((accumulator & 0x0F) > 0x09)) {
            adjustment += 0x06;
        }
        if (F.test(C) || (accumulator > 0x99)) {
            adjustment += 0x60;
            F.set(C);
        }
        accumulator += adjustment;
    }
    setZero(accumulator);
    F.clear(H);
}

inline void CPU::stop() {
    // this one is kind of ridiculous (and unused), just interpret as noop
    // maybe will implement in the future for completeness
}