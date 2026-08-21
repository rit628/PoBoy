#pragma once
#include "APU.hpp"
#include "MemoryConstants.hpp"
#include "IMU.hpp"
#include "Cartridge.hpp"
#include "PPU.hpp"
#include "SystemConstants.hpp"
#include <array>
#include <cstdint>
#include <span>

namespace Memory {

    template<MODEL Model>
    class Bus {
        public:
            Bus(uint64_t& cycleCount, Cartridge& cartridge, Interrupts::IMU& imu, Audio::APU<Model>& apu, Graphics::PPU<Model>& ppu);
            void initialize();
            bool loadBootrom();
            void initHLE();
            void tick(uint8_t tCycles);
            void tick();
            void switchSpeed();
    
            uint8_t read(uint16_t address);
            void write(uint16_t address, uint8_t value);
            
        private:
            bool inBootromRange(uint16_t address);
            uint8_t readEchoRam(uint16_t address);
            void writeEchoRam(uint16_t address, uint8_t value);
            void gdmaDispatch();
            void hdmaDispatch();
            void hdmaTransferBlock();
            uint8_t readIO(uint16_t registerAddress);
            void writeIO(uint16_t registerAddress, uint8_t value);

            static constexpr uint16_t BOOTROM_SIZE = 0x0100 + (Model == MODEL::CGB) * 0x0800;
            static constexpr uint16_t WRAM_SIZE    = WRAM_BANK_SIZE * (2 + 6 * (Model == MODEL::CGB));
    
            uint64_t& cycleCount;

            Cartridge& cartridge;
            Interrupts::IMU& imu;
            Audio::APU<Model>& apu;
            Graphics::PPU<Model>& ppu;

            std::array<uint8_t, BOOTROM_SIZE> bootrom;
            std::array<uint8_t, WRAM_SIZE> wram;
            std::array<uint8_t, HRAM_SIZE> hram;
            std::span<uint8_t, WRAM_BANK_SIZE> wram0, wram1;
            bool bootromDisabled;           // BANK register
            uint8_t dmaSourceAddress;       // DMA register
            /* CGB registers */
            uint8_t wramBank;               // SVBK register            
            uint16_t vramDmaSource;         // HDMA1 and HDMA2 registers
            uint16_t vramDmaDestination;    // HDMA3 and HDMA4 registers
            /* HDMA5 register components */
            bool hdmaTransferMode;          // HMDA5 bit 7
            uint8_t blocks;                 // HDMA5 bits 6-0
            /*  KEY1 register components */
            bool doubleSpeedMode;           // KEY1 bit 7
            bool speedSwitchArmed;          // KEY1 bit 0
    };

}