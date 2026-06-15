#include "PPU.hpp"
#include "FlagOps.hpp"
#include "GraphicsConstants.hpp"
#include "IMU.hpp"
#include <array>
#include <cstdint>
#include <utility>

using namespace Graphics;

#include "TerminalRenderer.inl"

PPU::PPU(Interrupts::IMU& imu) : imu(imu) {}

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
    updateInterrupts();
    updateMode();
}

void PPU::updateInterrupts() {
    uint8_t stat = readSTAT();
    using enum STAT_FLAG;
    if (testFlags(stat, LYC_INTERRUPT_ENABLE, LYC_INTERRUPT_BIT)
     || (testFlags(stat, MODE_2_INTERRUPT_ENABLE) && extractFlags(stat, PPU_MODE_BITS) == 2)
     || (testFlags(stat, MODE_1_INTERRUPT_ENABLE) && extractFlags(stat, PPU_MODE_BITS) == 1)
     || (testFlags(stat, MODE_0_INTERRUPT_ENABLE) && extractFlags(stat, PPU_MODE_BITS) == 0))
    {
        imu.writeIF(Interrupts::INTERRUPT_FLAG::LCD_STAT);
    }
}

void PPU::updateMode() {
    lineDotsElapsed++;
    frameDotsElapsed++;

    auto incrementLine = [this]() {
        lineDotsElapsed = 0;
        currentLine++;
    };
    
    switch (mode) {
        using enum MODE;
        case OAM_SCAN:
            if (lineDotsElapsed >= DOTS_PER_OAM_SCAN_MODE) [[ unlikely ]] {
                mode = PIXEL_TRANSFER;
                mixer.resetFifos();
            }
        break;
        
        case PIXEL_TRANSFER:
            if (mixer.atLineEnd()) [[ unlikely ]] mode = HBLANK;
        break;

        case HBLANK:
            if (lineDotsElapsed >= DOTS_PER_LINE) [[ unlikely ]] {
                mode = OAM_SCAN;
                incrementLine();
            }
            if (frameDotsElapsed >= DOTS_PER_LCD_SCAN) [[ unlikely ]] {
                mode = VBLANK;
                renderFrame(mixer.extractFrame());
            }
        break;

        case VBLANK:
            if (lineDotsElapsed >= DOTS_PER_LINE) [[ unlikely ]] incrementLine();
            if (frameDotsElapsed >= DOTS_PER_FRAME) [[ unlikely ]] {
                mode = OAM_SCAN;
                frameDotsElapsed = 0;
                currentLine = 0;
            }
        break;
    }
}

uint8_t PPU::readVRAM(uint16_t address) {
    if (mode == MODE::PIXEL_TRANSFER && testFlags(lcdControl, LCDC_FLAG::LCD_AND_PPU_ENABLE)) return 0xFF;
    return vram.at(address);
}

void PPU::writeVRAM(uint16_t address, uint8_t value) {
    if (mode == MODE::PIXEL_TRANSFER && testFlags(lcdControl, LCDC_FLAG::LCD_AND_PPU_ENABLE)) return;
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

uint8_t PPU::readLYC() {
    return lineCompare;
}

void PPU::writeLYC(uint8_t value) {
    lineCompare = value;
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

uint8_t PPU::readSTAT() {
    return interruptMask | (lineCompare == currentLine) << 2 | std::to_underlying(mode);
}

void PPU::writeSTAT(uint8_t value) {
    interruptMask = value & 0x78;   // bits 0-2 and 7 are read only
}

std::span<const uint8_t, TILE_DATA_SIZE> PPU::getTileData() {
    return std::span(vram).subspan<0, TILE_DATA_SIZE>();
}

std::span<const uint8_t, 2 * TILE_MAP_SIZE> PPU::getTileMaps() {
    return std::span(vram).subspan<TILE_DATA_SIZE, 2 * TILE_MAP_SIZE>();
}