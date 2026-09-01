#pragma once
#include "MemoryConstants.hpp"
#include "Register.hpp"
#include <cstdint>
#include <utility>

namespace Processing {

    template<typename BusType>
    class CPU {
        public:
            CPU() requires (!std::is_reference_v<BusType>);
            CPU(BusType& bus);
            void initialize();
            void bootHLE(const Memory::CartridgeMetadata& cartData);
            void tick();
            
        protected:
            enum class STATE : uint8_t { RUNNING, HALTED, BUGGED };
            enum class INTERRUPT_MASTER_FLAG : uint8_t { DISABLED = 0, ENABLED = 1, ENABLE_PENDING = 2 };
    
            void handleInterrupts();
            void handleHaltBug();

            template<uint8_t Index>
            static consteval auto decodeRegister();
            template<uint8_t Index, bool AddressSP = true>
            static consteval auto decodeRegisterPair();
            template<uint8_t Index>
            static consteval std::pair<REGISTER_FLAG, bool> decodeFlag();
            template<uint8_t Row, uint8_t Column>
            void decodeBlock0();
            template<uint8_t Row, uint8_t Column>
            void decodeBlock1();
            template<uint8_t Row, uint8_t Column>
            void decodeBlock2();
            template<uint8_t Row, uint8_t Column>
            void decodeBlock3();

            template<uint8_t Opcode>
            void decodeUnprefixed();
            template<uint8_t Opcode>
            void decodePrefixed();

            template<bool Tick = true>
            uint8_t read(uint16_t address);
            template<bool Tick = true>
            void write(uint16_t address, uint8_t value);

            uint8_t read8();
            uint16_t read16();
            uint8_t getCarry();
            uint8_t getHalfCarry();
            void setZero(uint8_t result);
            uint8_t addAndSetFlags(uint8_t a, uint8_t b, uint8_t carry = 0);
            uint16_t addAndSetFlags(uint16_t a, uint16_t b);
            uint16_t addAndSetFlags(uint16_t a, int8_t b);
            uint8_t subtractAndSetFlags(uint8_t a, uint8_t b, uint8_t carry = 0);
            
            /* load instructions */
            void load(Register<8> auto& target, uint8_t value);
            void load(Register<8> auto& target, uint16_t address);
            void load(uint16_t address, uint8_t value);
            void load16(Register<16> auto& target, uint16_t value);
            void load16(uint16_t address, uint16_t value);
            
            void loadHi(Register<8> auto& target, uint8_t address);
            void loadHi(uint8_t address, uint8_t value);
            
            void loadIncrement(Register<8> auto& target, Register<16> auto& indirectAddress);
            void loadIncrement(Register<16> auto& indirectAddress, uint8_t value);
            void loadDecrement(Register<8> auto& target, Register<16> auto& indirectAddress);
            void loadDecrement(Register<16> auto& indirectAddress, uint8_t value);
    
            void loadAdjusted(Register<16> auto& target, Register<16> auto& value, int8_t adjustment);
    
            /* arithmetic instructions */
            void add(Register<8> auto& target, uint8_t value);
            void add(Register<8> auto& target, Register<16> auto& indirectAddress);
            void adc(Register<8> auto& target, uint8_t value);
            void adc(Register<8> auto& target, Register<16> auto& indirectAddress);
            void add16(Register<16> auto& target, uint16_t value);
            void addRelative(Register<16> auto& target, int8_t value);
    
            void sub(Register<8> auto& target, uint8_t value);
            void sub(Register<8> auto& target, Register<16> auto& indirectAddress);
            void sbc(Register<8> auto& target, uint8_t value);
            void sbc(Register<8> auto& target, Register<16> auto& indirectAddress);
    
            void compare(uint8_t lhs, uint8_t rhs);
            void compare(uint8_t lhs, Register<16> auto& indirectAddress);
    
            void decrement(Integer<8> auto& target);
            void decrement(Register<16> auto& indirectAddress);
            void decrement16(Register<16> auto& target);
    
            void increment(Integer<8> auto& target);
            void increment(Register<16> auto& indirectAddress);
            void increment16(Register<16> auto& target);
    
            /* bitwise logic instructions */
            void bitAnd(Register<8> auto& lhs, uint8_t rhs);
            void bitAnd(Register<8> auto& lhs, Register<16> auto& indirectAddress);
    
            void bitNot(Register<8> auto& target);
    
            void bitOr(Register<8> auto& lhs, uint8_t rhs);
            void bitOr(Register<8> auto& lhs, Register<16> auto& indirectAddress);
    
            void bitXor(Register<8> auto& lhs, uint8_t rhs);
            void bitXor(Register<8> auto& lhs, Register<16> auto& indirectAddress);
    
            /* bit flag instructions */
            void bitTest(uint8_t test, uint8_t target);
            void bitTest(uint8_t test, Register<16> auto& indirectAddress);
    
            void bitReset(uint8_t test, Integer<8> auto& target);
            void bitReset(uint8_t test, Register<16> auto& indirectAddress);
    
            void bitSet(uint8_t test, Integer<8> auto& target);
            void bitSet(uint8_t test, Register<16> auto& indirectAddress);
    
            /* bit shift instructions */
            void rotateLeft(Integer<8> auto& target);
            void rotateLeft(Register<16> auto& indirectAddress);
    
            void rotateLeftCircular(Integer<8> auto& target);
            void rotateLeftCircular(Register<16> auto& indirectAddress);
    
            void rotateRight(Integer<8> auto& target);
            void rotateRight(Register<16> auto& indirectAddress);
    
            void rotateRightCircular(Integer<8> auto& target);
            void rotateRightCircular(Register<16> auto& indirectAddress);
    
            void shiftLeftArithmetic(Integer<8> auto& target);
            void shiftLeftArithmetic(Register<16> auto& indirectAddress);
    
            void shiftRightArithmetic(Integer<8> auto& target);
            void shiftRightArithmetic(Register<16> auto& indirectAddress);
    
            void shiftRightLogical(Integer<8> auto& target);
            void shiftRightLogical(Register<16> auto& indirectAddress);
    
            void swap(Integer<8> auto& target);
            void swap(Register<16> auto& indirectAddress);
    
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
    
            BusType bus;
    
            /* Register File */
            Register16 PC; // program counter
            Register16 SP; // stack pointer
    
            Register16 AF, BC, DE, HL; // general purpose 16bit
    
            RegisterView A{AF.hi()}; // accumulator
            RegisterView F{AF.lo()}; // flags
    
            RegisterView B{BC.hi()}, C{BC.lo()}; // general purpose 8bit BC
            RegisterView D{DE.hi()}, E{DE.lo()}; // general purpose 8bit DE
            RegisterView H{HL.hi()}, L{HL.lo()}; // general purpose 8bit HL
    
            INTERRUPT_MASTER_FLAG IME; // interrupt master enable flag
            
            STATE state;
    };

}

#include "CPU.tpp"
#include "Decoder.tpp"