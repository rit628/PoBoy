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

            PPU(Interrupts::IMU& imu, std::function<void(std::span<const uint8_t>)> renderFrame);
            void initialize();

            uint8_t readVRAM(uint16_t address);
            void writeVRAM(uint16_t address, uint8_t value);
            uint8_t readOAM(uint16_t address);
            void writeOAM(uint16_t address, uint8_t value);
            void dmaTransferOAM(std::span<const uint8_t, OAM_SIZE> sourceRange);
            template<uint16_t Register>
            uint8_t readIO();
            template<uint16_t Register>
            void writeIO(uint8_t value);

            void tick();
    
            std::span<const uint8_t, TILE_DATA_SIZE> getTileData();
            std::span<const uint8_t, 2 * TILE_MAP_SIZE> getTileMaps();
    
        private:
            void incrementLine();
            template<MODE Mode>
            void updateMode();
            template<MODE Mode>
            void tick();
            template<MODE Mode>
            void postTick();
            template<MODE Mode>
            void tickDispatch();
            void disableLCD();

            Interrupts::IMU& imu;
            std::function<void(std::span<const uint8_t>)> renderFrame;   // use std::function for simplicity
    
            std::array<uint8_t, VRAM_SIZE> vram;
            std::array<uint8_t, OAM_SIZE> oam;
            bool enabled;
            uint16_t lineDotsElapsed;
            uint32_t frameDotsElapsed;
        
            uint8_t currentLine;        // LY register
            uint8_t lineCompare;        // LYC register
            uint8_t lcdControl;         // LCDC register
            uint8_t scrollX;            // SCX register
            uint8_t scrollY;            // SCY register
            uint8_t windowX;            // WX register
            uint8_t windowY;            // WY register
            uint8_t backgroundPalette;  // BGP register
            uint8_t spritePalette0;     // OBP0 register
            uint8_t spritePalette1;     // OBP1 register
            /* STAT register components */
            uint8_t interruptMask;  // STAT bits 6-3
            MODE mode;              // STAT bits 1-0

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