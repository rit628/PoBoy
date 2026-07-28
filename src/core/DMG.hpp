#pragma once
#include "IMU.hpp"
#include "MemoryConstants.hpp"
#include "PPU.hpp"
#include "APU.hpp"
#include "MMU.hpp"
#include "CPU.hpp"
#include <chrono>
#include <cstdint>
#include <filesystem>
#include <functional>
#include <span>
#include <stop_token>

class DMG {
    public:
        static constexpr double DMG_CLOCK_MHZ = 4.194304;
        static constexpr auto DMG_CLOCK_US = std::chrono::duration<double, std::micro>(1.0 / DMG_CLOCK_MHZ);

        DMG(std::function<uint8_t()> readInput
          , std::function<void(std::span<const float>)> queueAudioData
          , std::function<void(std::span<const uint8_t>)> renderFrame);

        Memory::CartridgeMetadata loadRom(const std::filesystem::path& romFile);
        void run();
        void run(std::stop_token stoken);
        void frameAdvance();
        void synchronizeClock();
        void resetClock();

    private:
        void initialize();
        void systemTick();

        using clock = std::chrono::steady_clock;
        clock::time_point start;
        uint64_t cycleCount = 0;

        Interrupts::IMU imu;
        Graphics::PPU ppu;
        Audio::APU apu;
        Memory::MMU mmu;
        Processing::CPU<> cpu;
};