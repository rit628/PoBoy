#include "Bus.hpp"
#include "GraphicsConstants.hpp"
#include "IMU.hpp"
#include "MemoryConstants.hpp"
#include "SystemConstants.hpp"
#include <array>
#include <cstdint>
#include <filesystem>
#include <fstream>
#include <ranges>

using namespace Memory;

template<MODEL Model>
Bus<Model>::Bus(uint64_t& cycleCount, Cartridge& cartridge, Interrupts::IMU& imu, Audio::APU<Model>& apu, Graphics::PPU<Model>& ppu)
               : cycleCount(cycleCount), cartridge(cartridge), imu(imu), apu(apu), ppu(ppu)
               , wram0(std::span(wram). template subspan<0, WRAM_BANK_SIZE>())
               , wram1(std::span(wram). template subspan<WRAM_BANK_SIZE, WRAM_BANK_SIZE>())
{
    initialize();
}

template<MODEL Model>
void Bus<Model>::initialize() {
    bootrom.fill(0);
    wram.fill(0);
    hram.fill(0);
    bootromDisabled = false;
    dmaSourceAddress = 0;
    
    if constexpr (Model == MODEL::CGB) {
        wramBank = 0;
        vramDmaSource = 0;
        vramDmaDestination = 0;

        hdmaTransferMode = false;
        blocks = 0;

        doubleSpeedMode = false;
        speedSwitchArmed = false;
    }
    else {
        wramBank = 0xFF;
        vramDmaSource = 0xFFFF;
        vramDmaDestination = 0xFFFF;

        hdmaTransferMode = true;
        blocks = 0xFF;

        doubleSpeedMode = true;
        speedSwitchArmed = true;
    }
}

template<MODEL Model>
bool Bus<Model>::loadBootrom() {
    using enum MODEL;
    static constexpr std::string modelPrefix = (Model == DMG) ? "dmg" : "cgb";
    std::filesystem::path bootromFile(modelPrefix + "_boot.bin");
    auto availableBootroms = std::filesystem::directory_iterator(std::filesystem::current_path())
                           | std::views::filter([&](const auto& file) {
                                return file.is_regular_file()
                                    && file.path().extension() == ".bin"
                                    && file.path().filename().string().contains(modelPrefix);
                            });
    
    if (!std::filesystem::exists(bootromFile)) {
        if (availableBootroms.begin() == availableBootroms.end()) return false;
        bootromFile = availableBootroms.begin()->path();
    }
    
    std::ifstream bootromData(bootromFile, std::ios::binary);
    bootromData.read(reinterpret_cast<char*>(bootrom.data()), bootrom.size());
    bootromData.close();
    return true;
}

template<>
void Bus<MODEL::DMG>::initHLE() {
    bootromDisabled = true;     // unmap bootrom
    dmaSourceAddress = 0xFF;    // DMA

    imu.initHLE();
    apu.initHLE();
    ppu.initHLE();
}

template<>
void Bus<MODEL::CGB>::initHLE() {
    bootromDisabled = true;         // unmap bootrom
    dmaSourceAddress = 0x00;        // DMA

    vramDmaSource = 0xFFFF;         // HDMA 1-2
    vramDmaDestination = 0xFFFF;    // HDMA 3-4

    /* HDMA5 */
    hdmaTransferMode = true;
    blocks = 0x7E;

    /* KEY1 */
    doubleSpeedMode = false;
    speedSwitchArmed = false;

    write(SVBK, 0xF8);

    imu.initHLE();
    apu.initHLE();
    ppu.initHLE();
}

template<MODEL Model>
void Bus<Model>::tick(uint8_t tCycles) {
    bool shiftApuDivBit = false;
    if constexpr (Model == MODEL::CGB) shiftApuDivBit = doubleSpeedMode;
    imu.tick();
    apu.tickDivider(shiftApuDivBit);
    for (uint8_t i = 0; i < tCycles; i++) {
        cartridge.tick();
        apu.tick();
        ppu.tick();
        cycleCount++;
    }
}

template<MODEL Model>
void Bus<Model>::tick() {
    if constexpr (Model == MODEL::CGB) {
        if (doubleSpeedMode) tick(2);
        else tick(4);
    }
    else {
        tick(4);
    }
}

