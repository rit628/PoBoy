#pragma once
#include <array>
#include <cstdint>

namespace Interrupts {
    
    class IMU;
    
    class SystemTimer {
        public:
            SystemTimer(IMU& imu);
            
            template<uint16_t Register>
            uint8_t readIO();
            template<uint16_t Register>
            void writeIO(uint8_t value);
            
            void tick();

        private:
            static constexpr std::array<uint16_t, 4> timerClocks = {1024, 16, 64, 256};
    
            IMU& imu;
    
            bool prevTimaBit = 0;
            uint8_t timaReloadTCycle = 4;
            uint16_t systemCounter = 0; // DIV register (bits 15-8) and clock counter (bits 7-0)
            uint8_t timerCounter = 0;   // TIMA register
            uint8_t timerModulo = 0;    // TMA register
            /* TAC register components */
            bool timerEnabled = true;   // TAC bit 2
            uint8_t selectedClock = 0;  // TAC bits 1-0
    };

}