#pragma once
#include "SystemTimer.hpp"
#include <cstdint>

namespace Interrupts {

    enum class INTERRUPT_FLAG : uint8_t {
        JOYPAD      = 0b00010000,
        SERIAL      = 0b00001000,
        TIMER       = 0b00000100,
        LCD_STAT    = 0b00000010,
        VBLANK      = 0b00000001
    };
    
    class IMU {
        public:
            uint8_t readIF();
            void writeIF(uint8_t value);
            void writeIF(INTERRUPT_FLAG flag);
            uint8_t readIE();
            void writeIE(uint8_t value);
            void writeIE(INTERRUPT_FLAG flag);
    
            void tick(uint8_t tCycles);
            uint8_t readDIV();
            void writeDIV(uint8_t value);
            uint8_t readTIMA();
            void writeTIMA(uint8_t value);
            uint8_t readTMA();
            void writeTMA(uint8_t value);
            uint8_t readTAC();
            void writeTAC(uint8_t value);
    
        private:
            uint8_t interruptFlags = 0b11100000; // IF register
            uint8_t interruptEnable = 0;        // IE register
    
            SystemTimer timer{*this};
    };

}
