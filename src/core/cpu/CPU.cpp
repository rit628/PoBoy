#include "CPU.hpp"
#include "Opcodes.hpp"
#include "Register.hpp"
#include <array>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <print>
#include <utility>

void CPU::tick() {
    /* Argument Constants */
    uint8_t $00 = 0x00, $08 = 0x08, $10 = 0x10, $18 = 0x18, $20 = 0x20, $28 = 0x28, $30 = 0x30, $38 = 0x38;
    uint16_t a16 = 0, n16 = 0;
    uint8_t n8 = 0, a8 = 0;
    int8_t e8 = 0;
    RegisterView& Z = F, NZ = F, C = F, NC = F;

    if (readPrefixed) {
        auto opcode = static_cast<OPCODE_CBPREFIXED>(mmu[PC++]);
        switch (opcode) {
            case OPCODE_CBPREFIXED::BIT_0x7C: {
                BIT_0x7C(0b01000000, H);
            }
            break;

            case OPCODE_CBPREFIXED::SWAP_0x37: {
                SWAP_0x37(A);
            }
            break;

            case OPCODE_CBPREFIXED::RL_0x10: {
                RL_0x10(B);
            }
            break;

            default:
                std::println("UNIMPLEMENTED CBPREFIXED OPCODE");
            break;
        }
    }
    else {
        auto opcode = static_cast<OPCODE_UNPREFIXED>(mmu[PC++]);
        switch (opcode) {
            #define OPCODE_BEGIN(code, name, bytecount, ...) \
            case OPCODE_UNPREFIXED::name##_##code: { \
                std::print("{}: {} {}", uint16_t(PC), #code, #name);
                #define CYCLES_TAKEN(...)
                #define CYCLES_SKIPPED(...)
                #define FLAG_VALUE(...)
                #define OPERAND(name, type, bytecount, immediate, postop, ...) \
                if constexpr (bytecount > 0) { \
                    std::array<uint8_t, bytecount> bytes; \
                    for (auto&& byte : bytes) { \
                        byte = mmu[PC++]; \
                    } \
                    std::memcpy(&name, bytes.data(), bytecount); \
                }
                #define OPCODE_END(code, name, args...) \
                name##_##code(args); \
            } \
            break;
            #include "unprefixed.inc"
            #undef OPCODE_BEGIN
            #undef CYCLES_TAKEN
            #undef CYCLES_SKIPPED
            #undef FLAG_VALUE
            #undef OPERAND
            #undef OPCODE_END
        }
    }
}

uint8_t CPU::getCarry() {
    using enum REGISTER_FLAG;
    return (F & std::to_underlying(C)) >> 4; // 1 if C is set else 0
}

uint8_t CPU::getHalfCarry() {
    using enum REGISTER_FLAG;
    return (F & std::to_underlying(H)) >> 5; // 1 if H is set else 0
}

void CPU::setZero(uint8_t result) {
    using enum REGISTER_FLAG;
    (result == 0) ? F.set(Z) : F.clear(Z);
}

