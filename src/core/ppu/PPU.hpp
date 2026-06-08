#pragma once
#include <array>
#include <cstdint>
#include <span>

class PPU {
    public:
        enum class MODE {
            OAM_SCAN = 2,
            PIXEL_TRANSFER = 3,
            HBLANK = 0,
            VBLANK = 1
        };
                
        static constexpr uint8_t LCD_WIDTH      = 160;
        static constexpr uint8_t LCD_HEIGHT     = 144;
        
        static constexpr uint8_t FRAME_LINES    = 154;
        static constexpr uint8_t VBLANK_LINES   = 10;
        static constexpr uint8_t LCD_LINES      = FRAME_LINES - VBLANK_LINES;
        
        static constexpr uint32_t DOTS_PER_FRAME                    = 70224;
        static constexpr uint16_t DOTS_PER_LINE                     = DOTS_PER_FRAME / FRAME_LINES;
        static constexpr uint32_t DOTS_PER_LCD_SCAN                 = LCD_LINES * DOTS_PER_LINE;
        static constexpr uint16_t DOTS_PER_OAM_SCAN_MODE            = 80;
        static constexpr uint16_t MAX_DOTS_PER_PIXEL_TRANSFER_MODE  = 289; // shouldnt be necessary but just a safeguard

        static constexpr uint16_t VRAM_SIZE         = 0x2000;
        static constexpr uint8_t  OAM_SIZE          = 0xA0;

        static constexpr uint16_t TILE_COUNT        = 384;
        static constexpr uint16_t TILE_BYTES        = 16;
        static constexpr uint16_t TILE_DATA_SIZE    = TILE_COUNT * TILE_BYTES;
        
        void tick(uint8_t dots);
        void tick();
        uint8_t readVRAM(uint16_t address);
        void writeVRAM(uint16_t address, uint8_t value);
        uint8_t readOAM(uint16_t address);
        void writeOAM(uint16_t address, uint8_t value);
        uint8_t readLY();

        std::span<const uint8_t, TILE_DATA_SIZE> getTileData();

    private:
        void updateMode();

        std::array<uint8_t, VRAM_SIZE> vram{};
        std::array<uint8_t, OAM_SIZE> oam{};
        MODE currentMode = MODE::OAM_SCAN;
        uint8_t currentLine = 0;    // LY register
        uint16_t lineDotsElapsed = 0;
        uint32_t frameDotsElapsed = 0;
};