#include "PulseChannel.hpp"
#include "AudioConstants.hpp"
#include <cstdint>
#include <utility>

using namespace Audio;

template<uint16_t Register>
uint8_t PulseChannel::readIO() {
    if constexpr (Register == NRx1) return std::to_underlying(dutyCycle) << 6;
    if constexpr (Register == NRx2) return initialVolume << 4 | increaseVolume << 3 | envelopePeriod;
    if constexpr (Register == NRx3) return 0;
    if constexpr (Register == NRx4) return lengthEnable << 6;
}

template<uint16_t Register>
void PulseChannel::writeIO(uint8_t value) {
    if constexpr (Register == NRx1) {
        dutyCycle = static_cast<DUTY_CYCLE>(value >> 6);
        initialLengthTimer = value & 0x3F;
        lengthTimer = LENGTH_MAX - lengthTimer;
    }
    if constexpr (Register == NRx2) {
        initialVolume = value >> 4;
        increaseVolume = value & 0x08;
        envelopePeriod = value & 0x07;
    }
    if constexpr (Register == NRx3) {
        period = (period & 0xFF00) | value;
    }
    if constexpr (Register == NRx4) {
        triggered = value >> 7;
        lengthEnable = value & 0x40;
        period = (period & 0x00FF) | ((value & 0x07) << 8);
        if (triggered) {
            if (dacEnabled()) enabled = true;
            lengthTimer = (!lengthTimer) ? LENGTH_MAX : lengthTimer;
            resetPeriod();
            envelopeTimer = envelopePeriod;
            currentVolume = initialVolume;
        }
    }
}

float PulseChannel::tick() {
    if (--periodTimer == 0) {
        resetPeriod();
        bool carry = dutyCyclePositionBit & 0b1;
        dutyCyclePositionBit = (dutyCyclePositionBit >> 1) | (carry << 7);
    }
    if (!enabled || !dacEnabled()) return 0.0;
    bool hi = DUTY_CYCLE_PATTERNS.at(std::to_underlying(dutyCycle)) & dutyCyclePositionBit;
    uint8_t digitalSample = hi * currentVolume;
    float analogSample = (float(digitalSample) / DIGITAL_SAMPLE_MAX) * 2 - 1;
    return analogSample;
}

void PulseChannel::lengthTick() {
    if (!enabled || !lengthEnable) return;
    if (--lengthTimer == 0) {
        enabled = false;
    }
}

void PulseChannel::volumeEnvelopeTick() {
    if (!enabled || envelopePeriod == 0) return;
    if (--envelopeTimer == 0) {
        envelopeTimer = envelopePeriod;
        currentVolume = (increaseVolume) ? 
                        std::min(currentVolume + 1, int(DIGITAL_SAMPLE_MAX))
                        :
                        std::max(currentVolume - 1, 0);
    }
}

bool PulseChannel::dacEnabled() {
    return ((initialVolume << 1) | increaseVolume) > 0;
}

void PulseChannel::resetPeriod() {
    periodTimer = (PERIOD_MAX - period) * T_CYCLES_PER_PERIOD_TICK;
}

// template uint8_t PulseChannel::readIO<PulseChannel::NRx0>();
template uint8_t PulseChannel::readIO<PulseChannel::NRx1>();
template uint8_t PulseChannel::readIO<PulseChannel::NRx2>();
template uint8_t PulseChannel::readIO<PulseChannel::NRx3>();
template uint8_t PulseChannel::readIO<PulseChannel::NRx4>();

// template void PulseChannel::writeIO<PulseChannel::NRx0>(uint8_t);
template void PulseChannel::writeIO<PulseChannel::NRx1>(uint8_t);
template void PulseChannel::writeIO<PulseChannel::NRx2>(uint8_t);
template void PulseChannel::writeIO<PulseChannel::NRx3>(uint8_t);
template void PulseChannel::writeIO<PulseChannel::NRx4>(uint8_t);