#include "PPU.hpp"
#include "IO.hpp"
#include <cstdint>

void PPU::tick(uint8_t dots) {
    for (uint8_t i = 0; i < dots; i++) {
        tick();
    }
}

void PPU::tick() {
    // TODO: render based on mode
    updateMode();
}

void PPU::updateMode() {
    lineDotsElapsed++;
    frameDotsElapsed++;
    
    using enum MODE;
    switch (currentMode) {
        case OAM_SCAN:
            if (lineDotsElapsed >= DOTS_PER_OAM_SCAN_MODE) currentMode = PIXEL_TRANSFER;
        break;
        
        case PIXEL_TRANSFER:
            // for now just use cap; actual implementation using pixel fifo count will replace this
            if (lineDotsElapsed >= MAX_DOTS_PER_PIXEL_TRANSFER_MODE) currentMode = HBLANK;
        break;

        case HBLANK:
            if (lineDotsElapsed >= DOTS_PER_LINE) currentMode = OAM_SCAN;
            if (frameDotsElapsed >= DOTS_PER_LCD_SCAN) currentMode = VBLANK;
        break;

        case VBLANK:
            if (frameDotsElapsed >= DOTS_PER_FRAME) currentMode = OAM_SCAN;
        break;
    }
    
    /* for now wrap the counters here in case mode switching is broken */
    if (lineDotsElapsed >= DOTS_PER_LINE) {
        lineDotsElapsed = 0;
        mmu.write(IO::LY,  mmu.read(IO::LY) + 1);
    }
    if (frameDotsElapsed >= DOTS_PER_FRAME) {
        frameDotsElapsed = 0;
        mmu.write(IO::LY, 0);
    }
}