#pragma once
#include "IMU.hpp"
#include "PPU.hpp"
#include "MMU.hpp"
#include "CPU.hpp"
#include <filesystem>

class DMG {
    public:
        void run(const std::filesystem::path& romFile);

    private:
        IMU imu;
        PPU ppu;
        MMU mmu{imu, ppu};
        CPU cpu{mmu};
};