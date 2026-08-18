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
                     , std::span<const uint8_t, VRAM_SIZE<Model>> vram
                     , std::span<const uint8_t, PALETTE_RAM_BANK_SIZE<Model>> bgPaletteRam
                     , std::span<const uint8_t, PALETTE_RAM_BANK_SIZE<Model>> spritePaletteRam)
                     : bgPalette(bgp)
                     , spritePalette0(obp0)
                     , spritePalette1(obp1)
                     , scrollX(scx)
                     , bgPaletteRam(bgPaletteRam)
                     , spritePaletteRam(spritePaletteRam)
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
    /* color is the 2 bit nibble of palette starting at 2 * colorIndex */
    return (palette >> (2 * colorIndex)) & (PALETTE_SIZE - 1);
}

template<MODEL Model>
uint16_t PixelMixer<Model>::applyPalette(std::span<const uint8_t, PALETTE_RAM_BANK_SIZE<Model>> paletteRam, const Pixel& pixel) {
    static constexpr uint8_t BYTES_PER_COLOR = 2;
    uint8_t paletteAddress = BYTES_PER_COLOR * (PALETTE_SIZE * pixel.paletteNumber + pixel.colorIndex);
    /* CGB colors are stored as little endian 15 bit BGR */
    return (paletteRam[paletteAddress + 1] << 8 | paletteRam[paletteAddress]) & 0x7FFF;
}

template<>
void PixelMixer<MODEL::DMG>::mixPixel(Pixel&& backgroundPixel) {
    if (!backgroundAndWindowEnabled) backgroundPixel.colorIndex = 0;
    if (spriteFetcher.fifoEmpty()) return emitBackgroundPixel(backgroundPixel);

    Pixel spritePixel = spriteFetcher.fifoPop();
    if (spritePixel.colorIndex == 0) {   // transparent sprite pixel -> bg
        emitBackgroundPixel(backgroundPixel);
    }
    else if (spritePixel.bgPriority && backgroundPixel.colorIndex != 0) {   // bg priority -> bg
        emitBackgroundPixel(backgroundPixel);
    }
    else {
        emitSpritePixel(spritePixel);
    }
}

template<>
void PixelMixer<MODEL::CGB>::mixPixel(Pixel&& backgroundPixel) {
    if (spriteFetcher.fifoEmpty()) return emitBackgroundPixel(backgroundPixel);

    Pixel spritePixel = spriteFetcher.fifoPop();
    if (spritePixel.colorIndex == 0) {   // transparent sprite pixel -> bg
        emitBackgroundPixel(backgroundPixel);
    }
    else if (!backgroundAndWindowEnabled || backgroundPixel.colorIndex == 0) {  // bg disabled or 0 index bg pixel -> sprite 
        emitSpritePixel(spritePixel);
    }
    else if (backgroundPixel.bgPriority || spritePixel.bgPriority) {    // bg priority -> bg
        emitBackgroundPixel(backgroundPixel);
    }
    else {
        emitSpritePixel(spritePixel);
    }
}

template<MODEL Model>
void PixelMixer<Model>::emitBackgroundPixel(const Pixel& pixel) {
    if constexpr (Model == MODEL::DMG) {
        emitPixel(applyPalette(bgPalette, pixel.colorIndex));
    }
    else {
        emitPixel(applyPalette(bgPaletteRam, pixel));
    }
}

template<MODEL Model>
void PixelMixer<Model>::emitSpritePixel(const Pixel& pixel) {
    if constexpr (Model == MODEL::DMG) {
        uint8_t palette = (pixel.paletteNumber == 0) ? spritePalette0 : spritePalette1;
        emitPixel(applyPalette(palette, pixel.colorIndex));
    }
    else {
        emitPixel(applyPalette(spritePaletteRam, pixel));
    }
}

template<MODEL Model>
void PixelMixer<Model>::emitPixel(FrameBuffer::ElementType color) {
    if (pixelsToDiscard > 0) return void(--pixelsToDiscard);
    if (currentColumn++ < PIXEL_OVERSCAN) return;
    framebuffer.push(color);
}

template class Graphics::PixelMixer<MODEL::DMG>;
template class Graphics::PixelMixer<MODEL::CGB>;