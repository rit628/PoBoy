#include "DMG.hpp"
#include <chrono>
#include <cstdint>
#include <print>
#include <thread>

void DMG::run(const std::filesystem::path& romFile) {
    static constexpr double DMG_CLOCK_MHZ = 4.194304;
    static constexpr auto DMG_CLOCK_US = std::chrono::duration<double, std::micro>(1.0 / DMG_CLOCK_MHZ);
    
    mmu.loadRom(romFile);

    using clock = std::chrono::steady_clock;
    auto start = clock::now();
    uint64_t totalMCycles = 0;
    
    while (true) {
        auto mCycles = cpu.tick();
        sysClock.tick(mCycles * 4);
        ppu.tick(mCycles * 4);

        totalMCycles += mCycles;
        auto totalTCycles = totalMCycles * 4;

        auto now = clock::now();
        auto elapsed = std::chrono::duration<double, std::micro>(now - start);
        auto expectedElapsed = totalTCycles * DMG_CLOCK_US;

        if (elapsed < expectedElapsed) {
            auto waitTime = expectedElapsed - elapsed;
            std::this_thread::sleep_for(waitTime);
        }
    }
}