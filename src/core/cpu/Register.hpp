#pragma once
#include <cstddef>
#include <cstdint>
#include <utility>

enum class REGISTER_FLAG : uint8_t {
    Z = 0b0001,
    N = 0b0010,
    H = 0b0100,
    C = 0b1000
};

inline uint8_t operator|(REGISTER_FLAG a, REGISTER_FLAG b) {
    return std::to_underlying(a) | std::to_underlying(b);
}

inline uint8_t operator&(REGISTER_FLAG a, REGISTER_FLAG b) {
    return std::to_underlying(a) & std::to_underlying(b);
}

inline uint8_t operator^(REGISTER_FLAG a, REGISTER_FLAG b) {
    return std::to_underlying(a) ^ std::to_underlying(b);
}

inline uint8_t operator~(REGISTER_FLAG a) {
    return ~std::to_underlying(a);
}

template<size_t Bits, bool Owned = false> requires (Bits == 8 || Bits == 16)
class Register;

template<>
class Register<16> {
    public:
        Register(uint16_t val) : reg(val) {}

        Register<16>& operator=(uint16_t rhs) { reg = rhs; return *this; }
        Register<16>& operator&=(uint16_t rhs) { reg &= rhs; return *this; }
        Register<16>& operator|=(uint16_t rhs) { reg |= rhs; return *this; }
        
        Register<16>& operator++() { ++reg; return *this; }
        Register<16> operator++(int) { auto temp = *this; reg++; return temp; }
        Register<16>& operator--() { --reg; return *this; }
        Register<16> operator--(int) { auto temp = *this; reg--; return temp; }

        operator uint16_t() { return reg; }
        
        constexpr Register<8, true> hi();
        constexpr Register<8, true> lo();
        void setHi(uint8_t val) {
            reg &= 0x00FF; // mask hi
            reg |= (static_cast<uint16_t>(val) << 8);
        }
        void setLo(uint8_t val) {
            reg &= 0xFF00; // mask lo
            reg |= static_cast<uint16_t>(val);
        }

    private:
        uint16_t reg = 0;
};

template<>
class Register<8> {
    public:
        Register(uint8_t val) : reg(val) {}

        Register<8>& operator=(uint8_t val) { reg = val; return *this; }
        Register<8>& operator&=(uint8_t rhs) { reg &= rhs; return *this; }
        Register<8>& operator|=(uint8_t rhs) { reg |= rhs; return *this; }

        Register<8>& operator++() { ++reg; return *this; }
        Register<8> operator++(int) { auto temp = *this; reg++; return temp; }
        Register<8>& operator--() { --reg; return *this; }
        Register<8> operator--(int) { auto temp = *this; reg--; return temp; }

        operator uint8_t() { return reg; }

    private:
        uint8_t reg = 0;
};

template<>
class Register<8, true> {
    public:
        enum class ORDER : bool {
            HI,
            LO
        };

        constexpr Register(Register<16>& reg, ORDER order) : reg(reg), order(order) {}

        Register<8, true>& operator=(uint8_t rhs) {
            (order == ORDER::HI) ? reg.setHi(rhs) : reg.setLo(rhs);
            return *this;
        }
        Register<8, true>& operator&=(uint8_t rhs) {
            uint8_t result = (*this & rhs);
            (order == ORDER::HI) ? reg.setHi(result) : reg.setLo(result);
            return *this;
        }
        Register<8, true>& operator|=(uint8_t rhs) {
            uint8_t result = (*this | rhs);
            (order == ORDER::HI) ? reg.setHi(result) : reg.setLo(result);
            return *this;
        }

        // only implementing for ops with F register
        Register<8, true>& operator&=(REGISTER_FLAG rhs) { return (*this &= std::to_underlying(rhs)); }
        Register<8, true>& operator|=(REGISTER_FLAG rhs) { return (*this |= std::to_underlying(rhs)); }
        uint8_t operator&(REGISTER_FLAG rhs) { return (*this & std::to_underlying(rhs)); }
        uint8_t operator|(REGISTER_FLAG rhs) { return (*this | std::to_underlying(rhs)); }

        Register<8, true>& operator++() { ++reg; return *this; }
        Register<8> operator++(int) { Register<8> temp = uint8_t(*this); reg++; return temp; } // return anonymous reg8
        Register<8, true>& operator--() { --reg; return *this; }
        Register<8> operator--(int) { Register<8> temp = uint8_t(*this); reg--; return temp; } // return anonymous reg8

        operator uint8_t() { return (order == ORDER::HI) ? ((reg & 0xFF00) >> 8) : (reg & 0x00FF); }

    private:
        Register<16>& reg;
        const ORDER order;
};

// defined outside due to template instantiation
constexpr Register<8, true> Register<16>::hi() {
    using ORDER = Register<8, true>::ORDER;
    return Register<8, true>(*this, ORDER::HI);
}

constexpr Register<8, true> Register<16>::lo() {
    using ORDER = Register<8, true>::ORDER;
    return Register<8, true>(*this, ORDER::LO);
}

struct RegisterFile {
    Register<16> PC; // program counter
    Register<16> SP; // stack pointer

    Register<8> IR; // instruction
    Register<8> IE; // interrupt enable

    Register<16> AF, BC, DE, HL; // general purpose 16bit

    Register<8, true> A{AF.hi()}; // accumulator
    Register<8, true> F{AF.lo()}; // flags

    Register<8, true> B{BC.hi()}, C{BC.lo()}; // general purpose 8bit BC
    Register<8, true> D{DE.hi()}, E{DE.lo()}; // general purpose 8bit DE
    Register<8, true> H{HL.hi()}, L{HL.lo()}; // general purpose 8bit HL
};