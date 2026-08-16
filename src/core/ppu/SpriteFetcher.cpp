#include "SpriteFetcher.hpp"
#include "FlagOps.hpp"
#include "GraphicsConstants.hpp"
#include "SystemConstants.hpp"
#include <algorithm>
#include <cstdint>

using namespace Graphics;

template<MODEL Model>
SpriteFetcher<Model>::SpriteFetcher(const uint8_t& xPos
                           , const uint8_t& currentLine
                           , std::span<const uint8_t, VRAM_SIZE<Model>> vram)
                           : xPos(xPos)
                           , currentLine(currentLine)
                           , tileData(vram.template subspan<0, TILE_DATA_SIZE>())
                           {}

template<MODEL Model>
bool SpriteFetcher<Model>::spriteAvailable() {
    if (spriteBuffer.empty()) return false;
    if (!spritesEnabled) {
        fetchReset();
        return false;
    };
    if (fetchedSprite) return true; // dont recheck if already fetched

    auto& sprite = spriteBuffer.front();
    if (sprite.xPos == xPos) {
        fetchedSprite = &sprite;
        yFlip = testFlags(fetchedSprite->spriteFlags, ATTRIBUTE_FLAG::Y_FLIP);
        xFlip = testFlags(fetchedSprite->spriteFlags, ATTRIBUTE_FLAG::X_FLIP);
        return true;
    }
    return false;
}

template<MODEL Model>
void SpriteFetcher<Model>::reset() {
    fetchReset();
    spriteBuffer.clear();
}

template<MODEL Model>
void SpriteFetcher<Model>::addSprite(uint8_t yPos, uint8_t xPos, uint8_t tileNumber, uint8_t spriteFlags) {
    uint8_t spriteHeight = 8 * (doubleHeightSprites + 1);
    this->yPos = currentLine + SPRITE_Y_OFFSET;
    if (spriteBuffer.full()) return;                     // ensure buffer has space 
    if (yPos > this->yPos) return;                      // ensure visibility on current scanline rowwise
    if (yPos + spriteHeight <= this->yPos) return;      // ensure sprite has not been completely rendered previously
    spriteBuffer.push({yPos, xPos, tileNumber, spriteFlags});
}

template<MODEL Model>
void SpriteFetcher<Model>::sortSprites() {
    std::ranges::stable_sort(spriteBuffer.data(), [](uint8_t a, uint8_t b){
        return a < b;
    }, &Sprite::xPos);
}

template<MODEL Model>
void SpriteFetcher<Model>::updateFlags(uint8_t lcdControl) {
    spritesEnabled = testFlags(lcdControl, LCDC_FLAG::SPRITE_ENABLE);
    doubleHeightSprites = testFlags(lcdControl, LCDC_FLAG::SPRITE_SIZE_MODIFIER);
}

template<MODEL Model>
void SpriteFetcher<Model>::fetchReset() {
    resetState();
    spriteBuffer.pop();
    fetchedSprite = nullptr;
    pixelFifo.clear();
}

template<MODEL Model>
void SpriteFetcher<Model>::preTick() {}

template<MODEL Model>
uint16_t SpriteFetcher<Model>::getTileRowAddress() {
    uint16_t tileAddress = tileId * TILE_BYTES;
    uint8_t tileRow = (yPos - fetchedSprite->yPos) % 8;
    tileRow = 0b111 & ((yFlip) ? ~tileRow : tileRow);   // negate and mask to flip and remain in range
    return tileAddress + tileRow * TILE_ROW_BYTES;
}

template<MODEL Model>
void SpriteFetcher<Model>::getTile() {
    tileId = fetchedSprite->tileNumber;
    if (doubleHeightSprites) {
        bool onSecondTile = yPos >= fetchedSprite->yPos + 8;
        tileId = (tileId & ~0b1) + ((yFlip) ? !onSecondTile : onSecondTile);
    }
}

template<MODEL Model>
void SpriteFetcher<Model>::getTileDataLo() {
    rowBitPlaneLo = tileData[getTileRowAddress()];
}

template<MODEL Model>
void SpriteFetcher<Model>::getTileDataHi() {
    rowBitPlaneHi = tileData[getTileRowAddress() + 1];
}

template<MODEL Model>
void SpriteFetcher<Model>::sleep() {
    state = STATE::PUSH;
}

template<MODEL Model>
void SpriteFetcher<Model>::push() {
    Pixel pixel;
    pixel.palette = testFlags(fetchedSprite->spriteFlags, ATTRIBUTE_FLAG::DMG_PALETTE);
    pixel.priority = testFlags(fetchedSprite->spriteFlags, ATTRIBUTE_FLAG::PRIORITY);
    
    auto getMask = (xFlip) ? [](uint8_t pixelIndex) { return 0x1 << pixelIndex; }
                                             : [](uint8_t pixelIndex) { return 0x1 << (7 - pixelIndex); };
    auto getPixelColor = [this, getMask](uint8_t pixelIndex) -> uint8_t {
        uint8_t mask = getMask(pixelIndex);
        bool lsb = rowBitPlaneLo & mask;
        bool msb = rowBitPlaneHi & mask;
        return (msb << 1) | lsb;
    };

    /* drawing priority replacement for transparent sprites */
    for (uint8_t i = 0; i < pixelFifo.size(); i++) {
        pixel.color = getPixelColor(i);
        auto& currentPixel = pixelFifo.at(i);
        if (currentPixel.color == 0) currentPixel = pixel;
    }

    /* push remaining pixels */
    for (uint8_t i = pixelFifo.size(); i < pixelFifo.capacity(); i++) {
        pixel.color =  getPixelColor(i);
        pixelFifo.push(pixel);
    }

    /* discard fetched sprite */
    spriteBuffer.pop();
    fetchedSprite = nullptr;
}

template class Graphics::SpriteFetcher<MODEL::DMG>;
template class Graphics::SpriteFetcher<MODEL::CGB>;