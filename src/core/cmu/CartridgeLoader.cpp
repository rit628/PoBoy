#include "CartridgeLoader.hpp"
#include "HeaderInfo.hpp"
#include <filesystem>

void CartridgeLoader::loadCartridge(const std::filesystem::path& romFile) {
    rom.open(romFile);
    readRomMetadata();
}

void CartridgeLoader::readRomMetadata() {
    rom.seekg(HEADER_START);
    rom.seekg(ENTRYPOINT_SIZE, std::ios::cur); // skip entrypoint instructions
    rom.seekg(NINTENDO_LOGO_SIZE, std::ios::cur); // skip bootrom nintendo logo data
    readInto(rom, title);
    manufacturerCode = title.substr(title.size() - 5, manufacturerCode.capacity());
    cgbFlag = title.at(title.size() - 1);
    readInto(rom, licenseeCode);
    sgbFlag = rom.get();
    cartridgeType = rom.get();
    uint8_t encodedSize = rom.get();
    romSize = getRomSize(encodedSize);
    encodedSize = rom.get();
    ramSize = getRamSize(encodedSize);
    destinationCode = rom.get();
    uint8_t oldLicenseeCode = rom.get();
    if (oldLicenseeCode != NEW_LICENSEE_CODE_FLAG) {
        licenseeCode[0] = oldLicenseeCode;
        licenseeCode.resize(1);
    }
    romVersion = rom.get();
    headerChecksum = rom.get();
    readInto(rom, globalChecksum);
}