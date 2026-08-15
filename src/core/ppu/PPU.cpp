#include "PPU.hpp"
#include "FlagOps.hpp"
#include "GraphicsConstants.hpp"
#include "IMU.hpp"
#include "MemoryConstants.hpp"
#include "SystemConstants.hpp"
#include <algorithm>
#include <array>
#include <cstdint>
#include <utility>

using namespace Graphics;

template<MODEL Model>
PPU<Model>::PPU(Interrupts::IMU& imu, std::function<void(std::span<const uint8_t>)> renderFrame)
        : imu(imu), renderFrame(renderFrame)
        , currentBank(std::span(vram).template subspan<0, VRAM_BANK_SIZE>())
{
    initialize();
}

template<MODEL Model>
void PPU<Model>::initialize() {
    vram.fill(0);
    oam.fill(0);
    enabled = false;
    lineDotsElapsed = 0;
    frameDotsElapsed = 0;
    statInterrupted = false;

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
    mode = PPU_MODE::HBLANK;

    if constexpr (Model == MODEL::CGB) {
        vramBank = 0;
    }
    else {
        vramBank = 0xFF;
    }

    mixer.extractFrame(); // resets pixel fifos to initial frame state
}

template<MODEL Model>
void PPU<Model>::initHLE() {
    using namespace Memory;

    writeIO<LCDC>   (0x91);
    writeIO<STAT>   (0x85);
    writeIO<SCY>    (0x00);
    writeIO<SCX>    (0x00);
    writeIO<LY>     (0x00);
    writeIO<LYC>    (0x00);
    writeIO<BGP>    (0xFC);
    writeIO<OBP0>   (0xFF);  // random/uninitialized (treat as 0xFF)
    writeIO<OBP1>   (0xFF);  // random/uninitialized (treat as 0xFF)
    writeIO<WY>     (0x00);
    writeIO<WX>     (0x00);
}

template<MODEL Model>
void PPU<Model>::tick() {
    if (!enabled) return;
    switch (mode) {
        using enum PPU_MODE;
        case OAM_SCAN:          return tickDispatch<OAM_SCAN>();
        case PIXEL_TRANSFER:    return tickDispatch<PIXEL_TRANSFER>();
        case HBLANK:            return tickDispatch<HBLANK>();
        case VBLANK:            return tickDispatch<VBLANK>();
    }
}

template<MODEL Model>
template<PPU_MODE Mode>
void PPU<Model>::tickDispatch() {
    tick<Mode>();
    lineDotsElapsed++;
    frameDotsElapsed++;
    postTick<Mode>();
}

template<MODEL Model>
template<PPU_MODE Mode>
void PPU<Model>::tick() {
    using enum PPU_MODE;
    if constexpr (Mode == OAM_SCAN) {
        if (lineDotsElapsed % 2 > 0) return;  // oam scan tick every 2 dots
        uint8_t spriteIndex = lineDotsElapsed / 2 * SPRITE_BYTES;
        uint8_t yPos = oam.at(spriteIndex++);
        uint8_t xPos = oam.at(spriteIndex++);
        uint8_t tileNumber = oam.at(spriteIndex++);
        uint8_t spriteFlags = oam.at(spriteIndex++);
        mixer.addSprite(yPos, xPos, tileNumber, spriteFlags);
    }
    else if constexpr (Mode == PIXEL_TRANSFER) {
        mixer.tick();
    }
    else {
        /* HBLANK and VBLANK do nothing */    
    }
}

