#include "EnvelopeGenerator.hpp"
#include "AudioConstants.hpp"
#include <algorithm>
#include <cstdint>
#include <utility>

using namespace Audio;

void EnvelopeGenerator::tick() {
    if (currentPeriod == 0) return;
    if (--timer == 0) {
        timer = currentPeriod;
        currentVolume = (currentDirection == DIRECTION::INCREASING) ? 
                        std::min(currentVolume + 1, int(DIGITAL_SAMPLE_MAX))
                        :
                        std::max(currentVolume - 1, 0);
    }
}

void EnvelopeGenerator::trigger() {
    currentPeriod = period;
    currentVolume = volume;
    currentDirection = direction;
    timer = currentPeriod;
}

uint8_t EnvelopeGenerator::readRegister() const {
    return volume << 4 | std::to_underlying(direction) << 3 | period;
}

void EnvelopeGenerator::writeRegister(uint8_t value) {
    volume = value >> 4;
    direction = static_cast<DIRECTION>(value & 0x08);
    period = value & 0x07;
}

uint8_t EnvelopeGenerator::getVolume() const {
    return currentVolume;
}