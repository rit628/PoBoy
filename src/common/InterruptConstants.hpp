#pragma once
#include <cstdint>

namespace Interrupts {
    
    enum class INTERRUPT_FLAG : uint8_t {
        JOYPAD      = 0b00010000,
        SERIAL      = 0b00001000,
        TIMER       = 0b00000100,
        LCD_STAT    = 0b00000010,
        VBLANK      = 0b00000001
    };

    enum class P1_FLAG : uint8_t {
        SELECT_BUTTONS  = 0b00100000,
        SELECT_DPAD     = 0b00010000,
        START_OR_DOWN   = 0b00001000,
        SELECT_OR_UP    = 0b00000100,
        B_OR_LEFT       = 0b00000010,
        A_OR_RIGHT      = 0b00000001,
    };
    
    /* upper nibble holds buttons lower nibble holds dpad (aligned for P1 register masking) */
    enum class JOYPAD_INPUT : uint8_t {
        START       = 0b10000000,
        SELECT      = 0b01000000,
        B           = 0b00100000,
        A           = 0b00010000,
        DPAD_DOWN   = 0b00001000,
        DPAD_UP     = 0b00000100,
        DPAD_LEFT   = 0b00000010,
        DPAD_RIGHT  = 0b00000001,
    };

}