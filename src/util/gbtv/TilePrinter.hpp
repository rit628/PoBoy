#pragma once
#include <array>
#include <cstdint>
#include <span>

class TilePrinter {
    public:
        static constexpr uint16_t TILE_COUNT        = 384;
        static constexpr uint16_t TILE_BYTES        = 16;
        static constexpr uint16_t TILE_DATA_SIZE    = TILE_COUNT * TILE_BYTES;

        static void printTileData(const std::array<uint8_t, TILE_DATA_SIZE>& tileData);
    
    private:
        static void printTile(std::span<const uint8_t, TILE_BYTES>& tile);
        static void printRow(std::pair<uint8_t, uint8_t> bitPlane);
};