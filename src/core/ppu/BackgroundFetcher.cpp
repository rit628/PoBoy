#include "BackgroundFetcher.hpp"
#include "FlagOps.hpp"
#include "GraphicsConstants.hpp"
#include <cstdint>

using namespace Graphics;

BackgroundFetcher::BackgroundFetcher(const uint8_t& lcdControl
                                   , const uint8_t& xPos
                                   , const uint8_t& yPos
                                   , const uint8_t& scrollX
                                   , const uint8_t& scrollY
                                   , const uint8_t& windowX
                                   , const uint8_t& windowY
                                   , std::span<const uint8_t, TILE_DATA_SIZE> tileData
                                   , std::span<const uint8_t, 2 * TILE_MAP_SIZE> tileMaps)
                                   : lcdControl(lcdControl)
                                   , xPos(xPos)
                                   , yPos(yPos)
                                   , scrollX(scrollX)
                                   , scrollY(scrollY)
                                   , windowX(windowX)
                                   , windowY(windowY)
                                   , tileData(tileData)
                                   , tileMaps(tileMaps)
                                   {}

void BackgroundFetcher::frameReset() {
    scanlineReset();
    windowYCondition = false;
    currentWindowLine = UINT8_MAX;  // ensures wrap around to 0 on first increment
}

void BackgroundFetcher::scanlineReset() {
    resetState();
    pixelFifo.fill();  // fill fifo to account for overscan
    renderingWindow = false;
    currentWindowColumn = 0;
    windowXCondition = false;
    windowYCondition = windowYCondition || yPos == windowY;
}

void BackgroundFetcher::preTick() {
    /* Update Fetcher Mode */
    windowXCondition = windowXCondition || (xPos == windowX + ADJUSTED_WINDOW_X_OFFSET);
    bool windowEnabled = testFlags(lcdControl, LCDC_FLAG::WINDOW_ENABLE);
    bool inWindow = windowEnabled && windowXCondition && windowYCondition;
    if (!renderingWindow && inWindow) {
        renderingWindow = true;
        currentWindowLine++;    // increment here to emulate mid scanline window rendering bug
        /* clear remaining pixels of last background tile and restart operation */
        pixelFifo.clear();
        resetState();   
    }
    else if (renderingWindow && !inWindow) {    // may not be necessary but handles mid scanline window disabling
        renderingWindow = false;
        windowXCondition = false;
        /* dont reset state since the last window tile must be completely rendered */
    }
}

uint16_t BackgroundFetcher::getTileRowAddress() {
    bool unsignedAddressing = testFlags(lcdControl, LCDC_FLAG::BACKGROUND_AND_WINDOW_DATA_AREA);
    uint16_t tileAddress = (unsignedAddressing) ? tileId * TILE_BYTES : 0x1000 + static_cast<int8_t>(tileId) * TILE_BYTES;
    uint8_t tileRow = 0;
    if (renderingWindow) {  // get window tile data
        tileRow = currentWindowLine % 8;
    }
    else {  // get background tile data
        tileRow = (yPos + scrollY) % 8;
    }
    return tileAddress + tileRow * TILE_ROW_BYTES;
}

void BackgroundFetcher::getTile() {
    uint8_t selectedTileMap = 0;
    uint8_t yCoordinate = 0;
    uint8_t xCoordinate = 0;
    if (renderingWindow) {  // get window tile
        selectedTileMap = testFlags(lcdControl, LCDC_FLAG::WINDOW_TILEMAP_AREA);
        yCoordinate = currentWindowLine / 8;
        xCoordinate = currentWindowColumn / 8;
    }
    else {  // get background tile
        selectedTileMap = testFlags(lcdControl, LCDC_FLAG::BACKGROUND_TILEMAP_AREA);
        /* x and y coordinates of tile are computed in 8 bits to allow wraparound scrolling */
        yCoordinate = ((yPos + scrollY) & 0xFF) / 8;
        xCoordinate = ((xPos + scrollX) & 0xFF) / 8;
    }
    uint16_t tileIdAddress = selectedTileMap * TILE_MAP_SIZE + yCoordinate * TILE_MAP_WIDTH + xCoordinate;
    tileId = tileMaps[tileIdAddress];
}

void BackgroundFetcher::getTileDataLo() {
    rowBitPlaneLo = tileData[getTileRowAddress()];
}

void BackgroundFetcher::getTileDataHi() {
    rowBitPlaneHi = tileData[getTileRowAddress() + 1];
}

void BackgroundFetcher::sleep() {
    if (pixelFifo.empty()) state = STATE::PUSH;
}

void BackgroundFetcher::push() {
    for (uint8_t i = 0; i < pixelFifo.capacity(); i++) {
        bool lsb = rowBitPlaneLo & (0x1 << (7 - i));
        bool msb = rowBitPlaneHi & (0x1 << (7 - i));
        Pixel pixel;
        pixel.color = (msb << 1) | lsb;
        pixelFifo.push(pixel);
    }
    if (renderingWindow) currentWindowColumn += 8;
}