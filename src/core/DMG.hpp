#pragma once
#include "IMU.hpp"
#include "PPU.hpp"
#include "MMU.hpp"
#include "CPU.hpp"
#include <chrono>
#include <cstdint>
#include <filesystem>
#include <functional>
#include <stop_token>

class DMG {
    public:
        static constexpr double DMG_CLOCK_MHZ = 4.194304;
        static constexpr auto DMG_CLOCK_US = std::chrono::duration<double, std::micro>(1.0 / DMG_CLOCK_MHZ);

        DMG(std::function<uint8_t()> readInput, std::function<void(std::array<uint8_t, Graphics::FRAMEBUFFER_SIZE>&)> renderFrame);
        void systemTick();
        void run(const std::filesystem::path& romFile);
        void run(std::stop_token stoken, const std::filesystem::path& romFile);

    private:
        void initialize(const std::filesystem::path& romFile);
        void emuLoop();

        using clock = std::chrono::steady_clock;
        clock::time_point start;
        uint64_t totalMCycles = 0;

        Interrupts::IMU imu;
        Graphics::PPU ppu;
        Memory::MMU mmu;
        Processing::CPU<> cpu;
};