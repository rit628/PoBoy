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
                           , tileDataBank0(tileData)
                           , tileDataBank1(tileData)
{
    if constexpr (Model == MODEL::CGB) {
        tileDataBank1 = vram.template subspan<VRAM_BANK_SIZE, TILE_DATA_SIZE>();
    }
}

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
        if constexpr (Model == MODEL::CGB) {
            tileData = testFlags(fetchedSprite->spriteFlags, ATTRIBUTE_FLAG::CGB_BANK) ? tileDataBank1 : tileDataBank0;
        }
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
    if (spriteBuffer.full()) return;                    // ensure buffer has space 
    if (yPos > this->yPos) return;                      // ensure visibility on current scanline rowwise
    if (yPos + spriteHeight <= this->yPos) return;      // ensure sprite has not been completely rendered previously
    spriteBuffer.push({
                       yPos
                     , xPos
                     , tileNumber
                     , spriteFlags
                     , static_cast<uint8_t>(spriteBuffer.size())
                    });
}

template<MODEL Model>
void SpriteFetcher<Model>::sortSprites() {
    std::ranges::stable_sort(spriteBuffer.data(), {}, &Sprite::xPos);
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
    uint8_t tileRow = (yPos - fetchedSprite->yPos) % TILE_ROW_COUNT;
    if (testFlags(fetchedSprite->spriteFlags, ATTRIBUTE_FLAG::Y_FLIP)) {
        tileRow ^= (TILE_ROW_COUNT - 1);    // inverts row index
    }
    return tileAddress + tileRow * TILE_ROW_BYTES;
}

template<MODEL Model>
void SpriteFetcher<Model>::getTile() {
    tileId = fetchedSprite->tileNumber;
    if (doubleHeightSprites) {
        bool onSecondTile = yPos >= fetchedSprite->yPos + TILE_ROW_COUNT;
        if (testFlags(fetchedSprite->spriteFlags, ATTRIBUTE_FLAG::Y_FLIP)) {
            onSecondTile = !onSecondTile;   // flip stacked tile ordering
        }
        tileId = (tileId & 0xFE) + onSecondTile;    // stacked tile index is determined by lowest bit
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
    bool xFlip = testFlags(fetchedSprite->spriteFlags, ATTRIBUTE_FLAG::X_FLIP);
    Pixel pixel;
    pixel.bgPriority = testFlags(fetchedSprite->spriteFlags, ATTRIBUTE_FLAG::BG_PRIORITY);
    if constexpr (Model == MODEL::DMG) {
        pixel.paletteNumber = testFlags(fetchedSprite->spriteFlags, ATTRIBUTE_FLAG::DMG_PALETTE);
    }
    else {
        pixel.paletteNumber = extractFlags(fetchedSprite->spriteFlags, ATTRIBUTE_FLAG::CGB_PALETTE);
        pixel.spritePriority = fetchedSprite->priority;
    }
    
    auto getColorIndex = createColorIndexExtractor(xFlip);

    /* drawing priority replacement */
    for (uint8_t i = 0; i < pixelFifo.size(); i++) {
        if ((pixel.colorIndex = getColorIndex(i)) == 0) continue;   // skip transparent pixels
        auto& currentPixel = pixelFifo.at(i);
        /* transparent pixel replacement */
        if (currentPixel.colorIndex == 0) currentPixel = pixel;
        else if constexpr (Model == MODEL::CGB) {
            /* opaque sprite oam index based priority replacement */
            if (pixel.spritePriority < currentPixel.spritePriority) currentPixel = pixel;
        }
    }

    /* push remaining pixels */
    for (uint8_t i = pixelFifo.size(); i < pixelFifo.capacity(); i++) {
        pixel.colorIndex = getColorIndex(i);
        pixelFifo.push(pixel);
    }

    /* discard fetched sprite */
    spriteBuffer.pop();
    fetchedSprite = nullptr;
}

template class Graphics::SpriteFetcher<MODEL::DMG>;
template class Graphics::SpriteFetcher<MODEL::CGB>;