template<MODEL Model>
template<PPU_MODE Mode>
void PPU<Model>::postTick() {
    using enum PPU_MODE;
    if constexpr (Mode == OAM_SCAN) {
        if (lineDotsElapsed >= DOTS_PER_OAM_SCAN_MODE) [[ unlikely ]] {
            mixer.scanlineInitialize();
            updateMode<PIXEL_TRANSFER>();
        }
    }
    else if constexpr (Mode == PIXEL_TRANSFER) {
        if (mixer.atLineEnd()) [[ unlikely ]] {
            updateMode<HBLANK>();
        }
    }
    else if constexpr (Mode == HBLANK) {
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
    else if constexpr (Mode == VBLANK) {
        if (lineDotsElapsed >= DOTS_PER_LINE) [[ unlikely ]]
            incrementLine();
        
        if (currentLine == 153 && lineDotsElapsed == 4) [[ unlikely ]] { // scanline 153 quirk
            currentLine = 0;
            attemptStatusInterrupt();
        }

        if (frameDotsElapsed >= DOTS_PER_FRAME) [[ unlikely ]] {
            updateMode<OAM_SCAN>();
            frameDotsElapsed = 0;
            currentLine = 0;
            mixer.scanlineReset();
        }
    }
}

template<MODEL Model>
uint8_t PPU<Model>::readVRAM(uint16_t address) {
    if (mode == PPU_MODE::PIXEL_TRANSFER && enabled) return 0xFF;
    return currentBank[address];
}

template<MODEL Model>
void PPU<Model>::writeVRAM(uint16_t address, uint8_t value) {
    if (mode == PPU_MODE::PIXEL_TRANSFER && enabled) return;
    currentBank[address] = value;
}

template<MODEL Model>
uint8_t PPU<Model>::readOAM(uint16_t address) {
    using enum PPU_MODE;
    if ((mode == PIXEL_TRANSFER || mode == OAM_SCAN) && enabled) return 0xFF;
    return oam.at(address);
}

template<MODEL Model>
void PPU<Model>::writeOAM(uint16_t address, uint8_t value) {
    using enum PPU_MODE;
    if ((mode == PIXEL_TRANSFER || mode == OAM_SCAN) && enabled) return;
    oam.at(address) = value;
}

template<MODEL Model>
void PPU<Model>::dmaTransferOAM(std::span<const uint8_t, OAM_SIZE> sourceRange) {
    std::copy(sourceRange.begin(), sourceRange.end(), oam.begin());
}

template<MODEL Model>
void PPU<Model>::attemptStatusInterrupt() {
    if (!enabled) return;
    using enum STAT_FLAG;
    uint8_t stat = readIO<Memory::STAT>();
    uint8_t modeNumber = std::to_underlying(mode);
    bool interruptUnblocked = !statInterrupted;
    statInterrupted = testFlags(stat, LYC_INTERRUPT_ENABLE, LYC_INTERRUPT_BIT)
                   || (modeNumber == 0 && testFlags(stat, MODE_0_INTERRUPT_ENABLE))
                   || (modeNumber == 1 && testFlags(stat, MODE_1_INTERRUPT_ENABLE))
                   || (modeNumber == 2 && testFlags(stat, MODE_2_INTERRUPT_ENABLE));
    if (interruptUnblocked && statInterrupted) {
        imu.triggerInterrupt(Interrupts::INTERRUPT_FLAG::LCD_STAT);
    }
}

template<MODEL Model>
void PPU<Model>::incrementLine() {
    lineDotsElapsed = 0;
    currentLine++;
    attemptStatusInterrupt();
}

template<MODEL Model>
template<PPU_MODE Mode>
void PPU<Model>::updateMode() {
    mode = Mode;
    attemptStatusInterrupt();
}

template<MODEL Model>
void PPU<Model>::disableLCD() {
    /* reset ppu state and render blank frame to emulate lcd shutting off */
    frameDotsElapsed = 0;
    lineDotsElapsed = 0;
    currentLine = 0;
    mode = PPU_MODE::HBLANK;
    statInterrupted = false;
    mixer.extractFrame();
    static constexpr std::array<uint8_t, FRAMEBUFFER_SIZE> blank{};
    renderFrame(blank);
}

template<MODEL Model>
void PPU<Model>::enableLCD() {
    updateMode<PPU_MODE::OAM_SCAN>();
}

template class Graphics::PPU<MODEL::DMG>;
template class Graphics::PPU<MODEL::CGB>;