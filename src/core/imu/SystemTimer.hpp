#pragma once
#include <array>
#include <cstdint>

namespace Interrupts {
    
    class IMU;
    
    class SystemTimer {
        public:
            SystemTimer(IMU& imu);
            void initialize();
            
            template<uint16_t Register>
            uint8_t readIO();
            template<uint16_t Register>
            void writeIO(uint8_t value);
            
            void initHLE();
            void tick();    // per m-cycle

        private:
            static constexpr std::array<uint16_t, 4> timerClocks = {256, 4, 16, 64};    // m-cycle based
    
            IMU& imu;
    
            bool prevTimaBit;
            bool reloadTima;
            bool timaReloaded;
            uint16_t overflowBit;
            
            uint16_t systemCounter; // DIV register (bits 13-6) and clock counter (bits 5-0)
            uint8_t timerCounter;   // TIMA register
            uint8_t timerModulo;    // TMA register

            /* TAC register components */
            bool timerEnabled;      // TAC bit 2
            uint8_t selectedClock;  // TAC bits 1-0
    };

}