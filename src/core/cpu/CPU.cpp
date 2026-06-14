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
    #define DEBUG_PRINT_OPCODE(code, name) \
    std::print("{:#06x}: {} {}", PC - 1, #code, #name);
    #define DEBUG_PRINT_ARGS(name, type, bytecount, immediate, postop) \
    if constexpr (immediate) { \
        if constexpr (bytecount == 1 || std::is_same_v<type, RegisterView&>) { \
            std::print(" {}{} ({:#04x})", #name, #postop, name); \
        } \
        else { \
            std::print(" {}{} ({:#06x})", #name, #postop, name); \
        } \
    } \
    else { \
        if constexpr (bytecount == 1 || std::is_same_v<type, RegisterView&>) { /* for ldh */ \
            std::print(" [{}{}] ([{:#06x}] = {:#04x})", #name, #postop, name | 0xFF00, mmu.read(name | 0xFF00)); \
        } \
        else { \
            std::print(" [{}{}] ([{:#06x}] = {:#04x})", #name, #postop, name, mmu.read(name)); \
        } \
    } 
    #define DEBUG_PRINT_NEWLINE() \
    std::println();
    #define DEBUG_PRINT_INTERRUPT(interruptType) \
    std::println("{} INTERRUPT: RST {:#04x}", #interruptType, interruptType##_INTERRUPT_ADDRESS);
#else
    #define DEBUG_PRINT_OPCODE(code, name)
    #define DEBUG_PRINT_ARGS(name, type, bytecount, immediate, postop)
    #define DEBUG_PRINT_NEWLINE()
    #define DEBUG_PRINT_INTERRUPT(interruptType)
#endif


uint8_t CPU::tick() {
    /* Unprefixed Opcode Argument Constants */
    uint8_t $00 = 0x00, $08 = 0x08, $10 = 0x10, $18 = 0x18, $20 = 0x20, $28 = 0x28, $30 = 0x30, $38 = 0x38;
    uint16_t a16 = 0, n16 = 0;
    uint8_t n8 = 0, a8 = 0;
    int8_t e8 = 0;
    RegisterView& F_Z = F, F_NZ = F, F_C = F, F_NC = F;

    /* these are just here to keep xmacro happy when no cycle count is given */
    static constexpr uint8_t cyclesTaken [[ maybe_unused ]] = 0, cyclesSkipped [[ maybe_unused ]] = 0;

    handleInterrupts();

    /* pause instruction execution while halted */
    if (state == STATE::HALTED) return 1;

    /* set IME since next opcode read will consume one M cycle */
    if (IME == INTERRUPT_MASTER_FLAG::ENABLE_PENDING) IME = INTERRUPT_MASTER_FLAG::ENABLED;

    auto opcode = mmu.read(PC++);
    handleHaltBug();
    switch (static_cast<OPCODE_UNPREFIXED>(opcode)) {
        #define OPCODE_BEGIN(code, name, bytecount, ...) \
        case OPCODE_UNPREFIXED::name##_##code: { \
            if constexpr (code == 0xCB) { break; } /* decode cb prefixed opcode */ \
            DEBUG_PRINT_OPCODE(code, name)
            #define CYCLES_TAKEN(count) \
            constexpr uint8_t cyclesTaken = count;
            #define CYCLES_SKIPPED(count) \
            constexpr uint8_t cyclesSkipped = count;
            #define FLAG_VALUE(...)
            #define OPERAND(name, type, bytecount, immediate, postop, ...) \
            if constexpr (bytecount > 0) { \
                std::array<uint8_t, bytecount> bytes; \
                for (auto&& byte : bytes) { \
                    byte = mmu.read(PC++); \
                } \
                std::memcpy(&name, bytes.data(), bytecount); \
            } \
            DEBUG_PRINT_ARGS(name, type, bytecount, immediate, postop)
            #define OPCODE_END(code, name, args...) \
            DEBUG_PRINT_NEWLINE() \
            return (name##_##code(args)) ? cyclesTaken : cyclesSkipped; \
        }
        #include "unprefixed.inc"
        #undef OPCODE_BEGIN
        #undef CYCLES_TAKEN
        #undef CYCLES_SKIPPED
        #undef FLAG_VALUE
        #undef OPERAND
        #undef OPCODE_END
    }

    opcode = mmu.read(PC++);
    switch (static_cast<OPCODE_CBPREFIXED>(opcode)) {
        #define OPCODE_BEGIN(code, name, bytecount, ...) \
        case OPCODE_CBPREFIXED::name##_##code: { \
            DEBUG_PRINT_OPCODE(code, name)
            #define CYCLES_TAKEN(count) \
            constexpr uint8_t cyclesTaken = count;
            #define CYCLES_SKIPPED(...)
            #define FLAG_VALUE(...)
            #define OPERAND(name, type, bytecount, immediate, postop, ...) \
            DEBUG_PRINT_ARGS(name, type, bytecount, immediate, postop)
            #define OPCODE_END(code, name, args...) \
            DEBUG_PRINT_NEWLINE() \
            name##_##code(args); \
            return cyclesTaken; /* no cb prefixed instructions contain branching */ \
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

void CPU::handleInterrupts() {
    static constexpr uint8_t VBLANK_INTERRUPT_ADDRESS      = 0x40;
    static constexpr uint8_t LCD_STAT_INTERRUPT_ADDRESS    = 0x48;
    static constexpr uint8_t TIMER_INTERRUPT_ADDRESS       = 0X50;
    static constexpr uint8_t SERIAL_INTERRUPT_ADDRESS      = 0X58;
    static constexpr uint8_t JOYPAD_INTERRUPT_ADDRESS      = 0X60;

    auto IF = mmu.read(Memory::IF);
    auto IE = mmu.read(Memory::IE);
    uint8_t interrupts = IF & IE & 0x1F;

    /* break out of halt mode on interrupt */
    using enum STATE;
    if (state == HALTED) state = (!interrupts) ? HALTED : RUNNING;

    /* skip interrupt handling if master flag is not enabled or none found in current cycle */
    if (IME != INTERRUPT_MASTER_FLAG::ENABLED || !interrupts) return;

    auto handleInterrupt = [&, this]<INTERRUPT_FLAG flag, uint8_t address>() {
        if (flagTest(interrupts, flag)) {
            IME = INTERRUPT_MASTER_FLAG::DISABLED;
            flagClear(IF, flag);
            mmu.write(Memory::IF, IF);
            restart(address);
            return true;
        }
        return false;
    };

    using enum INTERRUPT_FLAG;
    if (handleInterrupt.operator()<VBLANK, VBLANK_INTERRUPT_ADDRESS>())     {DEBUG_PRINT_INTERRUPT(VBLANK) return;}
    if (handleInterrupt.operator()<LCD_STAT, LCD_STAT_INTERRUPT_ADDRESS>()) {DEBUG_PRINT_INTERRUPT(LCD_STAT) return;}
    if (handleInterrupt.operator()<TIMER, TIMER_INTERRUPT_ADDRESS>())       {DEBUG_PRINT_INTERRUPT(TIMER) return;}
    if (handleInterrupt.operator()<SERIAL, SERIAL_INTERRUPT_ADDRESS>())     {DEBUG_PRINT_INTERRUPT(SERIAL) return;}
    if (handleInterrupt.operator()<JOYPAD, JOYPAD_INTERRUPT_ADDRESS>())     {DEBUG_PRINT_INTERRUPT(JOYPAD) return;}
}

void CPU::handleHaltBug() {
    using enum STATE;
    if (state == BUGGED) {
        PC--;
        state = RUNNING;
    }
}