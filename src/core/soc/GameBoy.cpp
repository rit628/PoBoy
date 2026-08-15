#include "GameBoy.hpp"
#include "DMG.hpp"
#include "GraphicsConstants.hpp"
#include <chrono>
#include <cstdint>
#include <thread>

GameBoy::GameBoy(std::function<uint8_t()> readInput
       , std::function<void(std::span<const float>)> queueAudioData
       , std::function<void(std::span<const uint8_t>)> renderFrame)
       : dmg(cycleCount, cartridge, readInput, queueAudioData, renderFrame) {}

Memory::CartridgeMetadata GameBoy::loadRom(const std::filesystem::path& romFile) {
    resetClock();
    auto cartData = cartridge.loadRom(romFile);
    dmg.initialize(cartData);
    return cartData;
}

void GameBoy::run() {
    while (true) {
        frameAdvance();
        synchronizeClock();
    }
}

void GameBoy::run(std::stop_token stoken) {
    while (!stoken.stop_requested()) {
        frameAdvance();
        synchronizeClock();
    }
}

void GameBoy::frameAdvance() {
    uint64_t prevCycles = cycleCount;
    while (cycleCount - prevCycles < Graphics::DOTS_PER_FRAME) {
        dmg.tick();
    }
}

void GameBoy::synchronizeClock() {
    auto now = clock::now();
    auto elapsed = std::chrono::duration<double, std::micro>(now - start);
    auto expectedElapsed = cycleCount * DMG::CLOCK_US;

    if (elapsed < expectedElapsed) {
        auto waitTime = expectedElapsed - elapsed;
        std::this_thread::sleep_for(waitTime);
    }
}

void GameBoy::resetClock() {
    start = clock::now();
    cycleCount = 0;
}