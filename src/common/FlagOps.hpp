#pragma once
#include <cstdint>
#include <utility>

namespace {
    template<typename T>
    concept Flag = std::is_enum_v<T> && std::is_same_v<std::underlying_type_t<T>, uint8_t>;

    template<typename T>
    concept Testable = requires (T a, uint8_t b) {
        { a |= b } -> std::same_as<T&>;
        { a &= b } -> std::same_as<T&>;
        { a & b } -> std::convertible_to<uint8_t>;
    };
}

template<typename... T> requires (Flag<T> && ...)
inline void setFlags(Testable auto& target, T... flags) {
    target |= (std::to_underlying(flags) | ...);
}

template<typename... T> requires (Flag<T> && ...)
inline void clearFlags(Testable auto& target, T... flags) {
    target &= (~std::to_underlying(flags) & ...);
}

template<typename... T> requires (Flag<T> && ...)
inline uint8_t extractFlags(Testable auto target, T... flags) {
    return target & (std::to_underlying(flags) | ...);
}

template<typename... T> requires (Flag<T> && ...)
inline bool testFlags(Testable auto target, T... flags) {
    return extractFlags(target, flags...) == (std::to_underlying(flags) | ...);
}