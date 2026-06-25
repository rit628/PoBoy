#include "PPU.hpp"
#include "FlagOps.hpp"
#include "GraphicsConstants.hpp"
#include "IMU.hpp"
#include <algorithm>
#include <array>
#include <cstdint>
#include <utility>

#include "TerminalRenderer.inl"

using namespace Graphics;

PPU::PPU(Interrupts::IMU& imu) : imu(imu) {}

void PPU::tick(uint8_t dots) {
    for (uint8_t i = 0; i < dots; i++) {
        tick();
    }
}

void PPU::tick() {
    if (disabled()) return;
    switch (mode) {
        using enum MODE;
        case OAM_SCAN:
            scanOAM();
        break;
        
        case PIXEL_TRANSFER:
            if (lineDotsElapsed >= DOTS_PER_OAM_SCAN_MODE + DOTS_PER_RENDER_STARTUP) [[ likely ]]
                mixer.tick();
        break;

        case HBLANK:
            
        break;

        case VBLANK:
            
        break;
    }
    updateStatus();
}

void PPU::scanOAM() {
    if (lineDotsElapsed % 2 > 0) return;    // oam scan tick every 2 dots
    uint8_t spriteIndex = lineDotsElapsed / 2 * SPRITE_BYTES;
    uint8_t yPos = oam.at(spriteIndex++);
    uint8_t xPos = oam.at(spriteIndex++);
    uint8_t tileNumber = oam.at(spriteIndex++);
    uint8_t spriteFlags = oam.at(spriteIndex++);
    mixer.addSprite(yPos, xPos, tileNumber, spriteFlags);
}

bool PPU::disabled() {
    if (!testFlags(lcdControl, LCDC_FLAG::LCD_AND_PPU_ENABLE)) {
        frameDotsElapsed = 0;
        lineDotsElapsed = 0;
        currentLine = 0;
        mode = MODE::OAM_SCAN;
        mixer.scanlineReset();
        return true;
    }
    return false;
}

void PPU::updateStatus() {
    lineDotsElapsed++;
    frameDotsElapsed++;

    auto incrementLine = [this]() {
        lineDotsElapsed = 0;
        currentLine++;
        using enum STAT_FLAG;
        if (testFlags(readSTAT(), LYC_INTERRUPT_ENABLE, LYC_INTERRUPT_BIT)) {
            imu.writeIF(Interrupts::INTERRUPT_FLAG::LCD_STAT);
        }
    };

    auto updateMode = [this]<MODE mode>() {
        this->mode = mode;
        uint8_t stat = readSTAT();
        using enum STAT_FLAG;
        constexpr uint8_t modeNumber = std::to_underlying(mode);
        if constexpr (modeNumber == 0) {
            if (testFlags(stat, MODE_0_INTERRUPT_ENABLE))
                imu.writeIF(Interrupts::INTERRUPT_FLAG::LCD_STAT);
        }
        else if constexpr (modeNumber == 1) {
            if (testFlags(stat, MODE_1_INTERRUPT_ENABLE))
                imu.writeIF(Interrupts::INTERRUPT_FLAG::LCD_STAT);
        }
        else if constexpr (modeNumber == 2) {
            if (testFlags(stat, MODE_2_INTERRUPT_ENABLE))
                imu.writeIF(Interrupts::INTERRUPT_FLAG::LCD_STAT);
        }
    };
    
    switch (mode) {
        using enum MODE;
        case OAM_SCAN:
            if (lineDotsElapsed >= DOTS_PER_OAM_SCAN_MODE) [[ unlikely ]]
                updateMode.operator()<PIXEL_TRANSFER>();
        break;
        
        case PIXEL_TRANSFER:
            if (mixer.atLineEnd()) [[ unlikely ]]
                updateMode.operator()<HBLANK>();
        break;

        case HBLANK:
            if (lineDotsElapsed >= DOTS_PER_LINE) [[ unlikely ]] {
                updateMode.operator()<OAM_SCAN>();
                incrementLine();
                mixer.scanlineReset();
            }
            
            if (frameDotsElapsed >= DOTS_PER_LCD_SCAN) [[ unlikely ]] {
                updateMode.operator()<VBLANK>();
                imu.writeIF(Interrupts::INTERRUPT_FLAG::VBLANK);
                renderFrame(mixer.extractFrame());
            }
        break;

        case VBLANK:
            if (lineDotsElapsed >= DOTS_PER_LINE) [[ unlikely ]]
                incrementLine();
            
            if (frameDotsElapsed >= DOTS_PER_FRAME) [[ unlikely ]] {
                updateMode.operator()<OAM_SCAN>();
                frameDotsElapsed = 0;
                currentLine = 0;
                mixer.scanlineReset();
            }
        break;
    }
}

uint8_t PPU::readVRAM(uint16_t address) {
    if (mode == MODE::PIXEL_TRANSFER && !disabled()) return 0xFF;
    return vram.at(address);
}

void PPU::writeVRAM(uint16_t address, uint8_t value) {
    if (mode == MODE::PIXEL_TRANSFER && !disabled()) return;
    vram.at(address) = value;
}

uint8_t PPU::readOAM(uint16_t address) {
    using enum MODE;
    if ((mode == PIXEL_TRANSFER || mode == OAM_SCAN) && !disabled()) return 0xFF;
    return oam.at(address);
}

void PPU::writeOAM(uint16_t address, uint8_t value) {
    using enum MODE;
    if ((mode == PIXEL_TRANSFER || mode == OAM_SCAN) && !disabled()) return;
    oam.at(address) = value;
}

void PPU::dmaTransferOAM(std::span<const uint8_t, OAM_SIZE> sourceRange) {
    std::copy(sourceRange.begin(), sourceRange.end(), oam.begin());
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

uint8_t PPU::readWX() {
    return windowX;
}

void PPU::writeWX(uint8_t value) {
    windowX = value;
}

uint8_t PPU::readWY() {
    return windowY;
}

void PPU::writeWY(uint8_t value) {
    windowY = value;
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

uint8_t PPU::readOBP0() {
    return spritePalette0;
}

void PPU::writeOBP0(uint8_t value) {
    spritePalette0 = value;
}

uint8_t PPU::readOBP1() {
    return spritePalette1;
}

void PPU::writeOBP1(uint8_t value) {
    spritePalette1 = value;
}

uint8_t PPU::readSTAT() {
    return interruptMask
        | (lineCompare == currentLine % FRAME_LINES) << 2
        | std::to_underlying(mode);
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