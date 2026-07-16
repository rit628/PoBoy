#include "APU.hpp"
#include "AudioConstants.hpp"
#include "ConstevalMath.hpp"
#include "MemoryConstants.hpp"
#include <cstdint>
#include <type_traits>

using namespace Audio;

constexpr float FILTER_CAPACITOR_CHARGE_RATE = cpow(0.999958f, cround(SAMPLES_TO_DISCARD));

APU::APU(Interrupts::IMU& imu, std::function<void(std::span<const float>)> queueAudioData) : imu(imu), queueAudioData(queueAudioData) {}

template<uint16_t Register>
uint8_t APU::readIO() {
    using namespace Memory;
    if constexpr (Register == NR50) return masterVolumeControl;
    if constexpr (Register == NR51) return soundPanControl;
    if constexpr (Register == NR52) {
        return audioEnabled << 7
            | 0x70
            | channel4.on() << 3
            | channel3.on() << 2
            | channel2.on() << 1
            | channel1.on();
    }
    if constexpr (NR10 <= Register && Register <= NR14) return channel1.readIO<Register - NR10>();
    if constexpr (NR21 <= Register && Register <= NR24) return channel2.readIO<Register - NR21 + 1>();
    if constexpr (NR30 <= Register && Register <= NR34) return channel3.readIO<Register - NR30>();
    if constexpr (NR41 <= Register && Register <= NR44) return channel4.readIO<Register - NR41 + 1>();
}

template<uint16_t Register>
void APU::writeIO(uint8_t value) {
    using namespace Memory;
    if constexpr (Register == NR50) return void(masterVolumeControl = value);
    if constexpr (Register == NR51) return void(soundPanControl = value);
    if constexpr (Register == NR52) return void(audioEnabled = value & 0x80);
    if constexpr (NR10 <= Register && Register <= NR14) return channel1.writeIO<Register - NR10>(value);
    if constexpr (NR21 <= Register && Register <= NR24) return channel2.writeIO<Register - NR21 + 1>(value);
    if constexpr (NR30 <= Register && Register <= NR34) return channel3.writeIO<Register - NR30>(value);
    if constexpr (NR41 <= Register && Register <= NR44) return channel4.writeIO<Register - NR41 + 1>(value);
}

uint8_t APU::readWaveRAM(uint8_t address) {
    return channel3.readWaveRAM(address);
}

void APU::writeWaveRAM(uint8_t address, uint8_t value) {
    return channel3.writeWaveRAM(address, value);
}

void APU::tick() {
    incrementDivider();
    sample<&APU::channel1>();
    sample<&APU::channel2>();
    sample<&APU::channel3>();
    sample<&APU::channel4>();
    mixChannels();
    if (samples.full()) queueAudioData(samples.extract());
}

void APU::incrementDivider() {
    static constexpr uint8_t APU_DIV_BIT = 0x10;
    bool currDividerBit = imu.readIO<Memory::DIV>() & APU_DIV_BIT;
    bool increment = prevDividerBit > currDividerBit;
    prevDividerBit = currDividerBit;
    if (!increment) return; // only increment apu divider and tick channel units on falling edge
    apuDivider++; 
    if (!(apuDivider & 0x01)) { // sound length tick every other increment
        channel1.tickLength();
        channel2.tickLength();
        channel3.tickLength();
        channel4.tickLength();
    }
    if (!(apuDivider & 0x03)) { // CH1 freq sweep every 4 increments
        channel1.tickSweep();
    }
    if (!(apuDivider & 0x07)) { // envelope sweep every 8 increments
        channel1.tickEnvelope();
        channel2.tickEnvelope();
        channel4.tickEnvelope();
    }
}

void APU::mixChannels() {
    if (!audioEnabled) return addSample(0, 0);
    float left = 0, right = 0;
    for (uint8_t i = 0; i < dacs.size(); i++) {
        left += getChannelPan<true>(i) * dacs.at(i);
        right += getChannelPan<false>(i) * dacs.at(i);
    }
    left /= CHANNEL_COUNT;
    right /= CHANNEL_COUNT;
    left *= getVolume<true>() / VOLUME_MAX;
    right *= getVolume<false>() / VOLUME_MAX;
    addSample(left, right);
}

template<bool Left>
bool APU::getChannelPan(uint8_t channel) {
    if constexpr (Left) return (soundPanControl >> 4) & (1 << channel);
    else return soundPanControl & (1 << channel);
}

