#pragma once
#include "InterruptConstants.hpp"
#include "Joypad.hpp"
#include "SystemTimer.hpp"
#include <cstdint>
#include <functional>

namespace Interrupts {
    
    class IMU {
        public:
            IMU(std::function<uint8_t()> readInput);

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

            uint8_t readP1();
            void writeP1(uint8_t value);
    
        private:
            uint8_t interruptFlags = 0b11100000; // IF register
            uint8_t interruptEnable = 0;        // IE register
    
            SystemTimer timer;
            Joypad joypad;
    };

}
