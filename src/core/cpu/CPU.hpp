#pragma once
#include "MemoryConstants.hpp"
#include "Register.hpp"
#include "MMU.hpp"
#include <array>
#include <cstddef>
#include <cstdint>
#include <functional>
#include <type_traits>

namespace Processing {

    template<bool FlatMemory = false>
    class CPU {
        public:
            CPU(std::function<void()> systemTick) requires FlatMemory;
            CPU(Memory::MMU& mmu, std::function<void()> systemTick) requires (!FlatMemory);
            void tick();
            
        protected:
            enum class STATE : uint8_t { RUNNING, HALTED, BUGGED };
            enum class INTERRUPT_MASTER_FLAG : uint8_t { DISABLED = 0, ENABLED = 1, ENABLE_PENDING = 2 };
    
            void handleInterrupts();
            void handleHaltBug();
    
            #define OPCODE_BEGIN(code, name, ...) \
            void name##_##code(
            #define CYCLES_TAKEN(...)
            #define CYCLES_SKIPPED(...)
            #define FLAG_VALUE(...)
            #define OPERAND(name, type, bytes, immediate, postop, index) \
            type _##name##_##index,
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

            template<bool Tick = true>
            uint8_t read(uint16_t address);
            template<bool Tick = true>
            void write(uint16_t address, uint8_t value);
    
            uint8_t getCarry();
            uint8_t getHalfCarry();
            void setZero(uint8_t result);
            uint8_t addAndSetFlags(uint8_t a, uint8_t b, uint8_t carry = 0);
            uint16_t addAndSetFlags(uint16_t a, uint16_t b);
            uint16_t addAndSetFlags(uint16_t a, int8_t b);
            uint8_t subtractAndSetFlags(uint8_t a, uint8_t b, uint8_t carry = 0);
            
            /* load instructions */
            template<size_t N>
            void load(Register<N> auto& target, RegisterValue<N> value);
            void loadIndirect(uint16_t address, uint8_t value);
            void loadIndirect(uint16_t address, uint16_t value);
            void loadIndirect(Register<8> auto& target, uint16_t address);
            
            void loadHiIndirect(Register<8> auto& target, uint8_t address);
            void loadHiIndirect(uint16_t address, uint8_t value);
            
            void loadIncrement(Register<8> auto& target, Register<16> auto& address);
            void loadIncrement(Register<16> auto& address, uint8_t value);
            void loadDecrement(Register<8> auto& target, Register<16> auto& address);
            void loadDecrement(Register<16> auto& address, uint8_t value);
    
            void loadAdjusted(Register<16> auto& target, Register<16> auto& value, int8_t adjust);
    
            /* arithmetic instructions */
            template<size_t N>
            void add(Register<N> auto& target, RegisterValue<N> value);
            void addIndirect(Register<8> auto& target, uint16_t address);
            void adc(Register<8> auto& target, uint8_t value);
            void adcIndirect(Register<8> auto& target, uint16_t address);
            void addRelative(Register<16> auto& target, int8_t value);
    
            void sub(Register<8> auto& target, uint8_t value);
            void subIndirect(Register<8> auto& target, uint16_t address);
            void sbc(Register<8> auto& target, uint8_t value);
            void sbcIndirect(Register<8> auto& target, uint16_t address);
    
            void compare(uint8_t lhs, uint8_t rhs);
            void compareIndirect(uint8_t lhs, uint16_t address);
    
            void decrement(Integer<8> auto& target);
            void decrement(Register<16> auto& target);
            void decrementIndirect(uint16_t address);
    
            void increment(Integer<8> auto& target);
            void increment(Register<16> auto& target);
            void incrementIndirect(uint16_t address);
    
            /* bitwise logic instructions */
            void bitAnd(Register<8> auto& lhs, uint8_t rhs);
            void bitAndIndirect(Register<8> auto& lhs, uint16_t address);
    
