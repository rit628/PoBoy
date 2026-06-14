#include "BackgroundFetcher.hpp"
#include "FlagOps.hpp"
#include "GraphicsConstants.hpp"
#include "Pixel.hpp"
#include <cstdint>

using namespace Graphics;

BackgroundFetcher::BackgroundFetcher(const uint8_t& lcdControl
                                   , const uint8_t& bgPalette
                                   , const uint8_t& yPos
                                   , const uint8_t& bgX
                                   , const uint8_t& bgY
                                   , std::span<const uint8_t, TILE_DATA_SIZE> tileData
                                   , std::span<const uint8_t, 2 * TILE_MAP_SIZE> tileMaps)
                                   : lcdControl(lcdControl)
                                   , bgPalette(bgPalette)
                                   , yPos(yPos)
                                   , bgX(bgX)
                                   , bgY(bgY)
                                   , tileData(tileData)
                                   , tileMaps(tileMaps)
                                   {}

void BackgroundFetcher::tick() {
    switch (state) {
        using enum STATE;
        case GET_TILE:
            getTile();
        break;

        case GET_TILE_DATA_LO:
            getTileDataLo();
        break;

        case GET_TILE_DATA_HI:
            getTileDataHi();
        break;
        
        case SLEEP:
            sleep();
        break;

        case PUSH:
            push();
        break;
    }
    onSecondDot = !onSecondDot;
}

void BackgroundFetcher::reset() {
    fifoFront = pixelFifo.size();
    state = STATE::GET_TILE;
    onSecondDot = false;
    xPos = 0;
}

Pixel BackgroundFetcher::fifoPop() {
    return pixelFifo.at(fifoFront++);
}

bool BackgroundFetcher::fifoEmpty() {
    return fifoFront == pixelFifo.size();
}

uint8_t BackgroundFetcher::getXCoordinate() {
    return xPos;
}

void BackgroundFetcher::getTile() {
    if (!onSecondDot) return; // wait for one dot
    bool selectedTileMap = flagTest(lcdControl, LCDC_FLAG::BACKGROUND_TILEMAP_OFFSET);
    uint8_t xCoordinate = (xPos + bgX) / 8;
    uint8_t yCoordinate = (yPos + bgY) / 8;
    uint16_t posAddress = selectedTileMap << 10 | yCoordinate << 5 | xCoordinate;
    tileId = tileMaps[posAddress];
    state = STATE::GET_TILE_DATA_LO;
}

void BackgroundFetcher::getTileDataLo() {
    if (!onSecondDot) return; // wait for one dot
    bool selectedTileData = flagTest(lcdControl, LCDC_FLAG::BACKGROUND_AND_WINDOW_DATA_OFFSET);
    uint8_t tileRow = (yPos + bgY) % 8;
    tileAddress = selectedTileData << 12 | tileId << 4 | tileRow << 1;
    tileBitPlaneLo = tileData[tileAddress];
    state = STATE::GET_TILE_DATA_HI;
}

void BackgroundFetcher::getTileDataHi() {
    if (!onSecondDot) return; // wait for one dot
    tileBitPlaneHi = tileData[tileAddress + 1];
    state = STATE::SLEEP;
}

void BackgroundFetcher::sleep() {
    if (!onSecondDot) return; // wait for one dot
    state = STATE::PUSH;
    push();
}

void BackgroundFetcher::push() {
    if (!fifoEmpty()) return;
    /* refill pixel fifo */
    for (uint8_t i = 0; i < pixelFifo.size(); i++) {
        bool msb = tileBitPlaneLo & (0x1 << (7 - i));
        bool lsb = tileBitPlaneHi & (0x1 << (7 - i));
        uint8_t paletteIndex = (msb << 1) | lsb;
        Pixel pixel;
        pixel.color = (bgPalette >> (2 * paletteIndex)) & 0b11;
        pixelFifo.at(i) = pixel;
    }
    fifoFront = 0;
       
    /* increment x past row and return to first state */
    xPos += 8;
    state = STATE::GET_TILE;
    onSecondDot = true;
}