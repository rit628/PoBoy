#pragma once
#include "Bus.hpp"
#include "CPU.hpp"
#include "MemoryConstants.hpp"
#include "SystemConstants.hpp"
#include <bit>
#include <cstdint>
#include <utility>

namespace Processing {

    template<typename BusType>
    inline void CPU<BusType>::setFlag(REGISTER_FLAG bitFlag) {
        setFlags(F, bitFlag);
    }

    template<typename BusType>
    inline void CPU<BusType>::clearFlag(REGISTER_FLAG bitFlag) {
        clearFlags(F, bitFlag);
    }

    template<typename BusType>
    inline bool CPU<BusType>::testFlag(REGISTER_FLAG bitFlag) {
        return testFlags(F, bitFlag);
    }

    template<typename BusType>
    template<bool Tick>
    inline uint8_t CPU<BusType>::read(uint16_t address) {
        auto result = bus.read(address);
        if constexpr (Tick) bus.tick();
        return result;
    }

    template<typename BusType>
    template<bool Tick>
    inline void CPU<BusType>::write(uint16_t address, uint8_t value) {
        bus.write(address, value);
        if constexpr (Tick) bus.tick();
    }

    template<typename BusType>
    inline uint8_t CPU<BusType>::read8() {
        return read(PC++);
    }

    template<typename BusType>
    inline uint16_t CPU<BusType>::read16() {
        uint8_t lo = read8();
        uint8_t hi = read8();
        return hi << 8 | lo;
    }

    template<typename BusType>
    inline uint8_t CPU<BusType>::getCarry() {
        using enum REGISTER_FLAG;
        return (F & std::to_underlying(C)) >> 4; // 1 if C is set else 0
    }
    
    template<typename BusType>
    inline uint8_t CPU<BusType>::getHalfCarry() {
        using enum REGISTER_FLAG;
        return (F & std::to_underlying(H)) >> 5; // 1 if H is set else 0
    }
    
    template<typename BusType>
    inline void CPU<BusType>::setZero(uint8_t result) {
        using enum REGISTER_FLAG;
        (result == 0) ? setFlag(Z) : clearFlag(Z);
    }
    
    template<typename BusType>
    inline uint8_t CPU<BusType>::addAndSetFlags(uint8_t a, uint8_t b, uint8_t carry) {
        using enum REGISTER_FLAG;
        uint8_t result = a + b + carry;
    
        setZero(result);
        clearFlag(N);
    
        // set half carry (H): mask upper 4 bits and check if exceeds lower 4
        (((a & 0x0F) + (b & 0x0F) + carry) > 0x0F) ? setFlag(H) : clearFlag(H);
    
        // set carry (C): check if greater than uint8_t max
        ((static_cast<uint16_t>(a) + static_cast<uint16_t>(b) + carry) > 0xFF) ? setFlag(C) : clearFlag(C);
        
        return result;
    }
    
    template<typename BusType>
    inline uint16_t CPU<BusType>::addAndSetFlags(uint16_t a, uint16_t b) {
        using enum REGISTER_FLAG;
        uint16_t result = a + b;
    
        clearFlag(N);
    
        // set half carry (H): mask upper 4 bits and check if exceeds lower 12
        (((a & 0x0FFF) + (b & 0x0FFF)) > 0x0FFF) ? setFlag(H) : clearFlag(H);
    
        // set carry (C): check if greater than uint16_t max
        ((static_cast<uint32_t>(a) + static_cast<uint32_t>(b)) > 0xFFFF) ? setFlag(C) : clearFlag(C);
        
        return result;
    }
    
    template<typename BusType>
    inline uint16_t CPU<BusType>::addAndSetFlags(uint16_t a, int8_t b) {
        // set carry and half carry based on 8 bit unsigned addition
        addAndSetFlags(static_cast<uint8_t>(a & 0x00FF), std::bit_cast<uint8_t, int8_t>(b));
        
        // clear Z and N flags unconditionally
        using enum REGISTER_FLAG;
        clearFlag(Z);
        clearFlag(N);
    
        return a + b;
    }
    
