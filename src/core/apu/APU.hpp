#pragma once
#include "AudioConstants.hpp"
#include "IMU.hpp"
#include "NoiseChannel.hpp"
#include "PulseChannel.hpp"
#include "StaticQueue.hpp"
#include "SystemConstants.hpp"
#include "WaveChannel.hpp"
#include <array>
#include <cstdint>
#include <span>

namespace Audio {
    
    template<MODEL Model>
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

            void initHLE();
            void tick();                                // per t-cycle
            void tickDivider(bool shiftBit = false);    // per m-cycle

        private:
            void disableAudio();
            void enableAudio();
            void sampleChannels();
            void mixChannels();
            template<bool Left>
            bool getChannelPan(uint8_t channel);
            template<bool Left>
            uint8_t getVolume();
            float highPassFilter(float sample, float& capacitor);
            template<auto Channel>
            void sample();
            void addSample(float left, float right);

            static constexpr std::array<float, 16> DAC_TABLE = []() consteval {
                std::array<float, 16> lut{};
                for (uint8_t i = 0; i < lut.size(); i++) {
                    lut.at(i) = -2 * (float(i) / DIGITAL_SAMPLE_MAX) + 1;
                }
                return lut;
            }();

            Interrupts::IMU& imu;
            std::function<void(std::span<const float>)> queueAudioData;

            uint8_t masterVolumeControl;    // NR50 register
            uint8_t soundPanControl;        // NR51 register
            bool audioEnabled;              // NR52 register bit 7

            uint8_t apuDivider;
            bool prevDividerBit;
            StaticQueue<float, 1 << 10> samples;
            uint8_t discardedSamples;
            float leftFilterCapacitor, rightFilterCapacitor;
            std::array<float, CHANNEL_COUNT> dacs;

            SweepChannel channel1;
            PulseChannel channel2;
            WaveChannel channel3;
            NoiseChannel channel4;
    };

}

#include "APU.tpp"