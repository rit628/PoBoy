#include "APU.hpp"
#include "MemoryConstants.hpp"
#include <cstdint>

using namespace Audio;

APU::APU(Interrupts::IMU& imu, std::function<void(std::span<const float>)> queueAudioData) : imu(imu), queueAudioData(queueAudioData) {}

template<uint16_t Register>
uint8_t APU::readIO() {
    using namespace Memory;
    if constexpr (Register == NR50) return masterVolumeControl;
    if constexpr (Register == NR51) return soundPanControl;
    if constexpr (Register == NR52) return audioEnabled << 7 | channel2.dacEnabled() << 2;
    if constexpr (NR10 <= Register && Register <= NR14) return channel1.readIO<Register - NR10>();
    if constexpr (NR21 <= Register && Register <= NR24) return channel2.readIO<Register - NR21 + 1>();
}

template<uint16_t Register>
void APU::writeIO(uint8_t value) {
    using namespace Memory;
    if constexpr (Register == NR50) return void(masterVolumeControl = value);
    if constexpr (Register == NR51) return void(soundPanControl = value);
    if constexpr (Register == NR52) return void(audioEnabled = value & 0x80);
    if constexpr (NR10 <= Register && Register <= NR14) return channel1.writeIO<Register - NR10>(value);
    if constexpr (NR21 <= Register && Register <= NR24) return channel2.writeIO<Register - NR21 + 1>(value);
}

void APU::tick() {
    incrementDivider();
    dacs.at(0) = channel1.tick();
    dacs.at(1) = channel2.tick();
    mixChannels();
    if (samples.full()) queueAudioData(samples.extract());
}

void APU::incrementDivider() {
    static constexpr uint8_t APU_DIV_BIT = 0x08;
    uint8_t currentTime = imu.readIO<Memory::DIV>();
    bool currentBit = currentTime++ & APU_DIV_BIT;
    bool nextBit = currentTime & APU_DIV_BIT;
    if (currentBit <= nextBit) return; // only increment apu divider and clock channel units on falling edge
    apuDivider++; 
    if (!(apuDivider & 0x01)) { // sound length tick every other increment
        channel1.lengthTick();
        channel2.lengthTick();
    }
    if (!(apuDivider & 0x03)) { // CH1 freq sweep every 4 increments
    
    }
    if (!(apuDivider & 0x07)) { // envelope sweep every 8 increments
        // channel1.volumeEnvelopeTick();
        // channel2.volumeEnvelopeTick();
    }
}

void APU::mixChannels() {
    if (!audioEnabled) return addSample(0, 0);
    float left = 0, right = 0;
    for (uint8_t i = 0; i < dacs.size(); i++) {
        left += dacs.at(i);
        right += dacs.at(i);
    }
    left *= ((masterVolumeControl >> 4) & 0x07) + 1;
    right *= (masterVolumeControl & 0x07) + 1;
    left /= 4 * 8;
    right /= 4 * 8;
    addSample(left, right);
}

void APU::addSample(float left, float right) {
    samples.push(left);
    samples.push(right);
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