    template<typename BusType>
    inline uint8_t CPU<BusType>::subtractAndSetFlags(uint8_t a, uint8_t b, uint8_t carry) {
        using enum REGISTER_FLAG;
        uint8_t result = static_cast<int16_t>(a) - static_cast<int16_t>(b) - carry;
    
        setZero(result);
        setFlag(N);
    
        // set half carry (H): mask upper 4 bits and check if negative (borrow from bit 4)
        ((static_cast<int8_t>(a & 0x0F) - static_cast<int8_t>(b & 0x0F) - carry) < 0) ? setFlag(H) : clearFlag(H);
    
        // set carry (C): check if result is negative (borrow required)
        ((static_cast<int16_t>(a) - static_cast<int16_t>(b) - carry) < 0) ? setFlag(C) : clearFlag(C);
    
        return result;
    }
    
    template<typename BusType>
    inline void CPU<BusType>::load(Register<8> auto& target, uint8_t value) {
        target = value;
    }

    template<typename BusType>
    inline void CPU<BusType>::load(Register<8> auto& target, uint16_t address) {
        target = read(address);
    }

    template<typename BusType>
    inline void CPU<BusType>::load(uint16_t address, uint8_t value) {
        write(address, value);
    }

    template<typename BusType>
    void CPU<BusType>::load16(Register<16> auto& target, uint16_t value) {
        target = value;
    }
    
    template<typename BusType>
    inline void CPU<BusType>::load16(uint16_t address, uint16_t value) {
        write(address, value & 0x00FF);
        write(address + 1, value >> 8);
    }
    
    template<typename BusType>
    inline void CPU<BusType>::loadHi(Register<8> auto& target, uint8_t address) {
        target = read(static_cast<uint16_t>(address) | 0xFF00);
    }
    
    template<typename BusType>
    inline void CPU<BusType>::loadHi(uint8_t address, uint8_t value) {
        write(static_cast<uint8_t>(address) | 0xFF00, value);
    }
    
    template<typename BusType>
    inline void CPU<BusType>::loadIncrement(Register<8> auto& target, Register<16> auto& indirectAddress) {
        target = read(indirectAddress++);
    }
    
    template<typename BusType>
    inline void CPU<BusType>::loadIncrement(Register<16> auto& indirectAddress, uint8_t value) {
        write(indirectAddress++, value);
    }
    
    template<typename BusType>
    inline void CPU<BusType>::loadDecrement(Register<8> auto& target, Register<16> auto& indirectAddress) {
        target = read(indirectAddress--);
    }
    
    template<typename BusType>
    inline void CPU<BusType>::loadDecrement(Register<16> auto& indirectAddress, uint8_t value) {
        write(indirectAddress--, value);
    }
    
    template<typename BusType>
    inline void CPU<BusType>::loadAdjusted(Register<16> auto& target, Register<16> auto& value, int8_t adjustment) {
        auto adjustedValue = addAndSetFlags(value, adjustment);
        target = adjustedValue;
        bus.tick();   // extra cycle for opcode 0xF8
    }
    
    template<typename BusType>
    inline void CPU<BusType>::add(Register<8> auto& target, uint8_t value) {
        target = addAndSetFlags(target, value);
    }
    
    template<typename BusType>
    inline void CPU<BusType>::add(Register<8> auto& target, Register<16> auto& indirectAddress) {
        auto value = read(indirectAddress);
        add(target, value);
    }
    
    template<typename BusType>
    inline void CPU<BusType>::adc(Register<8> auto& target, uint8_t value) {
        auto carry = getCarry();
        target = addAndSetFlags(target, value, carry);
    }
    
    template<typename BusType>
    inline void CPU<BusType>::adc(Register<8> auto& target, Register<16> auto& indirectAddress) {
        auto value = read(indirectAddress);
        adc(target, value);
    }

    template<typename BusType>
    inline void CPU<BusType>::add16(Register<16> auto& target, uint16_t value) {
        target = addAndSetFlags(target, value);
        bus.tick();    // 16 bit add takes extra cycle
    }
    
