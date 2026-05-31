#pragma once
#include "MMU.hpp"
#include "CPU.hpp"

class GameBoy {
    public:
        void run();

    private:
        MMU mmu;
        CPU cpu{mmu};
};