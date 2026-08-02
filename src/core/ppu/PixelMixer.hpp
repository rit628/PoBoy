#pragma once
#include "BackgroundFetcher.hpp"
#include "BitBuffer.hpp"
#include "SpriteFetcher.hpp"
#include "GraphicsConstants.hpp"
#include <cstdint>

namespace Graphics {

    class PixelMixer {
        public:
            PixelMixer(const uint8_t& lcdc
                     , const uint8_t& bgp
                     , const uint8_t& obp0
                     , const uint8_t& obp1
                     , const uint8_t& ly
                     , const uint8_t& scx
                     , const uint8_t& scy
                     , const uint8_t& wx
                     , const uint8_t& wy
                     , std::span<const uint8_t, TILE_DATA_SIZE> tileData
                     , std::span<const uint8_t, 2 * TILE_MAP_SIZE> tileMaps);

            void tick();
            void scanlineReset();
            std::span<const uint8_t> extractFrame();
            bool atLineEnd();
            void addSprite(uint8_t yPos, uint8_t xPos, uint8_t tileNumber, uint8_t spriteFlags);
            void scanlineInitialize();
    
        private:
            uint8_t applyPalette(uint8_t palette, uint8_t colorIndex);
            void mixPixel(const Pixel& backgroundPixel);
            void emitBackgroundPixel(const Pixel& pixel);
            void emitSpritePixel(const Pixel& pixel);
            void emitPixel(uint8_t colorIndex);
            
            const uint8_t& lcdControl;      // LCDC register reference
            const uint8_t& bgPalette;       // BGP register reference
            const uint8_t& spritePalette0;  // OBP0 register reference
            const uint8_t& spritePalette1;  // OBP1 register reference
            const uint8_t& scrollX;         // SCX register reference
            uint8_t pixelsToDiscard = 0;

            BackgroundFetcher backgroundFetcher;
            SpriteFetcher spriteFetcher;
            BitBuffer<FRAMEBUFFER_SIZE, BITS_PER_PIXEL> framebuffer;
            uint8_t currentColumn = 0;
    };

}