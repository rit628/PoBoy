#pragma once
#include <cstdint>

/* some consteval math functions since no compiler fully supports C++23 constexpr cmath yet */

template<typename T>
consteval T cpow(T base, int64_t exp) {
    if (exp < 0) return T(1) / cpow(base, -exp);
    T result = 1;
    while (exp-- > 0) result *= base;
    return result;
}

template<typename T>
consteval int64_t cround(T value) {
    float half = (value >= 0) ? 0.5f : -0.5f;
    return static_cast<int64_t>(value + half);
}