template<MODEL Model>
void Bus<Model>::switchSpeed() {
    doubleSpeedMode = !doubleSpeedMode;
    speedSwitchArmed = false;
}

template<MODEL Model>
bool Bus<Model>::inBootromRange(uint16_t address) {
    using enum MODEL;
    if constexpr (Model == DMG) {
        return address < BOOTROM_SIZE;
    }
    else {
        /* cgb bootrom has a hole in its memory map for the cartridge header */
        static constexpr uint16_t HOLE_START = 0x0100;
        static constexpr uint16_t HOLE_END = HOLE_START + 0x0100;
        return address < BOOTROM_SIZE && !(HOLE_START <= address && address < HOLE_END);
    }
}

template<MODEL Model>
uint8_t Bus<Model>::readEchoRam(uint16_t address) {
    using enum MODEL;
    if constexpr (Model == DMG) {
        return wram.at(address);
    }
    else {
        if (address < WRAM_BANK_SIZE) {
            return wram0[address];
        }
        else {
            return wram1[address - WRAM_BANK_SIZE];
        }
    }
}

template<MODEL Model>
void Bus<Model>::writeEchoRam(uint16_t address, uint8_t value) {
    using enum MODEL;
    if constexpr (Model == DMG) {
        wram.at(address) = value;
    }
    else {
        if (address < WRAM_BANK_SIZE) {
            wram0[address] = value;
        }
        else {
            wram1[address - WRAM_BANK_SIZE] = value;
        }
    }
}

template<MODEL Model>
void Bus<Model>::gdmaDispatch() {
    hdmaTransferMode = false;
    while (!hdmaTransferMode) {
        hdmaTransferBlock();
    }
}

template<MODEL Model>
void Bus<Model>::hdmaDispatch() {
    hdmaTransferMode = false;
    ppu.setHdmaCallback(std::bind(&Bus<Model>::hdmaTransferBlock, std::ref(*this)));
}

template<MODEL Model>
void Bus<Model>::hdmaTransferBlock() {
    for (uint16_t i = 0; i < 0x10; i++) {
        auto value = read(vramDmaSource++);
        ppu.writeVRAM(vramDmaDestination++, value);
        tick(2); // cpu is halted during hdma so tick everything else here at 2 t-cycles/byte (might cause issues due to double speed)
    }
    if (blocks-- == 0) {    // end hdma
        blocks &= 0x7F;
        hdmaCancel();
    }
}

template<MODEL Model>
void Bus<Model>::hdmaCancel() {
    hdmaTransferMode = true;
    ppu.setHdmaCallback(nullptr);
}

