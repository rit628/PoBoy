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

MMU::MMU(Interrupts::IMU& imu, Audio::APU& apu, Graphics::PPU& ppu)
        : imu(imu), apu(apu), ppu(ppu) {}

void MMU::tick() {
    cartridge.tick();
}

CartridgeMetadata MMU::loadRom(const std::filesystem::path& romFile) {
    io.fill(0xFF);
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
    return imu.readIO<IE>();
}

void MMU::write(uint16_t address, uint8_t value) {
    if (!bootRomDisabled && address < BOOTROM_SIZE) {
        return; // bootrom is not writeable
    }
    if (address < ROM_BANK_0_END) {
        return cartridge.writeBank0(address - ROM_BANK_0_START, value);
    }
    if (address < ROM_BANK_1_END) {
        return cartridge.writeBank1(address - ROM_BANK_1_START, value);
    }
    if (address < VRAM_END) {
        return ppu.writeVRAM(address - VRAM_START, value);
    }
    if (address < CARTRIDGE_RAM_END) {
        return cartridge.writeSRAM(address - CARTRIDGE_RAM_START, value);
    }
    if (address < WRAM_END) {
        return void(wram.at(address - WRAM_START) = value);
    }
    if (address < ECHO_RAM_END) {
        return void(wram.at(address - ECHO_RAM_START) = value);
    }
    if (address < OAM_END) {
        return ppu.writeOAM(address - OAM_START, value);
    }
    if (address < PROHIBITED_END) {
        return; // oam bug
    }
    if (address < IO_END) {
        return writeIO(address, value);
    }
    if (address < HRAM_END) {
        return void(hram.at(address - HRAM_START) = value);
    }
    return imu.writeIO<IE>(value);
}

uint8_t MMU::readIO(uint16_t registerAddress) {
    switch (registerAddress) {            
        case BANK:  return bootRomDisabled;
        case DMA:   return dmaSourceAddress;
        case SB:    return 0xFF;
        
        case IF:    return imu.readIO<IF>();
        case DIV:   return imu.readIO<DIV>();
        case TIMA:  return imu.readIO<TIMA>();
        case TMA:   return imu.readIO<TMA>();
        case TAC:   return imu.readIO<TAC>();
        case P1:    return imu.readIO<P1>();

        case NR50:  return apu.readIO<NR50>();
        case NR51:  return apu.readIO<NR51>();
        case NR52:  return apu.readIO<NR52>();
        case NR21:  return apu.readIO<NR21>();
        case NR22:  return apu.readIO<NR22>();
        case NR23:  return apu.readIO<NR23>();
        case NR24:  return apu.readIO<NR24>();

        case LY:    return ppu.readIO<LY>();
        case LYC:   return ppu.readIO<LYC>();
        case SCX:   return ppu.readIO<SCX>();
        case SCY:   return ppu.readIO<SCY>();
        case WX:    return ppu.readIO<WX>();
        case WY:    return ppu.readIO<WY>();
        case LCDC:  return ppu.readIO<LCDC>();
        case BGP:   return ppu.readIO<BGP>();
        case OBP0:  return ppu.readIO<OBP0>();
        case OBP1:  return ppu.readIO<OBP1>();
        case STAT:  return ppu.readIO<STAT>();

        default:    return io.at(registerAddress - IO_START);
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

        case IF:    return imu.writeIO<IF>(value);
        case DIV:   return imu.writeIO<DIV>(value);
        case TIMA:  return imu.writeIO<TIMA>(value);
        case TMA:   return imu.writeIO<TMA>(value);
        case TAC:   return imu.writeIO<TAC>(value);
        case P1:    return imu.writeIO<P1>(value);

        case NR50:  return apu.writeIO<NR50>(value);
        case NR51:  return apu.writeIO<NR51>(value);
        case NR52:  return apu.writeIO<NR52>(value);
        case NR21:  return apu.writeIO<NR21>(value);
        case NR22:  return apu.writeIO<NR22>(value);
        case NR23:  return apu.writeIO<NR23>(value);
        case NR24:  return apu.writeIO<NR24>(value);

        case LYC:   return ppu.writeIO<LYC>(value);
        case SCX:   return ppu.writeIO<SCX>(value);
        case SCY:   return ppu.writeIO<SCY>(value);
        case WX:    return ppu.writeIO<WX>(value);
        case WY:    return ppu.writeIO<WY>(value);
        case LCDC:  return ppu.writeIO<LCDC>(value);
        case BGP:   return ppu.writeIO<BGP>(value);
        case OBP0:  return ppu.writeIO<OBP0>(value);
        case OBP1:  return ppu.writeIO<OBP1>(value);
        case STAT:  return ppu.writeIO<STAT>(value);

        default:    return void(io.at(registerAddress - IO_START) = value);
    }
}