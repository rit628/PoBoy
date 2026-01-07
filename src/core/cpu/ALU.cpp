#include "ALU.hpp"
#include "Register.hpp"
#include <cstddef>
#include <cstdint>
#include <utility>

uint8_t ALU::getCarry() {
    using enum REGISTER_FLAG;
    return (rf.F & std::to_underlying(C)) >> 4; // 1 if C is set else 0
}

uint8_t ALU::getHalfCarry() {
    using enum REGISTER_FLAG;
    return (rf.F & std::to_underlying(H)) >> 5; // 1 if H is set else 0
}

void ALU::setZero(uint8_t result) {
    using enum REGISTER_FLAG;
    (result == 0) ? rf.F.set(Z) : rf.F.clear(Z);
}

uint8_t ALU::addAndSetFlags(uint8_t a, uint8_t b, uint8_t carry) {
    using enum REGISTER_FLAG;
    uint8_t result = a + b + carry;

    auto& flags = rf.F;
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

uint16_t ALU::addAndSetFlags(uint16_t a, uint16_t b) {
    using enum REGISTER_FLAG;
    uint8_t result = a + b;

    auto& flags = rf.F;
    setZero(result);
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

uint8_t ALU::subtractAndSetFlags(uint8_t a, uint8_t b, uint8_t carry) {
    using enum REGISTER_FLAG;
    uint8_t result = static_cast<int16_t>(a) - static_cast<int16_t>(b) - carry;

    auto& flags = rf.F;
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
void ALU::load(Register<N> auto& target, RegisterValue<N> value) {
    target = value;
}

void ALU::loadIndirect(uint16_t address, uint8_t value) {
    mmu[address] = value;
}

void ALU::loadIndirect(Register<8> auto& target, uint16_t address) {
    target = mmu[address];
}

void ALU::loadHiIndirect(Register<8> auto& target, uint8_t address) {
    target = mmu[static_cast<uint16_t>(address) + 0xFF00];
}

void ALU::loadHiIndirect(uint16_t address, uint8_t value) {
    mmu[static_cast<uint16_t>(address) + 0xFF00] = value;
}

void ALU::loadIncrement(Register<8> auto& target, Register<16> auto& address) {
    target = mmu[address++];
}

void ALU::loadIncrement(Register<16> auto& address, uint8_t value) {
    mmu[address++] = value;
}

void ALU::loadDecrement(Register<8> auto& target, Register<16> auto& address) {
    target = mmu[address--];
}

void ALU::loadDecrement(Register<16> auto& address, uint8_t value) {
    mmu[address--] = value;
}

template<size_t N>
void ALU::add(Register<N> auto& target, RegisterValue<N> value) {
    target = addAndSetFlags(target, value); // may need to add a check if N == 16 to ensure Z isnt modified
}

void ALU::addIndirect(Register<8> auto& target, uint16_t address) {
    auto& value = mmu[address];
    add(target, value);
}

void ALU::adc(Register<8> auto& target, uint8_t value) {
    auto carry = getCarry();
    target = addAndSetFlags(target, value, carry);
}

void ALU::adcIndirect(Register<8> auto& target, uint16_t address) {
    auto& value = mmu[address];
    adc(target, value);
}

void ALU::sub(Register<8> auto& target, uint8_t value) {
    target = subtractAndSetFlags(target, value);
}

void ALU::subIndirect(Register<8> auto& target, uint16_t address) {
    auto& value = mmu[address];
    sub(target, value);
}

void ALU::sbc(Register<8> auto& target, uint8_t value) {
    auto carry = getCarry();
    target = subAndSetFlags(target, value, carry);
}

void ALU::sbcIndirect(Register<8> auto& target, uint16_t address) {
    auto& value = mmu[address];
    sbc(target, value);
}

void ALU::compare(uint8_t lhs, uint8_t rhs) {
    subtractAndSetFlags(lhs, rhs); // subtract and discard result
}

void ALU::compareIndirect(uint8_t lhs, uint16_t address) {
    auto& rhs = mmu[address];
    compare(lhs, rhs);
}

void ALU::decrement(Integer<8> auto& target) {
    using enum REGISTER_FLAG;
    bool carrySet = getCarry();
    auto& flags = rf.F;
    target = subtractAndSetFlags(target, 1);
    (carrySet) ? flags.set(C) : flags.clear(C); // ensure C remains unmodified
}

void ALU::decrement(Register<16> auto& target) {
    target -= 1;
}

void ALU::decrementIndirect(uint16_t address) {
    auto& target = mmu[address];
    decrement(target);
}

void ALU::increment(Integer<8> auto& target) {
    using enum REGISTER_FLAG;
    bool carrySet = getCarry();
    auto& flags = rf.F;
    target = addAndSetFlags(target, 1);
    (carrySet) ? flags.set(C) : flags.clear(C); // ensure C remains unmodified
}

void ALU::increment(Register<16> auto& target) {
    target += 1;
}

void ALU::incrementIndirect(uint16_t address) {
    auto& target = mmu[address];
    increment(target);
}

void ALU::bitAnd(Register<8> auto& lhs, uint8_t rhs) {
    using enum REGISTER_FLAG;
    lhs &= rhs;
    auto& flags = rf.F;
    setZero(lhs);
    flags.clear(N);
    flags.set(H);
    flags.clear(C);
}

void ALU::bitAndIndirect(Register<8> auto& lhs, uint16_t address) {
    auto& rhs = mmu[address];
    bitAnd(lhs, rhs);
}

void ALU::bitNot(Register<8> auto& target) {
    using enum REGISTER_FLAG;
    target = ~target;
    auto& flags = rf.F;
    flags.set(N);
    flags.set(H);
}

void ALU::bitOr(Register<8> auto& lhs, uint8_t rhs) {
    using enum REGISTER_FLAG;
    lhs |= rhs;
    auto& flags = rf.F;
    setZero(lhs);
    flags.clear(N);
    flags.clear(H);
    flags.clear(C);
}

void ALU::bitOrIndirect(Register<8> auto& lhs, uint16_t address) {
    auto& rhs = mmu[address];
    bitOr(lhs, rhs);
}

void ALU::bitXor(Register<8> auto& lhs, uint8_t rhs) {
    using enum REGISTER_FLAG;
    lhs ^= rhs;
    auto& flags = rf.F;
    setZero(lhs);
    flags.clear(N);
    flags.clear(H);
    flags.clear(C);
}

void ALU::bitXorIndirect(Register<8> auto& lhs, uint16_t address) {
    auto& rhs = mmu[address];
    bitXor(lhs, rhs);
}

void ALU::bitTest(uint8_t test, uint8_t target) {
    using enum REGISTER_FLAG;
    auto& flags = rf.F;
    if (!(test & target)) {
        flags.set(Z);
    }
    flags.clear(N);
    flags.set(H);
}

void ALU::bitTestIndirect(uint8_t test, uint16_t address) {
    auto& target = mmu[address];
    bitTest(test, target);
}

void ALU::bitReset(uint8_t test, Integer<8> auto& target) {
    target &= ~test; // set bit "test" to 0 in target
}

void ALU::bitResetIndirect(uint8_t test, uint16_t address) {
    auto& target = mmu[address];
    bitReset(test, target);
}

void ALU::bitSet(uint8_t test, Integer<8> auto& target) {
    target |= test; // set bit "test" to 1 in target
}

void ALU::bitSetIndirect(uint8_t test, uint16_t address) {
    auto& target = mmu[address];
    bitReset(test, target);
}

void ALU::rotateLeft(Integer<8> auto& target) {
    using enum REGISTER_FLAG;
    uint8_t carry = getCarry(); // carry becomes new lsb
    bool msb = target & 0x80;
    target = (target << 1) | carry; // rotate through carry

    auto& flags = rf.F;
    setZero(target);
    flags.clear(N);
    flags.clear(H);
    (msb) ? flags.set(C) : flags.clear(C); // carry = former msb
}

void ALU::rotateLeftIndirect(uint16_t address) {
    auto& target = mmu[address];
    rotateLeft(target);
}

void ALU::rotateLeftCircular(Integer<8> auto& target) {
    using enum REGISTER_FLAG;
    uint8_t msb = (target & 0x80) >> 7; // msb becomes new lsb
    target = (target << 1) | msb; 

    auto& flags = rf.F;
    setZero(target);
    flags.clear(N);
    flags.clear(H);
    (msb) ? flags.set(C) : flags.clear(C); // carry = former msb
}

void ALU::rotateLeftCircularIndirect(uint16_t address) {
    auto& target = mmu[address];
    rotateLeftCircular(target);
}

void ALU::rotateRight(Integer<8> auto& target) {
    using enum REGISTER_FLAG;
    uint8_t carry = getCarry() << 7; // carry becomes new msb
    bool lsb = target & 0x01;
    target = (target >> 1) | carry; // rotate through carry

    auto& flags = rf.F;
    setZero(target);
    flags.clear(N);
    flags.clear(H);
    (lsb) ? flags.set(C) : flags.clear(C); // carry = former lsb
}

void ALU::rotateRightIndirect(uint16_t address) {
    auto& target = mmu[address];
    rotateRight(target);
}

void ALU::rotateRightCircular(Integer<8> auto& target) {
    using enum REGISTER_FLAG;
    uint8_t lsb = (target & 0x01) << 7; // lsb becomes new msb
    target = (target >> 1) | lsb;

    auto& flags = rf.F;
    setZero(target);
    flags.clear(N);
    flags.clear(H);
    (lsb) ? flags.set(C) : flags.clear(C); // carry = former lsb
}

void ALU::rotateRightCircularIndirect(uint16_t address) {
    auto& target = mmu[address];
    rotateRightCircular(target);
}

void ALU::shiftLeftArithmetic(Integer<8> auto& target) {
    using enum REGISTER_FLAG;
    bool msb = target & 0x80;
    target <<= 1;
    auto& flags = rf.F;
    setZero(target);
    flags.clear(N);
    flags.clear(H);
    (msb) ? flags.set(C) : flags.clear(C);
}

void ALU::shiftLeftArithmeticIndirect(uint16_t address) {
    auto& target = mmu[address];
    shiftLeftArithmetic(target);
}

void ALU::shiftRightArithmetic(Integer<8> auto& target) {
    using enum REGISTER_FLAG;
    bool lsb = target & 0x01;
    bool msb = target & 0x80;
    target = (target >> 1) | msb; // preserve msb
    auto& flags = rf.F;
    setZero(target);
    flags.clear(N);
    flags.clear(H);
    (lsb) ? flags.set(C) : flags.clear(C);
}

void ALU::shiftRightArithmeticIndirect(uint16_t address) {
    auto& target = mmu[address];
    shiftRightArithmetic(target);
}

void ALU::shiftRightLogical(Integer<8> auto& target) {
    using enum REGISTER_FLAG;
    bool lsb = target & 0x01;
    target >>= 1;
    auto& flags = rf.F;
    setZero(target);
    flags.clear(N);
    flags.clear(H);
    (lsb) ? flags.set(C) : flags.clear(C);
}

void ALU::shiftRightLogicalIndirect(uint16_t address) {
    auto& target = mmu[address];
    shiftRightLogical(target);
}

void ALU::swap(Integer<8> auto& target) {
    using enum REGISTER_FLAG;
    target = (target << 4) | (target >> 4);
    auto& flags = rf.F;
    setZero(target);
    flags.clear(N);
    flags.clear(H);
    flags.clear(C);
}

void ALU::swapIndirect(uint16_t address) {
    auto& target = mmu[address];
    swap(target);
}

void ALU::pop(Register<16> auto& target) {
    auto& sp = rf.SP;
    target.setLo(mmu[sp++]);
    target.setHi(mmu[sp++]);
}

void ALU::push(Register<16> auto& target) {
    auto& sp = rf.SP;
    mmu[--sp] = target.hi();
    mmu[--sp] = target.lo();
}

void ALU::call(uint16_t address) {
    push(rf.PC);
    jump(address);
}

void ALU::jump(uint16_t address) {
    rf.PC = address;
}

void ALU::jumpRelative(int8_t offset) {
    rf.PC += offset;
}

void ALU::ret() {
    pop(rf.PC);
}

void ALU::reti() {
    ret();
    rf.IME = INTERRUPT_MASTER_FLAG::ENABLE_PENDING; // skip to this state since ret() counts as next instruction
}

void ALU::restart(uint8_t address) {
    call(address);
}

void ALU::complementCarryFlag() {
    using enum REGISTER_FLAG;
    auto& flags = rf.F;
    flags.clear(N);
    flags.clear(H);
    (flags.test(C)) ? flags.clear(C) : flags.set(C);
}

void ALU::setCarryFlag() {
    using enum REGISTER_FLAG;
    auto& flags = rf.F;
    flags.clear(N);
    flags.clear(H);
    flags.set(C);
}

void ALU::disableInterrupts() {
    rf.IME = INTERRUPT_MASTER_FLAG::DISABLED;
}

void ALU::enableInterrupts() {
    rf.IME = INTERRUPT_MASTER_FLAG::ENABLE_REQUESTED; // signal to enable IME after next instruction
}

void ALU::halt() {
    if (rf.IME == INTERRUPT_MASTER_FLAG::ENABLED) {
        // pause execution until interrupt is serviced (call handler)
    }
    else {
        if ((mmu[MMU::IE] & mmu[MMU::IF]) == 0) { // interrupts are not pending
            // pause execution until interrupt becomes pending, dont call handler
        }
        else { // interrupts are pending
            // continue execution, read next byte twice, dont increment pc (bug emulation)
        }
    }
}

void ALU::decimalAdjustAccumulator() {
    using enum REGISTER_FLAG;
    auto& flags = rf.F;
    auto& accumulator = rf.A;
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

void ALU::stop() {
    // this one is kind of ridiculous (and unused), just interpret as noop
    // maybe will implement in the future for completeness
}