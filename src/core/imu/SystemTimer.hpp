#pragma once
#include <array>
#include <cstdint>

namespace Interrupts {
    
    class IMU;
    
    class SystemTimer {
        public:
            SystemTimer(IMU& imu);
            void tick(uint8_t tCycles);
            void tick();
            uint8_t readDIV();
            void writeDIV(uint8_t value);
            uint8_t readTIMA();
            void writeTIMA(uint8_t value);
            uint8_t readTMA();
            void writeTMA(uint8_t value);
            uint8_t readTAC();
            void writeTAC(uint8_t value);
    
        private:
            static constexpr std::array<uint16_t, 4> timerClocks = {1024, 16, 64, 256};
            static constexpr uint16_t dividerClock = timerClocks.at(3);
    
            IMU& imu;
    
            uint16_t systemCounter = 0;
            uint8_t divider = 0;        // DIV register
            uint8_t timerCounter = 0;   // TIMA register
            uint8_t timerModulo = 0;    // TMA register
            /* TAC register components */
            bool timerEnabled = true;   // TAC bit 2
            uint8_t selectedClock = 0;  // TAC bits 1-0
    };

}