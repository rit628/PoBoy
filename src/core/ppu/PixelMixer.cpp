#include "PixelMixer.hpp"
#include "BackgroundFetcher.hpp"
#include "FlagOps.hpp"
#include "GraphicsConstants.hpp"
#include <cstdint>

using namespace Graphics;

PixelMixer::PixelMixer(const uint8_t& lcdc
                     , const uint8_t& bgp
                     , const uint8_t& ly
                     , const uint8_t& scx
                     , const uint8_t& scy
                     , const uint8_t& wx
                     , const uint8_t& wy
                     , std::span<const uint8_t, TILE_DATA_SIZE> tileData
                     , std::span<const uint8_t, 2 * TILE_MAP_SIZE> tileMaps)
                     : lcdControl(lcdc)
                     , bgPalette(bgp)
                     , scrollX(scx)
                     , backgroundFetcher(lcdc
                                       , currentColumn
                                       , ly
                                       , scx
                                       , scy
                                       , wx
                                       , wy
                                       , tileData
                                       , tileMaps)
                     {}

void PixelMixer::tick() {
    addPixel(backgroundFetcher.fifoPop());
    backgroundFetcher.tick();
}

void PixelMixer::resetFifos() {
    backgroundFetcher.reset();
    pixelsToDiscard = scrollX & 0b111;
    currentColumn = 0;
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
    return currentColumn == LCD_WIDTH + PIXEL_OVERSCAN;
}

void PixelMixer::addPixel(const Pixel& pixel) {
    if (pixelsToDiscard > 0) {
        pixelsToDiscard--;
        return;
    }
    if (currentColumn++ < PIXEL_OVERSCAN) return;
    uint8_t paletteIndex = (bgPalette >> (2 * pixel.color)) & 0b11; // apply background palette 
    framebuffer.at(currentByte) &= 0xFF - (0b11 << currentBit); // clear former bits
    if (testFlags(lcdControl, LCDC_FLAG::BACKGROUND_AND_WINDOW_ENABLE)) {
        framebuffer.at(currentByte) |= paletteIndex << currentBit;  // overwrite
    }
    currentBit = (currentBit + 2) & 0b111;  // increment bit by 2 mod 8
    currentByte += !currentBit;
}
