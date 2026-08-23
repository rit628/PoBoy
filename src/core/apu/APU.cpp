#include "APU.hpp"
#include "AudioConstants.hpp"
#include "ConstevalMath.hpp"
#include "MemoryConstants.hpp"
#include "SystemConstants.hpp"
#include <cstdint>
#include <type_traits>

using namespace Audio;

constexpr float FILTER_CAPACITOR_CHARGE_RATE = cpow(0.999958f, cround(SAMPLES_TO_DISCARD));

template<MODEL Model>
APU<Model>::APU(Interrupts::IMU& imu, std::function<void(std::span<const float>)> queueAudioData)
        : imu(imu), queueAudioData(queueAudioData)
{
    initialize();
}

template<MODEL Model>
void APU<Model>::initialize() {
    masterVolumeControl = 0;
    soundPanControl = 0;
    audioEnabled = false;

    apuDivider = 0;
    prevDividerBit = 0;
    samples.extract();  // empty samples
    discardedSamples = 0;
    leftFilterCapacitor = 0.0f;
    rightFilterCapacitor = 0.0f;
    dacs.fill(0);

    channel1.initialize();
    channel2.initialize();
    channel3.initialize();
    channel4.initialize();
}

template<MODEL Model>
uint8_t APU<Model>::readWaveRAM(uint8_t address) {
    return channel3.readWaveRAM(address);
}

template<MODEL Model>
void APU<Model>::writeWaveRAM(uint8_t address, uint8_t value) {
    return channel3.writeWaveRAM(address, value);
}

template<MODEL Model>
void APU<Model>::initHLE() {
    using namespace Memory;

    audioEnabled = true;    // enable register writes

    writeIO<NR10>(0x80);
    writeIO<NR11>(0xBF);
    writeIO<NR12>(0xF3);
    writeIO<NR13>(0xFF);
    writeIO<NR14>(0xBF);
    writeIO<NR21>(0x3F);
    writeIO<NR22>(0x00);
    writeIO<NR23>(0xFF);
    writeIO<NR24>(0xBF);
    writeIO<NR30>(0x7F);
    writeIO<NR31>(0xFF);
    writeIO<NR32>(0x9F);
    writeIO<NR33>(0xFF);
    writeIO<NR34>(0xBF);
    writeIO<NR41>(0xFF);
    writeIO<NR42>(0x00);
    writeIO<NR43>(0x00);
    writeIO<NR44>(0xBF);
    writeIO<NR50>(0x77);
    writeIO<NR51>(0xF3);
    writeIO<NR52>(0xF1);
}

template<MODEL Model>
void APU<Model>::tick() {
    channel1.tick();
    channel2.tick();
    channel3.tick();
    channel4.tick();
    if (++discardedSamples == SAMPLES_TO_DISCARD) sampleChannels();
}

template<MODEL Model>
void APU<Model>::tickDivider(bool shiftBit) {
    static constexpr uint8_t APU_DIV_BIT = 0x10;
    bool currDividerBit = imu.readIO<Memory::DIV>() & (APU_DIV_BIT << shiftBit);
    bool increment = prevDividerBit > currDividerBit;
    prevDividerBit = currDividerBit;
    if (!audioEnabled || !increment) return; // only increment apu divider and tick channel units on falling edge
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
    ++apuDivider;
}

template<MODEL Model>
void APU<Model>::disableAudio() {
    masterVolumeControl = 0;
    soundPanControl = 0;
    
    channel1.disable();
    channel2.disable();
    channel3.disable();
    channel4.disable();
}

template<MODEL Model>
void APU<Model>::enableAudio() {
    apuDivider = 0;
    
    channel1.initialize();
    channel2.initialize();
    channel3.initialize();
    channel4.initialize();
}

template<MODEL Model>
void APU<Model>::sampleChannels() {
    if (audioEnabled) {
        sample<&APU::channel1>();
        sample<&APU::channel2>();
        sample<&APU::channel3>();
        sample<&APU::channel4>();
        mixChannels();
    }
    else {
        addSample(0, 0);
    }
    if (samples.full()) queueAudioData(samples.extract());
}

template<MODEL Model>
void APU<Model>::mixChannels() {
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

template<MODEL Model>template<bool Left>
bool APU<Model>::getChannelPan(uint8_t channel) {
    if constexpr (Left) return (soundPanControl >> 4) & (1 << channel);
    else return soundPanControl & (1 << channel);
}

template<MODEL Model>
template<bool Left>
uint8_t APU<Model>::getVolume() {
    if constexpr (Left) return ((masterVolumeControl & 0x70) >> 4) + 1;
    else return (masterVolumeControl & 0x07) + 1;
}

template<MODEL Model>
float APU<Model>::highPassFilter(float sample, float& capacitor) {
    float filtered = 0.0f;
    if (channel1.dacEnabled() || channel2.dacEnabled() || channel3.dacEnabled() || channel4.dacEnabled()) {
        filtered = sample - capacitor;
        capacitor = sample - filtered * FILTER_CAPACITOR_CHARGE_RATE;
    }
    return filtered;
}

template<MODEL Model>
template<auto Channel>
void APU<Model>::sample() {
    uint8_t digitalSample = (this->*Channel).getDigitalSample();
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

template<MODEL Model>
void APU<Model>::addSample(float left, float right) {
    discardedSamples = 0;
    samples.push(highPassFilter(left, leftFilterCapacitor));
    samples.push(highPassFilter(right, rightFilterCapacitor));
}

template class Audio::APU<MODEL::DMG>;
template class Audio::APU<MODEL::CGB>;