    template<typename BusType>
    inline void CPU<BusType>::addRelative(Register<16> auto& target, int8_t value) {
        target = addAndSetFlags(target, value);
        /* relative add takes 2 extra cycles */
        bus.tick();
        bus.tick();
    }
    
    template<typename BusType>
    inline void CPU<BusType>::sub(Register<8> auto& target, uint8_t value) {
        target = subtractAndSetFlags(target, value);
    }
    
    template<typename BusType>
    inline void CPU<BusType>::sub(Register<8> auto& target, Register<16> auto& indirectAddress) {
        auto value = read(indirectAddress);
        sub(target, value);
    }
    
    template<typename BusType>
    inline void CPU<BusType>::sbc(Register<8> auto& target, uint8_t value) {
        auto carry = getCarry();
        target = subtractAndSetFlags(target, value, carry);
    }
    
    template<typename BusType>
    inline void CPU<BusType>::sbc(Register<8> auto& target, Register<16> auto& indirectAddress) {
        auto value = read(indirectAddress);
        sbc(target, value);
    }
    
    template<typename BusType>
    inline void CPU<BusType>::compare(uint8_t lhs, uint8_t rhs) {
        subtractAndSetFlags(lhs, rhs); // subtract and discard result
    }
    
    template<typename BusType>
    inline void CPU<BusType>::compare(uint8_t lhs, Register<16> auto& indirectAddress) {
        auto rhs = read(indirectAddress);
        compare(lhs, rhs);
    }
    
    template<typename BusType>
    inline void CPU<BusType>::decrement(Integer<8> auto& target) {
        using enum REGISTER_FLAG;
        bool carrySet = getCarry();
        target = subtractAndSetFlags(target, 1);
        (carrySet) ? setFlag(C) : clearFlag(C); // ensure C remains unmodified
    }

    template<typename BusType>
    inline void CPU<BusType>::decrement(Register<16> auto& indirectAddress) {
        auto value = read(indirectAddress);
        decrement(value);
        write(indirectAddress, value);
    }
    
    template<typename BusType>
    inline void CPU<BusType>::decrement16(Register<16> auto& target) {
        --target;
        bus.tick();   // 16 bit dec takes extra cycle
    }
    
    template<typename BusType>
    inline void CPU<BusType>::increment(Integer<8> auto& target) {
        using enum REGISTER_FLAG;
        bool carrySet = getCarry();
        target = addAndSetFlags(target, 1);
        (carrySet) ? setFlag(C) : clearFlag(C); // ensure C remains unmodified
    }

    template<typename BusType>
    inline void CPU<BusType>::increment(Register<16> auto& indirectAddress) {
        auto value = read(indirectAddress);
        increment(value);
        write(indirectAddress, value);
    }
    
    template<typename BusType>
    inline void CPU<BusType>::increment16(Register<16> auto& target) {
        ++target;
        bus.tick();   // 16 bit inc takes extra cycle
    }
    
    template<typename BusType>
    inline void CPU<BusType>::bitAnd(Register<8> auto& lhs, uint8_t rhs) {
        using enum REGISTER_FLAG;
        lhs &= rhs;
        setZero(lhs);
        clearFlag(N);
        setFlag(H);
        clearFlag(C);
    }
    
    template<typename BusType>
    inline void CPU<BusType>::bitAnd(Register<8> auto& lhs, Register<16> auto& indirectAddress) {
        auto rhs = read(indirectAddress);
        bitAnd(lhs, rhs);
    }
    
    template<typename BusType>
    inline void CPU<BusType>::bitNot(Register<8> auto& target) {
        using enum REGISTER_FLAG;
        target = ~target;
        setFlag(N);
        setFlag(H);
    }
    
    template<typename BusType>
    inline void CPU<BusType>::bitOr(Register<8> auto& lhs, uint8_t rhs) {
        using enum REGISTER_FLAG;
        lhs |= rhs;
        setZero(lhs);
        clearFlag(N);
        clearFlag(H);
        clearFlag(C);
    }
    
    template<typename BusType>
    inline void CPU<BusType>::bitOr(Register<8> auto& lhs, Register<16> auto& indirectAddress) {
        auto rhs = read(indirectAddress);
        bitOr(lhs, rhs);
    }
    
