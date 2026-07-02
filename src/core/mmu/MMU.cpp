#include "MMU.hpp"
#include "GraphicsConstants.hpp"
#include "IMU.hpp"
#include "MemoryConstants.hpp"
#include <array>
#include <cstdint>
#include <iostream>
#include <print>
#include <ranges>

using namespace Memory;

MMU::MMU(Interrupts::IMU& imu, Graphics::PPU& ppu)
        : imu(imu), ppu(ppu) {}

CartridgeMetadata MMU::loadRom(const std::filesystem::path& romFile) {
    return cartridge.loadRom(romFile);
}

uint8_t MMU::read(uint16_t address) {
    if (!bootRomDisabled && address < BOOTROM_SIZE) {
        return bootrom.at(address);
    }
    if (address < ROM_BANK_0_END) {
        return cartridge.readBank0(address - ROM_BANK_0_START);
    }
    if (address < ROM_BANK_1_END) {
        return cartridge.readBank1(address - ROM_BANK_1_START);
    }
    if (address < VRAM_END) {
        return ppu.readVRAM(address - VRAM_START);
    }
    if (address < CARTRIDGE_RAM_END) {
        return cartridge.readSRAM(address - CARTRIDGE_RAM_START);
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
        return readIO(address);
    }
    if (address < HRAM_END) {
        return hram.at(address - HRAM_START);
    }
    return imu.readIE();
}

void MMU::write(uint16_t address, uint8_t value) {
    if (!bootRomDisabled && address < BOOTROM_SIZE) {
        return; // bootrom is not writeable
    }
    if (address < ROM_BANK_0_END) {
        cartridge.writeBank0(address - ROM_BANK_0_START, value);
    }
    else if (address < ROM_BANK_1_END) {
        cartridge.writeBank1(address - ROM_BANK_1_START, value);
    }
    else if (address < VRAM_END) {
        ppu.writeVRAM(address - VRAM_START, value);
    }
    else if (address < CARTRIDGE_RAM_END) {
        cartridge.writeSRAM(address - CARTRIDGE_RAM_START, value);
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
        writeIO(address, value);
    }
    else if (address < HRAM_END) {
        hram.at(address - HRAM_START) = value;
    }
    else {
        imu.writeIE(value);
    }
}

uint8_t MMU::readIO(uint16_t registerAddress) {
    switch (registerAddress) {            
        case BANK:
            return bootRomDisabled;
        break;

        case DMA:
            return dmaSourceAddress;
        break;

        case SB:
            return 0xFF;
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

        case LY:
            return ppu.readLY();
        break;

        case LYC:
            return ppu.readLYC();
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

        case OBP0:
            return ppu.readOBP0();
        break;

        case OBP1:
            return ppu.readOBP1();
        break;

        case STAT:
            return ppu.readSTAT();
        break;

        case P1:
            return imu.readP1();
        break;

        default:
            return io.at(registerAddress - IO_START);
        break;
    }
}

void MMU::writeIO(uint16_t registerAddress, uint8_t value) {
    switch (registerAddress) {
        case BANK:
            // bootrom can only be unmapped
            bootRomDisabled = bootRomDisabled || value;
        break;

        case DMA:
            // for now this will be emulated as an instant transfer for simplicity and compatibility with most games
            // timings and bus conflicts can be dealt with later if desired
            dmaSourceAddress = value;
            std::array<uint8_t, Graphics::OAM_SIZE> sourceRange;
            for (auto&& [i, byte] : std::ranges::views::enumerate(sourceRange)) {
                byte = read((dmaSourceAddress << 8) | i);
            } 
            ppu.dmaTransferOAM(sourceRange);
        break;

        case SB:
            serialBuffer += static_cast<char>(std::min(uint8_t(0x7F), value));
        break;

        case SC:
            std::print(std::cerr, "{}", serialBuffer);
            serialBuffer.clear();
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

        case LYC:
            ppu.writeLYC(value);
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

        case OBP0:
            ppu.writeOBP0(value);
        break;

        case OBP1:
            ppu.writeOBP1(value);
        break;

        case STAT:
            ppu.writeSTAT(value);
        break;

        case P1:
            imu.writeP1(value);
        break;

        default:
            io.at(registerAddress - IO_START) = value;
        break;
    }
}