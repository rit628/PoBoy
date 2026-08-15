#pragma once
#include "APU.hpp"
#include "Bus.hpp"
#include "CPU.hpp"
#include "IMU.hpp"
#include "MemoryConstants.hpp"
#include "PPU.hpp"
#include <cstdint>
#include <functional>
#include <span>

class DMG {
    public:
        static constexpr double CLOCK_MHZ = 4.194304;
        static constexpr auto CLOCK_US = std::chrono::duration<double, std::micro>(1.0 / CLOCK_MHZ);

        DMG(uint64_t& cycleCount
          , Memory::Cartridge& cartridge
          , std::function<uint8_t()> readInput
          , std::function<void(std::span<const float>)> queueAudioData
          , std::function<void(std::span<const uint8_t>)> renderFrame);
        
        void initialize(const Memory::CartridgeMetadata& cartData);
        void tick();

    private:
        void systemTick();

        uint64_t& cycleCount;

        Memory::Cartridge& cartridge;
        Interrupts::IMU imu;
        Graphics::PPU ppu;
        Audio::APU apu;
        Memory::Bus bus;
        Processing::CPU<Memory::Bus&> cpu;
};