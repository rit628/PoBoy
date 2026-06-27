#pragma once
#include <algorithm>
#include <array>
#include <cstddef>
#include <format>
#include <istream>
#include <span>
#include <string_view>

template<size_t N>
class StaticString {
    public:
        StaticString() = default;
        StaticString(std::span<char, N> str);
        StaticString(std::span<char> str);

        operator std::string_view() const noexcept;
        bool operator==(std::string_view rhs) const noexcept;
        auto operator<=>(std::string_view rhs) const noexcept;

        char& at(size_t index) noexcept;
        char* data() noexcept;
        std::span<char> substr(size_t position, size_t count = N) noexcept;
        size_t size() const noexcept;
        constexpr size_t capacity() const noexcept;
        void resize(size_t n) noexcept;

    private:
        std::array<char, N> str{};
        size_t end = 0;
};

template<size_t N>
StaticString<N>::StaticString(std::span<char, N> str) {
    std::ranges::copy(str, this->str.begin());
    end = str.size();
}

template<size_t N>
StaticString<N>::StaticString(std::span<char> str) : StaticString<N>(str.first<N>()) {}

template<size_t N>
StaticString<N>::operator std::string_view() const noexcept {
    return std::string_view(std::span(str).first(end));
}

template<size_t N>
bool StaticString<N>::operator==(std::string_view rhs) const noexcept {
    return std::string_view(*this) == rhs;
}

template<size_t N>
auto StaticString<N>::operator<=>(std::string_view rhs) const noexcept {
    return std::string_view(*this) <=> rhs;
}

template<size_t N>
char& StaticString<N>::at(size_t index) noexcept {
    return str[std::min(index, end)];
}

template<size_t N>
char* StaticString<N>::data() noexcept {
    return str.data();
}

template<size_t N>
std::span<char> StaticString<N>::substr(size_t position, size_t count) noexcept {
    return std::span(str).subspan(std::min(position, end), std::min(count, N - position));
}

template<size_t N>
size_t StaticString<N>::size() const noexcept {
    return end;
}

template<size_t N>
constexpr size_t StaticString<N>::capacity() const noexcept {
    return N;
}

template<size_t N>
void StaticString<N>::resize(size_t n) noexcept {
    end = std::min(n, N);
}

namespace std {
    template<size_t N>
    struct formatter<StaticString<N>> : formatter<std::string_view> {
        template<typename FormatContext>
        auto format(const StaticString<N>& str, FormatContext& ctx) const {
            return formatter<std::string_view>::format(str, ctx);
        }
    };
}

template<typename T>
std::istream& readInto(std::istream& is, T& out) {
    is.read(reinterpret_cast<char*>(&out), sizeof(T));
    return is;
}

template<size_t N>
std::istream& readInto(std::istream& is, StaticString<N>& str, size_t bytes = N) {
    str.resize(bytes);
    is.read(str.data(), bytes);
    return is;
}