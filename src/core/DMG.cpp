#include "DMG.hpp"
#include "GraphicsConstants.hpp"
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
        cycleCount++;
    }
    if (cycleCount % Graphics::DOTS_PER_FRAME == 0) wait();
}

Memory::CartridgeMetadata DMG::loadRom(const std::filesystem::path& romFile) {
    return mmu.loadRom(romFile);
}

void DMG::run() {
    initialize();
    while (true) cpu.tick();
}

void DMG::run(std::stop_token stoken) {
    initialize();
    while (!stoken.stop_requested()) cpu.tick();
}

void DMG::initialize() {
    start = clock::now();
    cycleCount = 0;
    cpu.initialize();
    imu.initialize();
    mmu.initialize();
    apu.initialize();
    ppu.initialize();
}

void DMG::wait() {
    auto now = clock::now();
    auto elapsed = std::chrono::duration<double, std::micro>(now - start);
    auto expectedElapsed = cycleCount * DMG_CLOCK_US;

    if (elapsed < expectedElapsed) {
        auto waitTime = expectedElapsed - elapsed;
        std::this_thread::sleep_for(waitTime);
    }
}