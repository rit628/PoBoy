#pragma once
#include "CPU.hpp"
#include "Register.hpp"
#include <cstdint>

/* decoding based on table provided by https://izik1.github.io/gbops set to 8x width */

namespace Processing {

    template<typename BusType>
    template<uint8_t Index>
    consteval auto CPU<BusType>::decodeRegister() {
        if constexpr (Index == 00) return &CPU<BusType>::B;
        if constexpr (Index == 01) return &CPU<BusType>::C;
        if constexpr (Index == 02) return &CPU<BusType>::D;
        if constexpr (Index == 03) return &CPU<BusType>::E;
        if constexpr (Index == 04) return &CPU<BusType>::H;
        if constexpr (Index == 05) return &CPU<BusType>::L;
        if constexpr (Index == 06) return &CPU<BusType>::HL;
        if constexpr (Index == 07) return &CPU<BusType>::A;
    }

    template<typename BusType>
    template<uint8_t Index, bool AddressSP>
    consteval auto CPU<BusType>::decodeRegisterPair() {
        if constexpr (Index == 00) return &CPU<BusType>::BC;
        if constexpr (Index == 01) return &CPU<BusType>::DE;
        if constexpr (Index == 02) return &CPU<BusType>::HL;
        if constexpr (Index == 03) {
            if constexpr (AddressSP) return &CPU<BusType>::SP;
            else return &CPU<BusType>::AF;
        };
    }

    template<typename BusType>
    template<uint8_t Index>
    consteval std::pair<REGISTER_FLAG, bool> CPU<BusType>::decodeFlag() {
        using enum REGISTER_FLAG;
        auto flag = !((Index >> 1) & 0b1) ? Z : C;
        bool negate = !(Index & 0b1);
        return {flag, negate};
    }

    template<typename BusType>
    template<uint8_t Row, uint8_t Column>
    inline void CPU<BusType>::decodeBlock0() {
        if constexpr (Column == 0) {
            if constexpr (Row == 0) {
                /* NOP */
            }
            else if constexpr (Row == 1) {
                uint16_t indirectAddress = read16();
                load16(indirectAddress, SP);
            }
            else if constexpr (Row == 2) {
                stop();
            }
            else if constexpr (Row == 3) {
                int8_t offset = static_cast<int8_t>(read8());
                jumpRelative(offset);
            }
            else {
                constexpr auto flags = decodeFlag<(Row - 4)>();
                int8_t offset = static_cast<int8_t>(read8());
                jumpRelative<flags.first, flags.second>(offset);
            }
        }
        else if constexpr (Column == 1) {
            constexpr auto reg = decodeRegisterPair<(Row >> 1)>();
            if constexpr (!(Row & 0b1)) {
                uint16_t value = read16();
                load16(this->*reg, value);
            }
            else {
                add16(HL, this->*reg);
            }
        }
        else if constexpr (Column == 2) {
            if constexpr (Row < 4) {
                constexpr auto reg = decodeRegisterPair<(Row >> 1)>();
                if constexpr (!(Row & 0b1)) {
                    load(this->*reg, A);
                }
                else {
                    load(A, this->*reg);
                }
            }
            else if constexpr (Row == 4) loadIncrement(HL, A);
            else if constexpr (Row == 5) loadIncrement(A, HL);                
            else if constexpr (Row == 6) loadDecrement(HL, A);
            else if constexpr (Row == 7) loadDecrement(A, HL);
        }
        else if constexpr (Column == 3) {
            constexpr auto reg = decodeRegisterPair<(Row >> 1)>();
            if constexpr (!(Row & 0b1)) {
                increment16(this->*reg);
            }
            else {
                decrement16(this->*reg);
            }
        }
        else if constexpr (Column == 4) {
            constexpr auto reg = decodeRegister<Row>();
            increment(this->*reg);
        }
        else if constexpr (Column == 5) {
            constexpr auto reg = decodeRegister<Row>();
            decrement(this->*reg);
        }
        else if constexpr (Column == 6) {
            constexpr auto reg = decodeRegister<Row>();
            uint8_t value = read8();
            load(this->*reg, value);
        }
        else {
            if      constexpr (Row == 0) rotateLeftCircular(A);
            else if constexpr (Row == 1) rotateRightCircular(A);
            else if constexpr (Row == 2) rotateLeft(A);
            else if constexpr (Row == 3) rotateRight(A);
            else if constexpr (Row == 4) decimalAdjustAccumulator();
            else if constexpr (Row == 5) bitNot(A);
            else if constexpr (Row == 6) setCarryFlag();
            else if constexpr (Row == 7) complementCarryFlag();

            if constexpr (Row < 4) F.clear(REGISTER_FLAG::Z); // rotate A opcodes clear Z unconditionally
        }
    }

    template<typename BusType>
    template<uint8_t Row, uint8_t Column>
    inline void CPU<BusType>::decodeBlock1() {
        if constexpr (Row == 6 && Column == 6) {
            halt();
        }
        else {  // 8 bit loads
            constexpr auto dest = decodeRegister<Row>();
            constexpr auto src = decodeRegister<Column>();
            load(this->*dest, this->*src);
        }
    }

    template<typename BusType>
    template<uint8_t Row, uint8_t Column>
    inline void CPU<BusType>::decodeBlock2() {
        constexpr auto rhs = decodeRegister<Column>();
        if      constexpr (Row == 0) add(A, this->*rhs);
        else if constexpr (Row == 1) adc(A, this->*rhs);
        else if constexpr (Row == 2) sub(A, this->*rhs);
        else if constexpr (Row == 3) sbc(A, this->*rhs);
        else if constexpr (Row == 4) bitAnd(A, this->*rhs);
        else if constexpr (Row == 5) bitXor(A, this->*rhs);
        else if constexpr (Row == 6) bitOr(A, this->*rhs);
        else if constexpr (Row == 7) compare(A, this->*rhs);
    }

