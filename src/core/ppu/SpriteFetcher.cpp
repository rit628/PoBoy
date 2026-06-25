#include "SpriteFetcher.hpp"
#include "FlagOps.hpp"
#include "GraphicsConstants.hpp"
#include <cstdint>

using namespace Graphics;

SpriteFetcher::SpriteFetcher(const uint8_t& lcdControl
                           , const uint8_t& xPos
                           , const uint8_t& currentLine
                           , std::span<const uint8_t, TILE_DATA_SIZE> tileData)
                           : lcdControl(lcdControl)
                           , xPos(xPos)
                           , currentLine(currentLine)
                           , tileData(tileData)
                           {}

bool SpriteFetcher::spriteAvailable() {
    if (!testFlags(lcdControl, LCDC_FLAG::SPRITE_ENABLE)) {
        fetchReset();
        return false;
    };
    if (fetchedSprite) return true; // dont recheck if already fetched
    for (uint8_t i = 0; i < bufferIndex; i++) { // for loop should be reasonable for this
        auto& sprite = spriteBuffer.at(i);
        if (sprite.xPos == xPos) {
            fetchedSprite = &sprite;
            return true;
        }
    }
    return false;
}

void SpriteFetcher::reset() {
    bufferIndex = 0;
    fetchReset();
}

void SpriteFetcher::addSprite(uint8_t yPos, uint8_t xPos, uint8_t tileNumber, uint8_t spriteFlags) {
    uint8_t spriteHeight = 8 * (testFlags(lcdControl, LCDC_FLAG::SPRITE_SIZE_MODIFIER) + 1);
    this->yPos = currentLine + SPRITE_Y_OFFSET;
    if (bufferIndex == MAX_SPRITES_PER_LINE) return;    // ensure buffer has space 
    if (xPos == 0) return;                              // ensure visibility on current scanline columnwise
    if (yPos > this->yPos) return;                      // ensure visibility on current scanline rowwise
    if (yPos + spriteHeight <= this->yPos) return;      // ensure sprite has not been completely rendered previously
    spriteBuffer.at(bufferIndex++) = {yPos, xPos, tileNumber, spriteFlags};
}

void SpriteFetcher::fetchReset() {
    resetState();
    fetchedSprite = nullptr;
    pixelFifo.clear();
}

void SpriteFetcher::preTick() {}

uint16_t SpriteFetcher::getTileRowAddress() {
    uint16_t tileAddress = tileId * TILE_BYTES;
    uint8_t tileRow = (yPos - fetchedSprite->yPos) % 8;
    bool yFlip = testFlags(fetchedSprite->spriteFlags, SPRITE_FLAG::Y_FLIP);
    tileRow = 0b111 & ((yFlip) ? ~tileRow : tileRow);   // negate and mask to flip and remain in range
    return tileAddress + tileRow * TILE_ROW_BYTES;
}

void SpriteFetcher::getTile() {
    tileId = fetchedSprite->tileNumber;
    if (testFlags(lcdControl, LCDC_FLAG::SPRITE_SIZE_MODIFIER)) {
        bool onSecondTile = yPos >= fetchedSprite->yPos + 8;
        bool yFlip = testFlags(fetchedSprite->spriteFlags, SPRITE_FLAG::Y_FLIP);
        tileId = (tileId & ~0b1) + ((yFlip) ? !onSecondTile : onSecondTile);
    }
}

void SpriteFetcher::getTileDataLo() {
    rowBitPlaneLo = tileData[getTileRowAddress()];
}

void SpriteFetcher::getTileDataHi() {
    rowBitPlaneHi = tileData[getTileRowAddress() + 1];
}

void SpriteFetcher::sleep() {
    state = STATE::PUSH;
}

void SpriteFetcher::push() {
    Pixel pixel;
    pixel.palette = testFlags(fetchedSprite->spriteFlags, SPRITE_FLAG::PALETTE_NUMBER);
    pixel.backgroundPriority = testFlags(fetchedSprite->spriteFlags, SPRITE_FLAG::OBJ_TO_BG_PRIORITY);
    bool xFlip = testFlags(fetchedSprite->spriteFlags, SPRITE_FLAG::X_FLIP);
    auto getPixelColor = [this, xFlip](uint8_t pixelIndex) -> uint8_t {
        uint8_t mask = 0x1 << ((xFlip) ? pixelIndex : 7 - pixelIndex);
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
    fetchedSprite->xPos = UINT8_MAX;
    fetchedSprite = nullptr;
}