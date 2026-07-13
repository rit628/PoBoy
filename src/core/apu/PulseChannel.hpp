#pragma once
#include "Channel.hpp"
#include <array>
#include <cstdint>

namespace Audio {

    class PulseChannel : public Channel<VOLUME_TYPE::ENVELOPE, TICK_RATE::M_CYCLE> {
        public:
            template<uint8_t Register>
            uint8_t readIO();
            template<uint8_t Register>
            void writeIO(uint8_t value);

            uint8_t tick();
            bool dacEnabled();
    
        private:
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

            uint8_t dutyCyclePositionBit = 1 << 7;

            /* NRx1 Register Components */
            DUTY_CYCLE dutyCycle = DUTY_CYCLE::P12_5;   // NRx1 bits 7-6
    };

    class SweepChannel : public PulseChannel {
        public:
            template<uint8_t Register>
            uint8_t readIO();
            template<uint8_t Register>
            void writeIO(uint8_t value);

            void tickSweep();

        private:
            uint16_t computeNewPeriod();

            enum class DIRECTION : bool { INCREASING = 0, DECREASING = 1 };
            
            bool sweepEnabled = false;
            uint8_t sweepTimer = 0;
            uint16_t shadowPeriod = 0;

            /* NR10 Register Components */
            uint8_t sweepPeriod = 0;                        // NR10 bits 6-4
            DIRECTION direction = DIRECTION::INCREASING;    // NR10 bit 3
            uint8_t step = 0;                               // NR10 bits 2-0
    };

}
