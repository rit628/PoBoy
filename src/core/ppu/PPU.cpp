#include "PPU.hpp"
#include "FlagOps.hpp"
#include "GraphicsConstants.hpp"
#include "IMU.hpp"
#include "MemoryConstants.hpp"
#include <algorithm>
#include <array>
#include <cstdint>
#include <utility>

using namespace Graphics;

PPU::PPU(Interrupts::IMU& imu, std::function<void(std::span<const uint8_t>)> renderFrame)
        : imu(imu), renderFrame(renderFrame)
{
    initialize();
}

void PPU::initialize() {
    vram.fill(0);
    oam.fill(0);
    enabled = false;
    lineDotsElapsed = 0;
    frameDotsElapsed = 0;

    currentLine = 0;
    lineCompare = 0;
    lcdControl = 0;
    scrollX = 0;
    scrollY = 0;
    windowX = 0;
    windowY = 0;
    backgroundPalette = 0;
    spritePalette0 = 0;
    spritePalette1 = 0;

    interruptMask = 0x80;
    mode = MODE::OAM_SCAN;

    mixer.extractFrame(); // resets pixel fifos to initial frame state
}

uint8_t PPU::readVRAM(uint16_t address) {
    if (mode == MODE::PIXEL_TRANSFER && enabled) return 0xFF;
    return vram.at(address);
}

void PPU::writeVRAM(uint16_t address, uint8_t value) {
    if (mode == MODE::PIXEL_TRANSFER && enabled) return;
    vram.at(address) = value;
}

uint8_t PPU::readOAM(uint16_t address) {
    using enum MODE;
    if ((mode == PIXEL_TRANSFER || mode == OAM_SCAN) && enabled) return 0xFF;
    return oam.at(address);
}

void PPU::writeOAM(uint16_t address, uint8_t value) {
    using enum MODE;
    if ((mode == PIXEL_TRANSFER || mode == OAM_SCAN) && enabled) return;
    oam.at(address) = value;
}

void PPU::dmaTransferOAM(std::span<const uint8_t, OAM_SIZE> sourceRange) {
    std::copy(sourceRange.begin(), sourceRange.end(), oam.begin());
}

template<uint16_t Register>
uint8_t PPU::readIO() {
    using namespace Memory;
    if constexpr (Register == LY)   return currentLine;
    if constexpr (Register == LYC)  return lineCompare;
    if constexpr (Register == SCX)  return scrollX;
    if constexpr (Register == SCY)  return scrollY;
    if constexpr (Register == WX)   return windowX;
    if constexpr (Register == WY)   return windowY;
    if constexpr (Register == LCDC) return lcdControl;
    if constexpr (Register == BGP)  return backgroundPalette;
    if constexpr (Register == OBP0) return spritePalette0;
    if constexpr (Register == OBP1) return spritePalette1;
}

template<>
uint8_t PPU::readIO<Memory::STAT>() {
    if (!enabled) return 0x80 | interruptMask;   // bits 0-2 return 0 when lcd is off
    return 0x80
        | interruptMask
        | (lineCompare == currentLine % FRAME_LINES) << 2
        | std::to_underlying(mode);
}

template<uint16_t Register>
void PPU::writeIO(uint8_t value) {
    using namespace Memory;
    if constexpr (Register == LYC)  lineCompare = value;
    if constexpr (Register == SCX)  scrollX = value;
    if constexpr (Register == SCY)  scrollY = value;
    if constexpr (Register == WX)   windowX = value;
    if constexpr (Register == WY)   windowY = value;
    if constexpr (Register == BGP)  backgroundPalette = value;
    if constexpr (Register == OBP0) spritePalette0 = value;
    if constexpr (Register == OBP1) spritePalette1 = value;
}

template<>
void PPU::writeIO<Memory::LCDC>(uint8_t value) {
    lcdControl = value;
    mixer.updateFlags(lcdControl);
    enabled = testFlags(lcdControl, LCDC_FLAG::LCD_AND_PPU_ENABLE);
    if (!enabled && mode == MODE::VBLANK) disableLCD();
}

template<>
void PPU::writeIO<Memory::STAT>(uint8_t value) {
    interruptMask = value & 0x78;   // bits 0-2 and 7 are read only
}

void PPU::incrementLine() {
    lineDotsElapsed = 0;
    currentLine++;
    using enum STAT_FLAG;
    if (testFlags(readIO<Memory::STAT>(), LYC_INTERRUPT_ENABLE, LYC_INTERRUPT_BIT)) {
        imu.triggerInterrupt(Interrupts::INTERRUPT_FLAG::LCD_STAT);
    }
}

template<PPU::MODE Mode>
void PPU::updateMode() {
    mode = Mode;
    uint8_t stat = readIO<Memory::STAT>();
    using enum STAT_FLAG;
    constexpr uint8_t modeNumber = std::to_underlying(Mode);
    if constexpr (modeNumber == 0) {
        if (testFlags(stat, MODE_0_INTERRUPT_ENABLE))
            imu.triggerInterrupt(Interrupts::INTERRUPT_FLAG::LCD_STAT);
    }
    else if constexpr (modeNumber == 1) {
        if (testFlags(stat, MODE_1_INTERRUPT_ENABLE))
            imu.triggerInterrupt(Interrupts::INTERRUPT_FLAG::LCD_STAT);
    }
    else if constexpr (modeNumber == 2) {
        if (testFlags(stat, MODE_2_INTERRUPT_ENABLE))
            imu.triggerInterrupt(Interrupts::INTERRUPT_FLAG::LCD_STAT);
    }
}

