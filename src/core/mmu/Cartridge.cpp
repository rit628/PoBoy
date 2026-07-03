#include "Cartridge.hpp"
#include "MBC.hpp"
#include "MemoryConstants.hpp"
#include <cstdint>
#include <variant>

using namespace Memory;

template<class... Ts>
struct overloaded : Ts... { using Ts::operator()...; };

void Cartridge::tick() {
    using enum MBC_HARDWARE;
    std::visit(overloaded {
        []<SRAM_TYPE R> (MBC3<R, RTC>&& mbc) { mbc.tick(); },
        [](auto&&) {}
    }, mbc);
}

CartridgeMetadata Cartridge::loadRom(const std::filesystem::path& romFile) {
    std::ifstream romData(romFile);
    auto metadata = readRomMetadata(romData);
    auto romSize = decodeRomSize(metadata.encodedRomSize);
    rom.resize(romSize);
    romData.seekg(0);
    romData.read(reinterpret_cast<char*>(rom.data()), romSize);
    romData.close();
    setMBC(metadata.cartridgeType, metadata.encodedRomSize, metadata.encodedRamSize);
    return metadata;
}

CartridgeMetadata Cartridge::readRomMetadata(std::ifstream& romData) {
    CartridgeMetadata metadata;
    romData.seekg(CARTRIDGE_HEADER_START + ENTRYPOINT_SIZE + NINTENDO_LOGO_SIZE);
    readInto(romData, metadata.title);
    metadata.manufacturerCode = metadata.title.substr(metadata.title.size() - 5, metadata.manufacturerCode.capacity());
    metadata.cgbFlag = metadata.title.at(metadata.title.size() - 1);
    readInto(romData, metadata.licenseeCode);
    metadata.sgbFlag = romData.get();
    metadata.cartridgeType = static_cast<MBC_TYPE>(romData.get());
    metadata.encodedRomSize = romData.get();
    metadata.encodedRamSize = romData.get();
    metadata.destinationCode = romData.get();
    uint8_t oldLicenseeCode = romData.get();
    if (oldLicenseeCode != NEW_LICENSEE_CODE_FLAG) {
        metadata.licenseeCode.at(0) = static_cast<char>(oldLicenseeCode);
        metadata.licenseeCode.resize(1);
    }
    metadata.romVersion = romData.get();
    metadata.headerChecksum = romData.get();
    readInto(romData, metadata.globalChecksum);
    return metadata;
}

void Cartridge::setMBC(MBC_TYPE mbcType, uint8_t encodedRomSize, uint8_t encodedRamSize) {
    switch (mbcType) {
        case MBC_TYPE::MBC0:
            mbc.emplace<MBC0<>>(rom, encodedRomSize, encodedRamSize);
        break;
        case MBC_TYPE::MBC0_RAM:
            mbc.emplace<MBC0<SRAM_TYPE::UNBUFFERED>>(rom, encodedRomSize, encodedRamSize);
        break;
        case MBC_TYPE::MBC0_RAM_BATTERY:
            mbc.emplace<MBC0<SRAM_TYPE::BATTERY_BUFFERED>>(rom, encodedRomSize, encodedRamSize);
        break;

        case MBC_TYPE::MBC1:
            mbc.emplace<MBC1<>>(rom, encodedRomSize, encodedRamSize);
        break;
        case MBC_TYPE::MBC1_RAM:
            mbc.emplace<MBC1<SRAM_TYPE::UNBUFFERED>>(rom, encodedRomSize, encodedRamSize);
        break;
        case MBC_TYPE::MBC1_RAM_BATTERY:
            mbc.emplace<MBC1<SRAM_TYPE::BATTERY_BUFFERED>>(rom, encodedRomSize, encodedRamSize);
        break;
        
        case MBC_TYPE::MBC3:
            mbc.emplace<MBC3<>>(rom, encodedRomSize, encodedRamSize);
        break;
        case MBC_TYPE::MBC3_RAM:
            mbc.emplace<MBC3<SRAM_TYPE::UNBUFFERED>>(rom, encodedRomSize, encodedRamSize);
        break;
        case MBC_TYPE::MBC3_RAM_BATTERY:
            mbc.emplace<MBC3<SRAM_TYPE::BATTERY_BUFFERED>>(rom, encodedRomSize, encodedRamSize);
        break;
        case MBC_TYPE::MBC3_RTC:
            mbc.emplace<MBC3<SRAM_TYPE::NONE, MBC_HARDWARE::RTC>>(rom, encodedRomSize, encodedRamSize);
        break;
        case MBC_TYPE::MBC3_RTC_RAM_BATTERY:
            mbc.emplace<MBC3<SRAM_TYPE::BATTERY_BUFFERED, MBC_HARDWARE::RTC>>(rom, encodedRomSize, encodedRamSize);
        break;

        case MBC_TYPE::MBC5:
            mbc.emplace<MBC5<>>(rom, encodedRomSize, encodedRamSize);
        break;
        case MBC_TYPE::MBC5_RAM:
            mbc.emplace<MBC5<SRAM_TYPE::UNBUFFERED>>(rom, encodedRomSize, encodedRamSize);
        break;
        case MBC_TYPE::MBC5_RAM_BATTERY:
            mbc.emplace<MBC5<SRAM_TYPE::BATTERY_BUFFERED>>(rom, encodedRomSize, encodedRamSize);
        break;
        case MBC_TYPE::MBC5_RUMBLE:
            mbc.emplace<MBC5<SRAM_TYPE::NONE, MBC_HARDWARE::RUMBLE>>(rom, encodedRomSize, encodedRamSize);
        break;
        case MBC_TYPE::MBC5_RUMBLE_RAM_BATTERY:
            mbc.emplace<MBC5<SRAM_TYPE::BATTERY_BUFFERED, MBC_HARDWARE::RUMBLE>>(rom, encodedRomSize, encodedRamSize);
        break;

        default: break;
    }
}

uint8_t Cartridge::readBank0(uint16_t address) {
    return std::visit([=](auto&& mbc) { return mbc.readBank0(address); }, mbc);
}

void Cartridge::writeBank0(uint16_t address, uint8_t value) {
    std::visit([=](auto&& mbc) { return mbc.writeBank0(address, value); }, mbc);
}

uint8_t Cartridge::readBank1(uint16_t address) {
    return std::visit([=](auto&& mbc) { return mbc.readBank1(address); }, mbc);
}

void Cartridge::writeBank1(uint16_t address, uint8_t value) {
    std::visit([=](auto&& mbc) { return mbc.writeBank1(address, value); }, mbc);
}

uint8_t Cartridge::readSRAM(uint16_t address) {
    return std::visit([=](auto&& mbc) { return mbc.readSRAM(address); }, mbc);
}

void Cartridge::writeSRAM(uint16_t address, uint8_t value) {
    std::visit([=](auto&& mbc) { return mbc.writeSRAM(address, value); }, mbc);
}
