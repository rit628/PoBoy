#pragma once
#include <chrono>
#include <cstdint>

enum class MODEL : uint8_t {
    DMG,
    CGB
};

constexpr uint32_t CLOCK_HZ = 1 << 22;
constexpr double CLOCK_MHZ  = CLOCK_HZ / 1e6;
constexpr auto CLOCK_US     = std::chrono::duration<double, std::micro>(1.0 / CLOCK_MHZ);