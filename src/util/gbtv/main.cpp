#include "TilePrinter.hpp"
#include <array>
#include <cstdint>
#include <iostream>

int main() {
    std::array<uint8_t, TilePrinter::TILE_DATA_SIZE> tileData;
    TilePrinter tp;
    while (true) {
        std::cin.read(reinterpret_cast<char *>(tileData.data()), tileData.size());
        tp.printTileData(tileData);
    }
}