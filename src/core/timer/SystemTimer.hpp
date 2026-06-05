#pragma once
#include "MMU.hpp"
#include <array>
#include <cstdint>

class SystemTimer {
    public:
        SystemTimer(MMU& mmu) : mmu(mmu) {}
        void tick(uint8_t tCycles);
        void tick();

    private:
        static constexpr std::array<uint16_t, 4> timerClocks = {1024, 16, 64, 256};

        MMU& mmu;
        uint16_t systemCounter = 0;
};