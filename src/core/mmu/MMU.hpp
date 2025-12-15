#pragma once
#include <array>
#include <cstdint>

class MMU {
    private:
        std::array<uint8_t, 0xFFFF> memory;

    public:
        uint8_t& operator[](uint16_t address);
};