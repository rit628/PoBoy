#pragma once
#include <concepts>
#include <cstddef>
#include <cstdint>
#include <type_traits>
#include <utility>
#include <format>

enum class REGISTER_FLAG : uint8_t {
    Z = 0b10000000,
    N = 0b01000000,
    H = 0b00100000,
    C = 0b00010000
};

enum class INTERRUPT_MASTER_FLAG : uint8_t {
    DISABLED = 0,
    ENABLED = 1,
    ENABLE_PENDING = 2
};

enum class INTERRUPT_BIT : uint8_t {
    JOYPAD      = 0b00010000,
    SERIAL      = 0b00001000,
    TIMER       = 0b00000100,
    LCD_STAT    = 0b00000010,
    VBLANK      = 0b00000001
};

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

void flagSet(Testable auto& target, Flag auto flag) { target |= std::to_underlying(flag); }
void flagClear(Testable auto& target, Flag auto flag) { target &= ~std::to_underlying(flag); }
bool flagTest(Testable auto target, Flag auto flag) { return target & std::to_underlying(flag); }

template<size_t Bits> requires (Bits == 8 || Bits == 16)
class RegisterTag {};

template<size_t Bits> requires (Bits == 8 || Bits == 16)
class RegisterBase : public RegisterTag<Bits> {
    public:
        using value_t = std::conditional_t<Bits == 8, uint8_t, uint16_t>;
        RegisterBase(value_t val = 0) : reg(val) {}

        RegisterBase<Bits>& operator=(value_t val) { reg = val; return *this; }
        RegisterBase<Bits>& operator+=(value_t val) { reg += val; return *this; }
        RegisterBase<Bits>& operator+=(int8_t val) { reg += val; return *this; }
        RegisterBase<Bits>& operator-=(value_t val) { reg -= val; return *this; }
        RegisterBase<Bits>& operator-=(int8_t val) { reg -= val; return *this; }
        RegisterBase<Bits>& operator&=(value_t rhs) { reg &= rhs; return *this; }
        RegisterBase<Bits>& operator|=(value_t rhs) { reg |= rhs; return *this; }
        RegisterBase<Bits>& operator^=(value_t rhs) { reg ^= rhs; return *this; }
        RegisterBase<Bits>& operator<<=(value_t rhs) { reg <<= rhs; return *this; }
        RegisterBase<Bits>& operator>>=(value_t rhs) { reg >>= rhs; return *this; }

        RegisterBase<Bits>& operator++() { ++reg; return *this; }
        RegisterBase<Bits> operator++(int) { auto temp = *this; reg++; return temp; }
        RegisterBase<Bits>& operator--() { --reg; return *this; }
        RegisterBase<Bits> operator--(int) { auto temp = *this; reg--; return temp; }

        operator value_t() const { return reg; }

    protected:
        value_t reg = 0;
};

class RegisterView;

template<typename T, size_t Bits>
concept Register = std::derived_from<T, RegisterTag<Bits>>;

template<size_t Bits>
using RegisterValue = std::conditional_t<Bits == 8, uint8_t, uint16_t>;

/*
    stand in for when function can take Register<N> or RegisterValue<N> 
    typically where implicit conversion from Register<N> to RegisterValue<N>
    is not possible (such as reference contexts)
*/
template<typename T, size_t Bits>
concept Integer = Register<T, Bits> || std::same_as<T, RegisterValue<Bits>>;

using Register8 = RegisterBase<8>;

class Register16 : public RegisterBase<16> {
    public:
        Register16(value_t value = 0) : RegisterBase<16>(value) { }
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
        RegisterView& operator+=(uint8_t rhs) {
            *this = *this + rhs;
            return *this;
        }
        RegisterView& operator-=(uint8_t rhs) {
            *this = *this - rhs;
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
        RegisterView& operator^=(uint8_t rhs) {
            *this = (*this ^ rhs);
            return *this;
        }
        RegisterView& operator<<=(uint8_t rhs) {
            *this = (*this << rhs);
            return *this;
        }
        RegisterView& operator>>=(uint8_t rhs) {
            *this = (*this >> rhs);
            return *this;
        }

        // only implementing for ops with F register
        void set(REGISTER_FLAG bitFlag) { flagSet(*this, bitFlag); }
        void clear(REGISTER_FLAG bitFlag) { flagClear(*this, bitFlag); }
        bool test(REGISTER_FLAG bitFlag) { return flagTest(*this, bitFlag); }

        // cant use r16++ due to hi/lo byte difference
        RegisterView& operator++() { *this = *this + 1; return *this; }
        Register8 operator++(int) { Register8 temp = uint8_t(*this); *this = *this + 1; return temp; } // return anonymous reg8
        RegisterView& operator--() { *this = *this - 1; return *this; }
        Register8 operator--(int) { Register8 temp = uint8_t(*this); *this = *this - 1; return temp; } // return anonymous reg8

        operator uint8_t() const { return (order == ORDER::HI) ? ((reg & 0xFF00) >> 8) : (reg & 0x00FF); }

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

template <Register<8> T>
struct std::formatter<T> : std::formatter<uint8_t> {
    auto format(const T& reg, std::format_context& ctx) const {
        return std::formatter<uint8_t>::format(reg, ctx);
    }
};

template <Register<16> T>
struct std::formatter<T> : std::formatter<uint16_t> {
    auto format(const T& reg, std::format_context& ctx) const {
        return std::formatter<uint16_t>::format(reg, ctx);
    }
};