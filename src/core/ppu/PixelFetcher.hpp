#pragma once
#include "GraphicsConstants.hpp"
#include "StaticQueue.hpp"
#include <cstdint>

namespace Graphics {

    struct Pixel {
        uint8_t colorIndex = 0;
        uint8_t paletteNumber = 0;
        bool bgPriority = false;
        uint8_t spritePriority = MAX_SPRITES_PER_LINE;  // exclusively used for CGB oam index based drawing priority
    };

    class PixelFetcher {
        public:
            enum class STATE { GET_TILE, GET_TILE_DATA_LO, GET_TILE_DATA_HI, SLEEP, PUSH };

            void tick(this auto&& self);
            Pixel fifoPop();
            bool fifoEmpty();
            void resetState();
            bool asleep();

        protected:
            void preTick(this auto&& self);
            void executeGetTile(this auto&& self);
            void executeGetTileDataLo(this auto&& self);
            void executeGetTileDataHi(this auto&& self);
            void executeSleep(this auto&& self);
            void executePush(this auto&& self);
            auto createColorIndexExtractor(bool xFlip);

            STATE state = STATE::GET_TILE;
            bool onSecondDot = false;

            uint8_t tileId = 0;
            uint8_t rowBitPlaneLo = 0, rowBitPlaneHi = 0;
            StaticQueue<Pixel, 8> pixelFifo;
    };

}

#include "PixelFetcher.tpp"