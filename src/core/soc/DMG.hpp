#pragma once
#include "MMU.hpp"
#include "CPU.hpp"
#include "PPU.hpp"
#include <filesystem>

class DMG {
    public:
        void run(const std::filesystem::path& romFile);

    private:
        MMU mmu;
        CPU cpu{mmu};
        PPU ppu{mmu};
};