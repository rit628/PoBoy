#include "PixelMixer.hpp"
#include "BackgroundFetcher.hpp"
#include "GraphicsConstants.hpp"
#include <cstdint>

using namespace Graphics;

PixelMixer::PixelMixer(const uint8_t& lcdc
                     , const uint8_t& bgp
                     , const uint8_t& ly
                     , const uint8_t& scx
                     , const uint8_t& scy
                     , std::span<const uint8_t, TILE_DATA_SIZE> tileData
                     , std::span<const uint8_t, 2 * TILE_MAP_SIZE> tileMaps)
                     : backgroundFetcher(lcdc, bgp, ly, scx, scy, tileData, tileMaps)
                     {}

void PixelMixer::tick() {
    if (!backgroundFetcher.fifoEmpty()) {
        addPixel(backgroundFetcher.fifoPop());
    }
    backgroundFetcher.tick();
}

void PixelMixer::resetFifos() {
    backgroundFetcher.reset();
}

std::array<uint8_t, FRAMEBUFFER_SIZE> PixelMixer::extractFrame() {
    resetFifos();
    currentByte = 0;
    currentBit = 0;
    return framebuffer;
}

uint16_t PixelMixer::getCurrentPixel() {
    return currentByte * PIXELS_PER_BYTE + currentBit / BITS_PER_PIXEL;
}

bool PixelMixer::atLineEnd() {
    return backgroundFetcher.getXCoordinate() > LCD_WIDTH;
}

void PixelMixer::addPixel(const Pixel& pixel) {
    uint8_t colorIndex = pixel.color;
    framebuffer.at(currentByte) &= 0xFF - (0b11 << currentBit); // clear former bits
    framebuffer.at(currentByte) |= colorIndex << currentBit;    // overwrite
    currentBit = (currentBit + 2) & 0b111; // increment bit by 2 mod 8
    currentByte += !currentBit;
}
