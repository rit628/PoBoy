#include "Disassembler.hpp"
#include "HeaderInfo.hpp"
#include "Opcodes.hpp"
#include "StaticString.hpp"
#include <array>
#include <bitset>
#include <cstddef>
#include <cstdint>
#include <ios>
#include <iostream>
#include <istream>
#include <print>

void Disassembler::readCartridge(std::istream& rom) {
    readHeader(rom);
    while (!rom.eof()) {
        readInstruction(rom); // ignore interlaced data just checking instruction decoding
    }
}

void Disassembler::readEntrypoint(std::istream& rom) {
    auto opcode = static_cast<OPCODE_UNPREFIXED>(rom.get()); // should be NOP
    std::println("Entrypoint Instruction 1: {}", getMnemonic(opcode));

    opcode = static_cast<OPCODE_UNPREFIXED>(rom.get()); // should be JP
    readInto(rom, entrypointJumpAddress);
    std::println("Entrypoint Instruction 2: {} 0x{:04X}", getMnemonic(opcode), entrypointJumpAddress); 
}

void Disassembler::readLogo(std::istream& rom) {
    std::array<uint8_t, NINTENDO_LOGO_SIZE> logoEncoding;
    rom.read(reinterpret_cast<char*>(logoEncoding.data()), NINTENDO_LOGO_SIZE);

    constexpr int ROW_SIZE = 12;
    constexpr int COLUMN_SIZE = 8;
    std::array<std::array<std::bitset<4>, ROW_SIZE>, COLUMN_SIZE> pixelGrid;
    for (int i = 0; i < NINTENDO_LOGO_SIZE; i++) {
        auto byte = logoEncoding.at(i);
        auto columnIdx = 2 * (i % 2); // every two bytes encode 8 pixels in the current column
        columnIdx += (i >= NINTENDO_LOGO_SIZE / 2) ? 4 : 0; // logo encoding is split into upper and lower half
        pixelGrid.at(columnIdx).at(i/2 % ROW_SIZE) = (byte & 0b11110000) >> 4; // 4 pixels for top row
        pixelGrid.at(columnIdx + 1).at(i/2 % ROW_SIZE) = byte & 0b00001111; // 4 pixels for bottom row
    }

    for (auto&& row : pixelGrid) {
        for (auto column : row) {
            for (int k = column.size() - 1; k >= 0 ; k--) {
                if (column.test(k)) {
                    std::print("X");
                }
                else {
                    std::print(".");
                }
            }
            std::print(" ");
        }
        std::println();
    }

}

void Disassembler::readHeader(std::istream& rom) {
    rom.seekg(HEADER_START); // skip bootrom mapping

    readEntrypoint(rom);
    
    readLogo(rom);

    readInto(rom, title);
    std::println("Rom Title: {}", title);

    manufacturerCode = title.substr(title.size() - 5, manufacturerCode.capacity());
    std::println("Manufacturer Code: {}", manufacturerCode);

    cgbFlag = title.at(title.size() - 1);
    std::println("CGB Flag: 0x{:02X}", cgbFlag);

    readInto(rom, licenseeCode);

    sgbFlag = rom.get();
    std::println("SGB Flag: 0x{:02X}", sgbFlag);

    cartridgeType = rom.get();
    std::println("Cartridge Type: {}", getCartridgeType(cartridgeType));

    uint8_t encodedSize = rom.get();
    romSize = getRomSize(encodedSize);
    std::println("Rom Size: {}", romSize);

    encodedSize = rom.get();
    ramSize = getRamSize(encodedSize);
    std::println("Ram Size: {}", ramSize);
    
    destinationCode = rom.get();
    std::println("Region: {}", getRegion(destinationCode));
    
    uint8_t oldLicenseeCode = rom.get();
    if (oldLicenseeCode != NEW_LICENSEE_CODE_FLAG) {
        std::println("Using Old Licensee Code");
        licenseeCode[0] = oldLicenseeCode;
        licenseeCode.resize(1);
    }
    std::println("Licensee: {}", getLicensee(licenseeCode));

    romVersion = rom.get();
    std::println("Rom Version: 0x{:02X}", romVersion);

    headerChecksum = rom.get();

    readInto(rom, globalChecksum);

    std::println();
}

#define PRINT_OPERAND(name, bytecount, immediate) \
    if constexpr (immediate) { \
        if constexpr (bytecount == 1) { \
            uint8_t op_##name = rom.get(); \
            std::print(" ${:02x}", op_##name); \
        } \
        else if constexpr (bytecount == 2) { \
            uint16_t op_##name; \
            readInto(rom, op_##name); \
            std::print(" ${:04x}", op_##name); \
        } \
        else { \
            std::print(" {}", #name); \
        } \
    } \
    else { \
        if constexpr (bytecount == 1) { \
            uint8_t op_##name = rom.get(); \
            std::print(" [${:02x}]", op_##name); \
        } \
        else if constexpr (bytecount == 2) { \
            uint16_t op_##name; \
            readInto(rom, op_##name); \
            std::print(" [${:04x}]", op_##name); \
        } \
        else { \
            std::print(" [{}]", #name); \
        } \
    }

void Disassembler::readInstruction(std::istream& rom) {
    std::print("0x{:04X}: ", static_cast<uint16_t>(rom.tellg()));
    auto opcode = static_cast<OPCODE_UNPREFIXED>(rom.get());
    switch (opcode) {
        #define OPCODE_BEGIN(code, name, bytecount, ...) \
        case OPCODE_UNPREFIXED::name##__##code: \
            std::print("{}", #name); \
            if constexpr (OPCODE_UNPREFIXED::name##__##code == OPCODE_UNPREFIXED::PREFIX__0xCB) { \
                readPrefixedInstruction(rom); \
            }
            #define CYCLES_TAKEN(...)
            #define CYCLES_SKIPPED(...)
            #define FLAG_VALUE(...)
            #define OPERAND(name, bytecount, immediate) \
            PRINT_OPERAND(name, bytecount, immediate)
            #define OPCODE_END(...) \
            std::println(); \
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

void Disassembler::readPrefixedInstruction(std::istream& rom) {
    std::print(" ");
    auto opcode = static_cast<OPCODE_CBPREFIXED>(rom.get());
    switch (opcode) {
        #define OPCODE_BEGIN(code, name, bytecount, ...) \
        case OPCODE_CBPREFIXED::name##__##code: \
            std::print("{}", #name);
            #define CYCLES_TAKEN(...)
            #define CYCLES_SKIPPED(...)
            #define FLAG_VALUE(...)
            #define OPERAND(name, bytecount, immediate) \
            PRINT_OPERAND(name, bytecount, immediate)
            #define OPCODE_END(...) \
        break;
        #include "cbprefixed.inc"
        #undef OPCODE_BEGIN
        #undef CYCLES_TAKEN
        #undef CYCLES_SKIPPED
        #undef FLAG_VALUE
        #undef OPERAND
        #undef OPCODE_END
    }
}