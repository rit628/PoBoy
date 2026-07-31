#include "WaveChannel.hpp"
#include "AudioConstants.hpp"
#include <cstdint>

using namespace Audio;

WaveChannel::WaveChannel() {
    init();
}

void WaveChannel::init() {
    waveram.fill(0);
    currentOutputLevel = 0;
    waveRamIndex = 0;
    currentSample = 0;

    dacEnable = false;

    outputLevel = 0;
}

template<uint8_t Register>
uint8_t WaveChannel::readIO() {
    if constexpr (Register == NRx0) return 0x7F | dacEnable << 7;
    if constexpr (Register == NRx1) return 0xFF;
    if constexpr (Register == NRx2) return 0x9F | outputLevel << 5;
    if constexpr (Register == NRx3) return 0xFF;
    if constexpr (Register == NRx4) return 0xBF | lengthController.getState() << 6;
}

template<uint8_t Register>
void WaveChannel::writeIO(uint8_t value) {
    if constexpr (Register == NRx0) {
        dacEnable = value >> 7;
        if (!dacEnable) enabled = false;
    }
    else if constexpr (Register == NRx1) {
        lengthController.setPeriod(value);
    }
    else if constexpr (Register == NRx2) {
        outputLevel = (value >> 5) & 0b11;
    }
    else if constexpr (Register == NRx3) {
        setPeriodLo(value);
    }
    else if constexpr (Register == NRx4) {
        channelControl(value);
    }
}

uint8_t WaveChannel::readWaveRAM(uint8_t address) {
    if (enabled) return 0xFF;
    return waveram.at(address);
}

void WaveChannel::writeWaveRAM(uint8_t address, uint8_t value) {
    if (enabled) return;
    waveram.at(address) = value;
}

void WaveChannel::advanceOutput() {
    waveRamIndex = (waveRamIndex + 1) & WAVE_RAM_INDEX_MAX; // increment first so sample 0 is skipped initially
    uint8_t shift = 4 * !(waveRamIndex & 0b1); // hi nibble gets shifted down
    currentSample = (waveram.at(waveRamIndex >> 1) >> shift) & DIGITAL_SAMPLE_MAX;
    currentOutputLevel = outputLevel;
}

uint8_t WaveChannel::sample() {
    uint8_t shift = 0;
    switch (currentOutputLevel) {
        case 0b00: shift = 4; break;
        case 0b01: shift = 0; break;
        case 0b10: shift = 1; break;
        case 0b11: shift = 2; break;
    }
    return currentSample >> shift;
}

void WaveChannel::trigger() {
    waveRamIndex = 0;
    currentOutputLevel = outputLevel;
}

template uint8_t WaveChannel::readIO<WaveChannel::NRx0>();
template uint8_t WaveChannel::readIO<WaveChannel::NRx1>();
template uint8_t WaveChannel::readIO<WaveChannel::NRx2>();
template uint8_t WaveChannel::readIO<WaveChannel::NRx3>();
template uint8_t WaveChannel::readIO<WaveChannel::NRx4>();

template void WaveChannel::writeIO<WaveChannel::NRx0>(uint8_t);
template void WaveChannel::writeIO<WaveChannel::NRx1>(uint8_t);
template void WaveChannel::writeIO<WaveChannel::NRx2>(uint8_t);
template void WaveChannel::writeIO<WaveChannel::NRx3>(uint8_t);
template void WaveChannel::writeIO<WaveChannel::NRx4>(uint8_t);