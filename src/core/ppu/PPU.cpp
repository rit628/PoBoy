#include "PPU.hpp"
#include "FlagOps.hpp"
#include "GraphicsConstants.hpp"
#include <array>
#include <cstdint>

using namespace Graphics;

#include "TerminalRenderer.inl"

void PPU::tick(uint8_t dots) {
    for (uint8_t i = 0; i < dots; i++) {
        tick();
    }
}

void PPU::tick() {
    switch (mode) {
        using enum MODE;
        case OAM_SCAN:
            // for now we only render background
        break;
        
        case PIXEL_TRANSFER:
            if (lineDotsElapsed >= DOTS_PER_OAM_SCAN_MODE + DOTS_PER_RENDER_STARTUP) [[ likely ]] {
                mixer.tick();
            }
        break;

        case HBLANK:
            
        break;

        case VBLANK:
            
        break;
    }
    updateMode();
}

void PPU::updateMode() {
    lineDotsElapsed++;
    frameDotsElapsed++;
    
    switch (mode) {
        using enum MODE;
        case OAM_SCAN:
            if (lineDotsElapsed >= DOTS_PER_OAM_SCAN_MODE) mode = PIXEL_TRANSFER;
        break;
        
        case PIXEL_TRANSFER:
            if (mixer.atLineEnd()) {
                mode = HBLANK;
                mixer.resetFifos();
            }
        break;

        case HBLANK:
            if (lineDotsElapsed >= DOTS_PER_LINE) mode = OAM_SCAN;
            if (frameDotsElapsed >= DOTS_PER_LCD_SCAN) {
                mode = VBLANK;
                renderFrame(mixer.extractFrame());
            }
        break;

        case VBLANK:
            if (frameDotsElapsed >= DOTS_PER_FRAME) mode = OAM_SCAN;
        break;
    }
    
    /* for now wrap the counters here in case mode switching is broken */
    if (lineDotsElapsed >= DOTS_PER_LINE) {
        lineDotsElapsed = 0;
        currentLine++;
    }
    if (frameDotsElapsed >= DOTS_PER_FRAME) {
        frameDotsElapsed = 0;
        currentLine = 0;
    }
}

uint8_t PPU::readVRAM(uint16_t address) {
    if (mode == MODE::PIXEL_TRANSFER && flagTest(lcdControl, LCDC_FLAG::LCD_AND_PPU_ENABLE)) return 0xFF;
    return vram.at(address);
}

void PPU::writeVRAM(uint16_t address, uint8_t value) {
    if (mode == MODE::PIXEL_TRANSFER && flagTest(lcdControl, LCDC_FLAG::LCD_AND_PPU_ENABLE)) return;
    vram.at(address) = value;
}

uint8_t PPU::readOAM(uint16_t address) {
    return oam.at(address);
}

void PPU::writeOAM(uint16_t address, uint8_t value) {
    oam.at(address) = value;
}

uint8_t PPU::readLY() {
    return currentLine;
}

uint8_t PPU::readSCX() {
    return scrollX;
}

void PPU::writeSCX(uint8_t value) {
    scrollX = value;
}

uint8_t PPU::readSCY() {
    return scrollY;
}

void PPU::writeSCY(uint8_t value) {
    scrollY = value;
}

uint8_t PPU::readLCDC() {
    return lcdControl;
}

void PPU::writeLCDC(uint8_t value) {
    lcdControl = value;
}

uint8_t PPU::readBGP() {
    return backgroundPalette;
}

void PPU::writeBGP(uint8_t value) {
    backgroundPalette = value;
}

std::span<const uint8_t, TILE_DATA_SIZE> PPU::getTileData() {
    return std::span(vram).subspan<0, TILE_DATA_SIZE>();
}

std::span<const uint8_t, 2 * TILE_MAP_SIZE> PPU::getTileMaps() {
    return std::span(vram).subspan<TILE_DATA_SIZE, 2 * TILE_MAP_SIZE>();
}