    template<typename BusType>
    inline void CPU<BusType>::bitXor(Register<8> auto& lhs, uint8_t rhs) {
        using enum REGISTER_FLAG;
        lhs ^= rhs;
        setZero(lhs);
        clearFlag(N);
        clearFlag(H);
        clearFlag(C);
    }
    
    template<typename BusType>
    inline void CPU<BusType>::bitXor(Register<8> auto& lhs, Register<16> auto& indirectAddress) {
        auto rhs = read(indirectAddress);
        bitXor(lhs, rhs);
    }
    
    template<typename BusType>
    inline void CPU<BusType>::bitTest(uint8_t bitIndex, uint8_t target) {
        using enum REGISTER_FLAG;
        uint8_t test = 1 << bitIndex;
        (test & target) ? clearFlag(Z) : setFlag(Z);
        clearFlag(N);
        setFlag(H);
    }
    
    template<typename BusType>
    inline void CPU<BusType>::bitTest(uint8_t bitIndex, Register<16> auto& indirectAddress) {
        auto target = read(indirectAddress);
        bitTest(bitIndex, target);
    }
    
    template<typename BusType>
    inline void CPU<BusType>::bitReset(uint8_t bitIndex, Integer<8> auto& target) {
        uint8_t test = 1 << bitIndex;
        target &= ~test; // set bit "test" to 0 in target
    }
    
    template<typename BusType>
    inline void CPU<BusType>::bitReset(uint8_t bitIndex, Register<16> auto& indirectAddress) {
        auto value = read(indirectAddress);
        bitReset(bitIndex, value);
        write(indirectAddress, value);
    }
    
    template<typename BusType>
    inline void CPU<BusType>::bitSet(uint8_t bitIndex, Integer<8> auto& target) {
        uint8_t test = 1 << bitIndex;
        target |= test; // set bit "test" to 1 in target
    }
    
    template<typename BusType>
    inline void CPU<BusType>::bitSet(uint8_t bitIndex, Register<16> auto& indirectAddress) {
        auto value = read(indirectAddress);
        bitSet(bitIndex, value);
        write(indirectAddress, value);
    }
    
    template<typename BusType>
    inline void CPU<BusType>::rotateLeft(Integer<8> auto& target) {
        using enum REGISTER_FLAG;
        bool msb = target & 0x80;
        uint8_t carry = getCarry(); // carry becomes new lsb
        target = (target << 1) | carry; // rotate through carry
    
        setZero(target);
        clearFlag(N);
        clearFlag(H);
        (msb) ? setFlag(C) : clearFlag(C); // carry = former msb
    }
    
    template<typename BusType>
    inline void CPU<BusType>::rotateLeft(Register<16> auto& indirectAddress) {
        auto value = read(indirectAddress);
        rotateLeft(value);
        write(indirectAddress, value);
    }
    
    template<typename BusType>
    inline void CPU<BusType>::rotateLeftCircular(Integer<8> auto& target) {
        using enum REGISTER_FLAG;
        target = std::rotl(static_cast<uint8_t>(target), 1);
    
        setZero(target);
        clearFlag(N);
        clearFlag(H);
        (target & 0x01) ? setFlag(C) : clearFlag(C); // carry = former msb
    }
    
    template<typename BusType>
    inline void CPU<BusType>::rotateLeftCircular(Register<16> auto& indirectAddress) {
        auto value = read(indirectAddress);
        rotateLeftCircular(value);
        write(indirectAddress, value);
    }
    
    template<typename BusType>
    inline void CPU<BusType>::rotateRight(Integer<8> auto& target) {
        using enum REGISTER_FLAG;
        bool lsb = target & 0x01;
        uint8_t carry = getCarry() << 7; // carry becomes new msb
        target = (target >> 1) | carry; // rotate through carry
    
        setZero(target);
        clearFlag(N);
        clearFlag(H);
        (lsb) ? setFlag(C) : clearFlag(C); // carry = former lsb
    }
    
