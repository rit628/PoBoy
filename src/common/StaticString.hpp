#pragma once
#include <cstddef>
#include <format>
#include <boost/static_string/static_string.hpp>

namespace std {
    template<size_t N>
    struct formatter<boost::static_string<N>> : formatter<std::string_view> {
        template<typename FormatContext>
        auto format(const boost::static_string<N>& ss, FormatContext& ctx) const {
            return formatter<std::string_view>::format(std::string_view(ss.data(), ss.size()), ctx);
        }
    };
}

template<typename T>
std::istream& readInto(std::istream& is, T& out) {
    is.read(reinterpret_cast<char*>(&out), sizeof(T));
    return is;
}

template<size_t N>
std::istream& readInto(std::istream& is, boost::static_string<N>& str, size_t bytes = N) {
    str.resize(bytes);
    is.read(str.data(), bytes);
    return is;
}