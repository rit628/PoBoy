#pragma once
#include <array>
#include <cstddef>

template<size_t N>
concept PowerOfTwo = N > 0 && ((N & (N - 1)) == 0);

/* capacity must be a power of 2 size for fast masks and easy size tracking with integer overflow */
template<typename T, size_t N> requires PowerOfTwo<N>
class StaticQueue {
    public:
        void push(const T& element) noexcept;
        T pop() noexcept;
        bool empty() const noexcept;
        bool full() const noexcept;
        size_t size() const noexcept;
        constexpr size_t capacity() const noexcept;
        void clear() noexcept;
        void fill() noexcept;

    private:
        size_t mask(const size_t value) const noexcept;

        std::array<T, N> queue{};
        size_t front = 0;
        size_t end = 0;
};

template<typename T, size_t N> requires PowerOfTwo<N>
size_t StaticQueue<T, N>::mask(const size_t value) const noexcept {
    return value & (N - 1);
}

template<typename T, size_t N> requires PowerOfTwo<N>
void StaticQueue<T, N>::push(const T& element) noexcept {
    if (full()) return;
    queue[mask(end++)] = element;
}

template<typename T, size_t N> requires PowerOfTwo<N>
T StaticQueue<T, N>::pop() noexcept {
    if (empty()) return T();
    return queue[mask(front++)];
}

template<typename T, size_t N> requires PowerOfTwo<N>
bool StaticQueue<T, N>::empty() const noexcept {
    return size() == 0;
}

template<typename T, size_t N> requires PowerOfTwo<N>
bool StaticQueue<T, N>::full() const noexcept {
    return size() == N;
}

template<typename T, size_t N> requires PowerOfTwo<N>
size_t StaticQueue<T, N>::size() const noexcept {
    return end - front;
}

template<typename T, size_t N> requires PowerOfTwo<N>
constexpr size_t StaticQueue<T, N>::capacity() const noexcept {
    return N;
}

template<typename T, size_t N> requires PowerOfTwo<N>
void StaticQueue<T, N>::clear() noexcept {
    end = 0;
    front = 0;
}

template<typename T, size_t N> requires PowerOfTwo<N>
void StaticQueue<T, N>::fill() noexcept {
    end = N;
    front = 0;
}