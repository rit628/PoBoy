#pragma once
#include "MemoryConstants.hpp"
#include "RomMetadata.hpp"
#include "IMU.hpp"
#include "PPU.hpp"
#include <array>
#include <cstdint>
#include <fstream>
#include <filesystem>

namespace Memory {

    class MMU {
        public:
            MMU(IMU& imu, Graphics::PPU& ppu);
    
            const RomMetadata& loadRom(const std::filesystem::path& romFile);
            uint8_t read(uint16_t address);
            void write(uint16_t address, uint8_t value);
            
        private:
            void readRomMetadata();
    
            static constexpr std::array<uint8_t, BOOTROM_SIZE> bootrom = {
                #embed "bootix_dmg.bin"
            };
    
            IMU& imu;
            Graphics::PPU& ppu;
    
            std::ifstream rom;
            RomMetadata metadata;
            std::array<uint8_t, 2 * ROM_BANK_SIZE> romBanks{};
            std::array<uint8_t, CART_RAM_SIZE> cram{};
            std::array<uint8_t, WRAM_SIZE> wram{};
            std::array<uint8_t, HRAM_SIZE> hram{};
            std::array<uint8_t, IO_SIZE> io{};  // for now until all io registers are implemented
            bool bootRomDisabled = false;       // BANK register
    };

}