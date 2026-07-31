#pragma once
#include "AudioConstants.hpp"
#include "IMU.hpp"
#include "NoiseChannel.hpp"
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
            void initialize();

            template<uint16_t Register>
            uint8_t readIO();
            template<uint16_t Register>
            void writeIO(uint8_t value);
            uint8_t readWaveRAM(uint8_t address);
            void writeWaveRAM(uint8_t address, uint8_t value);

            void tick();
            
        private:
            void disableAudio();
            void incrementDivider();
            void sampleChannels();
            void mixChannels();
            template<bool Left>
            bool getChannelPan(uint8_t channel);
            template<bool Left>
            uint8_t getVolume();
            float highPassFilter(float sample);
            template<auto Channel>
            void sample();
            void addSample(float left, float right);

            Interrupts::IMU& imu;
            std::function<void(std::span<const float>)> queueAudioData;

            uint8_t masterVolumeControl;    // NR50 register
            uint8_t soundPanControl;        // NR51 register
            bool audioEnabled;              // NR52 register bit 7

            uint8_t apuDivider;
            bool prevDividerBit;
            StaticQueue<float, 1 << 10> samples;
            uint8_t discardedSamples;
            float filterCapacitor;
            std::array<float, CHANNEL_COUNT> dacs;

            SweepChannel channel1;
            PulseChannel channel2;
            WaveChannel channel3;
            NoiseChannel channel4;
    };

}