#include "EnvelopeGenerator.hpp"
#include "AudioConstants.hpp"
#include <cstdint>
#include <utility>

using namespace Audio;

EnvelopeGenerator::EnvelopeGenerator() {
    initialize();
}

void EnvelopeGenerator::initialize() {
    enabled = false;
    timer = 0;
    currentVolume = 0;
    currentPeriod = 0;
    currentDirection = DIRECTION::DECREASING;

    initialVolume = 0;
    direction = DIRECTION::DECREASING;
    period = 0;
}

void EnvelopeGenerator::tick() {
    if (!enabled || currentPeriod == 0) return;
    if (--timer == 0) {
        timer = currentPeriod;
        if (currentDirection == DIRECTION::INCREASING) {
            ++currentVolume;
            enabled = currentVolume < DIGITAL_SAMPLE_MAX;
        }
        else {
            --currentVolume;
            enabled = currentVolume > 0;
        }
    }
}

void EnvelopeGenerator::trigger() {
    currentPeriod = period;
    currentVolume = initialVolume;
    currentDirection = direction;
    timer = currentPeriod;
    enabled = true;
}

uint8_t EnvelopeGenerator::readRegister() const {
    return initialVolume << 4 | std::to_underlying(direction) << 3 | period;
}

void EnvelopeGenerator::writeRegister(uint8_t value, bool playing) {
    auto prevPeriod = period;
    auto prevDirection = direction;

    initialVolume = value >> 4;
    direction = static_cast<DIRECTION>(value & 0x08);
    period = value & 0x07;

    if (playing) {  // zombie mode bug (permits manual altering of volume)
        if (prevPeriod == 0 && enabled) {
            ++currentVolume;
        }
        else if (prevDirection == DIRECTION::DECREASING) {
            currentVolume += 2;
        }
        if (prevDirection != direction) {
            currentVolume = 0x10 - currentVolume;
        }
        currentVolume &= DIGITAL_SAMPLE_MAX;
    }
}

uint8_t EnvelopeGenerator::getVolume() const {
    return currentVolume;
}