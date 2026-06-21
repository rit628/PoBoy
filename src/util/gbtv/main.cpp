#include "TilePrinter.hpp"
#include <array>
#include <cstdint>
#include <iostream>

int main() {
    std::array<uint8_t, TilePrinter::VRAM_SIZE> vram;
    uint8_t bgPaletteMap = 0b11100100;
    TilePrinter tp;
    while (true) {
        std::cin.read(reinterpret_cast<char *>(&bgPaletteMap), 1);
        std::cin.read(reinterpret_cast<char *>(vram.data()), vram.size());
        tp.printVRAM(bgPaletteMap, vram);
    }
}