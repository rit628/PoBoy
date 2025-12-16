#pragma once
#include <concepts>
#include <cstddef>
#include <cstdint>
#include <type_traits>
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

template<size_t Bits> requires (Bits == 8 || Bits == 16)
class RegisterTag {};

template<size_t Bits> requires (Bits == 8 || Bits == 16)
class RegisterBase : public RegisterTag<Bits> {
    public:
        using value_t = std::conditional_t<Bits == 8, uint8_t, uint16_t>;
        RegisterBase(value_t val) : reg(val) {}

        RegisterBase<Bits>& operator=(value_t val) { reg = val; return *this; }
        RegisterBase<Bits>& operator&=(value_t rhs) { reg &= rhs; return *this; }
        RegisterBase<Bits>& operator|=(value_t rhs) { reg |= rhs; return *this; }

        RegisterBase<Bits>& operator++() { ++reg; return *this; }
        RegisterBase<Bits> operator++(int) { auto temp = *this; reg++; return temp; }
        RegisterBase<Bits>& operator--() { --reg; return *this; }
        RegisterBase<Bits> operator--(int) { auto temp = *this; reg--; return temp; }

        operator value_t() { return reg; }

    protected:
        value_t reg = 0;
};

class RegisterView;

template<typename T, size_t Bits>
concept Register = std::derived_from<T, RegisterTag<Bits>>;

template<size_t Bits>
using RegisterValue = std::conditional_t<Bits == 8, uint8_t, uint16_t>;

using Register8 = RegisterBase<8>;

class Register16 : public RegisterBase<16> {
    public:        
        constexpr RegisterView hi();
        constexpr RegisterView lo();
        void setHi(uint8_t val) {
            reg &= 0x00FF; // mask hi
            reg |= (static_cast<uint16_t>(val) << 8);
        }
        void setLo(uint8_t val) {
            reg &= 0xFF00; // mask lo
            reg |= static_cast<uint16_t>(val);
        }
};

class RegisterView : public RegisterTag<8> {
    public:
        enum class ORDER : bool {
            HI,
            LO
        };

        constexpr RegisterView(Register16& reg, ORDER order) : reg(reg), order(order) {}

        RegisterView& operator=(uint8_t rhs) {
            (order == ORDER::HI) ? reg.setHi(rhs) : reg.setLo(rhs);
            return *this;
        }
        RegisterView& operator&=(uint8_t rhs) {
            *this = (*this & rhs);
            return *this;
        }
        RegisterView& operator|=(uint8_t rhs) {
            *this = (*this | rhs);
            return *this;
        }

        // only implementing for ops with F register
        RegisterView& operator&=(REGISTER_FLAG rhs) { return (*this &= std::to_underlying(rhs)); }
        RegisterView& operator|=(REGISTER_FLAG rhs) { return (*this |= std::to_underlying(rhs)); }
        uint8_t operator&(REGISTER_FLAG rhs) { return (*this & std::to_underlying(rhs)); }
        uint8_t operator|(REGISTER_FLAG rhs) { return (*this | std::to_underlying(rhs)); }

        // cant use r16++ due to hi/lo byte difference
        RegisterView& operator++() { *this = *this + 1; return *this; }
        Register8 operator++(int) { Register8 temp = uint8_t(*this); *this = *this + 1; return temp; } // return anonymous reg8
        RegisterView& operator--() { *this = *this - 1; return *this; }
        Register8 operator--(int) { Register8 temp = uint8_t(*this); *this = *this - 1; return temp; } // return anonymous reg8

        operator uint8_t() { return (order == ORDER::HI) ? ((reg & 0xFF00) >> 8) : (reg & 0x00FF); }

    private:
        Register16& reg;
        const ORDER order;
};

constexpr RegisterView Register16::hi() {
    using ORDER = RegisterView::ORDER;
    return RegisterView(*this, ORDER::HI);
}

constexpr RegisterView Register16::lo() {
    using ORDER = RegisterView::ORDER;
    return RegisterView(*this, ORDER::LO);
}

struct RegisterFile {
    Register16 PC; // program counter
    Register16 SP; // stack pointer

    Register8 IR; // instruction
    Register8 IE; // interrupt enable

    Register16 AF, BC, DE, HL; // general purpose 16bit

    RegisterView A{AF.hi()}; // accumulator
    RegisterView F{AF.lo()}; // flags

    RegisterView B{BC.hi()}, C{BC.lo()}; // general purpose 8bit BC
    RegisterView D{DE.hi()}, E{DE.lo()}; // general purpose 8bit DE
    RegisterView H{HL.hi()}, L{HL.lo()}; // general purpose 8bit HL
};