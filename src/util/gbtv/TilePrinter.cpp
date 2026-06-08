#include "TilePrinter.hpp"
#include <codecvt>
#include <print>

void TilePrinter::printTileData(const std::array<uint8_t, TILE_DATA_SIZE>& tileData) {
    for (uint16_t i = 0; i < tileData.size(); i += TILE_BYTES) {
        std::span<const uint8_t, TILE_BYTES> tile = std::span(tileData).subspan(i).first<TILE_BYTES>();
        std::print("TILE {:02X}:", i / TILE_BYTES);
        for (auto&& byte : tile) {
            std::print(" {:02X}", byte);
        }
        std::println();
        printTile(tile);
    }
}

void TilePrinter::printTile(std::span<const uint8_t, TILE_BYTES>& tile) {
    for (uint8_t i = 0; i < tile.size(); i += 2) {
        printRow({tile[i], tile[i + 1]});
    }
}

void TilePrinter::printRow(std::pair<uint8_t, uint8_t> bitPlane) {
    static constexpr std::array<std::u16string, 4> palette = { u"░░", u"▒▒", u"▓▓", u"██" };
    std::string row = "";
    row.reserve(palette.at(0).size());
    std::wstring_convert<std::codecvt_utf8_utf16<char16_t>, char16_t> convert;
    for (uint8_t i = 0; i < 8; i++) {
        bool msb = bitPlane.first & (0x1 << (7 - i));
        bool lsb = bitPlane.second & (0x1 << (7 - i));
        uint8_t paletteIndex = (msb << 1) | lsb;
        auto pixel = convert.to_bytes(palette.at(paletteIndex));
        row += pixel;
    }
    std::println("{}", row);
}