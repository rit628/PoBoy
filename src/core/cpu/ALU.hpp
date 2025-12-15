#pragma once
#include "Register.hpp"
#include "MMU.hpp"
#include <cstdint>


class ALU {
    public:
        ALU(RegisterFile& rf, MMU& mmu) : rf(rf), mmu(mmu) {}

        #define OPCODE_BEGIN(code, name, ...) \
        void name##_##code(
        #define CYCLES_TAKEN(...)
        #define CYCLES_SKIPPED(...)
        #define FLAG_VALUE(...)
        #define OPERAND(name, type, bytes, immediate, postop, flag, index) \
        type& name##_##index,
        #define OPCODE_END(...) \
        int = 0);
        #include "unprefixed.inc"
        #include "cbprefixed.inc"
        #undef OPCODE_BEGIN
        #undef CYCLES_TAKEN
        #undef CYCLES_SKIPPED
        #undef FLAG_VALUE
        #undef OPERAND
        #undef OPCODE_END

    private:
        uint8_t getCarry();
        uint8_t getHalfCarry();
        void setCarry(uint8_t a, uint8_t b, uint8_t carry = 0);
        void setHalfCarry(uint8_t a, uint8_t b, uint8_t carry = 0);
        void setCarry(uint16_t a, uint16_t b);
        void setHalfCarry(uint16_t a, uint16_t b);
        void setZero(uint8_t result);

        RegisterFile& rf;
        MMU& mmu;
};