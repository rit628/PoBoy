#pragma once
#include "GraphicsConstants.hpp"
#include "PixelMixer.hpp"
#include "IMU.hpp"
#include "SystemConstants.hpp"
#include <array>
#include <cstdint>
#include <functional>
#include <span>

namespace Graphics {

    enum class PPU_MODE {
        OAM_SCAN = 2,
        PIXEL_TRANSFER = 3,
        HBLANK = 0,
        VBLANK = 1
    };

    template<MODEL Model>
    class PPU {
        public:
            PPU(Interrupts::IMU& imu, std::function<void(std::span<const uint8_t>)> renderFrame);
            void initialize();

            void initHLE();
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
    
        private:
            void attemptStatusInterrupt();
            void incrementLine();
            template<PPU_MODE Mode>
            void updateMode();
            template<PPU_MODE Mode>
            void tick();
            template<PPU_MODE Mode>
            void postTick();
            template<PPU_MODE Mode>
            void tickDispatch();
            void disableLCD();
            void enableLCD();

            Interrupts::IMU& imu;
            std::function<void(std::span<const uint8_t>)> renderFrame;   // use std::function for simplicity
    
            std::array<uint8_t, VRAM_SIZE<Model>> vram;
            std::array<uint8_t, OAM_SIZE> oam;
            std::span<uint8_t, VRAM_BANK_SIZE> currentBank;
            bool enabled;
            uint16_t lineDotsElapsed;
            uint32_t frameDotsElapsed;
            bool statInterrupted;   // STAT interrupt line
        
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
            PPU_MODE mode;              // STAT bits 1-0

            /* CGB registers */
            uint8_t vramBank;           // VBK register

            PixelMixer<Model> mixer{backgroundPalette
                                  , spritePalette0
                                  , spritePalette1
                                  , currentLine
                                  , scrollX
                                  , scrollY
                                  , windowX
                                  , windowY
                                  , vram};
    };

}

#include "PPU.tpp"