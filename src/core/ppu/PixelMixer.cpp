#include "PixelMixer.hpp"
#include "BackgroundFetcher.hpp"
#include "FlagOps.hpp"
#include "GraphicsConstants.hpp"
#include "SpriteFetcher.hpp"
#include <cstdint>

using namespace Graphics;

template<MODEL Model>
PixelMixer<Model>::PixelMixer(const uint8_t& bgp
                     , const uint8_t& obp0
                     , const uint8_t& obp1
                     , const uint8_t& ly
                     , const uint8_t& scx
                     , const uint8_t& scy
                     , const uint8_t& wx
                     , const uint8_t& wy
                     , std::span<const uint8_t, VRAM_SIZE<Model>> vram)
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
                                       , vram)
                     , spriteFetcher(currentColumn
                                   , ly
                                   , vram)
                     {}

template<MODEL Model>
void PixelMixer<Model>::tick() {
    backgroundFetcher.tick();
    bool spriteFetcherActive = spriteFetcher.spriteAvailable();
    /* sprite fetcher cant operate until background fetcher releases vram */
    if (spriteFetcherActive && backgroundFetcher.asleep()) { [[ unlikely ]]
        spriteFetcher.tick();
    }
    
    /* sprite fetching stalls pixel mixing and pushing */
    if (!backgroundFetcher.fifoEmpty() && !spriteFetcherActive) { [[ likely ]]
        mixPixel(backgroundFetcher.fifoPop());
    }
}

template<MODEL Model>
void PixelMixer<Model>::scanlineReset() {
    backgroundFetcher.scanlineReset();
    spriteFetcher.reset();
    currentColumn = 0;
}

template<MODEL Model>
std::span<const uint8_t> PixelMixer<Model>::extractFrame() {
    backgroundFetcher.frameReset();
    spriteFetcher.reset();
    return framebuffer.extract();
}

template<MODEL Model>
bool PixelMixer<Model>::atLineEnd() {
    return currentColumn == LCD_WIDTH + PIXEL_OVERSCAN;
}

template<MODEL Model>
void PixelMixer<Model>::addSprite(uint8_t yPos, uint8_t xPos, uint8_t tileNumber, uint8_t spriteFlags) {
    spriteFetcher.addSprite(yPos, xPos, tileNumber, spriteFlags);
}

template<MODEL Model>
void PixelMixer<Model>::scanlineInitialize() {
    spriteFetcher.sortSprites();
    pixelsToDiscard = scrollX & 0b111;
}

template<MODEL Model>
void PixelMixer<Model>::updateFlags(uint8_t lcdControl) {
    backgroundAndWindowEnabled = testFlags(lcdControl, LCDC_FLAG::BACKGROUND_AND_WINDOW_ENABLE);
    backgroundFetcher.updateFlags(lcdControl);
    spriteFetcher.updateFlags(lcdControl);
}

template<MODEL Model>
uint8_t PixelMixer<Model>::applyPalette(uint8_t palette, uint8_t colorIndex) {
    return (palette >> (2 * colorIndex)) & 0b11;
}

template<MODEL Model>
void PixelMixer<Model>::mixPixel(const Pixel& backgroundPixel) {
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

template<MODEL Model>
void PixelMixer<Model>::emitBackgroundPixel(const Pixel& pixel) {
    if (backgroundAndWindowEnabled) {
        emitPixel(applyPalette(bgPalette, pixel.color));
    }
    else {
        emitPixel(0b00);
    }
}

template<MODEL Model>
void PixelMixer<Model>::emitSpritePixel(const Pixel& pixel) {
    uint8_t palette = (pixel.palette == 0) ? spritePalette0 : spritePalette1;
    emitPixel(applyPalette(palette, pixel.color));
}

template<MODEL Model>
void PixelMixer<Model>::emitPixel(uint8_t colorIndex) {
    if (pixelsToDiscard > 0) return void(--pixelsToDiscard);
    if (currentColumn++ < PIXEL_OVERSCAN) return;
    framebuffer.push(colorIndex);
}

template class Graphics::PixelMixer<MODEL::DMG>;
template class Graphics::PixelMixer<MODEL::CGB>;