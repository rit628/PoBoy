#include "MMU.hpp"
#include <cstddef>
#include <cstdint>

namespace {
    constexpr size_t BOOTROM_SIZE = 0x0100;
    constexpr std::array<uint8_t, BOOTROM_SIZE> bootrom = {
        #embed "bootix_dmg.bin"
    };
}

uint8_t& MMU::operator[](uint16_t address) {
    if (readFromBootRom && address < BOOTROM_SIZE) {
        return const_cast<uint8_t&>(bootrom.at(address));
    }
    return memory.at(address);
}