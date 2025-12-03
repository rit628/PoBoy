#include "Disassembler.hpp"
#include "StaticString.hpp"
#include <cstdint>
#include <ios>
#include <iostream>
#include <print>

inline uint32_t getRomSize(uint8_t encodedSize) {
    return 32 * (1 << encodedSize);
}

inline uint16_t getRamSize(uint8_t encodedSize) {
    switch (encodedSize) {
        case 0x00:
            return 0;
        break;

        case 0x02:
            return 8*2^10;
        break;

        case 0x03:
            return 32*2^10;
        break;

        case 0x04:
            return 128*2^10;
        break;

        case 0x05:
            return 64*2^10;
        break;

        default:
            return 0;
        break;
    }
}

void Disassembler::readHeader(std::istream& rom) {
    rom.seekg(HEADER_START);
    rom.seekg(ENTRYPOINT_SIZE, std::ios::cur);
    rom.seekg(NINTENDO_LOGO_SIZE, std::ios::cur);

    readInto(rom, title);
    std::println("Current position: 0x{:04X}", uint32_t(rom.tellg()));

    std::println("{}", title.size());
    
    manufacturerCode = title.substr(title.size() - 5, manufacturerCode.capacity());

    std::println("{}", manufacturerCode);


    cgbFlag = title.at(title.size() - 1);

    readInto(rom, licenseeCode);
    std::println("Current position: 0x{:04X}", uint32_t(rom.tellg()));

    sgbFlag = rom.get();
    std::println("Current position: 0x{:04X}", uint32_t(rom.tellg()));

    cartridgeType = rom.get();
    std::println("Current position: 0x{:04X}", uint32_t(rom.tellg()));

    uint8_t encodedSize = rom.get();
    romSize = getRomSize(encodedSize);
    std::println("Current position: 0x{:04X}", uint32_t(rom.tellg()));

    encodedSize = rom.get();
    ramSize = getRamSize(encodedSize);
    std::println("Current position: 0x{:04X}", uint32_t(rom.tellg()));
    
    destinationCode = rom.get();
    std::println("Current position: 0x{:04X}", uint32_t(rom.tellg()));
    
    uint8_t oldLicenseeCode = rom.get();
    if (oldLicenseeCode != 0x33) {
        std::println("using old licensee code: 0x{:02X}", oldLicenseeCode);
        licenseeCode[0] = oldLicenseeCode;
        licenseeCode[1] = '\0';
    }
    std::println("Current position: 0x{:04X}", uint32_t(rom.tellg()));

    romVersion = rom.get();
    std::println("Current position: 0x{:04X}", uint32_t(rom.tellg()));

    headerChecksum = rom.get();
    std::println("Current position: 0x{:04X}", uint32_t(rom.tellg()));

    readInto(rom, globalChecksum);

    std::println("{}", title);
    std::println("Current position: 0x{:04X}", uint32_t(rom.tellg()));
}