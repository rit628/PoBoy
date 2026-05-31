#include "MMU.hpp"
#include <cstdint>

uint8_t& MMU::operator[](uint16_t address) {
    if (readFromBootRom && address < BOOTROM_SIZE) {
        return const_cast<uint8_t&>(bootrom.at(address));
    }
    return memory.at(address);
}

const RomMetadata& MMU::loadRom(const std::filesystem::path& romFile) {
    rom.open(romFile);
    readRomMetadata();
    rom.seekg(0);
    rom.read(reinterpret_cast<char*>(memory.data()), ROM_BANK_SIZE * 2);
    return metadata;
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