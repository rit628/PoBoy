#include "DMG.hpp"
#include <chrono>
#include <print>
#include <thread>

DMG::DMG(std::function<uint8_t()> readInput, std::function<void(std::array<uint8_t, Graphics::FRAMEBUFFER_SIZE>&)> renderFrame)
        : imu(readInput)
        , ppu(imu, renderFrame)
        , mmu(imu, ppu)
        , cpu(mmu, std::bind(&DMG::systemTick, std::ref(*this)))
        {}

void DMG::systemTick() {
    mmu.tick(4);
    imu.tick(4);
    ppu.tick(4);
    totalMCycles++;
}

void DMG::run(const std::filesystem::path& romFile) {
    initialize(romFile);
    while (true) emuLoop();
}

void DMG::run(std::stop_token stoken, const std::filesystem::path& romFile) {
    initialize(romFile);
    while (!stoken.stop_requested()) emuLoop();
}

void DMG::initialize(const std::filesystem::path& romFile) {
    mmu.loadRom(romFile);
    start = clock::now();
    totalMCycles = 0;
}

void DMG::emuLoop() {
    cpu.tick();

    auto totalTCycles = totalMCycles * 4;

    auto now = clock::now();
    auto elapsed = std::chrono::duration<double, std::micro>(now - start);
    auto expectedElapsed = totalTCycles * DMG_CLOCK_US;

    if (elapsed < expectedElapsed) {
        auto waitTime = expectedElapsed - elapsed;
        std::this_thread::sleep_for(waitTime);
    }
}