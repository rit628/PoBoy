#pragma once
#include <cstdint>
#include <functional>

namespace Interrupts {

    class IMU;

    class Joypad {
        public:
            Joypad(IMU& imu, std::function<uint8_t()> readInput);
            void tick(uint8_t tCycles);
            void tick();
            uint8_t readP1();
            void writeP1(uint8_t value);

        private:
            IMU& imu;

            std::function<uint8_t()> readInput;
            uint8_t selectedJoypadInput = 0;    // P1 register bits 5-4
            uint8_t currentJoypadInput = 0x0F;  // P1 register bits 3-0
    };

}