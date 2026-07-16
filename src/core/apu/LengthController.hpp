#pragma once
#include <cstdint>

namespace Audio {

    template<uint16_t TIMER_MAX>
    class LengthController {
        public:
            LengthController();
            void initialize();
            void tick(bool& channelEnabled);
            void trigger();
            uint8_t getPeriod() const;
            void setPeriod(uint8_t value);
            bool getState() const;
            void setState(bool enable);
    
        private:
            uint16_t timer;
            bool enabled;   // portion of several channel registers
            uint8_t period; // portion of several channel registers
    };

}
