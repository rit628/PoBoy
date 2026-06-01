#pragma once
#include "MMU.hpp"
#include "CPU.hpp"
#include "PPU.hpp"

class GameBoy {
    public:
        void run();

    private:
        MMU mmu;
        CPU cpu{mmu};
        PPU ppu{mmu};
};