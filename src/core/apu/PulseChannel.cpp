#include "PulseChannel.hpp"
#include <cstdint>
#include <utility>

using namespace Audio;

template<uint8_t Register>
uint8_t PulseChannel::readIO() {
    if constexpr (Register == NRx1) return std::to_underlying(dutyCycle) << 6;
    if constexpr (Register == NRx2) return envelopeGenerator.readRegister();
    if constexpr (Register == NRx3) return 0;
    if constexpr (Register == NRx4) return lengthController.getState() << 6;
}

template<uint8_t Register>
void PulseChannel::writeIO(uint8_t value) {
    if constexpr (Register == NRx1) {
        dutyCycle = static_cast<DUTY_CYCLE>(value >> 6);
        lengthController.setPeriod(value & 0x3F);
    }
    if constexpr (Register == NRx2) {
        envelopeGenerator.writeRegister(value);
    }
    if constexpr (Register == NRx3) {
        period = (period & 0xFF00) | value;
    }
    if constexpr (Register == NRx4) {
        triggered = value >> 7;
        lengthController.setState(value & 0x40);
        period = (period & 0x00FF) | ((value & 0x07) << 8);
        if (triggered) {
            enabled = true;
            resetPeriodTimer();
            lengthController.trigger();
            envelopeGenerator.trigger();
        }
    }
}

uint8_t PulseChannel::tick() {
    if (!enabled) return 0;
    if (--periodTimer == 0) {
        resetPeriodTimer();
        bool carry = dutyCyclePositionBit & 0b1;
        dutyCyclePositionBit = (dutyCyclePositionBit >> 1) | (carry << 7);
    }
    bool hi = DUTY_CYCLE_PATTERNS.at(std::to_underlying(dutyCycle)) & dutyCyclePositionBit;
    return hi * envelopeGenerator.getVolume();
}

bool PulseChannel::dacEnabled() {
    return (envelopeGenerator.readRegister() & 0xF8) != 0;
}

template<uint8_t Register>
uint8_t SweepChannel::readIO() {
    if constexpr (Register == NRx0) {
        return sweepPeriod << 4 | std::to_underlying(direction) | step;
    }
    else return PulseChannel::readIO<Register>();
}

template<uint8_t Register>
void SweepChannel::writeIO(uint8_t value) {
    if constexpr (Register == NRx0) {
        sweepPeriod = (value >> 4) & 0x07;
        direction = static_cast<DIRECTION>(value & 0x08);
        step = value & 0x07;
    }
    else PulseChannel::writeIO<Register>(value);
    if constexpr (Register == NRx4) { // sweep trigger routine
        if (triggered) {
            shadowPeriod = period;
            sweepTimer = sweepPeriod;
            sweepEnabled = sweepPeriod > 0 || step > 0;
            if (step > 0) computeNewPeriod();
        }
    }
}

void SweepChannel::tickSweep() {
    if (!sweepEnabled || sweepPeriod == 0 || period == 0) return;
    if (--sweepTimer == 0) {
        sweepTimer = sweepPeriod;
        uint16_t newPeriod = computeNewPeriod();
        if (newPeriod < PERIOD_MAX && step > 0) {
            shadowPeriod = newPeriod;
            period = shadowPeriod;
            computeNewPeriod();
        }
    }
}

uint16_t SweepChannel::computeNewPeriod() {
    uint16_t shiftedPeriod = shadowPeriod >> step;
    int32_t offset = (direction == DIRECTION::INCREASING) ? shiftedPeriod : -shiftedPeriod;
    uint16_t newPeriod = shadowPeriod + offset;
    if (newPeriod >= PERIOD_MAX) {
        enabled = false;
        sweepEnabled = false;
    }
    return newPeriod;
}

template uint8_t PulseChannel::readIO<PulseChannel::NRx1>();
template uint8_t PulseChannel::readIO<PulseChannel::NRx2>();
template uint8_t PulseChannel::readIO<PulseChannel::NRx3>();
template uint8_t PulseChannel::readIO<PulseChannel::NRx4>();

template void PulseChannel::writeIO<PulseChannel::NRx1>(uint8_t);
template void PulseChannel::writeIO<PulseChannel::NRx2>(uint8_t);
template void PulseChannel::writeIO<PulseChannel::NRx3>(uint8_t);
template void PulseChannel::writeIO<PulseChannel::NRx4>(uint8_t);

template uint8_t SweepChannel::readIO<SweepChannel::NRx0>();
template uint8_t SweepChannel::readIO<SweepChannel::NRx1>();
template uint8_t SweepChannel::readIO<SweepChannel::NRx2>();
template uint8_t SweepChannel::readIO<SweepChannel::NRx3>();
template uint8_t SweepChannel::readIO<SweepChannel::NRx4>();

template void SweepChannel::writeIO<SweepChannel::NRx0>(uint8_t);
template void SweepChannel::writeIO<SweepChannel::NRx1>(uint8_t);
template void SweepChannel::writeIO<SweepChannel::NRx2>(uint8_t);
template void SweepChannel::writeIO<SweepChannel::NRx3>(uint8_t);
template void SweepChannel::writeIO<SweepChannel::NRx4>(uint8_t);