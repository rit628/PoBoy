#pragma once
#include "GraphicsConstants.hpp"
#include "PixelMixer.hpp"
#include <array>
#include <cstdint>
#include <span>

namespace Graphics {

    class PPU {
        public:
            enum class MODE {
                OAM_SCAN = 2,
                PIXEL_TRANSFER = 3,
                HBLANK = 0,
                VBLANK = 1
            };
            
            void tick(uint8_t dots);
            void tick();
            uint8_t readVRAM(uint16_t address);
            void writeVRAM(uint16_t address, uint8_t value);
            uint8_t readOAM(uint16_t address);
            void writeOAM(uint16_t address, uint8_t value);
            uint8_t readLY();
            uint8_t readSCX();
            void writeSCX(uint8_t value);
            uint8_t readSCY();
            void writeSCY(uint8_t value);
            uint8_t readLCDC();
            void writeLCDC(uint8_t value);
            uint8_t readBGP();
            void writeBGP(uint8_t value);
    
            std::span<const uint8_t, TILE_DATA_SIZE> getTileData();
            std::span<const uint8_t, 2 * TILE_MAP_SIZE> getTileMaps();
    
        private:
            void updateMode();
    
            std::array<uint8_t, VRAM_SIZE> vram{};
            std::array<uint8_t, OAM_SIZE> oam{};
            uint16_t lineDotsElapsed = 0;
            uint32_t frameDotsElapsed = 0;
        
            uint8_t currentLine = 0;        // LY register
            uint8_t lineCompare = 0;        // LYC register
            uint8_t lcdControl = 0;         // LCDC register
            uint8_t scrollX = 0;            // SCX register
            uint8_t scrollY = 0;            // SCY register
            uint8_t backgroundPalette = 0;  // BGP register
            /* STAT register components */
            MODE mode = MODE::OAM_SCAN;  // STAT bits 1-0

            PixelMixer mixer{lcdControl
                           , backgroundPalette
                           , currentLine
                           , scrollX
                           , scrollY
                           , getTileData()
                           , getTileMaps()};
    };

}