template<PPU::MODE Mode>
void PPU::tick() {
    /* HBLANK and VBLANK do nothing */
}

template<>
void PPU::tick<PPU::MODE::OAM_SCAN>() {
    if (lineDotsElapsed % 2 > 0) return;  // oam scan tick every 2 dots
    uint8_t spriteIndex = lineDotsElapsed / 2 * SPRITE_BYTES;
    uint8_t yPos = oam.at(spriteIndex++);
    uint8_t xPos = oam.at(spriteIndex++);
    uint8_t tileNumber = oam.at(spriteIndex++);
    uint8_t spriteFlags = oam.at(spriteIndex++);
    mixer.addSprite(yPos, xPos, tileNumber, spriteFlags);
}

template<>
void PPU::tick<PPU::MODE::PIXEL_TRANSFER>() {
    if (lineDotsElapsed >= DOTS_PER_OAM_SCAN_MODE + DOTS_PER_RENDER_STARTUP) [[ likely ]] {
        mixer.tick();
    }
}

template<>
void PPU::postTick<PPU::MODE::OAM_SCAN>() {
    using enum MODE;
    if (lineDotsElapsed >= DOTS_PER_OAM_SCAN_MODE) [[ unlikely ]] {
        mixer.scanlineInitialize();
        updateMode<PIXEL_TRANSFER>();
    }
}

template<>
void PPU::postTick<PPU::MODE::PIXEL_TRANSFER>() {
    using enum MODE;
    if (mixer.atLineEnd()) [[ unlikely ]] {
        updateMode<HBLANK>();
    }
}

template<>
void PPU::postTick<PPU::MODE::HBLANK>() {
    using enum MODE;
    if (lineDotsElapsed >= DOTS_PER_LINE) [[ unlikely ]] {
        updateMode<OAM_SCAN>();
        incrementLine();
        mixer.scanlineReset();
    }
    
    if (frameDotsElapsed >= DOTS_PER_LCD_SCAN) [[ unlikely ]] {
        updateMode<VBLANK>();
        imu.triggerInterrupt(Interrupts::INTERRUPT_FLAG::VBLANK);
        renderFrame(mixer.extractFrame());
    }
}

template<>
void PPU::postTick<PPU::MODE::VBLANK>() {
    using enum MODE;
    if (lineDotsElapsed >= DOTS_PER_LINE) [[ unlikely ]]
        incrementLine();
    
    if (currentLine == 153 && lineDotsElapsed == 4) [[ unlikely ]] { // scanline 153 quirk
        currentLine = 0;
        using enum STAT_FLAG;
        if (testFlags(readIO<Memory::STAT>(), LYC_INTERRUPT_ENABLE, LYC_INTERRUPT_BIT)) {
            imu.triggerInterrupt(Interrupts::INTERRUPT_FLAG::LCD_STAT);
        }
    }

    if (frameDotsElapsed >= DOTS_PER_FRAME) [[ unlikely ]] {
        updateMode<OAM_SCAN>();
        frameDotsElapsed = 0;
        currentLine = 0;
        mixer.scanlineReset();
    }
}

template<PPU::MODE Mode>
void PPU::tickDispatch() {
    tick<Mode>();
    lineDotsElapsed++;
    frameDotsElapsed++;
    postTick<Mode>();
}

void PPU::tick() {
    if (!enabled) return;
    switch (mode) {
        using enum MODE;
        case OAM_SCAN:          return tickDispatch<OAM_SCAN>();
        case PIXEL_TRANSFER:    return tickDispatch<PIXEL_TRANSFER>();
        case HBLANK:            return tickDispatch<HBLANK>();
        case VBLANK:            return tickDispatch<VBLANK>();
    }
}

void PPU::disableLCD() {
    /* reset ppu state and render blank frame to emulate lcd shutting off */
    frameDotsElapsed = 0;
    lineDotsElapsed = 0;
    currentLine = 0;
    mode = MODE::OAM_SCAN;
    mixer.extractFrame();
    static constexpr std::array<uint8_t, FRAMEBUFFER_SIZE> blank{};
    renderFrame(blank);
}

std::span<const uint8_t, TILE_DATA_SIZE> PPU::getTileData() {
    return std::span(vram).subspan<0, TILE_DATA_SIZE>();
}

std::span<const uint8_t, 2 * TILE_MAP_SIZE> PPU::getTileMaps() {
    return std::span(vram).subspan<TILE_DATA_SIZE, 2 * TILE_MAP_SIZE>();
}

template uint8_t PPU::readIO<Memory::LY>();
template uint8_t PPU::readIO<Memory::LYC>();
template uint8_t PPU::readIO<Memory::SCX>();
template uint8_t PPU::readIO<Memory::SCY>();
template uint8_t PPU::readIO<Memory::WX>();
template uint8_t PPU::readIO<Memory::WY>();
template uint8_t PPU::readIO<Memory::LCDC>();
template uint8_t PPU::readIO<Memory::BGP>();
template uint8_t PPU::readIO<Memory::OBP0>();
template uint8_t PPU::readIO<Memory::OBP1>();

template void PPU::writeIO<Memory::LY>(uint8_t);
template void PPU::writeIO<Memory::LYC>(uint8_t);
template void PPU::writeIO<Memory::SCX>(uint8_t);
template void PPU::writeIO<Memory::SCY>(uint8_t);
template void PPU::writeIO<Memory::WX>(uint8_t);
template void PPU::writeIO<Memory::WY>(uint8_t);
template void PPU::writeIO<Memory::BGP>(uint8_t);
template void PPU::writeIO<Memory::OBP0>(uint8_t);
template void PPU::writeIO<Memory::OBP1>(uint8_t);