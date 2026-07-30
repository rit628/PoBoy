#pragma once
#include "GraphicsConstants.hpp"
#include "PixelFetcher.hpp"
#include "StaticQueue.hpp"
#include <cstdint>
#include <span>

namespace Graphics {
    
    struct Sprite {
        uint8_t yPos = 0;
        uint8_t xPos = 0;
        uint8_t tileNumber = 0;
        uint8_t spriteFlags = 0;
    };

    class SpriteFetcher : public PixelFetcher {
        friend class PixelFetcher;
        public:
            SpriteFetcher(const uint8_t& lcdControl
                        , const uint8_t& xPos
                        , const uint8_t& currentLine
                        , std::span<const uint8_t, TILE_DATA_SIZE> tileData);

            bool spriteAvailable();
            void reset();
            void addSprite(uint8_t yPos, uint8_t xPos, uint8_t tileNumber, uint8_t spriteFlags);
            void sortSprites();

        private:
            void fetchReset();
            void preTick();
            uint16_t getTileRowAddress();
            
            void getTile();
            void getTileDataLo();
            void getTileDataHi();
            void sleep();
            void push();

            const uint8_t& lcdControl;  // LCDC register reference
            const uint8_t& xPos;        // LX (internal) register reference
            const uint8_t& currentLine; // LY register reference
            uint8_t yPos = currentLine + SPRITE_Y_OFFSET;

            StaticQueue<Sprite, MAX_SPRITES_PER_LINE> spriteBuffer;
            const Sprite* fetchedSprite = nullptr;

            std::span<const uint8_t, TILE_DATA_SIZE> tileData;
    };

}