    template<typename BusType>
    inline void CPU<BusType>::rotateRight(Register<16> auto& indirectAddress) {
        auto value = read(indirectAddress);
        rotateRight(value);
        write(indirectAddress, value);
    }
    
    template<typename BusType>
    inline void CPU<BusType>::rotateRightCircular(Integer<8> auto& target) {
        using enum REGISTER_FLAG;
        target = std::rotr(static_cast<uint8_t>(target), 1);
    
        setZero(target);
        clearFlag(N);
        clearFlag(H);
        (target & 0x80) ? setFlag(C) : clearFlag(C); // carry = former lsb
    }
    
    template<typename BusType>
    inline void CPU<BusType>::rotateRightCircular(Register<16> auto& indirectAddress) {
        auto value = read(indirectAddress);
        rotateRightCircular(value);
        write(indirectAddress, value);
    }
    
    template<typename BusType>
    inline void CPU<BusType>::shiftLeftArithmetic(Integer<8> auto& target) {
        using enum REGISTER_FLAG;
        bool msb = target & 0x80;
        target <<= 1;
        setZero(target);
        clearFlag(N);
        clearFlag(H);
        (msb) ? setFlag(C) : clearFlag(C);
    }
    
    template<typename BusType>
    inline void CPU<BusType>::shiftLeftArithmetic(Register<16> auto& indirectAddress) {
        auto value = read(indirectAddress);
        shiftLeftArithmetic(value);
        write(indirectAddress, value);
    }
    
    template<typename BusType>
    inline void CPU<BusType>::shiftRightArithmetic(Integer<8> auto& target) {
        using enum REGISTER_FLAG;
        bool lsb = target & 0x01;
        uint8_t msb = target & 0x80;
        target = (target >> 1) | msb; // preserve msb
        setZero(target);
        clearFlag(N);
        clearFlag(H);
        (lsb) ? setFlag(C) : clearFlag(C);
    }
    
    template<typename BusType>
    inline void CPU<BusType>::shiftRightArithmetic(Register<16> auto& indirectAddress) {
        auto value = read(indirectAddress);
        shiftRightArithmetic(value);
        write(indirectAddress, value);
    }
    
    template<typename BusType>
    inline void CPU<BusType>::shiftRightLogical(Integer<8> auto& target) {
        using enum REGISTER_FLAG;
        bool lsb = target & 0x01;
        target >>= 1;
        setZero(target);
        clearFlag(N);
        clearFlag(H);
        (lsb) ? setFlag(C) : clearFlag(C);
    }
    
    template<typename BusType>
    inline void CPU<BusType>::shiftRightLogical(Register<16> auto& indirectAddress) {
        auto value = read(indirectAddress);
        shiftRightLogical(value);
        write(indirectAddress, value);
    }
    
    template<typename BusType>
    inline void CPU<BusType>::swap(Integer<8> auto& target) {
        using enum REGISTER_FLAG;
        target = (target << 4) | (target >> 4);
        setZero(target);
        clearFlag(N);
        clearFlag(H);
        clearFlag(C);
    }
    
    template<typename BusType>
    inline void CPU<BusType>::swap(Register<16> auto& indirectAddress) {
        auto value = read(indirectAddress);
        swap(value);
        write(indirectAddress, value);
    }
    
    template<typename BusType>
    inline void CPU<BusType>::pop(Register<16> auto& target) {
        target.setLo(read(SP++));
        target.setHi(read(SP++));
    }
    
    template<typename BusType>
    inline void CPU<BusType>::push(Register<16> auto& target) {
        bus.tick();   // match delay from pipelined decrement
        write(--SP, target.getHi());
        write(--SP, target.getLo());
    }
    
    template<typename BusType>
    template<REGISTER_FLAG Flag, bool N>
    inline bool CPU<BusType>::testCondition() {
        bool condition = testFlag(Flag);
        if constexpr (N) condition = !condition;
        return condition;
    }

    template<typename BusType>
    inline void CPU<BusType>::call(uint16_t address) {
        push(PC);
        PC = address;
    }

