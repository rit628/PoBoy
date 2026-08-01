#pragma once
#include <cstdint>

namespace Audio {
    
    class EnvelopeGenerator {
        public:
            EnvelopeGenerator();
            void initialize();
            void tick();
            void trigger();
            uint8_t readRegister() const;
            void writeRegister(uint8_t value, bool playing);
            uint8_t getVolume() const;
    
        private:
            enum class DIRECTION : bool { DECREASING = 0, INCREASING = 1 };

            /* latched values */
            bool enabled;
            uint8_t timer;
            uint8_t currentVolume;
            uint8_t currentPeriod;
            DIRECTION currentDirection;

            /* NRx2 Register Components */
            uint8_t initialVolume;  // NRx2 bits 7-4
            DIRECTION direction;    // NRx2 bit 3
            uint8_t period;         // NRx2 bits 2-0
    };

}
