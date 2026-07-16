#include "NoiseChannel.hpp"
#include <algorithm>

using namespace Audio;

NoiseChannel::NoiseChannel() {
    init();
}

void NoiseChannel::init() {
    feedbackRegister = 0;

    clockShift = 0;
    shortWidthMode = false;
    clockDivider = 0;
}

template<uint8_t Register>
uint8_t NoiseChannel::readIO() {
    if constexpr (Register == NRx1) return 0xFF;
    if constexpr (Register == NRx2) return envelopeGenerator.readRegister();
    if constexpr (Register == NRx3) return clockShift << 4 | shortWidthMode << 3 | clockDivider;
    if constexpr (Register == NRx4) return lengthController.getState() << 6 | 0x3F;
}

template<uint8_t Register>
void NoiseChannel::writeIO(uint8_t value) {
    if constexpr (Register == NRx1) {
        lengthController.setPeriod(value & 0x3F);
    }
    else if constexpr (Register == NRx2) {
        writeEnvelope(value);
    }
    else if constexpr (Register == NRx3) {
        clockShift = value >> 4;
        shortWidthMode = value & 0x08;
        clockDivider = value & 0x07;
    }
    else if constexpr (Register == NRx4) {
        channelControl(value);
    }
}

void NoiseChannel::advanceOutput() {
    bool equal = ((feedbackRegister & 0b10) >> 1) == (feedbackRegister & 0b01); // xnor
    feedbackRegister = (feedbackRegister & ~(1 << 15)) | (equal << 15); // replace bit 15
    if (shortWidthMode) {
        feedbackRegister = (feedbackRegister & ~(1 << 7)) | (equal << 7); // replace bit 7
    }
    feedbackRegister >>= 1;
}

uint8_t NoiseChannel::sample() {
    bool hi = feedbackRegister & 0b1;
    return hi * envelopeGenerator.getVolume();
}

void NoiseChannel::trigger() {
    feedbackRegister = 0;
}

void NoiseChannel::resetPeriodTimer() {
    periodTimer = (std::max(1, clockDivider << 1) << clockShift) * PERIOD_MIN;
}

template uint8_t NoiseChannel::readIO<NoiseChannel::NRx1>();
template uint8_t NoiseChannel::readIO<NoiseChannel::NRx2>();
template uint8_t NoiseChannel::readIO<NoiseChannel::NRx3>();
template uint8_t NoiseChannel::readIO<NoiseChannel::NRx4>();

template void NoiseChannel::writeIO<NoiseChannel::NRx1>(uint8_t);
template void NoiseChannel::writeIO<NoiseChannel::NRx2>(uint8_t);
template void NoiseChannel::writeIO<NoiseChannel::NRx3>(uint8_t);
template void NoiseChannel::writeIO<NoiseChannel::NRx4>(uint8_t);