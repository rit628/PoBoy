#include "DMG.hpp"
#include <chrono>
#include <cstdint>
#include <thread>

DMG::DMG(std::function<uint8_t()> readInput
       , std::function<void(std::span<const float>)> queueAudioData
       , std::function<void(std::array<uint8_t, Graphics::FRAMEBUFFER_SIZE>&)> renderFrame)
        : imu(readInput)
        , ppu(imu, renderFrame)
        , apu(imu, queueAudioData)
        , mmu(imu, apu, ppu)
        , cpu(mmu, std::bind(&DMG::systemTick, std::ref(*this)))
        {}

void DMG::systemTick() {
    for (uint8_t i = 0; i < 4; i++) {
        imu.tick();
        mmu.tick();
        apu.tick();
        ppu.tick();
    }
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