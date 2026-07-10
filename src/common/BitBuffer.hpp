#pragma once
#include <array>
#include <cstddef>
#include <cstdint>

template<uint8_t W>
concept BitStep = W == 1 || W == 2 || W == 4 || W == 8;

template<size_t N, uint8_t Step> requires BitStep<Step>
class BitBuffer {
    public:
        void push(uint8_t value);
        std::array<uint8_t, N>& extract();

    private:
        static constexpr uint8_t STEP_MASK = (1 << Step) - 1;

        std::array<uint8_t, N> buffer;
        size_t currentByte = 0;
        uint8_t currentBit = 0;
};

template<size_t N, uint8_t Step> requires BitStep<Step>
void BitBuffer<N, Step>::push(uint8_t value) {
    buffer.at(currentByte) &= ~(STEP_MASK << currentBit); // clear former bits
    buffer.at(currentByte) |= (value & STEP_MASK) << currentBit; // overwrite
    currentBit = (currentBit + Step) & 7;   // increment by step mod 8
    currentByte += !currentBit; // increment byte if we overflow bit count
}

template<size_t N, uint8_t Step> requires BitStep<Step>
std::array<uint8_t, N>& BitBuffer<N, Step>::extract() {
    currentByte = 0;
    currentBit = 0;
    return buffer;
}