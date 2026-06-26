#pragma once
#include "IMU.hpp"
#include "PPU.hpp"
#include "MMU.hpp"
#include "CPU.hpp"
#include <cstdint>
#include <filesystem>
#include <functional>

class DMG {
    public:
        DMG(std::function<uint8_t()> readInput, std::function<void(std::array<uint8_t, Graphics::FRAMEBUFFER_SIZE>&)> renderFrame);
        void systemTick();
        void run(const std::filesystem::path& romFile);

    private:
        uint64_t totalMCycles = 0;

        Interrupts::IMU imu;
        Graphics::PPU ppu;
        Memory::MMU mmu;
        Processing::CPU<> cpu;
};