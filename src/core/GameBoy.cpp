#include "GameBoy.hpp"
#include "GraphicsConstants.hpp"
#include "System.hpp"
#include "SystemConstants.hpp"
#include <chrono>
#include <cstdint>
#include <thread>
#include <variant>

template<class... Ts>
struct overloaded : Ts... { using Ts::operator()...; };

GameBoy::GameBoy(std::function<uint8_t()> readInput
       , std::function<void(std::span<const float>)> queueAudioData
       , std::function<void(std::span<const uint8_t>)> renderFrame)
       : readInput(readInput)
       , queueAudioData(queueAudioData)
       , renderFrame(renderFrame)
       {}

Memory::CartridgeMetadata GameBoy::loadRom(const std::filesystem::path& romFile) {
    resetClock();
    auto cartData = cartridge.loadRom(romFile);
    bool useCgb = cartData.cgbFlag == 0xC0 || cartData.cgbFlag == 0x80;
    if (useCgb) {
        auto& cgb = soc.emplace<System<CGB>>(cycleCount, cartridge, readInput, queueAudioData, renderFrame);
        cgb.initialize(cartData);
    }
    else {
        auto& dmg = soc.emplace<System<DMG>>(cycleCount, cartridge, readInput, queueAudioData, renderFrame);
        dmg.initialize(cartData);
    }
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
        std::visit(overloaded {
            [](std::monostate) {},
            [](auto&& soc) { soc.tick(); }
        }, soc);
    }
}

void GameBoy::synchronizeClock() {
    auto now = clock::now();
    auto elapsed = std::chrono::duration<double, std::micro>(now - start);
    auto expectedElapsed = cycleCount * CLOCK_US;

    if (elapsed < expectedElapsed) {
        auto waitTime = expectedElapsed - elapsed;
        std::this_thread::sleep_for(waitTime);
    }
}

void GameBoy::resetClock() {
    start = clock::now();
    cycleCount = 0;
}

MODEL GameBoy::getModel() {
    if (std::holds_alternative<System<CGB>>(soc)) {
        return CGB;
    }
    else {
        return DMG;
    }
}