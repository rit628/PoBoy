#pragma once
#include "Cartridge.hpp"
#include "DMG.hpp"
#include "MemoryConstants.hpp"
#include <chrono>
#include <cstdint>
#include <filesystem>
#include <functional>
#include <span>
#include <stop_token>

class GameBoy {
    public:
        GameBoy(std::function<uint8_t()> readInput
              , std::function<void(std::span<const float>)> queueAudioData
              , std::function<void(std::span<const uint8_t>)> renderFrame);

        Memory::CartridgeMetadata loadRom(const std::filesystem::path& romFile);
        void run();
        void run(std::stop_token stoken);
        void frameAdvance();
        void synchronizeClock();
        void resetClock();

    private:
        using clock = std::chrono::steady_clock;
        clock::time_point start;
        uint64_t cycleCount;

        Memory::Cartridge cartridge;
        DMG dmg;
};