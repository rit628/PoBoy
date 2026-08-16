#pragma once
#include <array>
#include <cstddef>
#include <cstdint>
#include <span>
#include <type_traits>

template<uint8_t S>
concept BitStep = S == 1 || S == 2 || S == 4 || S == 8 || S == 16;

template<size_t N, uint8_t Step> requires BitStep<Step>
class BitBufferBase {
    public:
        constexpr uint8_t clearMask() const noexcept;
        constexpr uint8_t writeMask(uint8_t value) const noexcept;
        constexpr void incrementBit() noexcept;

    protected:
        static constexpr uint8_t STEP_MASK = (1 << Step) - 1;

        uint8_t currentBit = 0;
};

template<size_t N, uint8_t Step> requires BitStep<Step>
constexpr uint8_t BitBufferBase<N, Step>::clearMask() const noexcept {
    return ~(STEP_MASK << currentBit);
}

template<size_t N, uint8_t Step> requires BitStep<Step>
constexpr uint8_t BitBufferBase<N, Step>::writeMask(uint8_t value) const noexcept {
    return (value & STEP_MASK) << currentBit;
}

template<size_t N, uint8_t Step> requires BitStep<Step>
constexpr void BitBufferBase<N, Step>::incrementBit() noexcept {
    currentBit = (currentBit + Step) & 7;   // increment by step mod 8
}

template<size_t N, uint8_t Step>
class BitBuffer : public BitBufferBase<N, Step> {
    public:
        using ElementType = uint8_t;
        static constexpr uint8_t ELEMENTS_PER_BYTE = 8 / Step;
        static constexpr size_t BYTE_COUNT = N / ELEMENTS_PER_BYTE;

        constexpr void push(ElementType value);
        constexpr std::span<const uint8_t> extract() noexcept;

    private:
        std::array<ElementType, BYTE_COUNT> buffer;
        size_t currentByte = 0;
};

template<size_t N, uint8_t Step>
constexpr void BitBuffer<N, Step>::push(ElementType value) {
    buffer.at(currentByte) &= this->clearMask();
    buffer.at(currentByte) |= this->writeMask(value);
    this->incrementBit();
    currentByte += !this->currentBit; // increment byte if we overflow bit count
}

template<size_t N, uint8_t Step>
constexpr std::span<const uint8_t> BitBuffer<N, Step>::extract() noexcept {
    auto size = currentByte + std::min(this->currentBit, uint8_t(1));
    auto result = std::span(buffer).subspan(0, size);
    currentByte = 0;
    this->currentBit = 0;
    return result;
}

/* regular buffer beyond sub byte step size */
template<size_t N, uint8_t Step> requires (Step >= 8)
class BitBuffer<N, Step> {
    public:
        using ElementType = std::conditional_t<Step == 8, uint8_t, uint16_t>;
        static constexpr uint8_t BYTES_PER_ELEMENT = Step / 8;
        static constexpr size_t BYTE_COUNT = N * BYTES_PER_ELEMENT;

        constexpr void push(ElementType value);
        constexpr std::span<const uint8_t> extract() noexcept;

    private:
        std::array<ElementType, N> buffer;
        size_t index = 0;
};

template<size_t N, uint8_t Step> requires (Step >= 8)
constexpr void BitBuffer<N, Step>::push(ElementType value) {
    buffer.at(index++) = value;
}

template<size_t N, uint8_t Step> requires (Step >= 8)
constexpr std::span<const uint8_t> BitBuffer<N, Step>::extract() noexcept {
    auto rawBytes = std::span(buffer).subspan(0, index);
    index = 0;
    return std::span(reinterpret_cast<const uint8_t*>(rawBytes.data()), rawBytes.size_bytes());
}