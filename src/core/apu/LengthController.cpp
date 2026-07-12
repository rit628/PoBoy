#include "LengthController.hpp"
#include <cstdint>

using namespace Audio;

void LengthController::tick(bool& channelEnabled) {
    if (!enabled || timer == 0) return;
    if (--timer == 0) channelEnabled = false;
}

void LengthController::trigger() {
    if (timer == 0) timer = TIMER_MAX;
}

uint8_t LengthController::getPeriod() const {
    return period;
}

void LengthController::setPeriod(uint8_t value) {
    period = value;
    timer = TIMER_MAX - period;
}

bool LengthController::getState() const {
    return enabled;
}

void LengthController::setState(bool enable) {
    enabled = enable;
}