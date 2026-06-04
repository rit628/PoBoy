#include "MMU.hpp"
#include "IO.hpp"
#include <cstdint>
#include <iostream>
#include <print>

uint8_t MMU::read(uint16_t address) {
    if (address >= MEMORY_SIZE) return READ_ERROR;
    if (address < BOOTROM_SIZE && !memory.at(IO::BANK)) return bootrom.at(address);
    return memory.at(address);
}

void MMU::write(uint16_t address, uint8_t value) {
    if (address >= MEMORY_SIZE) return;
    if (address == IO::BANK && memory.at(IO::BANK)) return; // bootrom remains unmapped until next reset
    memory.at(address) = value;
    if (address == IO::SB) { // for test rom output
        std::print(std::cerr, "{:c}", value);
    }
}

const RomMetadata& MMU::loadRom(const std::filesystem::path& romFile) {
    initialize();
    rom.open(romFile);
    readRomMetadata();
    rom.seekg(0);
    rom.read(reinterpret_cast<char*>(memory.data()), ROM_BANK_SIZE * 2);
    return metadata;
}

void MMU::initialize() {
    memory.fill(0);
    memory.at(IO::IF) = 0b11100000; // upper 3 bits of IF are always 1
}

void MMU::readRomMetadata() {
    rom.seekg(HEADER_START);
    rom.seekg(ENTRYPOINT_SIZE, std::ios::cur); // skip entrypoint instructions
    rom.seekg(NINTENDO_LOGO_SIZE, std::ios::cur); // skip bootrom nintendo logo data
    readInto(rom, metadata.title);
    metadata.manufacturerCode = metadata.title.substr(metadata.title.size() - 5, metadata.manufacturerCode.capacity());
    metadata.cgbFlag = metadata.title.at(metadata.title.size() - 1);
    readInto(rom, metadata.licenseeCode);
    metadata.sgbFlag = rom.get();
    metadata.cartridgeType = rom.get();
    uint8_t encodedSize = rom.get();
    metadata.romSize = getRomSize(encodedSize);
    encodedSize = rom.get();
    metadata.ramSize = getRamSize(encodedSize);
    metadata.destinationCode = rom.get();
    uint8_t oldLicenseeCode = rom.get();
    if (oldLicenseeCode != NEW_LICENSEE_CODE_FLAG) {
        metadata.licenseeCode[0] = oldLicenseeCode;
        metadata.licenseeCode.resize(1);
    }
    metadata.romVersion = rom.get();
    metadata.headerChecksum = rom.get();
    readInto(rom, metadata.globalChecksum);
}