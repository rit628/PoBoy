#include "CPU.hpp"
#include "Opcodes.hpp"
#include "MemoryConstants.hpp"
#include "Register.hpp"
#include <array>
#include <cstdint>
#include <cstring>
#include <print>

#define DEBUG_PRINT false

#if DEBUG_PRINT
    #include <iostream>
    #define DEBUG_PRINT_OPCODE(code, name) \
    std::print(std::cerr, "{:#06x}: {} {}", PC - 1, #code, #name);
    #define DEBUG_PRINT_ARGS(name, type, bytecount, immediate, postop) \
    if constexpr (immediate) { \
        if constexpr (bytecount == 1 || std::is_same_v<type, RegisterView&>) { \
            std::print(std::cerr, " {}{} ({:#04x})", #name, #postop, name); \
        } \
        else { \
            std::print(std::cerr, " {}{} ({:#06x})", #name, #postop, name); \
        } \
    } \
    else { \
        if constexpr (bytecount == 1 || std::is_same_v<type, RegisterView&>) { /* for ldh */ \
            std::print(std::cerr, " [{}{}] ([{:#06x}] = {:#04x})", #name, #postop, name | 0xFF00, read<false>(name | 0xFF00)); \
        } \
        else { \
            std::print(std::cerr, " [{}{}] ([{:#06x}] = {:#04x})", #name, #postop, name, read<false>(name)); \
        } \
    } 
    #define DEBUG_PRINT_NEWLINE() \
    std::println();
    #define DEBUG_PRINT_INTERRUPT(interruptType) \
    std::println(std::cerr, "{} INTERRUPT: RST {:#04x}", #interruptType, interruptType##_INTERRUPT_ADDRESS);
#else
    #define DEBUG_PRINT_OPCODE(code, name)
    #define DEBUG_PRINT_ARGS(name, type, bytecount, immediate, postop)
    #define DEBUG_PRINT_NEWLINE()
    #define DEBUG_PRINT_INTERRUPT(interruptType)
#endif

using namespace Processing;

template<bool FlatMemory>
CPU<FlatMemory>::CPU(std::function<void()> systemTick) requires FlatMemory
                    : systemTick(systemTick) {}

template<bool FlatMemory>
CPU<FlatMemory>::CPU(Memory::MMU& mmu, std::function<void()> systemTick) requires (!FlatMemory)
                    : mmu(mmu), systemTick(systemTick) {}

template<bool FlatMemory>
void CPU<FlatMemory>::tick() {
    /* Unprefixed Opcode Argument Constants */
    uint8_t $00 = 0x00, $08 = 0x08, $10 = 0x10, $18 = 0x18, $20 = 0x20, $28 = 0x28, $30 = 0x30, $38 = 0x38;
    uint16_t a16 = 0, n16 = 0;
    uint8_t n8 = 0, a8 = 0;
    int8_t e8 = 0;
    RegisterView& F_Z = F, F_NZ = F, F_C = F, F_NC = F;

    handleInterrupts();

    /* pause instruction execution while halted */
    if (state == STATE::HALTED) return systemTick();
    /* set IME since next opcode read will consume one M cycle */
    if (IME == INTERRUPT_MASTER_FLAG::ENABLE_PENDING) IME = INTERRUPT_MASTER_FLAG::ENABLED;

    auto opcode = read(PC++);
    handleHaltBug();
    switch (static_cast<OPCODE_UNPREFIXED>(opcode)) {
        #define OPCODE_BEGIN(code, name, bytecount, ...) \
        case OPCODE_UNPREFIXED::name##_##code: { \
            if constexpr (code == 0xCB) { break; } /* decode cb prefixed opcode */ \
            DEBUG_PRINT_OPCODE(code, name)
            #define CYCLES_TAKEN(...)
            #define CYCLES_SKIPPED(...)
            #define FLAG_VALUE(...)
            #define OPERAND(name, type, bytecount, immediate, postop, ...) \
            if constexpr (bytecount > 0) { \
                std::array<uint8_t, bytecount> bytes; \
                for (auto&& byte : bytes) { \
                    byte = read(PC++); \
                } \
                std::memcpy(&name, bytes.data(), bytecount); \
            } \
            DEBUG_PRINT_ARGS(name, type, bytecount, immediate, postop)
            #define OPCODE_END(code, name, args...) \
            DEBUG_PRINT_NEWLINE() \
            return name##_##code(args); \
        }
        #include "unprefixed.inc"
        #undef OPCODE_BEGIN
        #undef CYCLES_TAKEN
        #undef CYCLES_SKIPPED
        #undef FLAG_VALUE
        #undef OPERAND
        #undef OPCODE_END
    }

    opcode = read(PC++);
    switch (static_cast<OPCODE_CBPREFIXED>(opcode)) {
        #define OPCODE_BEGIN(code, name, bytecount, ...) \
        case OPCODE_CBPREFIXED::name##_##code: { \
            DEBUG_PRINT_OPCODE(code, name)
            #define CYCLES_TAKEN(...)
            #define CYCLES_SKIPPED(...)
            #define FLAG_VALUE(...)
            #define OPERAND(name, type, bytecount, immediate, postop, ...) \
            DEBUG_PRINT_ARGS(name, type, bytecount, immediate, postop)
            #define OPCODE_END(code, name, args...) \
            DEBUG_PRINT_NEWLINE() \
            return name##_##code(args); \
        }
        #include "cbprefixed.inc"
        #undef OPCODE_BEGIN
        #undef CYCLES_TAKEN
        #undef CYCLES_SKIPPED
        #undef FLAG_VALUE
        #undef OPERAND
        #undef OPCODE_END
    }
}

template<bool FlatMemory>
void CPU<FlatMemory>::handleInterrupts() {
    static constexpr uint8_t VBLANK_INTERRUPT_ADDRESS      = 0x40;
    static constexpr uint8_t LCD_STAT_INTERRUPT_ADDRESS    = 0x48;
    static constexpr uint8_t TIMER_INTERRUPT_ADDRESS       = 0X50;
    static constexpr uint8_t SERIAL_INTERRUPT_ADDRESS      = 0X58;
    static constexpr uint8_t JOYPAD_INTERRUPT_ADDRESS      = 0X60;

    auto IF = read<false>(Memory::IF);
    auto IE = read<false>(Memory::IE);
    uint8_t interrupts = IF & IE & 0x1F;

    /* break out of halt mode on interrupt */
    if (state == STATE::HALTED) state = (!interrupts) ? STATE::HALTED : STATE::RUNNING;

    /* skip interrupt handling if master flag is not enabled or none found in current cycle */
    if (IME != INTERRUPT_MASTER_FLAG::ENABLED || !interrupts) return;

    auto handleInterrupt = [&, this]<Interrupts::INTERRUPT_FLAG Flag, uint8_t Address>() {
        if (testFlags(interrupts, Flag)) {
            /* noop ticks */
            systemTick();
            systemTick();
            IME = INTERRUPT_MASTER_FLAG::DISABLED;
            clearFlags(IF, Flag);
            write<false>(Memory::IF, IF);
            restart<Address>();
            return true;
        }
        return false;
    };

    using enum Interrupts::INTERRUPT_FLAG;
    if (handleInterrupt.template operator()<VBLANK, VBLANK_INTERRUPT_ADDRESS>())     {DEBUG_PRINT_INTERRUPT(VBLANK) return;}
    if (handleInterrupt.template operator()<LCD_STAT, LCD_STAT_INTERRUPT_ADDRESS>()) {DEBUG_PRINT_INTERRUPT(LCD_STAT) return;}
    if (handleInterrupt.template operator()<TIMER, TIMER_INTERRUPT_ADDRESS>())       {DEBUG_PRINT_INTERRUPT(TIMER) return;}
    if (handleInterrupt.template operator()<SERIAL, SERIAL_INTERRUPT_ADDRESS>())     {DEBUG_PRINT_INTERRUPT(SERIAL) return;}
    if (handleInterrupt.template operator()<JOYPAD, JOYPAD_INTERRUPT_ADDRESS>())     {DEBUG_PRINT_INTERRUPT(JOYPAD) return;}
}

template<bool FlatMemory>
void CPU<FlatMemory>::handleHaltBug() {
    if (state == STATE::BUGGED) {
        PC--;
        state = STATE::RUNNING;
    }
}

template class Processing::CPU<false>;
template class Processing::CPU<true>;