#include "TilePrinter.hpp"
#include <codecvt>
#include <cstdint>
#include <iostream>
#include <print>

void TilePrinter::printTileData(const uint8_t bgPaletteMap, const std::array<uint8_t, TILE_DATA_SIZE>& tileData) {
    this->bgPaletteMap = bgPaletteMap;
    uint8_t tilesThisLine = 0;
    for (uint16_t i = 0; i < tileData.size(); i += TILE_BYTES) {
        std::span<const uint8_t, TILE_BYTES> tile = std::span(tileData).subspan(i).first<TILE_BYTES>();
        std::print(idLine, "TILE {:02X}          | ", uint8_t(i / TILE_BYTES));
        outputTile(tile);
        if (++tilesThisLine > 0x0F) {
            flushTiles();
            tilesThisLine = 0;
        }
    }
    std::println("{}\033[H", outputBuffer.str());
    outputBuffer.clear();
    outputBuffer.seekp(0);
    std::cout.flush();
}

void TilePrinter::flushTiles() {
    std::string divider(16 * (16 + 3), '-');
    std::println(outputBuffer, "{}", idLine.str());
    idLine.clear();
    idLine.seekp(0);
    std::println(outputBuffer, "{}", divider);
    for (auto&& row : tileRows) {
        std::println(outputBuffer, "{}", row.str());
        row.clear();
        row.seekp(0);
    }
    std::println(outputBuffer, "{}", divider);
}

void TilePrinter::outputTile(std::span<const uint8_t, TILE_BYTES>& tile) {
    for (uint8_t i = 0; i < tile.size(); i += 2) {
        auto& row = tileRows.at(i / 2);
        outputRow(row, {tile[i], tile[i + 1]});
        row << " | ";
    }
}

void TilePrinter::outputRow(std::stringstream& row, std::pair<uint8_t, uint8_t> bitPlane) {
    static constexpr std::array<std::u16string, 4> palette = { u"░░", u"▒▒", u"▓▓", u"██" };
    std::wstring_convert<std::codecvt_utf8_utf16<char16_t>, char16_t> convert;
    for (uint8_t i = 0; i < 8; i++) {
        bool msb = bitPlane.first & (0x1 << (7 - i));
        bool lsb = bitPlane.second & (0x1 << (7 - i));
        uint8_t paletteIndex = (msb << 1) | lsb;
        paletteIndex = (bgPaletteMap >> (2 * paletteIndex)) & 0b11;
        auto pixel = convert.to_bytes(palette.at(paletteIndex));
        row << pixel;
    }
}