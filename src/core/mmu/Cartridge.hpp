#pragma once
#include "MBC.hpp"
#include "MemoryConstants.hpp"
#include <cstdint>
#include <fstream>
#include <filesystem>
#include <vector>

namespace Memory {

    class Cartridge {
        public:
            CartridgeMetadata loadRom(const std::filesystem::path& romFile);

            uint8_t readBank0(uint16_t address);
            void writeBank0(uint16_t address, uint8_t value);
            uint8_t readBank1(uint16_t address);
            void writeBank1(uint16_t address, uint8_t value);
            uint8_t readSRAM(uint16_t address);
            void writeSRAM(uint16_t address, uint8_t value);
    
        private:
            CartridgeMetadata readRomMetadata(std::ifstream& romData);
            void setMBC(MBC_TYPE mbcType, uint8_t encodedRomSize, uint8_t encodedRamSize);

            std::vector<uint8_t> rom = std::vector<uint8_t>(2 * ROM_BANK_SIZE, 0xFF);
            MemoryBankController mbc = MBC0<>(rom, 0, 0);

    };

}