            void bitNot(Register<8> auto& target);
    
            void bitOr(Register<8> auto& lhs, uint8_t rhs);
            void bitOrIndirect(Register<8> auto& lhs, uint16_t address);
    
            void bitXor(Register<8> auto& lhs, uint8_t rhs);
            void bitXorIndirect(Register<8> auto& lhs, uint16_t address);
    
            /* bit flag instructions */
            void bitTest(uint8_t test, uint8_t target);
            void bitTestIndirect(uint8_t test, uint16_t address);
    
            void bitReset(uint8_t test, Integer<8> auto& target);
            void bitResetIndirect(uint8_t test, uint16_t address);
    
            void bitSet(uint8_t test, Integer<8> auto& target);
            void bitSetIndirect(uint8_t test, uint16_t address);
    
            /* bit shift instructions */
            void rotateLeft(Integer<8> auto& target);
            void rotateLeftIndirect(uint16_t address);
    
            void rotateLeftCircular(Integer<8> auto& target);
            void rotateLeftCircularIndirect(uint16_t address);
    
            void rotateRight(Integer<8> auto& target);
            void rotateRightIndirect(uint16_t address);
    
            void rotateRightCircular(Integer<8> auto& target);
            void rotateRightCircularIndirect(uint16_t address);
    
            void shiftLeftArithmetic(Integer<8> auto& target);
            void shiftLeftArithmeticIndirect(uint16_t address);
    
            void shiftRightArithmetic(Integer<8> auto& target);
            void shiftRightArithmeticIndirect(uint16_t address);
    
            void shiftRightLogical(Integer<8> auto& target);
            void shiftRightLogicalIndirect(uint16_t address);
    
            void swap(Integer<8> auto& target);
            void swapIndirect(uint16_t address);
    
            /* stack manipulation instructions */
            void pop(Register<16> auto& target);
            void push(Register<16> auto& target);
    
            /* control flow instructions */
            template<REGISTER_FLAG Flag, bool N = false>
            bool testCondition();
            
            void call(uint16_t address);
            template<REGISTER_FLAG Flag, bool N = false>
            void call(uint16_t address);

            void jump(uint16_t address);
            template<REGISTER_FLAG Flag, bool N = false>
            void jump(uint16_t address);

            void jumpRelative(int8_t offset);
            template<REGISTER_FLAG Flag, bool N = false>
            void jumpRelative(int8_t offset);

            void ret();
            template<REGISTER_FLAG Flag, bool N = false>
            void ret();

            void reti();

            template<uint8_t Address>
            void restart();
    
            /* carry flag instructions */
            void complementCarryFlag();
            void setCarryFlag();
    
            /* interrupt instructions */
            void disableInterrupts();
            void enableInterrupts();
            void halt();
    
            /* misc instructions */
            void decimalAdjustAccumulator();
            void stop();
    
            using MemoryType = std::conditional_t<FlatMemory, std::array<uint8_t, Memory::MEMORY_SIZE>, Memory::MMU&>;
            
            MemoryType mmu;
            std::function<void()> systemTick;   // use std::function for simplicity
    
            /* Register File */
            Register16 PC; // program counter
            Register16 SP; // stack pointer
    
            Register16 AF, BC, DE, HL; // general purpose 16bit
    
            RegisterView A{AF.hi()}; // accumulator
            RegisterView F{AF.lo()}; // flags
    
            RegisterView B{BC.hi()}, C{BC.lo()}; // general purpose 8bit BC
            RegisterView D{DE.hi()}, E{DE.lo()}; // general purpose 8bit DE
            RegisterView H{HL.hi()}, L{HL.lo()}; // general purpose 8bit HL
    
            INTERRUPT_MASTER_FLAG IME = INTERRUPT_MASTER_FLAG::DISABLED; // interrupt master enable flag
            
            STATE state = STATE::RUNNING;
    };

}

#include "CPU.tpp"