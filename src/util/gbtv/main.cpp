#include "TilePrinter.hpp"
#include <array>
#include <cstdint>
#include <iostream>

int main() {
    std::array<uint8_t, TilePrinter::TILE_DATA_SIZE> tileData;
    uint8_t bgPaletteMap = 0b11100100;
    TilePrinter tp;
    while (true) {
        std::cin.read(reinterpret_cast<char *>(&bgPaletteMap), 1);
        std::cin.read(reinterpret_cast<char *>(tileData.data()), tileData.size());
        tp.printTileData(bgPaletteMap, tileData);
    }
}