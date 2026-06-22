#pragma once
#include "BackgroundFetcher.hpp"
#include "Pixel.hpp"
#include "GraphicsConstants.hpp"
#include <array>
#include <cstdint>


namespace Graphics {

    class PixelMixer {
        public:
            PixelMixer(const uint8_t& lcdc
                     , const uint8_t& bgp
                     , const uint8_t& ly
                     , const uint8_t& scx
                     , const uint8_t& scy
                     , const uint8_t& wx
                     , const uint8_t& wy
                     , std::span<const uint8_t, TILE_DATA_SIZE> tileData
                     , std::span<const uint8_t, 2 * TILE_MAP_SIZE> tileMaps);

            void tick();
            void scanlineReset();
            std::array<uint8_t, FRAMEBUFFER_SIZE> extractFrame();
            uint16_t getCurrentPixel();
            bool atLineEnd();
    
        private:
            void addPixel(const Pixel& pixel);
            
            const uint8_t& lcdControl;      // LCDC register reference
            const uint8_t& bgPalette;       // BGP register reference
            const uint8_t& scrollX;         // SCX register reference
            uint8_t pixelsToDiscard = 0;

            BackgroundFetcher backgroundFetcher;
            std::array<uint8_t, FRAMEBUFFER_SIZE> framebuffer{};
            uint16_t currentByte = 0;
            uint8_t currentBit = 0;
            uint8_t currentColumn = 0;
    };

}