template<bool Left>
uint8_t APU::getVolume() {
    if constexpr (Left) return ((masterVolumeControl & 0x70) >> 4) + 1;
    else return (masterVolumeControl & 0x07) + 1;
}

float APU::highPassFilter(float sample) {
    float filtered = 0.0;
    if (channel1.dacEnabled() || channel2.dacEnabled() || channel3.dacEnabled() || channel4.dacEnabled()) {
        filtered = sample - filterCapacitor;
        filterCapacitor = sample - filtered * FILTER_CAPACITOR_CHARGE_RATE;
    }
    return filtered;
}

template<auto Channel>
void APU::sample() {
    uint8_t digitalSample = (this->*Channel).tick();
    float analogSample = 0.0f;
    if ((this->*Channel).dacEnabled()) {
        analogSample = -2 * (float(digitalSample) / DIGITAL_SAMPLE_MAX) + 1;
    }

    constexpr auto isChannel = [](auto targetChannel) consteval {
        /* ensure we dont compare different pointer types */
        if constexpr (std::is_same_v<decltype(Channel), decltype(targetChannel)>) {
            return Channel == targetChannel;
        }
        else {
            return false;
        }
    };

    if constexpr (isChannel(&APU::channel1)) dacs.at(0) = analogSample;
    if constexpr (isChannel(&APU::channel2)) dacs.at(1) = analogSample;
    if constexpr (isChannel(&APU::channel3)) dacs.at(2) = analogSample;
    if constexpr (isChannel(&APU::channel4)) dacs.at(3) = analogSample;
}

void APU::addSample(float left, float right) {
    if (++discardedSamples < SAMPLES_TO_DISCARD) return;
    discardedSamples = 0;
    samples.push(highPassFilter(left));
    samples.push(highPassFilter(right));
}

template uint8_t APU::readIO<Memory::NR50>();
template uint8_t APU::readIO<Memory::NR51>();
template uint8_t APU::readIO<Memory::NR52>();

template uint8_t APU::readIO<Memory::NR10>();
template uint8_t APU::readIO<Memory::NR11>();
template uint8_t APU::readIO<Memory::NR12>();
template uint8_t APU::readIO<Memory::NR13>();
template uint8_t APU::readIO<Memory::NR14>();

template uint8_t APU::readIO<Memory::NR21>();
template uint8_t APU::readIO<Memory::NR22>();
template uint8_t APU::readIO<Memory::NR23>();
template uint8_t APU::readIO<Memory::NR24>();

template uint8_t APU::readIO<Memory::NR30>();
template uint8_t APU::readIO<Memory::NR31>();
template uint8_t APU::readIO<Memory::NR32>();
template uint8_t APU::readIO<Memory::NR33>();
template uint8_t APU::readIO<Memory::NR34>();

template uint8_t APU::readIO<Memory::NR41>();
template uint8_t APU::readIO<Memory::NR42>();
template uint8_t APU::readIO<Memory::NR43>();
template uint8_t APU::readIO<Memory::NR44>();


template void APU::writeIO<Memory::NR50>(uint8_t);
template void APU::writeIO<Memory::NR51>(uint8_t);
template void APU::writeIO<Memory::NR52>(uint8_t);

template void APU::writeIO<Memory::NR10>(uint8_t);
template void APU::writeIO<Memory::NR11>(uint8_t);
template void APU::writeIO<Memory::NR12>(uint8_t);
template void APU::writeIO<Memory::NR13>(uint8_t);
template void APU::writeIO<Memory::NR14>(uint8_t);

template void APU::writeIO<Memory::NR21>(uint8_t);
template void APU::writeIO<Memory::NR22>(uint8_t);
template void APU::writeIO<Memory::NR23>(uint8_t);
template void APU::writeIO<Memory::NR24>(uint8_t);

template void APU::writeIO<Memory::NR30>(uint8_t);
template void APU::writeIO<Memory::NR31>(uint8_t);
template void APU::writeIO<Memory::NR32>(uint8_t);
template void APU::writeIO<Memory::NR33>(uint8_t);
template void APU::writeIO<Memory::NR34>(uint8_t);

template void APU::writeIO<Memory::NR41>(uint8_t);
template void APU::writeIO<Memory::NR42>(uint8_t);
template void APU::writeIO<Memory::NR43>(uint8_t);
template void APU::writeIO<Memory::NR44>(uint8_t);