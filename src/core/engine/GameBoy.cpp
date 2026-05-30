#include "GameBoy.hpp"
#include <thread>

void GameBoy::runBootRomTest() {
    while (true) {
        cpu.tick();
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
}