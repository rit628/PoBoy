#include "BackgroundFetcher.hpp"
#include "FlagOps.hpp"
#include "GraphicsConstants.hpp"
#include "Pixel.hpp"
#include <cstdint>

using namespace Graphics;

BackgroundFetcher::BackgroundFetcher(const uint8_t& lcdControl
                                   , const uint8_t& bgPalette
                                   , const uint8_t& yPos
                                   , const uint8_t& scrollX
                                   , const uint8_t& scrollY
                                   , const uint8_t& windowX
                                   , const uint8_t& windowY
                                   , std::span<const uint8_t, TILE_DATA_SIZE> tileData
                                   , std::span<const uint8_t, 2 * TILE_MAP_SIZE> tileMaps)
                                   : lcdControl(lcdControl)
                                   , bgPalette(bgPalette)
                                   , yPos(yPos)
                                   , scrollX(scrollX)
                                   , scrollY(scrollY)
                                   , windowX(windowX)
                                   , windowY(windowY)
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
    bool windowEnabled = testFlags(lcdControl, LCDC_FLAG::WINDOW_ENABLE);
    bool inWindow = windowEnabled && (xPos + WINDOW_X_OFFSET >= windowX) && (yPos >= windowY);
    uint16_t tileIdAddress = 0;
    if (inWindow) {  // get window tile
        bool selectedTileMap = testFlags(lcdControl, LCDC_FLAG::WINDOW_TILEMAP_OFFSET);
        uint8_t xCoordinate = xPos / 8;
        uint8_t yCoordinate = windowY / 8;
        tileIdAddress = selectedTileMap << 10 | yCoordinate << 5 | xCoordinate;
    }
    else {  // get background tile
        bool selectedTileMap = testFlags(lcdControl, LCDC_FLAG::BACKGROUND_TILEMAP_OFFSET);
        uint8_t xCoordinate = (xPos + scrollX) / 8;
        uint8_t yCoordinate = (yPos + scrollY) / 8;
        tileIdAddress = selectedTileMap << 10 | yCoordinate << 5 | xCoordinate;
    }
    tileId = tileMaps[tileIdAddress];
    state = STATE::GET_TILE_DATA_LO;
}

void BackgroundFetcher::getTileDataLo() {
    if (!onSecondDot) return; // wait for one dot
    bool windowEnabled = testFlags(lcdControl, LCDC_FLAG::WINDOW_ENABLE);
    bool inWindow = windowEnabled && (xPos + WINDOW_X_OFFSET >= windowX) && (yPos >= windowY);
    bool selectedTileData = testFlags(lcdControl, LCDC_FLAG::BACKGROUND_AND_WINDOW_DATA_OFFSET);
    uint8_t tileRow = 0;
    if (inWindow) {  // get window tile data
        tileRow = windowY % 8;
    }
    else {  // get background tile data
        tileRow = (yPos + scrollY) % 8;
    }
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