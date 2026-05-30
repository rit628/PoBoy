#include "GameBoy.hpp"

void GameBoy::runBootRomTest() {
    while (true) {
        cpu.tick();
    }
}