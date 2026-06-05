#include "SystemTimer.hpp"
#include "IO.hpp"
#include <cstdint>

void SystemTimer::tick(uint8_t tCycles) {
    for (uint8_t i = 0; i < tCycles; i++) {
        tick();
    }
}

void SystemTimer::tick() {
    auto prevCounter = systemCounter++;
    mmu.write(IO::DIV, systemCounter >> 8);
    auto timerControl = mmu.read(IO::TAC);
    bool timerEnabled = timerControl & 0b100;
    if (!timerEnabled) return;
    auto timerClock = timerClocks.at(timerControl & 0b11);
    /* check for bit change */
    if ((timerClock & prevCounter) ^ (timerClock & systemCounter)) {
        auto count = mmu.read(IO::TIMA);
        if (count == 0xFF) {
            count = mmu.read(IO::TMA);
            auto IF = mmu.read(IO::IF);
            flagSet(IF, INTERRUPT_BIT::TIMER);
            mmu.write(IO::IF,  IF);
        }
        else {
            count++;
        }
        mmu.write(IO::TIMA, count);
    }
}