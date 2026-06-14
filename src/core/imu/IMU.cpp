#include "IMU.hpp"
#include "FlagOps.hpp"

using namespace Interrupts;

uint8_t IMU::readIF() {
    return interruptFlags;
}

void IMU::writeIF(uint8_t value) {
    /* upper 3 bits of IF are always 1 */
    interruptFlags = value | 0b11100000;
}

void IMU::writeIF(INTERRUPT_FLAG flag) {
    setFlags(interruptFlags, flag);
}

uint8_t IMU::readIE() {
    return interruptEnable;
}

void IMU::writeIE(uint8_t value) {
    interruptEnable = value;
}

void IMU::writeIE(INTERRUPT_FLAG flag) {
    setFlags(interruptEnable, flag);
}


void IMU::tick(uint8_t tCycles) {
    timer.tick(tCycles);
}

uint8_t IMU::readDIV() {
    return timer.readDIV();
}

void IMU::writeDIV(uint8_t value) {
    timer.writeDIV(value);
}

uint8_t IMU::readTIMA() {
    return timer.readTIMA();
}

void IMU::writeTIMA(uint8_t value) {
    timer.writeTIMA(value);
}

uint8_t IMU::readTMA() {
    return timer.readTMA();
}

void IMU::writeTMA(uint8_t value) {
    timer.writeTMA(value);
}

uint8_t IMU::readTAC() {
    return timer.readTAC();
}

void IMU::writeTAC(uint8_t value) {
    timer.writeTAC(value);
}
