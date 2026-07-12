#pragma once
#include <array>
#include <cstdint>

namespace Audio {

    class PulseChannel {
        public:
            template<uint16_t Register>
            uint8_t readIO();
            template<uint16_t Register>
            void writeIO(uint8_t value);

            float tick();
            void volumeEnvelopeTick();
            void lengthTick();
            bool dacEnabled();
    
        private:
            void resetPeriod();

            enum class DUTY_CYCLE : uint8_t {
                P12_5 = 0b00,
                P25 = 0b01,
                P50 = 0b10,
                P75 = 0b11
            };
            
            static constexpr std::array<uint8_t, 4> DUTY_CYCLE_PATTERNS = {
                0b00000001,  // 12.5%
                0b10000001,  // 25%
                0b10000111,  // 50%
                0b01111110   // 75%
            };

            static constexpr uint8_t NRx0 = 0;
            static constexpr uint8_t NRx1 = 1;
            static constexpr uint8_t NRx2 = 2;
            static constexpr uint8_t NRx3 = 3;
            static constexpr uint8_t NRx4 = 4;

            static constexpr uint8_t  LENGTH_MAX                = 0X40;
            static constexpr uint16_t PERIOD_MAX                = 0x0800;
            static constexpr uint8_t  T_CYCLES_PER_PERIOD_TICK  = 4;
   
            uint8_t currentVolume = 0;
            uint8_t dutyCyclePositionBit = 1 << 7;
            uint16_t periodTimer = 0;
            uint8_t envelopeTimer = 0;
            uint8_t lengthTimer = 0;
            bool enabled = false;

            /* NRx1 Register Components */
            DUTY_CYCLE dutyCycle = DUTY_CYCLE::P12_5;   // NRx1 bits 7-6
            uint8_t initialLengthTimer = 0;             // NRx1 bits 5-0

            /* NRx2 Register Components */
            uint8_t initialVolume = 0;      // NRx2 bits 7-4
            bool increaseVolume = false;    // NRx2 bit 3
            uint8_t envelopePeriod = 0;     // NRx2 bits 2-0

            /* NRx3 + NRx4 Register Components */
            uint16_t period = 0;            // NRx3 register (bits 7-0) NRx4 register bits 2-0 (bits 10-8)
            bool triggered = false;         // NRx4 bit 7
            bool lengthEnable = false;      // NRx4 bit 6
    };

}
