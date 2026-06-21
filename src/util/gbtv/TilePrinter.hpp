#pragma once
#include <array>
#include <cstdint>
#include <span>
#include <sstream>

class TilePrinter {
    public:
        static constexpr uint16_t TILE_COUNT        = 384;
        static constexpr uint16_t TILE_BYTES        = 16;
        static constexpr uint16_t TILE_ROWS         = 8;
        static constexpr uint16_t TILE_DATA_SIZE    = TILE_COUNT * TILE_BYTES;
        static constexpr uint16_t TILE_MAP_SIZE     = 32 * 32;
        static constexpr uint16_t VRAM_SIZE         = 0x2000;

        void printVRAM(const uint8_t bgPaletteMap, const std::array<uint8_t, VRAM_SIZE>& vram);
    
    private:
        void flushTiles();
        void outputTileData(const uint8_t bgPaletteMap, std::span<const uint8_t, TILE_DATA_SIZE> tileData);
        void outputTile(std::span<const uint8_t, TILE_BYTES>& tile);
        void outputRow(std::stringstream& row, std::pair<uint8_t, uint8_t> bitPlane);

        std::stringstream outputBuffer;
        std::stringstream idLine;
        std::array<std::stringstream, TILE_ROWS> tileRows;
        uint8_t bgPaletteMap = 0b11100100;
};