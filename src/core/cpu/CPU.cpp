#include "CPU.hpp"
#include "InterruptConstants.hpp"
#include "Bus.hpp"
#include "Opcodes.hpp"
#include "MemoryConstants.hpp"
#include "Register.hpp"
#include "SystemConstants.hpp"
#include <cstdint>

#define DEBUG_PRINT false

#if DEBUG_PRINT
    #include <iostream>
    #include <print>
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
    std::println(std::cerr);
    #define DEBUG_PRINT_INTERRUPT(interruptType) \
    std::println(std::cerr, "{} INTERRUPT: RST {:#04x}", #interruptType, interruptType##_INTERRUPT_ADDRESS);
#else
    #define DEBUG_PRINT_OPCODE(code, name)
    #define DEBUG_PRINT_ARGS(name, type, bytecount, immediate, postop)
    #define DEBUG_PRINT_NEWLINE()
    #define DEBUG_PRINT_INTERRUPT(interruptType)
#endif

using namespace Processing;

template<typename BusType>
CPU<BusType>::CPU() requires (!std::is_reference_v<BusType>)
{
    initialize();
}

template<typename BusType>
CPU<BusType>::CPU(BusType& bus) : bus(bus)
{
    initialize();
}

template<typename BusType>
void CPU<BusType>::initialize() {
    PC = 0;
    SP = 0;
    AF = 0, BC = 0, DE = 0, HL = 0;

    IME = INTERRUPT_MASTER_FLAG::DISABLED;
    
    state = STATE::RUNNING;
}

template<>
void CPU<Memory::Bus<MODEL::DMG>&>::bootHLE(const Memory::CartridgeMetadata& cartData) {
    A = 0x01;
    {   // set F
        using enum REGISTER_FLAG;
        setFlag<Z>();
        clearFlag<N>();
        if (cartData.headerChecksum == 0x00) {
            clearFlag<H>();
            clearFlag<C>();
        }
        else {
            setFlag<H>();
            setFlag<C>();
        }
    }
    B = 0x00;
    C = 0x13;
    D = 0x00;
    E = 0xD8;
    H = 0x01;
    L = 0x4D;
    PC = 0x0100;
    SP = 0xFFFE;

    bus.initHLE();
}

template<>
void CPU<Memory::Bus<MODEL::CGB>&>::bootHLE(const Memory::CartridgeMetadata& cartData [[ maybe_unused ]]) {
    A = 0x11;
    {   // set F
        using enum REGISTER_FLAG;
        setFlag<Z>();
        clearFlag<N>();
        clearFlag<H>();
        clearFlag<C>();
    }
    B = 0x00;
    C = 0x00;
    D = 0xFF;
    E = 0x56;
    H = 0x00;
    L = 0x0D;
    PC = 0x0100;
    SP = 0xFFFE;

    bus.initHLE();
}

template<typename BusType>
void CPU<BusType>::tick() {
    handleInterrupts();

    /* pause instruction execution while halted */
    if (state == STATE::HALTED) return bus.tick();
    /* set IME since next opcode read will consume one M cycle */
    if (IME == INTERRUPT_MASTER_FLAG::ENABLE_PENDING) IME = INTERRUPT_MASTER_FLAG::ENABLED;

    auto opcode = read8();
    handleHaltBug();
    switch (static_cast<OPCODE_UNPREFIXED>(opcode)) {
        #define OPCODE_BEGIN(code, name, bytecount, ...) \
        case OPCODE_UNPREFIXED::name##_##code: { \
            if constexpr (code == 0xCB) { break; } /* decode cb prefixed opcode */ \
            DEBUG_PRINT_OPCODE(code, name)
            #define CYCLES_TAKEN(...)
            #define CYCLES_SKIPPED(...)
            #define FLAG_VALUE(...)
            #define OPERAND(name, type, bytecount, immediate, postop, ...)
            DEBUG_PRINT_ARGS(name, type, bytecount, immediate, postop)
            #define OPCODE_END(code, name, args...) \
            DEBUG_PRINT_NEWLINE() \
            return decodeUnprefixed<code>(); \
        }
        #include "unprefixed.inc"
        #undef OPCODE_BEGIN
        #undef CYCLES_TAKEN
        #undef CYCLES_SKIPPED
        #undef FLAG_VALUE
        #undef OPERAND
        #undef OPCODE_END
    }

    opcode = read8();
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
            return decodePrefixed<code>(); \
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

template<typename BusType>
void CPU<BusType>::handleInterrupts() {
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
            bus.tick();
            bus.tick();
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

template<typename BusType>
void CPU<BusType>::handleHaltBug() {
    if (state == STATE::BUGGED) {
        --PC;
        state = STATE::RUNNING;
    }
}

template class Processing::CPU<Memory::Bus<MODEL::DMG>&>;
template class Processing::CPU<Memory::Bus<MODEL::CGB>&>;
template class Processing::CPU<Memory::FlatBus>;