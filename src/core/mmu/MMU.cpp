#include "MMU.hpp"
#include <cstdint>

uint8_t& MMU::operator[](uint16_t address) {
    return memory.at(address);
}