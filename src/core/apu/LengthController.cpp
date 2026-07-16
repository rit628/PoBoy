#include "LengthController.hpp"
#include <cstdint>

using namespace Audio;

template<uint16_t TIMER_MAX>
LengthController<TIMER_MAX>::LengthController() {
    initialize();
}

template<uint16_t TIMER_MAX>
void LengthController<TIMER_MAX>::initialize() {
    timer = 0;
    enabled = false;
    period = 0;
}

template<uint16_t TIMER_MAX>
void LengthController<TIMER_MAX>::tick(bool& channelEnabled) {
    if (!enabled || timer == 0) return;
    if (--timer == 0) channelEnabled = false;
}

template<uint16_t TIMER_MAX>
void LengthController<TIMER_MAX>::trigger() {
    if (timer == 0) timer = TIMER_MAX;
}

template<uint16_t TIMER_MAX>
uint8_t LengthController<TIMER_MAX>::getPeriod() const {
    return period;
}

template<uint16_t TIMER_MAX>
void LengthController<TIMER_MAX>::setPeriod(uint8_t value) {
    period = value;
    timer = TIMER_MAX - period;
}

template<uint16_t TIMER_MAX>
bool LengthController<TIMER_MAX>::getState() const {
    return enabled;
}

template<uint16_t TIMER_MAX>
void LengthController<TIMER_MAX>::setState(bool enable) {
    enabled = enable;
}

template class Audio::LengthController<64>;
template class Audio::LengthController<256>;