#pragma once
#include "FlagOps.hpp"
#include "GraphicsConstants.hpp"
#include "PPU.hpp"
#include "MemoryConstants.hpp"
#include "SystemConstants.hpp"
#include <utility>

namespace Graphics {

    template<MODEL Model>
    template<uint16_t Register>
    uint8_t PPU<Model>::readIO() {
        using namespace Memory;
        using enum MODEL;

        if constexpr (Register == STAT) {
            if (!enabled) return 0x80 | interruptMask;   // bits 0-2 return 0 when lcd is off
            return 0x80
                | interruptMask
                | (lineCompare == currentLine) << 2
                | std::to_underlying(mode);
        }

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

        /* CGB registers */
        if constexpr (Register == BGPD) {
            if constexpr (Model == CGB) return bgPaletteRam.at(bgpAddress);
            else return 0xFF;
        }

        if constexpr (Register == OBPD) {
            if constexpr (Model == CGB) return spritePaletteRam.at(obpAddress);
            else return 0xFF;
        }

        if constexpr (Register == VBK)  return 0xFE | vramBank;
        if constexpr (Register == BGPI) return 0x40 | bgpPaletteAutoIncrement << 7 | bgpAddress;
        if constexpr (Register == OBPI) return 0x40 | obpPaletteAutoIncrement << 7 | obpAddress;
    }

    template<MODEL Model>
    template<uint16_t Register>
    void PPU<Model>::writeIO(uint8_t value) {
        using namespace Memory;
        using enum MODEL;

        if constexpr (Register == LCDC) {
            bool prevEnabled = enabled;
            lcdControl = value;
            mixer.updateFlags(lcdControl);
            enabled = testFlags(lcdControl, LCDC_FLAG::LCD_AND_PPU_ENABLE);
            if (prevEnabled && !enabled) disableLCD();
            else if (!prevEnabled && enabled) enableLCD();
        }
        
        if constexpr (Register == STAT) {
            interruptMask = value & 0x78;   // bits 0-2 and 7 are read only
            attemptStatusInterrupt();
        }

        if constexpr (Register == LY)   return;
        if constexpr (Register == LYC)  lineCompare = value;
        if constexpr (Register == SCX)  scrollX = value;
        if constexpr (Register == SCY)  scrollY = value;
        if constexpr (Register == WX)   windowX = value;
        if constexpr (Register == WY)   windowY = value;
        if constexpr (Register == BGP)  backgroundPalette = value;
        if constexpr (Register == OBP0) spritePalette0 = value;
        if constexpr (Register == OBP1) spritePalette1 = value;

        /* CGB registers */
        if constexpr (Model == CGB) {
            if constexpr (Register == VBK) {
                vramBank = value & 0b1;
                currentBank = std::span(vram).subspan(vramBank * VRAM_BANK_SIZE).template first<VRAM_BANK_SIZE>();
            }

            if constexpr (Register == BGPI) {
                bgpPaletteAutoIncrement = value & 0x80;
                bgpAddress = value & 0x3F;
            }
    
            if constexpr (Register == BGPD) {
                bgPaletteRam.at(bgpAddress) = value;
                if (bgpPaletteAutoIncrement) {
                    bgpAddress = (bgpAddress + 1) & 0x3F;
                }
            }
    
            if constexpr (Register == OBPI) {
                obpPaletteAutoIncrement = value & 0x80;
                obpAddress = value & 0x3F;
            }
    
            if constexpr (Register == OBPD) {
                spritePaletteRam.at(obpAddress) = value;
                if (obpPaletteAutoIncrement) {
                    obpAddress = (obpAddress + 1) & 0x3F;
                }
            }
        }

    }

}