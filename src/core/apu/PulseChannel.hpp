#pragma once
#include "Channel.hpp"
#include <array>
#include <cstdint>

namespace Audio {

    class PulseChannel : public Channel<> {
        friend class Channel<>;
        public:
            PulseChannel();
            template<uint8_t Register>
            uint8_t readIO();
            template<uint8_t Register>
            void writeIO(uint8_t value);

        protected:
            void init();
            void clearRegisters();
            void advanceOutput();
            uint8_t sample();
            void trigger();

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

            uint8_t dutyCyclePositionBit;

            /* NRx1 Register Components */
            DUTY_CYCLE dutyCycle;   // NRx1 bits 7-6
    };

    class SweepChannel : public PulseChannel {
        friend class Channel<>;
        public:
            SweepChannel();
            template<uint8_t Register>
            uint8_t readIO();
            template<uint8_t Register>
            void writeIO(uint8_t value);

            void tickSweep();

        private:
            void init();
            void clearRegisters();
            uint8_t getSweepPeriod();
            uint16_t computeNewPeriod();
            void trigger();

            enum class DIRECTION : bool { INCREASING = 0, DECREASING = 1 };
            
            bool sweepEnabled;
            uint8_t sweepTimer;
            uint16_t shadowPeriod;

            /* NR10 Register Components */
            uint8_t sweepPeriod;    // NR10 bits 6-4
            DIRECTION direction;    // NR10 bit 3
            uint8_t step;           // NR10 bits 2-0
    };

}
