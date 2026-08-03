#pragma once
#include "GraphicsConstants.hpp"
#include "PixelFetcher.hpp"
#include <cstdint>
#include <span>

namespace Graphics {

    class BackgroundFetcher : public PixelFetcher {
        friend class PixelFetcher;
        public:
            BackgroundFetcher(const uint8_t& xPos
                            , const uint8_t& yPos
                            , const uint8_t& scrollX
                            , const uint8_t& scrollY
                            , const uint8_t& windowX
                            , const uint8_t& windowY
                            , std::span<const uint8_t, TILE_DATA_SIZE> tileData
                            , std::span<const uint8_t, 2 * TILE_MAP_SIZE> tileMaps);

            void frameReset();
            void scanlineReset();
            void updateFlags(uint8_t lcdControl);

        private:
            void preTick();
            uint16_t getTileRowAddress();
            
            void getTile();
            void getTileDataLo();
            void getTileDataHi();
            void sleep();
            void push();
            
            const uint8_t& xPos;            // LX (internal) register reference
            const uint8_t& yPos;            // LY register reference
            const uint8_t& scrollX;         // SCX register reference
            const uint8_t& scrollY;         // SCY register reference
            const uint8_t& windowX;         // WX register reference
            const uint8_t& windowY;         // WY register reference

            bool windowEnabled = false;
            bool renderingWindow = false;
            bool windowXCondition = false;
            bool windowYCondition = false;
            uint8_t currentWindowColumn = 0;
            uint8_t currentWindowLine = UINT8_MAX;

            bool unsignedAddressing = false;
            uint8_t windowTileMap = 0;
            uint8_t backgroundTileMap = 0;

            std::span<const uint8_t, TILE_DATA_SIZE> tileData;
            std::span<const uint8_t, 2 * TILE_MAP_SIZE> tileMaps;
    };

}