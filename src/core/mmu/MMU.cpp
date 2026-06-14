#include "MMU.hpp"
#include "IMU.hpp"
#include "IO.hpp"
#include <cstdint>
#include <iostream>
#include <print>

MMU::MMU(IMU& imu, Graphics::PPU& ppu) : imu(imu), ppu(ppu) {}

uint8_t MMU::read(uint16_t address) {
    if (address < ROM_BANKS_END) {
        if (!bootRomDisabled && address < BOOTROM_SIZE) return bootrom.at(address);
        return romBanks.at(address - ROM_BANKS_START);
    }
    if (address < VRAM_END) {
        return ppu.readVRAM(address - VRAM_START);
    }
    if (address < CARTRIDGE_RAM_END) {
        return cram.at(address - CARTRIDGE_RAM_START);
    }
    if (address < WRAM_END) {
        return wram.at(address - WRAM_START);
    }
    if (address < ECHO_RAM_END) {
        return wram.at(address - ECHO_RAM_START);
    }
    if (address < OAM_END) {
        return ppu.readOAM(address - OAM_START);
    }
    if (address < PROHIBITED_END) {
        return READ_ERROR; // oam bug
    }
    if (address < IO_END) {
        switch (address) {
            case IO::BANK:
                return bootRomDisabled;
            break;

            case IO::LY:
                return ppu.readLY();
            break;

            case IO::IF:
                return imu.readIF();
            break;
            
            case IO::DIV:
                return imu.readDIV();
            break;

            case IO::TIMA:
                return imu.readTIMA();
            break;

            case IO::TMA:
                return imu.readTMA();
            break;

            case IO::TAC:
                return imu.readTAC();
            break;

            case IO::SCX:
                return ppu.readSCX();
            break;

            case IO::SCY:
                return ppu.readSCY();
            break;

            case IO::LCDC:
                return ppu.readLCDC();
            break;

            case IO::BGP:
                return ppu.readBGP();
            break;

            default:
                return io.at(address - IO_START);
            break;
        }
    }
    if (address < HRAM_END) {
        return hram.at(address - HRAM_START);
    }
    return imu.readIE();
}

void MMU::write(uint16_t address, uint8_t value) {
    if (address < ROM_BANKS_END) {
        return; // ROM is not writeable
    }
    else if (address < VRAM_END) {
        ppu.writeVRAM(address - VRAM_START, value);
    }
    else if (address < CARTRIDGE_RAM_END) {
        cram.at(address - CARTRIDGE_RAM_START) = value;
    }
    else if (address < WRAM_END) {
        wram.at(address - WRAM_START) = value;
    }
    else if (address < ECHO_RAM_END) {
        wram.at(address - ECHO_RAM_START) = value;
    }
    else if (address < OAM_END) {
        ppu.writeOAM(address - OAM_START, value);
    }
    else if (address < PROHIBITED_END) {
        return; // oam bug
    }
    else if (address < IO_END) {
        switch (address) {
            case IO::BANK:
                // bootrom can only be unmapped
                bootRomDisabled = bootRomDisabled || value;
            break;

            case IO::SB:
                std::print(std::cerr, "{:c}", value);
            break;

            case IO::IF:
                imu.writeIF(value);
            break;

            case IO::DIV:
                imu.writeDIV(value);
            break;

            case IO::TIMA:
                imu.writeTIMA(value);
            break;

            case IO::TMA:
                imu.writeTMA(value);
            break;

            case IO::TAC:
                imu.writeTAC(value);
            break;

            case IO::SCX:
                ppu.writeSCX(value);
            break;

            case IO::SCY:
                ppu.writeSCY(value);
            break;

            case IO::LCDC:
                ppu.writeLCDC(value);
            break;

            case IO::BGP:
                ppu.writeBGP(value);
            break;

            default:
                io.at(address - IO_START) = value;
            break;
        }
    }
    else if (address < HRAM_END) {
        hram.at(address - HRAM_START) = value;
    }
    else {
        imu.writeIE(value);
    }
}

const RomMetadata& MMU::loadRom(const std::filesystem::path& romFile) {
    rom.open(romFile);
    readRomMetadata();
    rom.seekg(0);
    rom.read(reinterpret_cast<char*>(romBanks.data()), ROM_BANK_SIZE * 2);
    bootRomDisabled = false;
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