template<MODEL Model>
uint8_t Bus<Model>::read(uint16_t address) {
    if (!bootromDisabled && inBootromRange(address)) { [[ unlikely ]]
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
    if (address < WRAM_BANK_0_END) {
        return wram0[address - WRAM_BANK_0_START];
    }
    if (address < WRAM_BANK_1_END) {
        return wram1[address - WRAM_BANK_1_START];
    }
    if (address < ECHO_RAM_END) {
        return readEchoRam(address - ECHO_RAM_START);
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

template<MODEL Model>
void Bus<Model>::write(uint16_t address, uint8_t value) {
    if (!bootromDisabled && inBootromRange(address)) { [[ unlikely ]]
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
    if (address < WRAM_BANK_0_END) {
        return void(wram0[address - WRAM_BANK_0_START] = value);
    }
    if (address < WRAM_BANK_1_END) {
        return void(wram1[address - WRAM_BANK_1_START] = value);
    }
    if (address < ECHO_RAM_END) {
        return writeEchoRam(address - ECHO_RAM_START, value);
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

template<MODEL Model>
uint8_t Bus<Model>::readIO(uint16_t registerAddress) {
    switch (registerAddress) {            
        case BANK:  return 0xFE | bootromDisabled;
        case DMA:   return dmaSourceAddress;
        case SVBK:  return 0xF8 | wramBank;
        case HDMA1: return 0xFF;
        case HDMA2: return 0xFF;
        case HDMA3: return 0xFF;
        case HDMA4: return 0xFF;
        case HDMA5: return hdmaTransferMode << 7 | blocks;
        case KEY1:  return 0x7E | doubleSpeedMode << 7 | speedSwitchArmed;

        case SB:    return imu.readIO<SB>();
        case SC:    return imu.readIO<SC>();
        case IF:    return imu.readIO<IF>();
        case DIV:   return imu.readIO<DIV>();
        case TIMA:  return imu.readIO<TIMA>();
        case TMA:   return imu.readIO<TMA>();
        case TAC:   return imu.readIO<TAC>();
        case P1:    return imu.readIO<P1>();

        case NR50:  return apu.template readIO<NR50>();
        case NR51:  return apu.template readIO<NR51>();
        case NR52:  return apu.template readIO<NR52>();
        case NR10:  return apu.template readIO<NR10>();
        case NR11:  return apu.template readIO<NR11>();
        case NR12:  return apu.template readIO<NR12>();
        case NR13:  return apu.template readIO<NR13>();
        case NR14:  return apu.template readIO<NR14>();
        case NR21:  return apu.template readIO<NR21>();
        case NR22:  return apu.template readIO<NR22>();
        case NR23:  return apu.template readIO<NR23>();
        case NR24:  return apu.template readIO<NR24>();
        case NR30:  return apu.template readIO<NR30>();
        case NR31:  return apu.template readIO<NR31>();
        case NR32:  return apu.template readIO<NR32>();
        case NR33:  return apu.template readIO<NR33>();
        case NR34:  return apu.template readIO<NR34>();
        case NR41:  return apu.template readIO<NR41>();
        case NR42:  return apu.template readIO<NR42>();
        case NR43:  return apu.template readIO<NR43>();
        case NR44:  return apu.template readIO<NR44>();

        case LY:    return ppu.template readIO<LY>();
        case LYC:   return ppu.template readIO<LYC>();
        case SCX:   return ppu.template readIO<SCX>();
        case SCY:   return ppu.template readIO<SCY>();
        case WX:    return ppu.template readIO<WX>();
        case WY:    return ppu.template readIO<WY>();
        case LCDC:  return ppu.template readIO<LCDC>();
        case BGP:   return ppu.template readIO<BGP>();
        case OBP0:  return ppu.template readIO<OBP0>();
        case OBP1:  return ppu.template readIO<OBP1>();
        case STAT:  return ppu.template readIO<STAT>();
        case VBK:   return ppu.template readIO<VBK>();
        case BGPI:  return ppu.template readIO<BGPI>();
        case BGPD:  return ppu.template readIO<BGPD>();
        case OBPI:  return ppu.template readIO<OBPI>();
        case OBPD:  return ppu.template readIO<OBPD>();
    }
    if (WAVEL <= registerAddress && registerAddress <= WAVEH) {
        return apu.readWaveRAM(registerAddress - WAVEL);
    }
    return 0xFF;
}

template<MODEL Model>
void Bus<Model>::writeIO(uint16_t registerAddress, uint8_t value) {
    using enum MODEL;
    switch (registerAddress) {
        case BANK:
            // bootrom can only be unmapped
            bootromDisabled = bootromDisabled || value;
        break;
        case DMA: {
            // for now this will be emulated as an instant transfer for simplicity and compatibility with most games
            // timings and bus conflicts can be dealt with later if desired
            dmaSourceAddress = value;
            std::array<uint8_t, Graphics::OAM_SIZE> sourceRange;
            for (size_t i = 0; i < sourceRange.size(); i++) {
                sourceRange.at(i) = read((dmaSourceAddress << 8) | i);
            }
            ppu.dmaTransferOAM(sourceRange);
        }
        break;
        case SVBK:
            if constexpr (Model == CGB) {
                wramBank = std::max(1, value & 0x07);
                wram1 = std::span(wram).subspan(wramBank * WRAM_BANK_SIZE).template first<WRAM_BANK_SIZE>();
            }
        break;
        case HDMA1:
            if constexpr (Model == CGB) {
                vramDmaSource = (vramDmaSource & 0x00FF) | (value << 8);
            }
        break;
        case HDMA2:
            if constexpr (Model == CGB) {
                vramDmaSource = (vramDmaSource & 0xFF00) | (value & 0xF0);  // lower 4 bits ignored
            }
        break;
        case HDMA3:
            if constexpr (Model == CGB) {
                vramDmaDestination = (vramDmaDestination & 0x00FF) | ((value & 0x1F) << 8); // upper 3 bits ignored
            }
        break;
        case HDMA4:
            if constexpr (Model == CGB) {
                vramDmaDestination = (vramDmaDestination & 0xFF00) | (value & 0xF0);  // lower 4 bits ignored
            }
        break;
        case HDMA5: 
            if constexpr (Model == CGB) {
                bool hdmaInProgress = !hdmaTransferMode;
                hdmaTransferMode = value >> 7;
                blocks = value & 0x7F;
                if (hdmaTransferMode) {
                    hdmaDispatch();
                }
                else if (!hdmaInProgress) {
                    gdmaDispatch();
                }
                else {  // cancel hdma
                    hdmaCancel();
                }
            }
        break;
        case KEY1:
            if constexpr (Model == CGB) {
                speedSwitchArmed = value & 0x01;
            }
        break;
        
        case SB:    return imu.writeIO<SB>(value);
        case SC:    return imu.writeIO<SC>(value);
        case IF:    return imu.writeIO<IF>(value);
        case DIV:   return imu.writeIO<DIV>(value);
        case TIMA:  return imu.writeIO<TIMA>(value);
        case TMA:   return imu.writeIO<TMA>(value);
        case TAC:   return imu.writeIO<TAC>(value);
        case P1:    return imu.writeIO<P1>(value);

        case NR50:  return apu.template writeIO<NR50>(value);
        case NR51:  return apu.template writeIO<NR51>(value);
        case NR52:  return apu.template writeIO<NR52>(value);
        case NR10:  return apu.template writeIO<NR10>(value);
        case NR11:  return apu.template writeIO<NR11>(value);
        case NR12:  return apu.template writeIO<NR12>(value);
        case NR13:  return apu.template writeIO<NR13>(value);
        case NR14:  return apu.template writeIO<NR14>(value);
        case NR21:  return apu.template writeIO<NR21>(value);
        case NR22:  return apu.template writeIO<NR22>(value);
        case NR23:  return apu.template writeIO<NR23>(value);
        case NR24:  return apu.template writeIO<NR24>(value);
        case NR30:  return apu.template writeIO<NR30>(value);
        case NR31:  return apu.template writeIO<NR31>(value);
        case NR32:  return apu.template writeIO<NR32>(value);
        case NR33:  return apu.template writeIO<NR33>(value);
        case NR34:  return apu.template writeIO<NR34>(value);
        case NR41:  return apu.template writeIO<NR41>(value);
        case NR42:  return apu.template writeIO<NR42>(value);
        case NR43:  return apu.template writeIO<NR43>(value);
        case NR44:  return apu.template writeIO<NR44>(value);

        case LY:    return ppu.template writeIO<LY>(value);
        case LYC:   return ppu.template writeIO<LYC>(value);
        case SCX:   return ppu.template writeIO<SCX>(value);
        case SCY:   return ppu.template writeIO<SCY>(value);
        case WX:    return ppu.template writeIO<WX>(value);
        case WY:    return ppu.template writeIO<WY>(value);
        case LCDC:  return ppu.template writeIO<LCDC>(value);
        case BGP:   return ppu.template writeIO<BGP>(value);
        case OBP0:  return ppu.template writeIO<OBP0>(value);
        case OBP1:  return ppu.template writeIO<OBP1>(value);
        case STAT:  return ppu.template writeIO<STAT>(value);
        case VBK:   return ppu.template writeIO<VBK>(value);
        case BGPI:  return ppu.template writeIO<BGPI>(value);
        case BGPD:  return ppu.template writeIO<BGPD>(value);
        case OBPI:  return ppu.template writeIO<OBPI>(value);
        case OBPD:  return ppu.template writeIO<OBPD>(value);
    }
    if (WAVEL <= registerAddress && registerAddress <= WAVEH) {
        return apu.writeWaveRAM(registerAddress - WAVEL, value);
    }
}

template class Memory::Bus<MODEL::DMG>;
template class Memory::Bus<MODEL::CGB>;