#pragma once
#include "IMU.hpp"
#include "PPU.hpp"
#include "MMU.hpp"
#include "CPU.hpp"
#include <filesystem>
#include <ostream>

class DMG {
    public:
        void run(const std::filesystem::path& romFile);
        void setTileOutputStream(std::ostream& os);

    private:
        std::ostream* tileStream = nullptr;

        Interrupts::IMU imu;
        Graphics::PPU ppu;
        Memory::MMU mmu{imu, ppu};
        CPU cpu{mmu};
};