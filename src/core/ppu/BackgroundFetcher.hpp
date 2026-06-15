#pragma once
#include "GraphicsConstants.hpp"
#include "Pixel.hpp"
#include <array>
#include <cstdint>
#include <span>

namespace Graphics {

    class BackgroundFetcher {
        public:
            enum class STATE { GET_TILE, GET_TILE_DATA_LO, GET_TILE_DATA_HI, SLEEP, PUSH };

            BackgroundFetcher(const uint8_t& lcdControl
                            , const uint8_t& bgPalette
                            , const uint8_t& yPos
                            , const uint8_t& scrollX
                            , const uint8_t& scrollY
                            , const uint8_t& windowX
                            , const uint8_t& windowY
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
            std::array<Pixel, 8> pixelFifo;
            uint8_t fifoFront = pixelFifo.size();
            
            const uint8_t& lcdControl;  // LCDC register reference
            const uint8_t& bgPalette;   // BGP register reference

            uint8_t xPos = 0;
            const uint8_t& yPos;            // LY register reference
            const uint8_t& scrollX;         // SCX register reference
            const uint8_t& scrollY;         // SCY register reference
            const uint8_t& windowX;         // WX register reference
            const uint8_t& windowY;         // WY register reference
            

            uint8_t tileId = 0;
            uint16_t tileAddress = 0;
            uint8_t tileBitPlaneLo = 0, tileBitPlaneHi = 0;
            std::span<const uint8_t, TILE_DATA_SIZE> tileData;
            std::span<const uint8_t, 2 * TILE_MAP_SIZE> tileMaps;
    };

}