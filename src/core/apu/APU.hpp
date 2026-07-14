#pragma once
#include "AudioConstants.hpp"
#include "IMU.hpp"
#include "PulseChannel.hpp"
#include "StaticQueue.hpp"
#include "WaveChannel.hpp"
#include <array>
#include <cstdint>
#include <span>

namespace Audio {
    
    class APU {
        public:
            APU(Interrupts::IMU& imu, std::function<void(std::span<const float>)> queueAudioData);

            template<uint16_t Register>
            uint8_t readIO();
            template<uint16_t Register>
            void writeIO(uint8_t value);
            uint8_t readWaveRAM(uint8_t address);
            void writeWaveRAM(uint8_t address, uint8_t value);

            void tick();
            void incrementDivider();
            void mixChannels();

        private:
            template<bool Left>
            bool getChannelPan(uint8_t channel);
            template<bool Left>
            uint8_t getVolume();
            float highPassFilter(float sample);
            template<auto Channel>
            void sample();
            void addSample(float left, float right);

            static constexpr float FILTER_CAPACITOR_CHARGE_RATE = 0.999958f;

            Interrupts::IMU& imu;
            std::function<void(std::span<const float>)> queueAudioData;

            uint8_t masterVolumeControl = 0;    // NR50 register
            uint8_t soundPanControl = 0;        // NR51 register
            bool audioEnabled = false;          // NR52 register bit 7

            uint8_t apuDivider = 0;
            bool prevDividerBit = 0;
            StaticQueue<float, 1 << 13> samples;
            float filterCapacitor = 0.0f;
            std::array<float, CHANNEL_COUNT> dacs{};
            SweepChannel channel1;
            PulseChannel channel2;
            WaveChannel channel3;
    };

}