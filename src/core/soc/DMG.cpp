#include "DMG.hpp"

DMG::DMG(uint64_t& cycleCount
       , Memory::Cartridge& cartridge
       , std::function<uint8_t()> readInput
       , std::function<void(std::span<const float>)> queueAudioData
       , std::function<void(std::span<const uint8_t>)> renderFrame)
       : cycleCount(cycleCount)
       , cartridge(cartridge)
       , imu(readInput)
       , ppu(imu, renderFrame)
       , apu(imu, queueAudioData)
       , bus(cartridge, imu, apu, ppu)
       , cpu(bus, std::bind(&DMG::systemTick, std::ref(*this))) 
       {}

void DMG::initialize(const Memory::CartridgeMetadata& cartData) {
    cpu.initialize();
    imu.initialize();
    bus.initialize();
    apu.initialize();
    ppu.initialize();
    if (!bus.loadBootrom()) cpu.bootHLE(cartData);
}

void DMG::tick() {
    cpu.tick();
}

void DMG::systemTick() {
    for (uint8_t i = 0; i < 4; i++) {
        cartridge.tick();
        imu.tick();
        apu.tick();
        ppu.tick();
        cycleCount++;
    }
}