#pragma once
#include "MMU.hpp"
#include "CPU.hpp"

class GameBoy {
    public:
        void runBootRomTest();


    private:
        MMU mmu;
        CPU cpu{mmu};
};