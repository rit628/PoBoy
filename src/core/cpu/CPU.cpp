#include "CPU.hpp"
#include "Opcodes.hpp"
#include "Register.hpp"
#include <array>
#include <cstdint>
#include <cstring>
#include <print>

void CPU::tick() {
    /* Argument Constants */
    uint8_t $00 = 0x00, $08 = 0x08, $10 = 0x10, $18 = 0x18, $20 = 0x20, $28 = 0x28, $30 = 0x30, $38 = 0x38;
    uint16_t a16 = 0, n16 = 0;
    uint8_t n8 = 0, a8 = 0;
    int8_t e8 = 0;
    RegisterView& F_Z = F, F_NZ = F, F_C = F, F_NC = F;

    if (readPrefixed) {
        auto opcode = static_cast<OPCODE_CBPREFIXED>(mmu[PC++]);
        switch (opcode) {
            case OPCODE_CBPREFIXED::BIT_0x7C: {
                std::println("{}: {} {} {} {}", PC, "0x7C", "BIT", "7", "B");
                BIT_0x7C(7, H);
            }
            break;

            case OPCODE_CBPREFIXED::SWAP_0x37: {
                std::println("{}: {} {} {}", PC, "0x37", "SWAP", "A");
                SWAP_0x37(A);
            }
            break;

            case OPCODE_CBPREFIXED::RL_0x10: {
                std::println("{}: {} {} {}", PC, "0x10", "RL", "B");
                RL_0x10(B);
            }
            break;

            default:
                std::println("UNIMPLEMENTED CBPREFIXED OPCODE");
            break;
        }
        readPrefixed = false;
    }
    else {
        auto opcode = static_cast<OPCODE_UNPREFIXED>(mmu[PC++]);
        switch (opcode) {
            #define OPCODE_BEGIN(code, name, bytecount, ...) \
            case OPCODE_UNPREFIXED::name##_##code: { \
                if constexpr (code != 0xCB) { \
                    std::print("{}: {} {}", PC, #code, #name); \
                }
                #define CYCLES_TAKEN(...)
                #define CYCLES_SKIPPED(...)
                #define FLAG_VALUE(...)
                #define OPERAND(name, type, bytecount, immediate, postop, ...) \
                if constexpr (bytecount > 0) { \
                    std::array<uint8_t, bytecount> bytes; \
                    for (auto&& byte : bytes) { \
                        byte = mmu[PC++]; \
                    } \
                    std::memcpy(&name, bytes.data(), bytecount); \
                    std::print(" {:#06X}", name); \
                } \
                else { \
                    std::print(" {}", #name); \
                }
                #define OPCODE_END(code, name, args...) \
                if constexpr (code != 0xCB) { \
                    std::println(); \
                } \
                name##_##code(args); \
            } \
            break;
            #include "unprefixed.inc"
            #undef OPCODE_BEGIN
            #undef CYCLES_TAKEN
            #undef CYCLES_SKIPPED
            #undef FLAG_VALUE
            #undef OPERAND
            #undef OPCODE_END
        }
    }
}