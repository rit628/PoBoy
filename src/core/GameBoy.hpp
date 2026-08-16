#pragma once
#include "Cartridge.hpp"
#include "System.hpp"
#include "MemoryConstants.hpp"
#include "SystemConstants.hpp"
#include <chrono>
#include <cstdint>
#include <filesystem>
#include <functional>
#include <span>
#include <stop_token>
#include <variant>

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
        MODEL getModel();

    private:
        using enum MODEL;
        using clock = std::chrono::steady_clock;

        const std::function<uint8_t()> readInput;
        const std::function<void(std::span<const float>)> queueAudioData;
        const std::function<void(std::span<const uint8_t>)> renderFrame;
        
        clock::time_point start;
        uint64_t cycleCount;

        Memory::Cartridge cartridge;
        std::variant<std::monostate, System<DMG>, System<CGB>> soc;
};