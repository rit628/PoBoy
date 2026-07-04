#pragma once
#include "StaticQueue.hpp"
#include <cstdint>

namespace Graphics {

    struct Pixel {
        uint8_t color = 0;
        uint8_t palette = 0;            // unused in dmg bg fetcher
        uint8_t spritePriority = 0;     // unused in dmg pipeline
        uint8_t backgroundPriority = 0; // unused in dmg bg fetcher
    };

    class PixelFetcher {
        public:
            enum class STATE { GET_TILE, GET_TILE_DATA_LO, GET_TILE_DATA_HI, SLEEP, PUSH };

            template<typename Self>
            void tick(this Self&& self);
            Pixel fifoPop();
            bool fifoEmpty();
            void resetState();
            bool asleep();

        protected:
            template<typename Self>
            void preTick(this Self&& self);
            template<typename Self>
            void executeGetTile(this Self&& self);
            template<typename Self>
            void executeGetTileDataLo(this Self&& self);
            template<typename Self>
            void executeGetTileDataHi(this Self&& self);
            template<typename Self>
            void executeSleep(this Self&& self);
            template<typename Self>
            void executePush(this Self&& self);

            STATE state = STATE::GET_TILE;
            bool onSecondDot = false;

            uint8_t tileId = 0;
            uint8_t rowBitPlaneLo = 0, rowBitPlaneHi = 0;
            StaticQueue<Pixel, 8> pixelFifo;
    };

}

#include "PixelFetcher.tpp"