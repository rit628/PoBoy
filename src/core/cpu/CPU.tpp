#pragma once
#include "CPU.hpp"
#include "MemoryConstants.hpp"
#include <cstddef>
#include <cstdint>
#include <utility>

namespace Processing {

    template<typename BusType>
    template<bool Tick>
    inline uint8_t CPU<BusType>::read(uint16_t address) {
        auto result = bus.read(address);
        if constexpr (Tick) systemTick();
        return result;
    }

    template<typename BusType>
    template<bool Tick>
    inline void CPU<BusType>::write(uint16_t address, uint8_t value) {
        bus.write(address, value);
        if constexpr (Tick) systemTick();
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
        (result == 0) ? F.set(Z) : F.clear(Z);
    }
    
    template<typename BusType>
    inline uint8_t CPU<BusType>::addAndSetFlags(uint8_t a, uint8_t b, uint8_t carry) {
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
    
    template<typename BusType>
    inline uint16_t CPU<BusType>::addAndSetFlags(uint16_t a, uint16_t b) {
        using enum REGISTER_FLAG;
        uint16_t result = a + b;
    
        F.clear(N);
    
        // set half carry (H): mask upper 4 bits and check if exceeds lower 12
        (((a & 0x0FFF) + (b & 0x0FFF)) > 0x0FFF) ? F.set(H) : F.clear(H);
    
        // set carry (C): check if greater than uint16_t max
        ((static_cast<uint32_t>(a) + static_cast<uint32_t>(b)) > 0xFFFF) ? F.set(C) : F.clear(C);
        
        return result;
    }
    
    template<typename BusType>
    inline uint16_t CPU<BusType>::addAndSetFlags(uint16_t a, int8_t b) {
        // set carry and half carry based on 8 bit unsigned addition
        addAndSetFlags(static_cast<uint8_t>(a & 0x00FF), std::bit_cast<uint8_t, int8_t>(b));
        
        // clear Z and N flags unconditionally
        using enum REGISTER_FLAG;
        F.clear(Z);
        F.clear(N);
    
        return a + b;
    }
    
    template<typename BusType>
    inline uint8_t CPU<BusType>::subtractAndSetFlags(uint8_t a, uint8_t b, uint8_t carry) {
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
    
    template<typename BusType>
    template<size_t N>
    inline void CPU<BusType>::load(Register<N> auto& target, RegisterValue<N> value) {
        target = value;
    }
    
    template<typename BusType>
    inline void CPU<BusType>::loadIndirect(uint16_t address, uint8_t value) {
        write(address, value);
    }
    
    template<typename BusType>
    inline void CPU<BusType>::loadIndirect(uint16_t address, uint16_t value) {
        write(address, value & 0x00FF);
        write(address + 1, (value & 0xFF00) >> 8);
    }
    
    template<typename BusType>
    inline void CPU<BusType>::loadIndirect(Register<8> auto& target, uint16_t address) {
        target = read(address);
    }
    
    template<typename BusType>
    inline void CPU<BusType>::loadHiIndirect(Register<8> auto& target, uint8_t address) {
        target = read(static_cast<uint16_t>(address) | 0xFF00);
    }
    
    template<typename BusType>
    inline void CPU<BusType>::loadHiIndirect(uint16_t address, uint8_t value) {
        write(static_cast<uint16_t>(address) | 0xFF00, value);
    }
    
    template<typename BusType>
    inline void CPU<BusType>::loadIncrement(Register<8> auto& target, Register<16> auto& address) {
        target = read(address++);
    }
    
    template<typename BusType>
    inline void CPU<BusType>::loadIncrement(Register<16> auto& address, uint8_t value) {
        write(address++, value);
    }
    
    template<typename BusType>
    inline void CPU<BusType>::loadDecrement(Register<8> auto& target, Register<16> auto& address) {
        target = read(address--);
    }
    
    template<typename BusType>
    inline void CPU<BusType>::loadDecrement(Register<16> auto& address, uint8_t value) {
        write(address--, value);
    }
    
    template<typename BusType>
    inline void CPU<BusType>::loadAdjusted(Register<16> auto& target, Register<16> auto& value, int8_t adjust) {
        auto adjustedValue = addAndSetFlags(value, adjust);
        target = adjustedValue;
        systemTick();   // extra cycle for opcode 0xF8
    }
    
    template<typename BusType>
    template<size_t N>
    inline void CPU<BusType>::add(Register<N> auto& target, RegisterValue<N> value) {
        target = addAndSetFlags(target, value);
        if constexpr (N == 16) systemTick();    // 16 bit add takes extra cycle
    }
    
    template<typename BusType>
    inline void CPU<BusType>::addIndirect(Register<8> auto& target, uint16_t address) {
        auto value = read(address);
        add<8>(target, value);
    }
    
    template<typename BusType>
    inline void CPU<BusType>::adc(Register<8> auto& target, uint8_t value) {
        auto carry = getCarry();
        target = addAndSetFlags(target, value, carry);
    }
    
    template<typename BusType>
    inline void CPU<BusType>::adcIndirect(Register<8> auto& target, uint16_t address) {
        auto value = read(address);
        adc(target, value);
    }
    
    template<typename BusType>
    inline void CPU<BusType>::addRelative(Register<16> auto& target, int8_t value) {
        target = addAndSetFlags(target, value);
        /* relative add takes 2 extra cycles */
        systemTick();
        systemTick();
    }
    
    template<typename BusType>
    inline void CPU<BusType>::sub(Register<8> auto& target, uint8_t value) {
        target = subtractAndSetFlags(target, value);
    }
    
    template<typename BusType>
    inline void CPU<BusType>::subIndirect(Register<8> auto& target, uint16_t address) {
        auto value = read(address);
        sub(target, value);
    }
    
    template<typename BusType>
    inline void CPU<BusType>::sbc(Register<8> auto& target, uint8_t value) {
        auto carry = getCarry();
        target = subtractAndSetFlags(target, value, carry);
    }
    
    template<typename BusType>
    inline void CPU<BusType>::sbcIndirect(Register<8> auto& target, uint16_t address) {
        auto value = read(address);
        sbc(target, value);
    }
    
    template<typename BusType>
    inline void CPU<BusType>::compare(uint8_t lhs, uint8_t rhs) {
        subtractAndSetFlags(lhs, rhs); // subtract and discard result
    }
    
    template<typename BusType>
    inline void CPU<BusType>::compareIndirect(uint8_t lhs, uint16_t address) {
        auto rhs = read(address);
        compare(lhs, rhs);
    }
    
    template<typename BusType>
    inline void CPU<BusType>::decrement(Integer<8> auto& target) {
        using enum REGISTER_FLAG;
        bool carrySet = getCarry();
        target = subtractAndSetFlags(target, 1);
        (carrySet) ? F.set(C) : F.clear(C); // ensure C remains unmodified
    }
    
    template<typename BusType>
    inline void CPU<BusType>::decrement(Register<16> auto& target) {
        target--;
        systemTick();   // 16 bit dec takes extra cycle
    }
    
    template<typename BusType>
    inline void CPU<BusType>::decrementIndirect(uint16_t address) {
        auto value = read(address);
        decrement(value);
        write(address, value);
    }
    
    template<typename BusType>
    inline void CPU<BusType>::increment(Integer<8> auto& target) {
        using enum REGISTER_FLAG;
        bool carrySet = getCarry();
        target = addAndSetFlags(target, 1);
        (carrySet) ? F.set(C) : F.clear(C); // ensure C remains unmodified
    }
    
    template<typename BusType>
    inline void CPU<BusType>::increment(Register<16> auto& target) {
        target++;
        systemTick();   // 16 bit inc takes extra cycle
    }
    
    template<typename BusType>
    inline void CPU<BusType>::incrementIndirect(uint16_t address) {
        auto value = read(address);
        increment(value);
        write(address, value);
    }
    
    template<typename BusType>
    inline void CPU<BusType>::bitAnd(Register<8> auto& lhs, uint8_t rhs) {
        using enum REGISTER_FLAG;
        lhs &= rhs;
        setZero(lhs);
        F.clear(N);
        F.set(H);
        F.clear(C);
    }
    
    template<typename BusType>
    inline void CPU<BusType>::bitAndIndirect(Register<8> auto& lhs, uint16_t address) {
        auto rhs = read(address);
        bitAnd(lhs, rhs);
    }
    
    template<typename BusType>
    inline void CPU<BusType>::bitNot(Register<8> auto& target) {
        using enum REGISTER_FLAG;
        target = ~target;
        F.set(N);
        F.set(H);
    }
    
    template<typename BusType>
    inline void CPU<BusType>::bitOr(Register<8> auto& lhs, uint8_t rhs) {
        using enum REGISTER_FLAG;
        lhs |= rhs;
        setZero(lhs);
        F.clear(N);
        F.clear(H);
        F.clear(C);
    }
    
    template<typename BusType>
    inline void CPU<BusType>::bitOrIndirect(Register<8> auto& lhs, uint16_t address) {
        auto rhs = read(address);
        bitOr(lhs, rhs);
    }
    
    template<typename BusType>
    inline void CPU<BusType>::bitXor(Register<8> auto& lhs, uint8_t rhs) {
        using enum REGISTER_FLAG;
        lhs ^= rhs;
        setZero(lhs);
        F.clear(N);
        F.clear(H);
        F.clear(C);
    }
    
    template<typename BusType>
    inline void CPU<BusType>::bitXorIndirect(Register<8> auto& lhs, uint16_t address) {
        auto rhs = read(address);
        bitXor(lhs, rhs);
    }
    
    template<typename BusType>
    inline void CPU<BusType>::bitTest(uint8_t bitIndex, uint8_t target) {
        using enum REGISTER_FLAG;
        uint8_t test = 1 << bitIndex;
        (test & target) ? F.clear(Z) : F.set(Z);
        F.clear(N);
        F.set(H);
    }
    
    template<typename BusType>
    inline void CPU<BusType>::bitTestIndirect(uint8_t bitIndex, uint16_t address) {
        auto target = read(address);
        bitTest(bitIndex, target);
    }
    
    template<typename BusType>
    inline void CPU<BusType>::bitReset(uint8_t bitIndex, Integer<8> auto& target) {
        uint8_t test = 1 << bitIndex;
        target &= ~test; // set bit "test" to 0 in target
    }
    
    template<typename BusType>
    inline void CPU<BusType>::bitResetIndirect(uint8_t bitIndex, uint16_t address) {
        auto value = read(address);
        bitReset(bitIndex, value);
        write(address, value);
    }
    
    template<typename BusType>
    inline void CPU<BusType>::bitSet(uint8_t bitIndex, Integer<8> auto& target) {
        uint8_t test = 1 << bitIndex;
        target |= test; // set bit "test" to 1 in target
    }
    
    template<typename BusType>
    inline void CPU<BusType>::bitSetIndirect(uint8_t bitIndex, uint16_t address) {
        auto value = read(address);
        bitSet(bitIndex, value);
        write(address, value);
    }
    
    template<typename BusType>
    inline void CPU<BusType>::rotateLeft(Integer<8> auto& target) {
        using enum REGISTER_FLAG;
        uint8_t carry = getCarry(); // carry becomes new lsb
        bool msb = target & 0x80;
        target = (target << 1) | carry; // rotate through carry
    
        setZero(target);
        F.clear(N);
        F.clear(H);
        (msb) ? F.set(C) : F.clear(C); // carry = former msb
    }
    
    template<typename BusType>
    inline void CPU<BusType>::rotateLeftIndirect(uint16_t address) {
        auto value = read(address);
        rotateLeft(value);
        write(address, value);
    }
    
    template<typename BusType>
    inline void CPU<BusType>::rotateLeftCircular(Integer<8> auto& target) {
        using enum REGISTER_FLAG;
        uint8_t msb = (target & 0x80) >> 7; // msb becomes new lsb
        target = (target << 1) | msb; 
    
        setZero(target);
        F.clear(N);
        F.clear(H);
        (msb) ? F.set(C) : F.clear(C); // carry = former msb
    }
    
    template<typename BusType>
    inline void CPU<BusType>::rotateLeftCircularIndirect(uint16_t address) {
        auto value = read(address);
        rotateLeftCircular(value);
        write(address, value);
    }
    
    template<typename BusType>
    inline void CPU<BusType>::rotateRight(Integer<8> auto& target) {
        using enum REGISTER_FLAG;
        uint8_t carry = getCarry() << 7; // carry becomes new msb
        bool lsb = target & 0x01;
        target = (target >> 1) | carry; // rotate through carry
    
        setZero(target);
        F.clear(N);
        F.clear(H);
        (lsb) ? F.set(C) : F.clear(C); // carry = former lsb
    }
    
    template<typename BusType>
    inline void CPU<BusType>::rotateRightIndirect(uint16_t address) {
        auto value = read(address);
        rotateRight(value);
        write(address, value);
    }
    
    template<typename BusType>
    inline void CPU<BusType>::rotateRightCircular(Integer<8> auto& target) {
        using enum REGISTER_FLAG;
        uint8_t lsb = (target & 0x01) << 7; // lsb becomes new msb
        target = (target >> 1) | lsb;
    
        setZero(target);
        F.clear(N);
        F.clear(H);
        (lsb) ? F.set(C) : F.clear(C); // carry = former lsb
    }
    
    template<typename BusType>
    inline void CPU<BusType>::rotateRightCircularIndirect(uint16_t address) {
        auto value = read(address);
        rotateRightCircular(value);
        write(address, value);
    }
    
    template<typename BusType>
    inline void CPU<BusType>::shiftLeftArithmetic(Integer<8> auto& target) {
        using enum REGISTER_FLAG;
        bool msb = target & 0x80;
        target <<= 1;
        setZero(target);
        F.clear(N);
        F.clear(H);
        (msb) ? F.set(C) : F.clear(C);
    }
    
    template<typename BusType>
    inline void CPU<BusType>::shiftLeftArithmeticIndirect(uint16_t address) {
        auto value = read(address);
        shiftLeftArithmetic(value);
        write(address, value);
    }
    
    template<typename BusType>
    inline void CPU<BusType>::shiftRightArithmetic(Integer<8> auto& target) {
        using enum REGISTER_FLAG;
        bool lsb = target & 0x01;
        uint8_t msb = target & 0x80;
        target = (target >> 1) | msb; // preserve msb
        setZero(target);
        F.clear(N);
        F.clear(H);
        (lsb) ? F.set(C) : F.clear(C);
    }
    
    template<typename BusType>
    inline void CPU<BusType>::shiftRightArithmeticIndirect(uint16_t address) {
        auto value = read(address);
        shiftRightArithmetic(value);
        write(address, value);
    }
    
    template<typename BusType>
    inline void CPU<BusType>::shiftRightLogical(Integer<8> auto& target) {
        using enum REGISTER_FLAG;
        bool lsb = target & 0x01;
        target >>= 1;
        setZero(target);
        F.clear(N);
        F.clear(H);
        (lsb) ? F.set(C) : F.clear(C);
    }
    
    template<typename BusType>
    inline void CPU<BusType>::shiftRightLogicalIndirect(uint16_t address) {
        auto value = read(address);
        shiftRightLogical(value);
        write(address, value);
    }
    
    template<typename BusType>
    inline void CPU<BusType>::swap(Integer<8> auto& target) {
        using enum REGISTER_FLAG;
        target = (target << 4) | (target >> 4);
        setZero(target);
        F.clear(N);
        F.clear(H);
        F.clear(C);
    }
    
    template<typename BusType>
    inline void CPU<BusType>::swapIndirect(uint16_t address) {
        auto value = read(address);
        swap(value);
        write(address, value);
    }
    
    template<typename BusType>
    inline void CPU<BusType>::pop(Register<16> auto& target) {
        target.setLo(read(SP++));
        target.setHi(read(SP++));
    }
    
    template<typename BusType>
    inline void CPU<BusType>::push(Register<16> auto& target) {
        systemTick();   // match delay from pipelined decrement
        write(--SP, target.hi());
        write(--SP, target.lo());
    }
    
    template<typename BusType>
    template<REGISTER_FLAG Flag, bool N>
    inline bool CPU<BusType>::testCondition() {
        bool condition = F.test(Flag);
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
        systemTick();
    }

    template<typename BusType>
    template<REGISTER_FLAG Flag, bool N>
    inline void CPU<BusType>::jump(uint16_t address) {
        if (testCondition<Flag, N>()) jump(address);
    }
    
    template<typename BusType>
    inline void CPU<BusType>::jumpRelative(int8_t offset) {
        PC += offset;
        systemTick();
    }

    template<typename BusType>
    template<REGISTER_FLAG Flag, bool N>
    inline void CPU<BusType>::jumpRelative(int8_t offset) {
        if (testCondition<Flag, N>()) jumpRelative(offset);
    }
    
    template<typename BusType>
    inline void CPU<BusType>::ret() {
        pop(PC);
        systemTick();
    }

    template<typename BusType>
    template<REGISTER_FLAG Flag, bool N>
    inline void CPU<BusType>::ret() {
        if (testCondition<Flag, N>()) ret();
        systemTick();
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
        F.clear(N);
        F.clear(H);
        (F.test(C)) ? F.clear(C) : F.set(C);
    }
    
    template<typename BusType>
    inline void CPU<BusType>::setCarryFlag() {
        using enum REGISTER_FLAG;
        F.clear(N);
        F.clear(H);
        F.set(C);
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
    
    template<typename BusType>
    inline void CPU<BusType>::stop() {
        // this one is kind of ridiculous (and unused), just interpret as noop
        // maybe will implement in the future for completeness
    }

}