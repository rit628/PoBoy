#pragma once
#include "AudioConstants.hpp"
#include "Channel.hpp"
#include <cstdint>

namespace Audio {

    class NoiseChannel : public Channel<> {
        friend class Channel<>;
        public:
            NoiseChannel();
            template<uint8_t Register>
            uint8_t readIO();
            template<uint8_t Register>
            void writeIO(uint8_t value);
        
        private:
            void init();
            void advanceOutput();
            uint8_t sample();
            void trigger();
            void resetPeriodTimer();

            static constexpr uint32_t BASE_TICK_RATE    = 262144;
            static constexpr uint8_t  PERIOD_MIN        = (INTERNAL_SAMPLE_RATE / BASE_TICK_RATE) >> 1; // account for .5 clock shift
            
            uint16_t feedbackRegister;

            /* NR43 Register Components */
            uint8_t clockShift;     // NR43 bits 7-4
            bool shortWidthMode;    // NR43 bit 3
            uint8_t clockDivider;   // NR43 bits 2-0

    };

}