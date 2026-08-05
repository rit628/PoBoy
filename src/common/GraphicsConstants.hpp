#pragma once
#include <cstdint>

namespace Graphics {
    
    /* LCD Control Register Flags */
    enum class LCDC_FLAG : uint8_t {
        LCD_AND_PPU_ENABLE                  = 0b10000000,
        WINDOW_TILEMAP_AREA                 = 0b01000000,   // 0 = 0x9800-0x9BFF; 1 = 0x9C00-0x9FFF
        WINDOW_ENABLE                       = 0b00100000,
        BACKGROUND_AND_WINDOW_DATA_AREA     = 0b00010000,   // 0 = 0x8800-0x97FF (signed); 1 = 0x8000–0x8FFF (unsigned)
        BACKGROUND_TILEMAP_AREA             = 0b00001000,   // 0 = 0x9800–0x9BFF; 1 = 0x9C00–0x9FFF
        SPRITE_SIZE_MODIFIER                = 0b00000100,
        SPRITE_ENABLE                       = 0b00000010,
        BACKGROUND_AND_WINDOW_ENABLE        = 0b00000001
    };

    /* LCD Status Register Flags */
    enum class STAT_FLAG : uint8_t {
        LYC_INTERRUPT_ENABLE        = 0b01000000,
        MODE_2_INTERRUPT_ENABLE     = 0b00100000,
        MODE_1_INTERRUPT_ENABLE     = 0b00010000,
        MODE_0_INTERRUPT_ENABLE     = 0b00001000,
        LYC_INTERRUPT_BIT           = 0b00000100,
        PPU_MODE_BITS               = 0b00000011
    };

    /*  Sprite Attributes */
    enum class SPRITE_FLAG : uint8_t {
        OBJ_TO_BG_PRIORITY  = 0b10000000,
        Y_FLIP              = 0b01000000,
        X_FLIP              = 0b00100000,
        PALETTE_NUMBER      = 0b00010000,
        CGB_BANK            = 0b00001000,
        CGB_PALETTE         = 0b00000111,
    };

    constexpr uint8_t  LCD_WIDTH                    = 160;
    constexpr uint8_t  LCD_HEIGHT                   = 144;
    constexpr uint8_t  BITS_PER_PIXEL               = 2;
    constexpr uint8_t  PIXELS_PER_BYTE              = 8 / BITS_PER_PIXEL;
    constexpr uint16_t FRAMEBUFFER_SIZE             = (LCD_HEIGHT * LCD_WIDTH) / PIXELS_PER_BYTE;

    constexpr uint8_t FRAME_LINES                   = 154;
    constexpr uint8_t VBLANK_LINES                  = 10;
    
    constexpr uint32_t DOTS_PER_FRAME               = 70224;
    constexpr uint16_t DOTS_PER_LINE                = DOTS_PER_FRAME / FRAME_LINES;
    constexpr uint32_t DOTS_PER_LCD_SCAN            = LCD_HEIGHT * DOTS_PER_LINE;
    constexpr uint16_t DOTS_PER_OAM_SCAN_MODE       = 80;

    constexpr float    FRAMES_PER_SECOND            = float(1 << 22) / DOTS_PER_FRAME;

    constexpr uint16_t VRAM_SIZE                    = 0x2000;

    constexpr uint8_t SPRITE_COUNT                  = 40;
    constexpr uint8_t SPRITE_BYTES                  = 4;
    constexpr uint8_t OAM_SIZE                      = SPRITE_COUNT * SPRITE_BYTES;
    constexpr uint8_t MAX_SPRITES_PER_LINE          = 10;
    constexpr uint8_t SPRITE_Y_OFFSET               = 16;

    constexpr uint16_t TILE_COUNT                   = 384;
    constexpr uint16_t TILE_BYTES                   = 16;
    constexpr uint16_t TILE_DATA_SIZE               = TILE_COUNT * TILE_BYTES;
    constexpr uint8_t  TILE_ROW_BYTES               = 2;

    constexpr uint8_t  TILE_MAP_WIDTH               = 32;
    constexpr uint8_t  TILE_MAP_HEIGHT              = 32;
    constexpr uint16_t TILE_MAP_SIZE                = TILE_MAP_WIDTH * TILE_MAP_HEIGHT;

    constexpr uint8_t PIXEL_OVERSCAN                = 8;
    constexpr uint8_t WINDOW_X_OFFSET               = 7;
    constexpr uint8_t ADJUSTED_WINDOW_X_OFFSET      = PIXEL_OVERSCAN - WINDOW_X_OFFSET;

}