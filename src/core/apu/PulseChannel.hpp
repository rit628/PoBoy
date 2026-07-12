#pragma once
#include "EnvelopeGenerator.hpp"
#include "LengthController.hpp"
#include <array>
#include <cstdint>

namespace Audio {

    class PulseChannel {
        public:
            template<uint16_t Register>
            uint8_t readIO();
            template<uint16_t Register>
            void writeIO(uint8_t value);

            uint8_t tick();
            void tickLength();
            void tickEnvelope();
            bool dacEnabled();
    
        private:
            void resetCycleTimer();

            enum class DUTY_CYCLE : uint8_t {
                P12_5   = 0b00,
                P25     = 0b01,
                P50     = 0b10,
                P75     = 0b11
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

            static constexpr uint16_t PERIOD_MAX                = 0x0800;
            static constexpr uint8_t  T_CYCLES_PER_PERIOD_TICK  = 4;
   
            bool enabled = false;
            uint8_t dutyCyclePositionBit = 1 << 7;
            uint16_t cycleTimer = 0;

            /* NRx1 Register Components */
            DUTY_CYCLE dutyCycle = DUTY_CYCLE::P12_5;   // NRx1 bits 7-6

            /* NRx3 + NRx4 Register Components */
            uint16_t period = 0;    // NRx3 register (bits 7-0) NRx4 register bits 2-0 (bits 10-8)
            bool triggered = false; // NRx4 bit 7

            LengthController lengthController;      // Manages NRx1 bits 5-0 and NRx4 bit 6
            EnvelopeGenerator envelopeGenerator;  // Manages NRx2 register
    };

}
