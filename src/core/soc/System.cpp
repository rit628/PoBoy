#include "System.hpp"
#include "SystemConstants.hpp"

template<MODEL Model>
System<Model>::System(uint64_t& cycleCount
                    , Memory::Cartridge& cartridge
                    , std::function<uint8_t()> readInput
                    , std::function<void(std::span<const float>)> queueAudioData
                    , std::function<void(std::span<const uint8_t>)> renderFrame)
                    : cartridge(cartridge)
                    , imu(readInput)
                    , apu(imu, queueAudioData)
                    , ppu(imu, renderFrame)
                    , bus(cycleCount, cartridge, imu, apu, ppu)
                    , cpu(bus) 
                    {}

template<MODEL Model>
void System<Model>::initialize(const Memory::CartridgeMetadata& cartData) {
    cpu.initialize();
    imu.initialize();
    bus.initialize();
    apu.initialize();
    ppu.initialize();
    if (!bus.loadBootrom()) cpu.bootHLE(cartData);
}

template<MODEL Model>
void System<Model>::tick() {
    cpu.tick();
}

template class System<MODEL::DMG>;
template class System<MODEL::CGB>;