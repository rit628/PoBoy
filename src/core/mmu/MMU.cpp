#include "MMU.hpp"
#include "GraphicsConstants.hpp"
#include "IMU.hpp"
#include "MemoryConstants.hpp"
#include <array>
#include <cstdint>

using namespace Memory;

namespace {
    /* clangd crashes if declared as a static constexpr member of the class */
    constexpr std::array<uint8_t, BOOTROM_SIZE> BOOTROM = {
        #embed "bootix_dmg.bin"
    };
}

MMU::MMU(Interrupts::IMU& imu, Audio::APU& apu, Graphics::PPU& ppu)
        : imu(imu), apu(apu), ppu(ppu)
{
    initialize();
}

void MMU::initialize() {
    wram.fill(0);
    hram.fill(0);
    bootromDisabled = false;
    dmaSourceAddress = 0;
}

void MMU::tick() {
    cartridge.tick();
}

CartridgeMetadata MMU::loadRom(const std::filesystem::path& romFile) {
    return cartridge.loadRom(romFile);
}

uint8_t MMU::read(uint16_t address) {
    if (!bootromDisabled && address < BOOTROM_SIZE) { [[ unlikely ]]
        return BOOTROM.at(address);
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
    if (!bootromDisabled && address < BOOTROM_SIZE) { [[ unlikely ]]
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
        case BANK:  return 0xFE | bootromDisabled;
        case DMA:   return dmaSourceAddress;
        case SB:    return 0xFF;
        case SC:    return 0xFF;
        
        case IF:    return imu.readIO<IF>();
        case DIV:   return imu.readIO<DIV>();
        case TIMA:  return imu.readIO<TIMA>();
        case TMA:   return imu.readIO<TMA>();
        case TAC:   return imu.readIO<TAC>();
        case P1:    return imu.readIO<P1>();

        case NR50:  return apu.readIO<NR50>();
        case NR51:  return apu.readIO<NR51>();
        case NR52:  return apu.readIO<NR52>();
        case NR10:  return apu.readIO<NR10>();
        case NR11:  return apu.readIO<NR11>();
        case NR12:  return apu.readIO<NR12>();
        case NR13:  return apu.readIO<NR13>();
        case NR14:  return apu.readIO<NR14>();
        case NR21:  return apu.readIO<NR21>();
        case NR22:  return apu.readIO<NR22>();
        case NR23:  return apu.readIO<NR23>();
        case NR24:  return apu.readIO<NR24>();
        case NR30:  return apu.readIO<NR30>();
        case NR31:  return apu.readIO<NR31>();
        case NR32:  return apu.readIO<NR32>();
        case NR33:  return apu.readIO<NR33>();
        case NR34:  return apu.readIO<NR34>();
        case NR41:  return apu.readIO<NR41>();
        case NR42:  return apu.readIO<NR42>();
        case NR43:  return apu.readIO<NR43>();
        case NR44:  return apu.readIO<NR44>();

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
    }
    if (WAVEL <= registerAddress && registerAddress <= WAVEH) {
        return apu.readWaveRAM(registerAddress - WAVEL);
    }
    return 0xFF;
}

void MMU::writeIO(uint16_t registerAddress, uint8_t value) {
    switch (registerAddress) {
        case BANK:
            // bootrom can only be unmapped
            bootromDisabled = bootromDisabled || value;
        break;
        case DMA:
            // for now this will be emulated as an instant transfer for simplicity and compatibility with most games
            // timings and bus conflicts can be dealt with later if desired
            dmaSourceAddress = value;
            std::array<uint8_t, Graphics::OAM_SIZE> sourceRange;
            for (size_t i = 0; i < sourceRange.size(); i++) {
                sourceRange.at(i) = read((dmaSourceAddress << 8) | i);
            }
            ppu.dmaTransferOAM(sourceRange);
        break;
        case SB:    break;
        case SC:    break;

        case IF:    return imu.writeIO<IF>(value);
        case DIV:   return imu.writeIO<DIV>(value);
        case TIMA:  return imu.writeIO<TIMA>(value);
        case TMA:   return imu.writeIO<TMA>(value);
        case TAC:   return imu.writeIO<TAC>(value);
        case P1:    return imu.writeIO<P1>(value);

        case NR50:  return apu.writeIO<NR50>(value);
        case NR51:  return apu.writeIO<NR51>(value);
        case NR52:  return apu.writeIO<NR52>(value);
        case NR10:  return apu.writeIO<NR10>(value);
        case NR11:  return apu.writeIO<NR11>(value);
        case NR12:  return apu.writeIO<NR12>(value);
        case NR13:  return apu.writeIO<NR13>(value);
        case NR14:  return apu.writeIO<NR14>(value);
        case NR21:  return apu.writeIO<NR21>(value);
        case NR22:  return apu.writeIO<NR22>(value);
        case NR23:  return apu.writeIO<NR23>(value);
        case NR24:  return apu.writeIO<NR24>(value);
        case NR30:  return apu.writeIO<NR30>(value);
        case NR31:  return apu.writeIO<NR31>(value);
        case NR32:  return apu.writeIO<NR32>(value);
        case NR33:  return apu.writeIO<NR33>(value);
        case NR34:  return apu.writeIO<NR34>(value);
        case NR41:  return apu.writeIO<NR41>(value);
        case NR42:  return apu.writeIO<NR42>(value);
        case NR43:  return apu.writeIO<NR43>(value);
        case NR44:  return apu.writeIO<NR44>(value);

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
    }
    if (WAVEL <= registerAddress && registerAddress <= WAVEH) {
        return apu.writeWaveRAM(registerAddress - WAVEL, value);
    }
}