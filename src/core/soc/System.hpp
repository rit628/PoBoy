#pragma once
#include "APU.hpp"
#include "Bus.hpp"
#include "CPU.hpp"
#include "IMU.hpp"
#include "MemoryConstants.hpp"
#include "PPU.hpp"
#include "SystemConstants.hpp"
#include <cstdint>
#include <functional>
#include <span>

template<MODEL Model>
class System {
    public:
        System(uint64_t& cycleCount
             , Memory::Cartridge& cartridge
             , std::function<uint8_t()> readInput
             , std::function<void(std::span<const float>)> queueAudioData
             , std::function<void(std::span<const uint8_t>)> renderFrame);
        
        void initialize(const Memory::CartridgeMetadata& cartData);
        void tick();

    private:
        void systemTick();

        uint64_t& cycleCount;

        Memory::Cartridge& cartridge;
        Interrupts::IMU imu;
        Audio::APU apu;
        Graphics::PPU<Model> ppu;
        Memory::Bus<Model> bus;
        Processing::CPU<Memory::Bus<Model>&> cpu;
};