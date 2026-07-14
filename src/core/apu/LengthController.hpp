#pragma once
#include <cstdint>

namespace Audio {

    template<uint16_t TIMER_MAX>
    class LengthController {
        public:
            void tick(bool& channelEnabled);
            void trigger();
            uint8_t getPeriod() const;
            void setPeriod(uint8_t value);
            bool getState() const;
            void setState(bool enable);
    
        private:
            uint16_t timer = 0;
            bool enabled = 0;   // portion of several channel registers
            uint8_t period = 0; // portion of several channel registers
    };

}
