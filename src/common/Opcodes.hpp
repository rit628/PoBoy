#pragma once
#include <cstdint>
#include <string>

enum class OPCODE_UNPREFIXED : uint8_t {
    #define OPCODE_BEGIN(code, name, ...) \
    name##_##code = code,
    #define CYCLES_TAKEN(...)
    #define CYCLES_SKIPPED(...)
    #define FLAG_VALUE(...)
    #define OPERAND(...)
    #define OPCODE_END(...)
    #include "unprefixed.inc"
    #undef OPCODE_BEGIN
    #undef CYCLES_TAKEN
    #undef CYCLES_SKIPPED
    #undef FLAG_VALUE
    #undef OPERAND
    #undef OPCODE_END
};

enum class OPCODE_CBPREFIXED : uint8_t {
    #define OPCODE_BEGIN(code, name, ...) \
    name##_##code = code,
    #define CYCLES_TAKEN(...)
    #define CYCLES_SKIPPED(...)
    #define FLAG_VALUE(...)
    #define OPERAND(...)
    #define OPCODE_END(...)
    #include "cbprefixed.inc"
    #undef OPCODE_BEGIN
    #undef CYCLES_TAKEN
    #undef CYCLES_SKIPPED
    #undef FLAG_VALUE
    #undef OPERAND
    #undef OPCODE_END
};

constexpr std::string getMnemonic(OPCODE_UNPREFIXED opcode) {
    switch (opcode) {
        #define OPCODE_BEGIN(code, name, ...) \
        case OPCODE_UNPREFIXED::name##_##code: \
            return #name; \
        break;
        #define CYCLES_TAKEN(...)
        #define CYCLES_SKIPPED(...)
        #define FLAG_VALUE(...)
        #define OPERAND(...)
        #define OPCODE_END(...)
        #include "unprefixed.inc"
        #undef OPCODE_BEGIN
        #undef CYCLES_TAKEN
        #undef CYCLES_SKIPPED
        #undef FLAG_VALUE
        #undef OPERAND
        #undef OPCODE_END
    }
}

constexpr std::string getMnemonic(OPCODE_CBPREFIXED opcode) {
    switch (opcode) {
        #define OPCODE_BEGIN(code, name, ...) \
        case OPCODE_CBPREFIXED::name##_##code: \
            return #name; \
        break;
        #define CYCLES_TAKEN(...)
        #define CYCLES_SKIPPED(...)
        #define FLAG_VALUE(...)
        #define OPERAND(...)
        #define OPCODE_END(...)
        #include "cbprefixed.inc"
        #undef OPCODE_BEGIN
        #undef CYCLES_TAKEN
        #undef CYCLES_SKIPPED
        #undef FLAG_VALUE
        #undef OPERAND
        #undef OPCODE_END
    }
}