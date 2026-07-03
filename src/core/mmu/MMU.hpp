#pragma once
#include "MemoryConstants.hpp"
#include "IMU.hpp"
#include "Cartridge.hpp"
#include "PPU.hpp"
#include <array>
#include <cstdint>

namespace Memory {

    class MMU {
        public:
            MMU(Interrupts::IMU& imu, Graphics::PPU& ppu);
    
            void tick(uint8_t tCycles);
            CartridgeMetadata loadRom(const std::filesystem::path& romFile);
            uint8_t read(uint16_t address);
            void write(uint16_t address, uint8_t value);
            
        private:
            uint8_t readIO(uint16_t registerAddress);
            void writeIO(uint16_t registerAddress, uint8_t value);

            static constexpr std::array<uint8_t, BOOTROM_SIZE> bootrom = {
                #embed "bootix_dmg.bin"
            };
    
            Interrupts::IMU& imu;
            Graphics::PPU& ppu;

            Cartridge cartridge;
            std::array<uint8_t, WRAM_SIZE> wram{};
            std::array<uint8_t, HRAM_SIZE> hram{};
            std::array<uint8_t, IO_SIZE> io{};  // for now until all io registers are implemented
            bool bootRomDisabled = false;       // BANK register
            uint8_t dmaSourceAddress = 0;       // DMA register
            
            std::string serialBuffer = "";  // for debugging
    };

}