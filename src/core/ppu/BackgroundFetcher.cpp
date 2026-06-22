#include "BackgroundFetcher.hpp"
#include "FlagOps.hpp"
#include "GraphicsConstants.hpp"
#include "Pixel.hpp"
#include <cstdint>

using namespace Graphics;

BackgroundFetcher::BackgroundFetcher(const uint8_t& lcdControl
                                   , const uint8_t& XPos
                                   , const uint8_t& yPos
                                   , const uint8_t& scrollX
                                   , const uint8_t& scrollY
                                   , const uint8_t& windowX
                                   , const uint8_t& windowY
                                   , std::span<const uint8_t, TILE_DATA_SIZE> tileData
                                   , std::span<const uint8_t, 2 * TILE_MAP_SIZE> tileMaps)
                                   : lcdControl(lcdControl)
                                   , xPos(XPos)
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
    fifoFront = 0;
    state = STATE::GET_TILE;
    onSecondDot = false;
}

Pixel BackgroundFetcher::fifoPop() {
    return pixelFifo.at(fifoFront++);
}

bool BackgroundFetcher::fifoEmpty() {
    return fifoFront == pixelFifo.size();
}

uint16_t BackgroundFetcher::getTileRowAddress() {
    bool windowEnabled = testFlags(lcdControl, LCDC_FLAG::WINDOW_ENABLE);
    bool inWindow = windowEnabled && (xPos + WINDOW_X_OFFSET >= windowX) && (yPos >= windowY);
    
    bool unsignedAddressing = testFlags(lcdControl, LCDC_FLAG::BACKGROUND_AND_WINDOW_DATA_AREA);
    uint16_t tileAddress = (unsignedAddressing) ? tileId * TILE_BYTES : 0x1000 + static_cast<int8_t>(tileId) * TILE_BYTES;
    uint8_t tileRow = 0;
    if (inWindow) {  // get window tile data
        tileRow = windowY % 8;
    }
    else {  // get background tile data
        tileRow = (yPos + scrollY) % 8;
    }
    return tileAddress + tileRow * TILE_ROW_BYTES;
}

void BackgroundFetcher::getTile() {
    if (!onSecondDot) return; // wait for one dot
    bool windowEnabled = testFlags(lcdControl, LCDC_FLAG::WINDOW_ENABLE);
    bool inWindow = windowEnabled && (xPos + WINDOW_X_OFFSET >= windowX) && (yPos >= windowY);
    
    uint8_t selectedTileMap = 0;
    uint8_t yCoordinate = 0;
    uint8_t xCoordinate = 0;
    if (inWindow) {  // get window tile
        selectedTileMap = testFlags(lcdControl, LCDC_FLAG::WINDOW_TILEMAP_AREA);
        yCoordinate = windowY / 8;
        xCoordinate = xPos / 8;
    }
    else {  // get background tile
        selectedTileMap = testFlags(lcdControl, LCDC_FLAG::BACKGROUND_TILEMAP_AREA);
        /* x and y coordinates of tile are computed in 8 bits to allow wraparound scrolling */
        yCoordinate = ((yPos + scrollY) & 0xFF) / 8;
        xCoordinate = ((xPos + scrollX) & 0xFF) / 8;
    }
    uint16_t tileIdAddress = selectedTileMap * TILE_MAP_SIZE + yCoordinate * TILE_MAP_WIDTH + xCoordinate;
    tileId = tileMaps[tileIdAddress];
    state = STATE::GET_TILE_DATA_LO;
}

void BackgroundFetcher::getTileDataLo() {
    if (!onSecondDot) return; // wait for one dot
    rowBitPlaneLo = tileData[getTileRowAddress()];
    state = STATE::GET_TILE_DATA_HI;
}

void BackgroundFetcher::getTileDataHi() {
    if (!onSecondDot) return; // wait for one dot
    rowBitPlaneHi = tileData[getTileRowAddress() + 1];
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
        bool lsb = rowBitPlaneLo & (0x1 << (7 - i));
        bool msb = rowBitPlaneHi & (0x1 << (7 - i));
        Pixel pixel;
        pixel.color = (msb << 1) | lsb;
        pixelFifo.at(i) = pixel;
    }
    fifoFront = 0;
    state = STATE::GET_TILE;
}