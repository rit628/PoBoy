#include "Disassembler.hpp"
#include "HeaderInfo.hpp"
#include "StaticString.hpp"
#include <cstdint>
#include <ios>
#include <iostream>
#include <print>

void Disassembler::readHeader(std::istream& rom) {
    rom.seekg(HEADER_START);
    rom.seekg(ENTRYPOINT_SIZE, std::ios::cur);
    rom.seekg(NINTENDO_LOGO_SIZE, std::ios::cur);

    std::println("Reading Title at: 0x{:04X}", uint32_t(rom.tellg()));
    readInto(rom, title);
    std::println("Rom Title: {}", title);

    manufacturerCode = title.substr(title.size() - 5, manufacturerCode.capacity());
    std::println("Manufacturer Code: {}", manufacturerCode);

    cgbFlag = title.at(title.size() - 1);
    std::println("CGB Flag: 0x{:02X}", cgbFlag);

    std::println("Reading New Licensee Code at: 0x{:04X}", uint32_t(rom.tellg()));
    readInto(rom, licenseeCode);

    std::println("Reading SGB Flag at: 0x{:04X}", uint32_t(rom.tellg()));
    sgbFlag = rom.get();
    std::println("SGB Flag: 0x{:02X}", sgbFlag);

    std::println("Reading Cartridge Type at: 0x{:04X}", uint32_t(rom.tellg()));
    cartridgeType = rom.get();
    std::println("Cartridge Type: {}", getCartridgeType(cartridgeType));

    std::println("Reading Rom Size at: 0x{:04X}", uint32_t(rom.tellg()));
    uint8_t encodedSize = rom.get();
    romSize = getRomSize(encodedSize);
    std::println("Rom Size: {}", romSize);

    std::println("Reading Ram Size at: 0x{:04X}", uint32_t(rom.tellg()));
    encodedSize = rom.get();
    ramSize = getRamSize(encodedSize);
    std::println("Ram Size: {}", ramSize);
    
    std::println("Reading Destination Code at: 0x{:04X}", uint32_t(rom.tellg()));
    destinationCode = rom.get();
    std::println("Region: {}", getRegion(destinationCode));
    
    std::println("Reading Old Licensee Code at: 0x{:04X}", uint32_t(rom.tellg()));
    uint8_t oldLicenseeCode = rom.get();
    if (oldLicenseeCode != NEW_LICENSEE_CODE_FLAG) {
        std::println("Using Old Licensee Code");
        licenseeCode[0] = oldLicenseeCode;
        licenseeCode.resize(1);
    }
    std::println("Licensee: {}", getLicensee(licenseeCode));

    std::println("Reading Rom Version at: 0x{:04X}", uint32_t(rom.tellg()));
    romVersion = rom.get();
    std::println("Rom Version: 0x{:02X}", romVersion);

    std::println("Reading Header Checksum at: 0x{:04X}", uint32_t(rom.tellg()));
    headerChecksum = rom.get();

    std::println("Reading Global Checksum at: 0x{:04X}", uint32_t(rom.tellg()));
    readInto(rom, globalChecksum);
}