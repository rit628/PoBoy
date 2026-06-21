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
        void systemTick();
        void run(const std::filesystem::path& romFile);

    private:
        uint64_t totalMCycles = 0;

        Interrupts::IMU imu;
        Graphics::PPU ppu{imu};
        Memory::MMU mmu{imu, ppu};
        Processing::CPU<> cpu{mmu, std::bind(&DMG::systemTick, std::ref(*this))};
};