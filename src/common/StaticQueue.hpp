#pragma once
#include <array>
#include <cstddef>
#include <span>

template<size_t N>
concept PowerOfTwo = N > 0 && ((N & (N - 1)) == 0);

/* capacity must be a power of 2 size for fast masks and easy size tracking with integer overflow */
template<typename T, size_t N> requires PowerOfTwo<N>
class StaticQueue {
    public:
        void push(const T& element) noexcept;
        T pop() noexcept;
        T& front() noexcept;
        T& back() noexcept;
        T& at(size_t index) noexcept;
        bool empty() const noexcept;
        bool full() const noexcept;
        size_t size() const noexcept;
        constexpr size_t capacity() const noexcept;
        void clear() noexcept;
        void fill() noexcept;
        std::span<const T> extract() noexcept;

    private:
        size_t mask(const size_t value) const noexcept;

        std::array<T, N> queue{};
        size_t start = 0;
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
    return queue[mask(start++)];
}

template<typename T, size_t N> requires PowerOfTwo<N>
T& StaticQueue<T, N>::front() noexcept {
    return queue[mask(start)];
}

template<typename T, size_t N> requires PowerOfTwo<N>
T& StaticQueue<T, N>::back() noexcept {
    if (size() == 0) return front();
    return queue[mask(end -1)];
}

template<typename T, size_t N> requires PowerOfTwo<N>
T& StaticQueue<T, N>::at(size_t index) noexcept {
    if (index >= size()) return queue[mask(start)];
    return queue[mask(start + index)];
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
    return end - start;
}

template<typename T, size_t N> requires PowerOfTwo<N>
constexpr size_t StaticQueue<T, N>::capacity() const noexcept {
    return N;
}

template<typename T, size_t N> requires PowerOfTwo<N>
void StaticQueue<T, N>::clear() noexcept {
    end = 0;
    start = 0;
}

template<typename T, size_t N> requires PowerOfTwo<N>
void StaticQueue<T, N>::fill() noexcept {
    end = N;
    start = 0;
}

template<typename T, size_t N> requires PowerOfTwo<N>
std::span<const T> StaticQueue<T, N>::extract() noexcept {
    auto result = std::span(queue).subspan(mask(start), size());
    clear();
    return result;
}