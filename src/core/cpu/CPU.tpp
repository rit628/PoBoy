#pragma once
#include "CPU.hpp"
#include "MemoryConstants.hpp"
#include <cstddef>
#include <cstdint>
#include <utility>

namespace Processing {

    template<bool FlatMemory>
    inline uint8_t CPU<FlatMemory>::getCarry() {
        using enum REGISTER_FLAG;
        return (F & std::to_underlying(C)) >> 4; // 1 if C is set else 0
    }
    
    template<bool FlatMemory>
    inline uint8_t CPU<FlatMemory>::getHalfCarry() {
        using enum REGISTER_FLAG;
        return (F & std::to_underlying(H)) >> 5; // 1 if H is set else 0
    }
    
    template<bool FlatMemory>
    inline void CPU<FlatMemory>::setZero(uint8_t result) {
        using enum REGISTER_FLAG;
        (result == 0) ? F.set(Z) : F.clear(Z);
    }
    
    template<bool FlatMemory>
    inline uint8_t CPU<FlatMemory>::addAndSetFlags(uint8_t a, uint8_t b, uint8_t carry) {
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
    
    template<bool FlatMemory>
    inline uint16_t CPU<FlatMemory>::addAndSetFlags(uint16_t a, uint16_t b) {
        using enum REGISTER_FLAG;
        uint16_t result = a + b;
    
        F.clear(N);
    
        // set half carry (H): mask upper 4 bits and check if exceeds lower 12
        (((a & 0x0FFF) + (b & 0x0FFF)) > 0x0FFF) ? F.set(H) : F.clear(H);
    
        // set carry (C): check if greater than uint16_t max
        ((static_cast<uint32_t>(a) + static_cast<uint32_t>(b)) > 0xFFFF) ? F.set(C) : F.clear(C);
        
        return result;
    }
    
    template<bool FlatMemory>
    inline uint16_t CPU<FlatMemory>::addAndSetFlags(uint16_t a, int8_t b) {
        // set carry and half carry based on 8 bit unsigned addition
        addAndSetFlags(static_cast<uint8_t>(a & 0x00FF), std::bit_cast<uint8_t, int8_t>(b));
        
        // clear Z and N flags unconditionally
        using enum REGISTER_FLAG;
        F.clear(Z);
        F.clear(N);
    
        return a + b;
    }
    
    template<bool FlatMemory>
    inline uint8_t CPU<FlatMemory>::subtractAndSetFlags(uint8_t a, uint8_t b, uint8_t carry) {
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
    
    template<bool FlatMemory>
    template<size_t N>
    inline void CPU<FlatMemory>::load(Register<N> auto& target, RegisterValue<N> value) {
        target = value;
    }
    
    template<bool FlatMemory>
    inline void CPU<FlatMemory>::loadIndirect(uint16_t address, uint8_t value) {
        write(address, value);
    }
    
    template<bool FlatMemory>
    inline void CPU<FlatMemory>::loadIndirect(uint16_t address, uint16_t value) {
        write(address, value & 0x00FF);
        write(address + 1, (value & 0xFF00) >> 8);
    }
    
    template<bool FlatMemory>
    inline void CPU<FlatMemory>::loadIndirect(Register<8> auto& target, uint16_t address) {
        target = read(address);
    }
    
    template<bool FlatMemory>
    inline void CPU<FlatMemory>::loadHiIndirect(Register<8> auto& target, uint8_t address) {
        target = read(static_cast<uint16_t>(address) | 0xFF00);
    }
    
    template<bool FlatMemory>
    inline void CPU<FlatMemory>::loadHiIndirect(uint16_t address, uint8_t value) {
        write(static_cast<uint16_t>(address) | 0xFF00, value);
    }
    
    template<bool FlatMemory>
    inline void CPU<FlatMemory>::loadIncrement(Register<8> auto& target, Register<16> auto& address) {
        target = read(address++);
    }
    
    template<bool FlatMemory>
    inline void CPU<FlatMemory>::loadIncrement(Register<16> auto& address, uint8_t value) {
        write(address++, value);
    }
    
    template<bool FlatMemory>
    inline void CPU<FlatMemory>::loadDecrement(Register<8> auto& target, Register<16> auto& address) {
        target = read(address--);
    }
    
    template<bool FlatMemory>
    inline void CPU<FlatMemory>::loadDecrement(Register<16> auto& address, uint8_t value) {
        write(address--, value);
    }
    
    template<bool FlatMemory>
    inline void CPU<FlatMemory>::loadAdjusted(Register<16> auto& target, Register<16> auto& value, int8_t adjust) {
        auto adjustedValue = addAndSetFlags(value, adjust);
        target = adjustedValue;
    }
    
    
    template<bool FlatMemory>
    template<size_t N>
    inline void CPU<FlatMemory>::add(Register<N> auto& target, RegisterValue<N> value) {
        target = addAndSetFlags(target, value);
    }
    
    template<bool FlatMemory>
    inline void CPU<FlatMemory>::addIndirect(Register<8> auto& target, uint16_t address) {
        auto value = read(address);
        add<8>(target, value);
    }
    
    template<bool FlatMemory>
    inline void CPU<FlatMemory>::adc(Register<8> auto& target, uint8_t value) {
        auto carry = getCarry();
        target = addAndSetFlags(target, value, carry);
    }
    
    template<bool FlatMemory>
    inline void CPU<FlatMemory>::adcIndirect(Register<8> auto& target, uint16_t address) {
        auto value = read(address);
        adc(target, value);
    }
    
    template<bool FlatMemory>
    inline void CPU<FlatMemory>::addRelative(Register<16> auto& target, int8_t value) {
        target = addAndSetFlags(target, value);
    }
    
    template<bool FlatMemory>
    inline void CPU<FlatMemory>::sub(Register<8> auto& target, uint8_t value) {
        target = subtractAndSetFlags(target, value);
    }
    
    template<bool FlatMemory>
    inline void CPU<FlatMemory>::subIndirect(Register<8> auto& target, uint16_t address) {
        auto value = read(address);
        sub(target, value);
    }
    
    template<bool FlatMemory>
    inline void CPU<FlatMemory>::sbc(Register<8> auto& target, uint8_t value) {
        auto carry = getCarry();
        target = subtractAndSetFlags(target, value, carry);
    }
    
    template<bool FlatMemory>
    inline void CPU<FlatMemory>::sbcIndirect(Register<8> auto& target, uint16_t address) {
        auto value = read(address);
        sbc(target, value);
    }
    
    template<bool FlatMemory>
    inline void CPU<FlatMemory>::compare(uint8_t lhs, uint8_t rhs) {
        subtractAndSetFlags(lhs, rhs); // subtract and discard result
    }
    
    template<bool FlatMemory>
    inline void CPU<FlatMemory>::compareIndirect(uint8_t lhs, uint16_t address) {
        auto rhs = read(address);
        compare(lhs, rhs);
    }
    
    template<bool FlatMemory>
    inline void CPU<FlatMemory>::decrement(Integer<8> auto& target) {
        using enum REGISTER_FLAG;
        bool carrySet = getCarry();
        target = subtractAndSetFlags(target, 1);
        (carrySet) ? F.set(C) : F.clear(C); // ensure C remains unmodified
    }
    
    template<bool FlatMemory>
    inline void CPU<FlatMemory>::decrement(Register<16> auto& target) {
        target--;
    }
    
    template<bool FlatMemory>
    inline void CPU<FlatMemory>::decrementIndirect(uint16_t address) {
        auto value = read(address);
        decrement(value);
        write(address, value);
    }
    
    template<bool FlatMemory>
    inline void CPU<FlatMemory>::increment(Integer<8> auto& target) {
        using enum REGISTER_FLAG;
        bool carrySet = getCarry();
        target = addAndSetFlags(target, 1);
        (carrySet) ? F.set(C) : F.clear(C); // ensure C remains unmodified
    }
    
    template<bool FlatMemory>
    inline void CPU<FlatMemory>::increment(Register<16> auto& target) {
        target++;
    }
    
    template<bool FlatMemory>
    inline void CPU<FlatMemory>::incrementIndirect(uint16_t address) {
        auto value = read(address);
        increment(value);
        write(address, value);
    }
    
    template<bool FlatMemory>
    inline void CPU<FlatMemory>::bitAnd(Register<8> auto& lhs, uint8_t rhs) {
        using enum REGISTER_FLAG;
        lhs &= rhs;
        setZero(lhs);
        F.clear(N);
        F.set(H);
        F.clear(C);
    }
    
    template<bool FlatMemory>
    inline void CPU<FlatMemory>::bitAndIndirect(Register<8> auto& lhs, uint16_t address) {
        auto rhs = read(address);
        bitAnd(lhs, rhs);
    }
    
    template<bool FlatMemory>
    inline void CPU<FlatMemory>::bitNot(Register<8> auto& target) {
        using enum REGISTER_FLAG;
        target = ~target;
        F.set(N);
        F.set(H);
    }
    
    template<bool FlatMemory>
    inline void CPU<FlatMemory>::bitOr(Register<8> auto& lhs, uint8_t rhs) {
        using enum REGISTER_FLAG;
        lhs |= rhs;
        setZero(lhs);
        F.clear(N);
        F.clear(H);
        F.clear(C);
    }
    
    template<bool FlatMemory>
    inline void CPU<FlatMemory>::bitOrIndirect(Register<8> auto& lhs, uint16_t address) {
        auto rhs = read(address);
        bitOr(lhs, rhs);
    }
    
    template<bool FlatMemory>
    inline void CPU<FlatMemory>::bitXor(Register<8> auto& lhs, uint8_t rhs) {
        using enum REGISTER_FLAG;
        lhs ^= rhs;
        setZero(lhs);
        F.clear(N);
        F.clear(H);
        F.clear(C);
    }
    
    template<bool FlatMemory>
    inline void CPU<FlatMemory>::bitXorIndirect(Register<8> auto& lhs, uint16_t address) {
        auto rhs = read(address);
        bitXor(lhs, rhs);
    }
    
    template<bool FlatMemory>
    inline void CPU<FlatMemory>::bitTest(uint8_t bitIndex, uint8_t target) {
        using enum REGISTER_FLAG;
        uint8_t test = 1 << bitIndex;
        (test & target) ? F.clear(Z) : F.set(Z);
        F.clear(N);
        F.set(H);
    }
    
    template<bool FlatMemory>
    inline void CPU<FlatMemory>::bitTestIndirect(uint8_t bitIndex, uint16_t address) {
        auto target = read(address);
        bitTest(bitIndex, target);
    }
    
    template<bool FlatMemory>
    inline void CPU<FlatMemory>::bitReset(uint8_t bitIndex, Integer<8> auto& target) {
        uint8_t test = 1 << bitIndex;
        target &= ~test; // set bit "test" to 0 in target
    }
    
    template<bool FlatMemory>
    inline void CPU<FlatMemory>::bitResetIndirect(uint8_t bitIndex, uint16_t address) {
        auto value = read(address);
        bitReset(bitIndex, value);
        write(address, value);
    }
    
    template<bool FlatMemory>
    inline void CPU<FlatMemory>::bitSet(uint8_t bitIndex, Integer<8> auto& target) {
        uint8_t test = 1 << bitIndex;
        target |= test; // set bit "test" to 1 in target
    }
    
    template<bool FlatMemory>
    inline void CPU<FlatMemory>::bitSetIndirect(uint8_t bitIndex, uint16_t address) {
        auto value = read(address);
        bitSet(bitIndex, value);
        write(address, value);
    }
    
    template<bool FlatMemory>
    inline void CPU<FlatMemory>::rotateLeft(Integer<8> auto& target) {
        using enum REGISTER_FLAG;
        uint8_t carry = getCarry(); // carry becomes new lsb
        bool msb = target & 0x80;
        target = (target << 1) | carry; // rotate through carry
    
        setZero(target);
        F.clear(N);
        F.clear(H);
        (msb) ? F.set(C) : F.clear(C); // carry = former msb
    }
    
    template<bool FlatMemory>
    inline void CPU<FlatMemory>::rotateLeftIndirect(uint16_t address) {
        auto value = read(address);
        rotateLeft(value);
        write(address, value);
    }
    
    template<bool FlatMemory>
    inline void CPU<FlatMemory>::rotateLeftCircular(Integer<8> auto& target) {
        using enum REGISTER_FLAG;
        uint8_t msb = (target & 0x80) >> 7; // msb becomes new lsb
        target = (target << 1) | msb; 
    
        setZero(target);
        F.clear(N);
        F.clear(H);
        (msb) ? F.set(C) : F.clear(C); // carry = former msb
    }
    
    template<bool FlatMemory>
    inline void CPU<FlatMemory>::rotateLeftCircularIndirect(uint16_t address) {
        auto value = read(address);
        rotateLeftCircular(value);
        write(address, value);
    }
    
    template<bool FlatMemory>
    inline void CPU<FlatMemory>::rotateRight(Integer<8> auto& target) {
        using enum REGISTER_FLAG;
        uint8_t carry = getCarry() << 7; // carry becomes new msb
        bool lsb = target & 0x01;
        target = (target >> 1) | carry; // rotate through carry
    
        setZero(target);
        F.clear(N);
        F.clear(H);
        (lsb) ? F.set(C) : F.clear(C); // carry = former lsb
    }
    
    template<bool FlatMemory>
    inline void CPU<FlatMemory>::rotateRightIndirect(uint16_t address) {
        auto value = read(address);
        rotateRight(value);
        write(address, value);
    }
    
    template<bool FlatMemory>
    inline void CPU<FlatMemory>::rotateRightCircular(Integer<8> auto& target) {
        using enum REGISTER_FLAG;
        uint8_t lsb = (target & 0x01) << 7; // lsb becomes new msb
        target = (target >> 1) | lsb;
    
        setZero(target);
        F.clear(N);
        F.clear(H);
        (lsb) ? F.set(C) : F.clear(C); // carry = former lsb
    }
    
    template<bool FlatMemory>
    inline void CPU<FlatMemory>::rotateRightCircularIndirect(uint16_t address) {
        auto value = read(address);
        rotateRightCircular(value);
        write(address, value);
    }
    
    template<bool FlatMemory>
    inline void CPU<FlatMemory>::shiftLeftArithmetic(Integer<8> auto& target) {
        using enum REGISTER_FLAG;
        bool msb = target & 0x80;
        target <<= 1;
        setZero(target);
        F.clear(N);
        F.clear(H);
        (msb) ? F.set(C) : F.clear(C);
    }
    
    template<bool FlatMemory>
    inline void CPU<FlatMemory>::shiftLeftArithmeticIndirect(uint16_t address) {
        auto value = read(address);
        shiftLeftArithmetic(value);
        write(address, value);
    }
    
    template<bool FlatMemory>
    inline void CPU<FlatMemory>::shiftRightArithmetic(Integer<8> auto& target) {
        using enum REGISTER_FLAG;
        bool lsb = target & 0x01;
        uint8_t msb = target & 0x80;
        target = (target >> 1) | msb; // preserve msb
        setZero(target);
        F.clear(N);
        F.clear(H);
        (lsb) ? F.set(C) : F.clear(C);
    }
    
    template<bool FlatMemory>
    inline void CPU<FlatMemory>::shiftRightArithmeticIndirect(uint16_t address) {
        auto value = read(address);
        shiftRightArithmetic(value);
        write(address, value);
    }
    
    template<bool FlatMemory>
    inline void CPU<FlatMemory>::shiftRightLogical(Integer<8> auto& target) {
        using enum REGISTER_FLAG;
        bool lsb = target & 0x01;
        target >>= 1;
        setZero(target);
        F.clear(N);
        F.clear(H);
        (lsb) ? F.set(C) : F.clear(C);
    }
    
    template<bool FlatMemory>
    inline void CPU<FlatMemory>::shiftRightLogicalIndirect(uint16_t address) {
        auto value = read(address);
        shiftRightLogical(value);
        write(address, value);
    }
    
    template<bool FlatMemory>
    inline void CPU<FlatMemory>::swap(Integer<8> auto& target) {
        using enum REGISTER_FLAG;
        target = (target << 4) | (target >> 4);
        setZero(target);
        F.clear(N);
        F.clear(H);
        F.clear(C);
    }
    
    template<bool FlatMemory>
    inline void CPU<FlatMemory>::swapIndirect(uint16_t address) {
        auto value = read(address);
        swap(value);
        write(address, value);
    }
    
    template<bool FlatMemory>
    inline void CPU<FlatMemory>::pop(Register<16> auto& target) {
        target.setLo(read(SP++));
        target.setHi(read(SP++));
    }
    
    template<bool FlatMemory>
    inline void CPU<FlatMemory>::push(Register<16> auto& target) {
        write(--SP, target.hi());
        write(--SP, target.lo());
    }
    
    template<bool FlatMemory>
    inline void CPU<FlatMemory>::call(uint16_t address) {
        push(PC);
        jump(address);
    }
    
    template<bool FlatMemory>
    inline void CPU<FlatMemory>::jump(uint16_t address) {
        PC = address;
    }
    
    template<bool FlatMemory>
    inline void CPU<FlatMemory>::jumpRelative(int8_t offset) {
        PC += offset;
    }
    
    template<bool FlatMemory>
    inline void CPU<FlatMemory>::ret() {
        pop(PC);
    }
    
    template<bool FlatMemory>
    inline void CPU<FlatMemory>::reti() {
        ret();
        IME = INTERRUPT_MASTER_FLAG::ENABLED; // skip to this state since ret() counts as next instruction
    }
    
    template<bool FlatMemory>
    inline void CPU<FlatMemory>::restart(uint8_t address) {
        call(address);
    }
    
    template<bool FlatMemory>
    inline void CPU<FlatMemory>::complementCarryFlag() {
        using enum REGISTER_FLAG;
        F.clear(N);
        F.clear(H);
        (F.test(C)) ? F.clear(C) : F.set(C);
    }
    
    template<bool FlatMemory>
    inline void CPU<FlatMemory>::setCarryFlag() {
        using enum REGISTER_FLAG;
        F.clear(N);
        F.clear(H);
        F.set(C);
    }
    
    template<bool FlatMemory>
    inline void CPU<FlatMemory>::disableInterrupts() {
        IME = INTERRUPT_MASTER_FLAG::DISABLED;
    }
    
    template<bool FlatMemory>
    inline void CPU<FlatMemory>::enableInterrupts() {
        if (IME != INTERRUPT_MASTER_FLAG::ENABLED) {
            IME = INTERRUPT_MASTER_FLAG::ENABLE_PENDING; // signal to enable interrupts after next machine cycle
        }
    }
    
    template<bool FlatMemory>
    inline void CPU<FlatMemory>::halt() {
        auto IF = read(Memory::IF);
        auto IE = read(Memory::IE);
        if (IME == INTERRUPT_MASTER_FLAG::ENABLED || !(IF & IE & 0x1F)) {
            state = STATE::HALTED;
        }
        else { // IME = 0 and some interrupt is pending => halt bug
            state = STATE::BUGGED;
        }
    }
    
    template<bool FlatMemory>
    inline void CPU<FlatMemory>::decimalAdjustAccumulator() {
        using enum REGISTER_FLAG;
        uint8_t adjustment = 0;
        if (F.test(N)) {
            if (F.test(H)) {
                adjustment += 0x06;
            }
            if (F.test(C)) {
                adjustment += 0x60;
            }
            A -= adjustment;
        }
        else {
            if (F.test(H) || ((A & 0x0F) > 0x09)) {
                adjustment += 0x06;
            }
            if (F.test(C) || (A > 0x99)) {
                adjustment += 0x60;
                F.set(C);
            }
            A += adjustment;
        }
        setZero(A);
        F.clear(H);
    }
    
    template<bool FlatMemory>
    inline void CPU<FlatMemory>::stop() {
        // this one is kind of ridiculous (and unused), just interpret as noop
        // maybe will implement in the future for completeness
    }

}