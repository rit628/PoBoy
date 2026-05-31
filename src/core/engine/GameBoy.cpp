#include "GameBoy.hpp"

void GameBoy::runBootRomTest() {
    mmu.loadRom("test.gb");
    while (true) {
        cpu.tick();
    }
}