    template<typename BusType>
    template<uint8_t Row, uint8_t Column>
    inline void CPU<BusType>::decodeBlock3() {
        if constexpr (Column == 0) {
            if constexpr (Row < 4) {
                constexpr auto flags = decodeFlag<Row>();
                ret<flags.first, flags.second>();
            }
            else if constexpr (Row == 4) {
                uint8_t indirectAddress = read8();
                loadHi(indirectAddress, A);
            }
            else if constexpr (Row == 5) {
                int8_t offset = static_cast<int8_t>(read8());
                addRelative(SP, offset);
            }
            else if constexpr (Row == 6) {
                uint8_t indirectAddress = read8();
                loadHi(A, indirectAddress);
            }
            else {
                int8_t offset = static_cast<int8_t>(read8());
                loadAdjusted(HL, SP, offset);
            }
        }
        else if constexpr (Column == 1) {
            if constexpr (!(Row & 0b1)) {
                constexpr auto reg = decodeRegisterPair<(Row >> 1), false>();
                pop(this->*reg);
                if constexpr (reg == &CPU<BusType>::AF) {
                    AF.setLo(F & 0xF0); // lower 4 bits of F must always be 0
                }
            }
            else if constexpr (Row == 1) {
                ret();
            }
            else if constexpr (Row == 3) {
                reti();
            }
            else if constexpr (Row == 5) {
                PC = HL;    // This jump takes no extra cycles due to pipelining with fetch
            }
            else {
                load16(SP, HL);
                bus.tick();   // this load takes an extra cycle to simulate pipelining despite no argument decode
            }
        }
        else if constexpr (Column == 2) {
            if constexpr (Row < 4) {
                constexpr auto flags = decodeFlag<Row>();
                uint16_t address = read16();
                jump<flags.first, flags.second>(address);
            }
            else if constexpr (Row == 4) {
                loadHi(static_cast<uint8_t>(C), A);
            }
            else if constexpr (Row == 5) {
                uint16_t indirectAddress = read16();
                load(indirectAddress, A);
            }
            else if constexpr (Row == 6) {
                loadHi(A, C);
            }
            else {
                uint16_t indirectAddress = read16();
                load(A, indirectAddress);
            }
        }
        else if constexpr (Column == 3) {
            if constexpr (Row == 0) {
                uint16_t address = read16();
                jump(address);
            }
            else if constexpr (Row == 1) {
                /* CB prefix */
            }
            else if constexpr (Row == 6) {  // DI
                disableInterrupts();
            }
            else if constexpr (Row == 7) {  // EI
                enableInterrupts();
            }
        }
        else if constexpr (Column == 4) {
            if constexpr (Row < 4) {
                constexpr auto flags = decodeFlag<Row>();
                uint16_t address = read16();
                call<flags.first, flags.second>(address);
            }
        }
        else if constexpr (Column == 5) {
            if constexpr (!(Row & 0b1)) {
                constexpr auto reg = decodeRegisterPair<(Row >> 1), false>();
                push(this->*reg);
            }
            else if constexpr (Row == 1) {
                uint16_t address = read16();
                call(address);
            }
        }
        else if constexpr (Column == 6) {
            uint8_t value = read8();
            if constexpr (Row == 0) add(A, value);
            if constexpr (Row == 1) adc(A, value);
            if constexpr (Row == 2) sub(A, value);
            if constexpr (Row == 3) sbc(A, value);
            if constexpr (Row == 4) bitAnd(A, value);
            if constexpr (Row == 5) bitXor(A, value);
            if constexpr (Row == 6) bitOr(A, value);
            if constexpr (Row == 7) compare(A, value);
        }
        else {
            constexpr uint8_t address = 0x08 * Row;
            restart<address>();
        }
    }

    template<typename BusType>
    template<uint8_t Opcode>
    inline void CPU<BusType>::decodeUnprefixed() {
        constexpr uint8_t row = Opcode >> 3;
        constexpr uint8_t column = Opcode & 0b111;
        constexpr uint8_t blockRow = row & 0b111;
        if      constexpr (row < 010)  decodeBlock0<blockRow, column>();
        else if constexpr (row < 020)  decodeBlock1<blockRow, column>();
        else if constexpr (row < 030)  decodeBlock2<blockRow, column>();
        else if constexpr (row < 040)  decodeBlock3<blockRow, column>();
    }

    template<typename BusType>
    template<uint8_t Opcode>
    inline void CPU<BusType>::decodePrefixed() {
        constexpr auto reg = decodeRegister<Opcode & 0b111>();
        constexpr uint8_t row = Opcode >> 3;
        constexpr uint8_t index = row & 0b111;
        if      constexpr (row == 00)  rotateLeftCircular(this->*reg);
        else if constexpr (row == 01)  rotateRightCircular(this->*reg);
        else if constexpr (row == 02)  rotateLeft(this->*reg);
        else if constexpr (row == 03)  rotateRight(this->*reg);
        else if constexpr (row == 04)  shiftLeftArithmetic(this->*reg);
        else if constexpr (row == 05)  shiftRightArithmetic(this->*reg);
        else if constexpr (row == 06)  swap(this->*reg);
        else if constexpr (row == 07)  shiftRightLogical(this->*reg);
        else if constexpr (row < 020)  bitTest(index, this->*reg);
        else if constexpr (row < 030)  bitReset(index, this->*reg);
        else if constexpr (row < 040)  bitSet(index, this->*reg);
    }

}