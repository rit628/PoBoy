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
        
        /* some important addresses and constants */
        static constexpr uint8_t READ_ERROR = 0xFF;

        static constexpr uint16_t MEMORY_SIZE   = 0xFFFF;
        static constexpr uint16_t BOOTROM_SIZE  = 0x0100;
        static constexpr uint16_t ROM_BANK_SIZE = 0x4000;

        static constexpr uint16_t SB    = 0xFF01; // serial transfer data
        static constexpr uint16_t LY    = 0xFF44; // lcd y coordinate
        static constexpr uint16_t BOOT  = 0xFF50; // boot rom lock register
        static constexpr uint16_t IE    = 0xFFFF; // interrupt enable
        static constexpr uint16_t IF    = 0xFF0F; // interrupt flag

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