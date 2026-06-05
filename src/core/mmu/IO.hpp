#pragma once
#include <cstdint>
#include <type_traits>
#include <utility>

namespace IO {

    constexpr uint16_t P1       = 0xFF00;   // Joypad
    constexpr uint16_t SB       = 0xFF01;   // Serial transfer data
    constexpr uint16_t SC       = 0xFF02;   // Serial transfer control
    constexpr uint16_t DIV      = 0xFF04;   // Divider register
    constexpr uint16_t TIMA     = 0xFF05;   // Timer counter
    constexpr uint16_t TMA      = 0xFF06;   // Timer modulo
    constexpr uint16_t TAC      = 0xFF07;   // Timer control
    constexpr uint16_t IF       = 0xFF0F;   // Interrupt flag
    constexpr uint16_t NR10     = 0xFF10;   // Sound channel 1 sweep
    constexpr uint16_t NR11     = 0xFF11;   // Sound channel 1 length timer & duty cycle
    constexpr uint16_t NR12     = 0xFF12;   // Sound channel 1 volume & envelope
    constexpr uint16_t NR13     = 0xFF13;   // Sound channel 1 period low
    constexpr uint16_t NR14     = 0xFF14;   // Sound channel 1 period high & control
    constexpr uint16_t NR21     = 0xFF16;   // Sound channel 2 length timer & duty cycle
    constexpr uint16_t NR22     = 0xFF17;   // Sound channel 2 volume & envelope
    constexpr uint16_t NR23     = 0xFF18;   // Sound channel 2 period low
    constexpr uint16_t NR24     = 0xFF19;   // Sound channel 2 period high & control
    constexpr uint16_t NR30     = 0xFF1A;   // Sound channel 3 DAC enable
    constexpr uint16_t NR31     = 0xFF1B;   // Sound channel 3 length timer
    constexpr uint16_t NR32     = 0xFF1C;   // Sound channel 3 output level
    constexpr uint16_t NR33     = 0xFF1D;   // Sound channel 3 period low
    constexpr uint16_t NR34     = 0xFF1E;   // Sound channel 3 period high & control
    constexpr uint16_t NR41     = 0xFF20;   // Sound channel 4 length timer
    constexpr uint16_t NR42     = 0xFF21;   // Sound channel 4 volume & envelope
    constexpr uint16_t NR43     = 0xFF22;   // Sound channel 4 frequency & randomness
    constexpr uint16_t NR44     = 0xFF23;   // Sound channel 4 control
    constexpr uint16_t NR50     = 0xFF24;   // Master volume & VIN panning
    constexpr uint16_t NR51     = 0xFF25;   // Sound panning
    constexpr uint16_t NR52     = 0xFF26;   // Sound on/off
    constexpr uint16_t FF3F     = 0xFF30;   // Wave RAM Storage for one of the sound channels' waveform
    constexpr uint16_t LCDC     = 0xFF40;   // LCD control
    constexpr uint16_t STAT     = 0xFF41;   // LCD status
    constexpr uint16_t SCY      = 0xFF42;   // Viewport Y position
    constexpr uint16_t SCX      = 0xFF43;   // Viewport X position
    constexpr uint16_t LY       = 0xFF44;   // LCD Y coordinate
    constexpr uint16_t LYC      = 0xFF45;   // LY compare
    constexpr uint16_t DMA      = 0xFF46;   // OAM DMA source address & start
    constexpr uint16_t BGP      = 0xFF47;   // BG palette data
    constexpr uint16_t OBP0     = 0xFF48;   // OBJ palette 0 data
    constexpr uint16_t OBP1     = 0xFF49;   // OBJ palette 1 data
    constexpr uint16_t WY       = 0xFF4A;   // Window Y position
    constexpr uint16_t WX       = 0xFF4B;   // Window X position plus 7
    constexpr uint16_t KEY0     = 0xFF4C;   // CPU mode select
    constexpr uint16_t KEY1     = 0xFF4D;   // Prepare speed switch
    constexpr uint16_t VBK      = 0xFF4F;   // VRAM bank
    constexpr uint16_t BANK     = 0xFF50;   // Boot ROM mapping control
    constexpr uint16_t HDMA1    = 0xFF51;   // VRAM DMA source high
    constexpr uint16_t HDMA2    = 0xFF52;   // VRAM DMA source low
    constexpr uint16_t HDMA3    = 0xFF53;   // VRAM DMA destination high
    constexpr uint16_t HDMA4    = 0xFF54;   // VRAM DMA destination low
    constexpr uint16_t HDMA5    = 0xFF55;   // VRAM DMA length/mode/start
    constexpr uint16_t RP       = 0xFF56;   // Infrared communications port
    constexpr uint16_t BCPS     = 0xFF68;   // Background color palette specification / Background palette index
    constexpr uint16_t BCPD     = 0xFF69;   // Background color palette data / Background palette data
    constexpr uint16_t OCPS     = 0xFF6A;   // OBJ color palette specification / OBJ palette index
    constexpr uint16_t OCPD     = 0xFF6B;   // OBJ color palette data / OBJ palette data
    constexpr uint16_t OPRI     = 0xFF6C;   // Object priority mode
    constexpr uint16_t SVBK     = 0xFF70;   // WRAM bank
    constexpr uint16_t PCM12    = 0xFF76;   // Audio digital outputs 1 & 2
    constexpr uint16_t PCM34    = 0xFF77;   // Audio digital outputs 3 & 4
    constexpr uint16_t IE       = 0xFFFF;   // Interrupt enable

}

enum class INTERRUPT_BIT : uint8_t {
    JOYPAD      = 0b00010000,
    SERIAL      = 0b00001000,
    TIMER       = 0b00000100,
    LCD_STAT    = 0b00000010,
    VBLANK      = 0b00000001
};

namespace {
    template<typename T>
    concept Flag = std::is_enum_v<T> && std::is_same_v<std::underlying_type_t<T>, uint8_t>;

    template<typename T>
    concept Testable = requires (T a, uint8_t b) {
        { a |= b } -> std::same_as<T&>;
        { a &= b } -> std::same_as<T&>;
        { a & b } -> std::convertible_to<uint8_t>;
    };
}

inline void flagSet(Testable auto& target, Flag auto flag) { target |= std::to_underlying(flag); }
inline void flagClear(Testable auto& target, Flag auto flag) { target &= ~std::to_underlying(flag); }
inline bool flagTest(Testable auto target, Flag auto flag) { return target & std::to_underlying(flag); }