    template<typename BusType>
    template<REGISTER_FLAG Flag, bool N>
    inline void CPU<BusType>::call(uint16_t address) {
        if (testCondition<Flag, N>()) call(address);
    }
    
    template<typename BusType>
    inline void CPU<BusType>::jump(uint16_t address) {
        PC = address;
        bus.tick();
    }

    template<typename BusType>
    template<REGISTER_FLAG Flag, bool N>
    inline void CPU<BusType>::jump(uint16_t address) {
        if (testCondition<Flag, N>()) jump(address);
    }
    
    template<typename BusType>
    inline void CPU<BusType>::jumpRelative(int8_t offset) {
        PC += offset;
        bus.tick();
    }

    template<typename BusType>
    template<REGISTER_FLAG Flag, bool N>
    inline void CPU<BusType>::jumpRelative(int8_t offset) {
        if (testCondition<Flag, N>()) jumpRelative(offset);
    }
    
    template<typename BusType>
    inline void CPU<BusType>::ret() {
        pop(PC);
        bus.tick();
    }

    template<typename BusType>
    template<REGISTER_FLAG Flag, bool N>
    inline void CPU<BusType>::ret() {
        if (testCondition<Flag, N>()) ret();
        bus.tick();
    }
    
    template<typename BusType>
    inline void CPU<BusType>::reti() {
        ret();
        IME = INTERRUPT_MASTER_FLAG::ENABLED; // skip to this state since ret() counts as next instruction
    }
    
    template<typename BusType>
    template<uint8_t Address>
    inline void CPU<BusType>::restart() {
        call(Address);
    }
    
    template<typename BusType>
    inline void CPU<BusType>::complementCarryFlag() {
        using enum REGISTER_FLAG;
        clearFlag(N);
        clearFlag(H);
        (testFlag(C)) ? clearFlag(C) : setFlag(C);
    }
    
    template<typename BusType>
    inline void CPU<BusType>::setCarryFlag() {
        using enum REGISTER_FLAG;
        clearFlag(N);
        clearFlag(H);
        setFlag(C);
    }
    
    template<typename BusType>
    inline void CPU<BusType>::disableInterrupts() {
        IME = INTERRUPT_MASTER_FLAG::DISABLED;
    }
    
    template<typename BusType>
    inline void CPU<BusType>::enableInterrupts() {
        if (IME != INTERRUPT_MASTER_FLAG::ENABLED) {
            IME = INTERRUPT_MASTER_FLAG::ENABLE_PENDING; // signal to enable interrupts after next machine cycle
        }
    }
    
    template<typename BusType>
    inline void CPU<BusType>::halt() {
        auto IF = read<false>(Memory::IF);
        auto IE = read<false>(Memory::IE);
        if (IME == INTERRUPT_MASTER_FLAG::ENABLED || !(IF & IE & 0x1F)) {
            state = STATE::HALTED;
        }
        else { // IME = 0 and some interrupt is pending => halt bug
            state = STATE::BUGGED;
        }
    }
    
    template<typename BusType>
    inline void CPU<BusType>::decimalAdjustAccumulator() {
        using enum REGISTER_FLAG;
        uint8_t adjustment = 0;
        if (testFlag(N)) {
            if (testFlag(H)) {
                adjustment += 0x06;
            }
            if (testFlag(C)) {
                adjustment += 0x60;
            }
            A -= adjustment;
        }
        else {
            if (testFlag(H) || ((A & 0x0F) > 0x09)) {
                adjustment += 0x06;
            }
            if (testFlag(C) || (A > 0x99)) {
                adjustment += 0x60;
                setFlag(C);
            }
            A += adjustment;
        }
        setZero(A);
        clearFlag(H);
    }
    
    template<typename BusType>
    inline void CPU<BusType>::stop() {
        /* skip all of the stalling stuff for now */
        if constexpr (std::is_same_v<BusType, Memory::Bus<MODEL::CGB>&>) {
            bool switchArmed = read<false>(Memory::KEY1) & 0x01;
            if (switchArmed) {
                bus.switchSpeed();
                write<false>(Memory::DIV, 0);
            }
        }
    }

}