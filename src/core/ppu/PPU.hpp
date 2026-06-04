#pragma once
#include "MMU.hpp"
#include <cstdint>

class PPU {
    public:
        enum class MODE {
            OAM_SCAN = 2,
            PIXEL_TRANSFER = 3,
            HBLANK = 0,
            VBLANK = 1
        };

        PPU(MMU& mmu) : mmu(mmu) {}
        void tick(uint8_t dots);
        void tick();

        static constexpr uint8_t FRAME_LINES = 154;
        static constexpr uint8_t VBLANK_LINES = 10;
        static constexpr uint8_t LCD_LINES = FRAME_LINES - VBLANK_LINES;

        static constexpr uint32_t DOTS_PER_FRAME = 70224;
        static constexpr uint16_t DOTS_PER_LINE = DOTS_PER_FRAME / FRAME_LINES;
        static constexpr uint32_t DOTS_PER_LCD_SCAN = LCD_LINES * DOTS_PER_LINE;
        static constexpr uint16_t DOTS_PER_OAM_SCAN_MODE = 80;
        static constexpr uint16_t MAX_DOTS_PER_PIXEL_TRANSFER_MODE = 289; // shouldnt be necessary but just a safeguard

    private:
        void updateMode();

        MMU& mmu;
        MODE currentMode = MODE::OAM_SCAN;
        uint16_t lineDotsElapsed = 0;
        uint32_t frameDotsElapsed = 0;

};