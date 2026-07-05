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

            void tick(uint8_t tCycles);
            template<uint16_t Register>
            uint8_t readIO();
            template<uint16_t Register>
            void writeIO(uint8_t value);
            void triggerInterrupt(INTERRUPT_FLAG flag);
            void enableInterrupt(INTERRUPT_FLAG flag);
    
        private:
            uint8_t interruptFlags = 0;     // IF register
            uint8_t interruptEnable = 0;    // IE register
    
            SystemTimer timer;
            Joypad joypad;
    };

}
