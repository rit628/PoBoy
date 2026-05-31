#include "CPU.hpp"
#include "Opcodes.hpp"
#include "Register.hpp"
#include <array>
#include <cstdint>
#include <cstring>
#include <print>

uint8_t CPU::tick() {
    constexpr bool debug = false;

    /* Unprefixed Opcode Argument Constants */
    uint8_t $00 = 0x00, $08 = 0x08, $10 = 0x10, $18 = 0x18, $20 = 0x20, $28 = 0x28, $30 = 0x30, $38 = 0x38;
    uint16_t a16 = 0, n16 = 0;
    uint8_t n8 = 0, a8 = 0;
    int8_t e8 = 0;
    RegisterView& F_Z = F, F_NZ = F, F_C = F, F_NC = F;

    /* these are just here to keep xmacro happy when no cycle count is given */
    constexpr uint8_t cyclesTaken [[ maybe_unused ]] = 0, cyclesSkipped [[ maybe_unused ]] = 0;

    auto opcode = mmu[PC++];
    switch (static_cast<OPCODE_UNPREFIXED>(opcode)) {
        #define OPCODE_BEGIN(code, name, bytecount, ...) \
        case OPCODE_UNPREFIXED::name##_##code: { \
            if constexpr (code == 0xCB) { break; } /* decode cb prefixed opcode */ \
            if constexpr (debug) { std::print("{}: {} {}", PC, #code, #name); }
            #define CYCLES_TAKEN(count) \
            constexpr uint8_t cyclesTaken = count;
            #define CYCLES_SKIPPED(count) \
            constexpr uint8_t cyclesSkipped = count;
            #define FLAG_VALUE(...)
            #define OPERAND(name, type, bytecount, immediate, postop, ...) \
            if constexpr (bytecount > 0) { \
                std::array<uint8_t, bytecount> bytes; \
                for (auto&& byte : bytes) { \
                    byte = mmu[PC++]; \
                } \
                std::memcpy(&name, bytes.data(), bytecount); \
                if constexpr (debug) { std::print(" {:#06X}", name); } \
            } \
            else if constexpr (debug) { std::print(" {}", #name); }
            #define OPCODE_END(code, name, args...) \
            if constexpr (debug) { std::println(); } \
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

    /* 0xCB Prefixed Opcode Argument Constants */
    constexpr uint8_t prefixCycles = 1;

    opcode = mmu[PC++];
    switch (static_cast<OPCODE_CBPREFIXED>(opcode)) {
        #define OPCODE_BEGIN(code, name, bytecount, ...) \
        case OPCODE_CBPREFIXED::name##_##code: { \
            if constexpr (debug) { std::print("{}: {} {}", PC, #code, #name); }
            #define CYCLES_TAKEN(count) \
            constexpr uint8_t cyclesTaken = count;
            #define CYCLES_SKIPPED(...)
            #define FLAG_VALUE(...)
            #define OPERAND(name, ...) \
            if constexpr (debug) { std::print(" {}", #name); } /* all cb prefixed instructions have immediate operands */
            #define OPCODE_END(code, name, args...) \
            if constexpr (debug) { std::println(); } \
            name##_##code(args); \
            return prefixCycles + cyclesTaken; /* no cb prefixed instructions contain branching */ \
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