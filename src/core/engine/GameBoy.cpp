#include "GameBoy.hpp"
#include <chrono>
#include <cstdint>
#include <print>

void GameBoy::run() {
    mmu.loadRom("test.gb");
    while (true) {
        auto start = std::chrono::steady_clock::now();
        uint8_t cyclesTaken = cpu.tick();
        auto end = std::chrono::steady_clock::now();
        auto diff = end - start;
        auto us = std::chrono::duration_cast<std::chrono::duration<double, std::micro>>(diff).count();
        std::println("Emulated Clock Speed: {} MHz", cyclesTaken / us);
    }
}