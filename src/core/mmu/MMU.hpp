#pragma once
#include <array>
#include <cstdint>

class MMU {
    private:
        std::array<uint8_t, 0xFFFF> memory;

    public:
        uint8_t& operator[](uint16_t address);

        bool readFromBootRom = true; // for testing

        /* some important addresses */
        static constexpr uint16_t IE = 0xFFFF; // interrupt enable
        static constexpr uint16_t IF = 0xFF0F; // interrupt flag
};