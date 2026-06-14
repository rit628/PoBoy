#pragma once
#include "RomMetadata.hpp"
#include "IMU.hpp"
#include "PPU.hpp"
#include <array>
#include <cstdint>
#include <fstream>
#include <filesystem>

class MMU {
    public:
        MMU(IMU& imu, Graphics::PPU& ppu);

        const RomMetadata& loadRom(const std::filesystem::path& romFile);
        uint8_t read(uint16_t address);
        void write(uint16_t address, uint8_t value);
        
        static constexpr uint8_t READ_ERROR = 0xFF;

        static constexpr uint32_t MEMORY_SIZE   = 0x10000;
        static constexpr uint16_t BOOTROM_SIZE  = 0x0100;
        static constexpr uint16_t ROM_BANK_SIZE = 0x4000;
        static constexpr uint16_t CART_RAM_SIZE = 0x2000;
        static constexpr uint16_t WRAM_SIZE     = 0x2000;
        static constexpr uint8_t  IO_SIZE       = 0x80;
        static constexpr uint8_t  HRAM_SIZE     = 0x7F;

        static constexpr uint16_t ROM_BANKS_START       = 0;
        static constexpr uint16_t ROM_BANKS_END         = ROM_BANK_SIZE * 2;
        static constexpr uint16_t VRAM_START            = ROM_BANKS_END;
        static constexpr uint16_t VRAM_END              = VRAM_START + Graphics::VRAM_SIZE;
        static constexpr uint16_t CARTRIDGE_RAM_START   = VRAM_END;
        static constexpr uint16_t CARTRIDGE_RAM_END     = CARTRIDGE_RAM_START + CART_RAM_SIZE;
        static constexpr uint16_t WRAM_START            = CARTRIDGE_RAM_END;
        static constexpr uint16_t WRAM_END              = WRAM_START + WRAM_SIZE;
        static constexpr uint16_t ECHO_RAM_START        = WRAM_END;
        static constexpr uint16_t ECHO_RAM_END          = ECHO_RAM_START + WRAM_SIZE - 0X0200;
        static constexpr uint16_t OAM_START             = ECHO_RAM_END;
        static constexpr uint16_t OAM_END               = OAM_START + Graphics::OAM_SIZE;
        static constexpr uint16_t PROHIBITED_START      = OAM_END;
        static constexpr uint16_t PROHIBITED_END        = PROHIBITED_START + 0x60;
        static constexpr uint16_t IO_START              = PROHIBITED_END;
        static constexpr uint16_t IO_END                = IO_START + IO_SIZE;
        static constexpr uint16_t HRAM_START            = IO_END;
        static constexpr uint16_t HRAM_END              = HRAM_START + HRAM_SIZE;
        
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
        std::array<uint8_t, IO_SIZE> io{}; // for now until all io registers are implemented
        bool bootRomDisabled = false;   // BANK register
};