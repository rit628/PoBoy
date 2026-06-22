#include "MMU.hpp"
#include "IMU.hpp"
#include "MemoryConstants.hpp"
#include <cstdint>
#include <iostream>
#include <print>

using namespace Memory;

MMU::MMU(Interrupts::IMU& imu, Graphics::PPU& ppu) : imu(imu), ppu(ppu) {}

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
        return 0xFF; // oam bug
    }
    if (address < IO_END) {
        switch (address) {
            case P1:
                return 0xFF;
            break;
            
            case BANK:
                return bootRomDisabled;
            break;

            case LY:
                return ppu.readLY();
            break;

            case LYC:
                return ppu.readLYC();
            break;

            case IF:
                return imu.readIF();
            break;
            
            case DIV:
                return imu.readDIV();
            break;

            case TIMA:
                return imu.readTIMA();
            break;

            case TMA:
                return imu.readTMA();
            break;

            case TAC:
                return imu.readTAC();
            break;

            case SCX:
                return ppu.readSCX();
            break;

            case SCY:
                return ppu.readSCY();
            break;

            case WX:
                return ppu.readWX();
            break;

            case WY:
                return ppu.readWY();
            break;

            case LCDC:
                return ppu.readLCDC();
            break;

            case BGP:
                return ppu.readBGP();
            break;

            case STAT:
                return ppu.readSTAT();
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
            case BANK:
                // bootrom can only be unmapped
                bootRomDisabled = bootRomDisabled || value;
            break;

            case SB:
                // for debugging
                std::print(std::cerr, "{:c}", std::min(uint8_t(0x7F), value));
            break;

            case LYC:
                ppu.writeLYC(value);
            break;

            case IF:
                imu.writeIF(value);
            break;

            case DIV:
                imu.writeDIV(value);
            break;

            case TIMA:
                imu.writeTIMA(value);
            break;

            case TMA:
                imu.writeTMA(value);
            break;

            case TAC:
                imu.writeTAC(value);
            break;

            case SCX:
                ppu.writeSCX(value);
            break;

            case SCY:
                ppu.writeSCY(value);
            break;

            case WX:
                ppu.writeWX(value);
            break;

            case WY:
                ppu.writeWY(value);
            break;

            case LCDC:
                ppu.writeLCDC(value);
            break;

            case BGP:
                ppu.writeBGP(value);
            break;

            case STAT:
                ppu.writeSTAT(value);
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