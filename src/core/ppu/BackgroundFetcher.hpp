#pragma once
#include "GraphicsConstants.hpp"
#include "Pixel.hpp"
#include <cstdint>
#include <queue>
#include <span>

namespace Graphics {

    class BackgroundFetcher {
        public:
            enum class STATE { GET_TILE, GET_TILE_DATA_LO, GET_TILE_DATA_HI, SLEEP, PUSH };

            BackgroundFetcher(const uint8_t& lcdControl
                            , const uint8_t& bgPalette
                            , const uint8_t& yPos
                            , const uint8_t& bgX
                            , const uint8_t& bgY
                            , std::span<const uint8_t, TILE_DATA_SIZE> tileData
                            , std::span<const uint8_t, 2 * TILE_MAP_SIZE> tileMaps);

            void tick();
            void reset();
            Pixel fifoPop();
            bool fifoEmpty();
            uint8_t getXCoordinate();

        private:
            void getTile();
            void getTileDataLo();
            void getTileDataHi();
            void sleep();
            void push();
            
            STATE state = STATE::GET_TILE;
            bool onSecondDot = false;
            std::queue<Pixel> pixelFifo;
            
            const uint8_t& lcdControl;  // LCDC register reference
            const uint8_t& bgPalette;   // BGP register reference

            uint8_t xPos = 0;
            const uint8_t& yPos;        // LY register reference
            const uint8_t& bgX;         // SCX register reference
            const uint8_t& bgY;         // SCY register reference
            

            uint8_t tileId = 0;
            uint16_t tileAddress = 0;
            uint8_t tileBitPlaneLo = 0, tileBitPlaneHi = 0;
            std::span<const uint8_t, TILE_DATA_SIZE> tileData;
            std::span<const uint8_t, 2 * TILE_MAP_SIZE> tileMaps;
    };

}