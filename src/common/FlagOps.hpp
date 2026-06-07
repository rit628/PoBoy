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

inline void flagSet(Testable auto& target, Flag auto flag) { target |= std::to_underlying(flag); }
inline void flagClear(Testable auto& target, Flag auto flag) { target &= ~std::to_underlying(flag); }
inline bool flagTest(Testable auto target, Flag auto flag) { return target & std::to_underlying(flag); }