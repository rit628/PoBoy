#pragma once
#include <array>
#include <cstddef>
#include <cstdint>
#include <span>

template<uint8_t S>
concept BitStep = S == 1 || S == 2 || S == 4 || S == 8;

template<size_t N, uint8_t Step> requires BitStep<Step>
class BitBufferBase {
    public:
        uint8_t clearMask() const noexcept;
        uint8_t writeMask(uint8_t value) const noexcept;
        void incrementBit() noexcept;

    protected:
        static constexpr uint8_t STEP_MASK = (1 << Step) - 1;

        uint8_t currentBit = 0;
};

template<size_t N, uint8_t Step> requires BitStep<Step>
uint8_t BitBufferBase<N, Step>::clearMask() const noexcept {
    return ~(STEP_MASK << currentBit);
}

template<size_t N, uint8_t Step> requires BitStep<Step>
uint8_t BitBufferBase<N, Step>::writeMask(uint8_t value) const noexcept {
    return (value & STEP_MASK) << currentBit;
}

template<size_t N, uint8_t Step> requires BitStep<Step>
void BitBufferBase<N, Step>::incrementBit() noexcept {
    currentBit = (currentBit + Step) & 7;   // increment by step mod 8
}

template<size_t N, uint8_t Step>
class BitBuffer : public BitBufferBase<N, Step> {
    public:
        void push(uint8_t value);
        std::span<const uint8_t> extract() noexcept;

    private:
        std::array<uint8_t, N> buffer;
        size_t currentByte = 0;
};

template<size_t N, uint8_t Step>
void BitBuffer<N, Step>::push(uint8_t value) {
    buffer.at(currentByte) &= this->clearMask();
    buffer.at(currentByte) |= this->writeMask(value);
    this->incrementBit();
    currentByte += !this->currentBit; // increment byte if we overflow bit count
}

template<size_t N, uint8_t Step>
std::span<const uint8_t> BitBuffer<N, Step>::extract() noexcept {
    auto size = currentByte + std::min(this->currentBit, uint8_t(1));
    auto result = std::span(buffer).subspan(0, size);
    currentByte = 0;
    this->currentBit = 0;
    return result;
}

template<uint8_t Step>
class BitBuffer<1, Step> : public BitBufferBase<1, Step> {
    public:
        void push(uint8_t value) noexcept;
        uint8_t extract() noexcept;

    private:
        uint8_t buffer;
};

template<uint8_t Step>
void BitBuffer<1, Step>::push(uint8_t value) noexcept {
    buffer &= this->clearMask();
    buffer |= this->writeMask(value);
    this->incrementBit();
}

template<uint8_t Step>
uint8_t BitBuffer<1, Step>::extract() noexcept {
    this->currentBit = 0;
    return buffer;
}