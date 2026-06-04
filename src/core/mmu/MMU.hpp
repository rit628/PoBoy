#pragma once
#include "RomMetadata.hpp"
#include <array>
#include <cstdint>
#include <fstream>
#include <filesystem>

class MMU {
    public:
        const RomMetadata& loadRom(const std::filesystem::path& romFile);
        uint8_t read(uint16_t address);
        void write(uint16_t address, uint8_t value);
        
        static constexpr uint8_t READ_ERROR = 0xFF;

        static constexpr uint16_t MEMORY_SIZE   = 0xFFFF;
        static constexpr uint16_t BOOTROM_SIZE  = 0x0100;
        static constexpr uint16_t ROM_BANK_SIZE = 0x4000;

        /* embedded binaries */
        static constexpr std::array<uint8_t, BOOTROM_SIZE> bootrom = {
            #embed "bootix_dmg.bin"
        };

    private:
        void initialize();
        void readRomMetadata();

        std::ifstream rom;
        RomMetadata metadata;
        std::array<uint8_t, MEMORY_SIZE> memory{};
};