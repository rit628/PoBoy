#pragma once
#include "AudioConstants.hpp"
#include "IMU.hpp"
#include "PulseChannel.hpp"
#include "StaticQueue.hpp"
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

            void tick();
            void incrementDivider();
            void mixChannels();

        private:
            template<bool Left>
            bool getChannelPan(uint8_t channel);
            template<bool Left>
            uint8_t getVolume();
            template<auto Channel>
            void sample();
            void addSample(float left, float right);

            Interrupts::IMU& imu;
            std::function<void(std::span<const float>)> queueAudioData;

            uint8_t masterVolumeControl = 0;    // NR50 register
            uint8_t soundPanControl = 0;        // NR51 register
            bool audioEnabled = false;          // NR52 register bit 7

            uint8_t apuDivider = 0;
            bool prevDividerBit = 0;
            StaticQueue<float, 1 << 13> samples;
            std::array<float, CHANNEL_COUNT> dacs{};
            PulseChannel channel1;
            PulseChannel channel2;
    };

}