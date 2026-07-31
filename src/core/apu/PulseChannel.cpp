#include "PulseChannel.hpp"
#include <cstdint>
#include <utility>

using namespace Audio;

PulseChannel::PulseChannel() {
    init();
}

void PulseChannel::init() {
    dutyCyclePositionBit = 1 << 7;

    dutyCycle = DUTY_CYCLE::P12_5;
}

template<uint8_t Register>
uint8_t PulseChannel::readIO() {
    if constexpr (Register == NRx1) return 0x3F | std::to_underlying(dutyCycle) << 6;
    if constexpr (Register == NRx2) return envelopeGenerator.readRegister();
    if constexpr (Register == NRx3) return 0xFF;
    if constexpr (Register == NRx4) return 0xBF | lengthController.getState() << 6;
}

template<uint8_t Register>
void PulseChannel::writeIO(uint8_t value) {
    if constexpr (Register == NRx1) {
        dutyCycle = static_cast<DUTY_CYCLE>(value >> 6);
        lengthController.setPeriod(value & 0x3F);
    }
    else if constexpr (Register == NRx2) {
        writeEnvelope(value);
    }
    else if constexpr (Register == NRx3) {
        setPeriodLo(value);
    }
    else if constexpr (Register == NRx4) {
        channelControl(value);
    }
}

void PulseChannel::advanceOutput() {
    bool carry = dutyCyclePositionBit & 0b1;
    dutyCyclePositionBit = (dutyCyclePositionBit >> 1) | (carry << 7);
}

uint8_t PulseChannel::sample() {
    bool hi = DUTY_CYCLE_PATTERNS.at(std::to_underlying(dutyCycle)) & dutyCyclePositionBit;
    return hi * envelopeGenerator.getVolume();
}

void PulseChannel::trigger() {
    /* Channel base already does everything necessary */
}

SweepChannel::SweepChannel() {
    init();
}

void SweepChannel::init() {
    PulseChannel::init();

    sweepEnabled = false;
    sweepTimer = 0;
    shadowPeriod = 0;

    sweepPeriod = 0;
    direction = DIRECTION::INCREASING;
    step = 0;
}

template<uint8_t Register>
uint8_t SweepChannel::readIO() {
    if constexpr (Register == NRx0) {
        return 0x80 | sweepPeriod << 4 | std::to_underlying(direction) << 3 | step;
    }
    else {
        return PulseChannel::readIO<Register>();
    }
}

template<uint8_t Register>
void SweepChannel::writeIO(uint8_t value) {
    if constexpr (Register == NRx0) {
        sweepPeriod = (value >> 4) & 0x07;
        direction = static_cast<DIRECTION>(value & 0x08);
        step = value & 0x07;
    }
    else if constexpr (Register == NRx4) {
        channelControl(value);  // must be called here to ensure correct trigger() call is dispatched
    }
    else {
        PulseChannel::writeIO<Register>(value);
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

void SweepChannel::trigger() {
    PulseChannel::trigger();
    shadowPeriod = period;
    sweepTimer = sweepPeriod;
    sweepEnabled = sweepPeriod > 0 || step > 0;
    if (step > 0) computeNewPeriod();
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