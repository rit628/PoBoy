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
            void initialize();

            template<uint16_t Register>
            uint8_t readIO();
            template<uint16_t Register>
            void writeIO(uint8_t value);
            
            void tick();

            void triggerInterrupt(INTERRUPT_FLAG flag);
            void enableInterrupt(INTERRUPT_FLAG flag);
    
        private:
            uint8_t interruptFlags;     // IF register
            uint8_t interruptEnable;    // IE register
    
            SystemTimer timer;
            Joypad joypad;
    };

}
