#include "PixelMixer.hpp"
#include "BackgroundFetcher.hpp"
#include "FlagOps.hpp"
#include "GraphicsConstants.hpp"
#include "SpriteFetcher.hpp"
#include <cstdint>

using namespace Graphics;

PixelMixer::PixelMixer(const uint8_t& bgp
                     , const uint8_t& obp0
                     , const uint8_t& obp1
                     , const uint8_t& ly
                     , const uint8_t& scx
                     , const uint8_t& scy
                     , const uint8_t& wx
                     , const uint8_t& wy
                     , std::span<const uint8_t, TILE_DATA_SIZE> tileData
                     , std::span<const uint8_t, 2 * TILE_MAP_SIZE> tileMaps)
                     : bgPalette(bgp)
                     , spritePalette0(obp0)
                     , spritePalette1(obp1)
                     , scrollX(scx)
                     , backgroundFetcher(currentColumn
                                       , ly
                                       , scx
                                       , scy
                                       , wx
                                       , wy
                                       , tileData
                                       , tileMaps)
                     , spriteFetcher(currentColumn
                                   , ly
                                   , tileData)
                     {}

void PixelMixer::tick() {
    /* sprite fetching stalls pixel mixing and pushing */
    bool spriteFetcherActive = spriteFetcher.spriteAvailable();
    if (!backgroundFetcher.fifoEmpty() && !spriteFetcherActive) { [[ likely ]]
        mixPixel(backgroundFetcher.fifoPop());
    }

    /* background and sprite fetchers mutually exclude vram access */
    if (spriteFetcherActive && backgroundFetcher.asleep()) { [[ unlikely ]]
        spriteFetcher.tick();
    }
    else {
        backgroundFetcher.tick();
    }
}

void PixelMixer::scanlineReset() {
    backgroundFetcher.scanlineReset();
    spriteFetcher.reset();
    currentColumn = 0;
}

std::span<const uint8_t> PixelMixer::extractFrame() {
    backgroundFetcher.frameReset();
    spriteFetcher.reset();
    return framebuffer.extract();
}

bool PixelMixer::atLineEnd() {
    return currentColumn == LCD_WIDTH + PIXEL_OVERSCAN;
}

void PixelMixer::addSprite(uint8_t yPos, uint8_t xPos, uint8_t tileNumber, uint8_t spriteFlags) {
    spriteFetcher.addSprite(yPos, xPos, tileNumber, spriteFlags);
}

void PixelMixer::scanlineInitialize() {
    spriteFetcher.sortSprites();
    pixelsToDiscard = scrollX & 0b111;
}

void PixelMixer::updateFlags(uint8_t lcdControl) {
    backgroundAndWindowEnabled = testFlags(lcdControl, LCDC_FLAG::BACKGROUND_AND_WINDOW_ENABLE);
    backgroundFetcher.updateFlags(lcdControl);
    spriteFetcher.updateFlags(lcdControl);
}

uint8_t PixelMixer::applyPalette(uint8_t palette, uint8_t colorIndex) {
    return (palette >> (2 * colorIndex)) & 0b11;
}

void PixelMixer::mixPixel(const Pixel& backgroundPixel) {
    Pixel spritePixel = (!spriteFetcher.fifoEmpty()) ? spriteFetcher.fifoPop() : Pixel{};
    if (spritePixel.color == 0) {   // blank sprite
        emitBackgroundPixel(backgroundPixel);
    }
    else if (spritePixel.backgroundPriority == 1 && backgroundPixel.color != 0) {   // transparency mixing
        emitBackgroundPixel(backgroundPixel);
    }
    else {
        emitSpritePixel(spritePixel);
    }
}

void PixelMixer::emitBackgroundPixel(const Pixel& pixel) {
    if (backgroundAndWindowEnabled) {
        emitPixel(applyPalette(bgPalette, pixel.color));
    }
    else {
        emitPixel(0b00);
    }
}

void PixelMixer::emitSpritePixel(const Pixel& pixel) {
    uint8_t palette = (pixel.palette == 0) ? spritePalette0 : spritePalette1;
    emitPixel(applyPalette(palette, pixel.color));
}

void PixelMixer::emitPixel(uint8_t colorIndex) {
    if (pixelsToDiscard > 0) return void(--pixelsToDiscard);
    if (currentColumn++ < PIXEL_OVERSCAN) return;
    framebuffer.push(colorIndex);
}