uint8_t CPU::addAndSetFlags(uint8_t a, uint8_t b, uint8_t carry) {
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

uint16_t CPU::addAndSetFlags(uint16_t a, uint16_t b) {
    using enum REGISTER_FLAG;
    uint8_t result = a + b;

    auto& flags = F;
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

uint8_t CPU::subtractAndSetFlags(uint8_t a, uint8_t b, uint8_t carry) {
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
void CPU::load(Register<N> auto& target, RegisterValue<N> value) {
    target = value;
}

void CPU::loadIndirect(uint16_t address, uint8_t value) {
    mmu[address] = value;
}

void CPU::loadIndirect(Register<8> auto& target, uint16_t address) {
    target = mmu[address];
}

void CPU::loadHiIndirect(Register<8> auto& target, uint8_t address) {
    target = mmu[static_cast<uint16_t>(address) + 0xFF00];
}

void CPU::loadHiIndirect(uint16_t address, uint8_t value) {
    mmu[static_cast<uint16_t>(address) + 0xFF00] = value;
}

void CPU::loadIncrement(Register<8> auto& target, Register<16> auto& address) {
    target = mmu[address++];
}

void CPU::loadIncrement(Register<16> auto& address, uint8_t value) {
    mmu[address++] = value;
}

void CPU::loadDecrement(Register<8> auto& target, Register<16> auto& address) {
    target = mmu[address--];
}

void CPU::loadDecrement(Register<16> auto& address, uint8_t value) {
    mmu[address--] = value;
}

template<size_t N>
void CPU::add(Register<N> auto& target, RegisterValue<N> value) {
    target = addAndSetFlags(target, value); // may need to add a check if N == 16 to ensure Z isnt modified
}

void CPU::addIndirect(Register<8> auto& target, uint16_t address) {
    auto& value = mmu[address];
    add(target, value);
}

void CPU::adc(Register<8> auto& target, uint8_t value) {
    auto carry = getCarry();
    target = addAndSetFlags(target, value, carry);
}

void CPU::adcIndirect(Register<8> auto& target, uint16_t address) {
    auto& value = mmu[address];
    adc(target, value);
}

void CPU::sub(Register<8> auto& target, uint8_t value) {
    target = subtractAndSetFlags(target, value);
}

void CPU::subIndirect(Register<8> auto& target, uint16_t address) {
    auto& value = mmu[address];
    sub(target, value);
}

void CPU::sbc(Register<8> auto& target, uint8_t value) {
    auto carry = getCarry();
    target = subAndSetFlags(target, value, carry);
}

void CPU::sbcIndirect(Register<8> auto& target, uint16_t address) {
    auto& value = mmu[address];
    sbc(target, value);
}

void CPU::compare(uint8_t lhs, uint8_t rhs) {
    subtractAndSetFlags(lhs, rhs); // subtract and discard result
}

void CPU::compareIndirect(uint8_t lhs, uint16_t address) {
    auto& rhs = mmu[address];
    compare(lhs, rhs);
}

void CPU::decrement(Integer<8> auto& target) {
    using enum REGISTER_FLAG;
    bool carrySet = getCarry();
    auto& flags = F;
    target = subtractAndSetFlags(target, 1);
    (carrySet) ? flags.set(C) : flags.clear(C); // ensure C remains unmodified
}

void CPU::decrement(Register<16> auto& target) {
    target -= 1;
}

void CPU::decrementIndirect(uint16_t address) {
    auto& target = mmu[address];
    decrement(target);
}

void CPU::increment(Integer<8> auto& target) {
    using enum REGISTER_FLAG;
    bool carrySet = getCarry();
    auto& flags = F;
    target = addAndSetFlags(target, 1);
    (carrySet) ? flags.set(C) : flags.clear(C); // ensure C remains unmodified
}

void CPU::increment(Register<16> auto& target) {
    target += 1;
}

void CPU::incrementIndirect(uint16_t address) {
    auto& target = mmu[address];
    increment(target);
}

void CPU::bitAnd(Register<8> auto& lhs, uint8_t rhs) {
    using enum REGISTER_FLAG;
    lhs &= rhs;
    auto& flags = F;
    setZero(lhs);
    flags.clear(N);
    flags.set(H);
    flags.clear(C);
}

void CPU::bitAndIndirect(Register<8> auto& lhs, uint16_t address) {
    auto& rhs = mmu[address];
    bitAnd(lhs, rhs);
}

void CPU::bitNot(Register<8> auto& target) {
    using enum REGISTER_FLAG;
    target = ~target;
    auto& flags = F;
    flags.set(N);
    flags.set(H);
}

void CPU::bitOr(Register<8> auto& lhs, uint8_t rhs) {
    using enum REGISTER_FLAG;
    lhs |= rhs;
    auto& flags = F;
    setZero(lhs);
    flags.clear(N);
    flags.clear(H);
    flags.clear(C);
}

void CPU::bitOrIndirect(Register<8> auto& lhs, uint16_t address) {
    auto& rhs = mmu[address];
    bitOr(lhs, rhs);
}

void CPU::bitXor(Register<8> auto& lhs, uint8_t rhs) {
    using enum REGISTER_FLAG;
    lhs ^= rhs;
    auto& flags = F;
    setZero(lhs);
    flags.clear(N);
    flags.clear(H);
    flags.clear(C);
}

void CPU::bitXorIndirect(Register<8> auto& lhs, uint16_t address) {
    auto& rhs = mmu[address];
    bitXor(lhs, rhs);
}

void CPU::bitTest(uint8_t test, uint8_t target) {
    using enum REGISTER_FLAG;
    auto& flags = F;
    if (!(test & target)) {
        flags.set(Z);
    }
    flags.clear(N);
    flags.set(H);
}

void CPU::bitTestIndirect(uint8_t test, uint16_t address) {
    auto& target = mmu[address];
    bitTest(test, target);
}

void CPU::bitReset(uint8_t test, Integer<8> auto& target) {
    target &= ~test; // set bit "test" to 0 in target
}

void CPU::bitResetIndirect(uint8_t test, uint16_t address) {
    auto& target = mmu[address];
    bitReset(test, target);
}

void CPU::bitSet(uint8_t test, Integer<8> auto& target) {
    target |= test; // set bit "test" to 1 in target
}

void CPU::bitSetIndirect(uint8_t test, uint16_t address) {
    auto& target = mmu[address];
    bitReset(test, target);
}

void CPU::rotateLeft(Integer<8> auto& target) {
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

void CPU::rotateLeftIndirect(uint16_t address) {
    auto& target = mmu[address];
    rotateLeft(target);
}

void CPU::rotateLeftCircular(Integer<8> auto& target) {
    using enum REGISTER_FLAG;
    uint8_t msb = (target & 0x80) >> 7; // msb becomes new lsb
    target = (target << 1) | msb; 

    auto& flags = F;
    setZero(target);
    flags.clear(N);
    flags.clear(H);
    (msb) ? flags.set(C) : flags.clear(C); // carry = former msb
}

void CPU::rotateLeftCircularIndirect(uint16_t address) {
    auto& target = mmu[address];
    rotateLeftCircular(target);
}

void CPU::rotateRight(Integer<8> auto& target) {
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

void CPU::rotateRightIndirect(uint16_t address) {
    auto& target = mmu[address];
    rotateRight(target);
}

void CPU::rotateRightCircular(Integer<8> auto& target) {
    using enum REGISTER_FLAG;
    uint8_t lsb = (target & 0x01) << 7; // lsb becomes new msb
    target = (target >> 1) | lsb;

    auto& flags = F;
    setZero(target);
    flags.clear(N);
    flags.clear(H);
    (lsb) ? flags.set(C) : flags.clear(C); // carry = former lsb
}

void CPU::rotateRightCircularIndirect(uint16_t address) {
    auto& target = mmu[address];
    rotateRightCircular(target);
}

void CPU::shiftLeftArithmetic(Integer<8> auto& target) {
    using enum REGISTER_FLAG;
    bool msb = target & 0x80;
    target <<= 1;
    auto& flags = F;
    setZero(target);
    flags.clear(N);
    flags.clear(H);
    (msb) ? flags.set(C) : flags.clear(C);
}

void CPU::shiftLeftArithmeticIndirect(uint16_t address) {
    auto& target = mmu[address];
    shiftLeftArithmetic(target);
}

void CPU::shiftRightArithmetic(Integer<8> auto& target) {
    using enum REGISTER_FLAG;
    bool lsb = target & 0x01;
    bool msb = target & 0x80;
    target = (target >> 1) | msb; // preserve msb
    auto& flags = F;
    setZero(target);
    flags.clear(N);
    flags.clear(H);
    (lsb) ? flags.set(C) : flags.clear(C);
}

void CPU::shiftRightArithmeticIndirect(uint16_t address) {
    auto& target = mmu[address];
    shiftRightArithmetic(target);
}

void CPU::shiftRightLogical(Integer<8> auto& target) {
    using enum REGISTER_FLAG;
    bool lsb = target & 0x01;
    target >>= 1;
    auto& flags = F;
    setZero(target);
    flags.clear(N);
    flags.clear(H);
    (lsb) ? flags.set(C) : flags.clear(C);
}

void CPU::shiftRightLogicalIndirect(uint16_t address) {
    auto& target = mmu[address];
    shiftRightLogical(target);
}

void CPU::swap(Integer<8> auto& target) {
    using enum REGISTER_FLAG;
    target = (target << 4) | (target >> 4);
    auto& flags = F;
    setZero(target);
    flags.clear(N);
    flags.clear(H);
    flags.clear(C);
}

void CPU::swapIndirect(uint16_t address) {
    auto& target = mmu[address];
    swap(target);
}

void CPU::pop(Register<16> auto& target) {
    auto& sp = SP;
    target.setLo(mmu[sp++]);
    target.setHi(mmu[sp++]);
}

void CPU::push(Register<16> auto& target) {
    auto& sp = SP;
    mmu[--sp] = target.hi();
    mmu[--sp] = target.lo();
}

void CPU::call(uint16_t address) {
    push(PC);
    jump(address);
}

void CPU::jump(uint16_t address) {
    PC = address;
}

void CPU::jumpRelative(int8_t offset) {
    PC += offset;
}

void CPU::ret() {
    pop(PC);
}

void CPU::reti() {
    ret();
    IME = INTERRUPT_MASTER_FLAG::ENABLE_PENDING; // skip to this state since ret() counts as next instruction
}

void CPU::restart(uint8_t address) {
    call(address);
}

void CPU::complementCarryFlag() {
    using enum REGISTER_FLAG;
    auto& flags = F;
    flags.clear(N);
    flags.clear(H);
    (flags.test(C)) ? flags.clear(C) : flags.set(C);
}

void CPU::setCarryFlag() {
    using enum REGISTER_FLAG;
    auto& flags = F;
    flags.clear(N);
    flags.clear(H);
    flags.set(C);
}

void CPU::disableInterrupts() {
    IME = INTERRUPT_MASTER_FLAG::DISABLED;
}

void CPU::enableInterrupts() {
    IME = INTERRUPT_MASTER_FLAG::ENABLE_REQUESTED; // signal to enable IME after next instruction
}

void CPU::halt() {
    if (IME == INTERRUPT_MASTER_FLAG::ENABLED) {
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

void CPU::decimalAdjustAccumulator() {
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

void CPU::stop() {
    // this one is kind of ridiculous (and unused), just interpret as noop
    // maybe will implement in the future for completeness
}