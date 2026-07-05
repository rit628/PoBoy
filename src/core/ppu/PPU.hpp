#pragma once
#include "GraphicsConstants.hpp"
#include "PixelMixer.hpp"
#include "IMU.hpp"
#include <array>
#include <cstdint>
#include <functional>
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

            PPU(Interrupts::IMU& imu, std::function<void(std::array<uint8_t, FRAMEBUFFER_SIZE>&)> renderFrame);
            
            void tick(uint8_t dots);
            void tick();

            uint8_t readVRAM(uint16_t address);
            void writeVRAM(uint16_t address, uint8_t value);
            uint8_t readOAM(uint16_t address);
            void writeOAM(uint16_t address, uint8_t value);
            void dmaTransferOAM(std::span<const uint8_t, OAM_SIZE> sourceRange);
            template<uint16_t Register>
            uint8_t readIO();
            template<uint16_t Register>
            void writeIO(uint8_t value);
    
            std::span<const uint8_t, TILE_DATA_SIZE> getTileData();
            std::span<const uint8_t, 2 * TILE_MAP_SIZE> getTileMaps();
    
        private:
            void scanOAM();
            bool disabled();
            void updateStatus();

            Interrupts::IMU& imu;
            std::function<void(std::array<uint8_t, FRAMEBUFFER_SIZE>&)> renderFrame;   // use std::function for simplicity
    
            std::array<uint8_t, VRAM_SIZE> vram{};
            std::array<uint8_t, OAM_SIZE> oam{};
            uint16_t lineDotsElapsed = 0;
            uint32_t frameDotsElapsed = 0;
        
            uint8_t currentLine = 0;        // LY register
            uint8_t lineCompare = 0;        // LYC register
            uint8_t lcdControl = 0;         // LCDC register
            uint8_t scrollX = 0;            // SCX register
            uint8_t scrollY = 0;            // SCY register
            uint8_t windowX = 0;            // WX register
            uint8_t windowY = 0;            // WY register
            uint8_t backgroundPalette = 0;  // BGP register
            uint8_t spritePalette0 = 0;     // OBP0 register
            uint8_t spritePalette1 = 0;     // OBP1 register
            /* STAT register components */
            uint8_t interruptMask = 0x80;   // STAT bits 6-3
            MODE mode = MODE::OAM_SCAN;     // STAT bits 1-0

            PixelMixer mixer{lcdControl
                           , backgroundPalette
                           , spritePalette0
                           , spritePalette1
                           , currentLine
                           , scrollX
                           , scrollY
                           , windowX
                           , windowY
                           , getTileData()
                           , getTileMaps()};
    };

}