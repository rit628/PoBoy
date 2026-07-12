#pragma once
#include <cstdint>

namespace Audio {

    class LengthController {
        public:
            void tick(bool& channelEnabled);
            void trigger();
            uint8_t getPeriod() const;
            void setPeriod(uint8_t value);
            bool getState() const;
            void setState(bool enable);
    
        private:
            static constexpr uint8_t TIMER_MAX = 64;

            uint8_t timer = 0;
            bool enabled = 0;   // portion of several channel registers
            uint8_t period = 0; // portion of several channel registers
    };

}
