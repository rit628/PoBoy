#pragma once
#include "APU.hpp"
#include "MemoryConstants.hpp"
#include "IMU.hpp"
#include "Cartridge.hpp"
#include "PPU.hpp"
#include <array>
#include <cstdint>

namespace Memory {

    class MMU {
        public:
            MMU(Interrupts::IMU& imu, Audio::APU& apu, Graphics::PPU& ppu);
            void initialize();
    
            void tick();
            CartridgeMetadata loadRom(const std::filesystem::path& romFile);
            uint8_t read(uint16_t address);
            void write(uint16_t address, uint8_t value);
            
        private:
            uint8_t readIO(uint16_t registerAddress);
            void writeIO(uint16_t registerAddress, uint8_t value);
    
            Interrupts::IMU& imu;
            Audio::APU& apu;
            Graphics::PPU& ppu;

            Cartridge cartridge;
            std::array<uint8_t, WRAM_SIZE> wram;
            std::array<uint8_t, HRAM_SIZE> hram;
            bool bootromDisabled;       // BANK register
            uint8_t dmaSourceAddress;   // DMA register
    };

}