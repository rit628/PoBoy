#pragma once
#include <cstdint>

namespace Audio {
    
    class EnvelopeGenerator {
        public:
            void tick();
            void trigger();
            uint8_t readRegister() const;
            void writeRegister(uint8_t value);
            uint8_t getVolume() const;
    
        private:
            enum class DIRECTION : bool { DECREASING = 0, INCREASING = 1 };

            /* latched values */
            uint8_t timer = 0;
            uint8_t currentVolume = 0;
            uint8_t currentPeriod = 0;
            DIRECTION currentDirection = DIRECTION::DECREASING;

            /* NRx2 Register Components */
            uint8_t volume = 0;                             // NRx2 bits 7-4
            DIRECTION direction = DIRECTION::DECREASING;    // NRx2 bit 3
            uint8_t period = 0;                             // NRx2 bits 2-0
    };

}
