#pragma once
#include <array>
#include <bit>
#include <cstddef>
#include <span>

/* internal capacity is a power of 2 for fast masks and easy size tracking with integer overflow */
template<typename T, size_t N>
class StaticQueue {
    public:
        constexpr void push(const T& element) noexcept;
        constexpr T pop() noexcept;
        constexpr T& front() noexcept;
        constexpr T& back() noexcept;
        constexpr T& at(size_t index) noexcept;
        constexpr bool empty() const noexcept;
        constexpr bool full() const noexcept;
        constexpr size_t size() const noexcept;
        constexpr size_t capacity() const noexcept;
        constexpr void clear() noexcept;
        constexpr void fill() noexcept;
        constexpr std::span<T> data() noexcept;
        constexpr std::span<const T> extract() noexcept;

    private:
        constexpr size_t mask(const size_t value) const noexcept;

        static constexpr size_t BUFFER_SIZE = std::bit_ceil(N);
        static constexpr size_t INDEX_MASK = BUFFER_SIZE - 1;

        std::array<T, BUFFER_SIZE> queue{};
        size_t start = 0;
        size_t end = 0;
};

template<typename T, size_t N>
constexpr size_t StaticQueue<T, N>::mask(const size_t value) const noexcept {
    [[ assume((value & INDEX_MASK) < N) ]];
    return value & INDEX_MASK;
}

template<typename T, size_t N>
constexpr void StaticQueue<T, N>::push(const T& element) noexcept {
    if (full()) return;
    queue[mask(end++)] = element;
}

template<typename T, size_t N>
constexpr T StaticQueue<T, N>::pop() noexcept {
    if (empty()) return T();
    return queue[mask(start++)];
}

template<typename T, size_t N>
constexpr T& StaticQueue<T, N>::front() noexcept {
    return queue[mask(start)];
}

template<typename T, size_t N>
constexpr T& StaticQueue<T, N>::back() noexcept {
    if (size() == 0) return front();
    return queue[mask(end - 1)];
}

template<typename T, size_t N>
constexpr T& StaticQueue<T, N>::at(size_t index) noexcept {
    if (index >= size()) return queue[mask(start)];
    return queue[mask(start + index)];
}

template<typename T, size_t N>
constexpr bool StaticQueue<T, N>::empty() const noexcept {
    return size() == 0;
}

template<typename T, size_t N>
constexpr bool StaticQueue<T, N>::full() const noexcept {
    return size() == N;
}

template<typename T, size_t N>
constexpr size_t StaticQueue<T, N>::size() const noexcept {
    [[ assume(end - start <= N) ]];
    return end - start;
}

template<typename T, size_t N>
constexpr size_t StaticQueue<T, N>::capacity() const noexcept {
    return N;
}

template<typename T, size_t N>
constexpr void StaticQueue<T, N>::clear() noexcept {
    end = 0;
    start = 0;
}

template<typename T, size_t N>
constexpr void StaticQueue<T, N>::fill() noexcept {
    end = N;
    start = 0;
}

template<typename T, size_t N>
constexpr std::span<T> StaticQueue<T, N>::data() noexcept {
    return std::span(queue).subspan(mask(start), size());
}

template<typename T, size_t N>
constexpr std::span<const T> StaticQueue<T, N>::extract() noexcept {
    auto result = data();
    clear();
    return result;
}