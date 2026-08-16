#pragma once
#include "BackgroundFetcher.hpp"
#include "BitBuffer.hpp"
#include "SpriteFetcher.hpp"
#include "GraphicsConstants.hpp"
#include "SystemConstants.hpp"
#include <cstdint>

namespace Graphics {

    template<MODEL Model>
    class PixelMixer {
        public:
            PixelMixer(const uint8_t& bgp
                     , const uint8_t& obp0
                     , const uint8_t& obp1
                     , const uint8_t& ly
                     , const uint8_t& scx
                     , const uint8_t& scy
                     , const uint8_t& wx
                     , const uint8_t& wy
                     , std::span<const uint8_t, VRAM_SIZE<Model>> vram
                     , std::span<const uint8_t, PALETTE_RAM_BANK_SIZE<Model>> bgPaletteRam
                     , std::span<const uint8_t, PALETTE_RAM_BANK_SIZE<Model>> spritePaletteRam);

            void tick();
            void scanlineReset();
            std::span<const uint8_t> extractFrame();
            bool atLineEnd();
            void addSprite(uint8_t yPos, uint8_t xPos, uint8_t tileNumber, uint8_t spriteFlags);
            void scanlineInitialize();
            void updateFlags(uint8_t lcdControl);
    
        private:
            using FrameBuffer = BitBuffer<FRAMEBUFFER_SIZE, BITS_PER_PIXEL<Model>>;

            uint8_t applyPalette(uint8_t palette, uint8_t colorIndex);
            uint16_t applyPalette(std::span<const uint8_t, PALETTE_RAM_BANK_SIZE<Model>> paletteRam, const Pixel& pixel);
            void mixPixel(const Pixel& backgroundPixel);
            void emitBackgroundPixel(const Pixel& pixel);
            void emitSpritePixel(const Pixel& pixel);
            void emitPixel(FrameBuffer::ElementType color);
            
            const uint8_t& bgPalette;       // BGP register reference
            const uint8_t& spritePalette0;  // OBP0 register reference
            const uint8_t& spritePalette1;  // OBP1 register reference
            const uint8_t& scrollX;         // SCX register reference

            std::span<const uint8_t, PALETTE_RAM_BANK_SIZE<Model>> bgPaletteRam, spritePaletteRam;

            bool backgroundAndWindowEnabled = false;
            uint8_t pixelsToDiscard = 0;

            BackgroundFetcher<Model> backgroundFetcher;
            SpriteFetcher<Model> spriteFetcher;
            FrameBuffer framebuffer;
            uint8_t currentColumn = 0;
    };

}