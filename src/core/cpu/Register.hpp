#pragma once
#include <cstddef>
#include <cstdint>
#include <type_traits>
#include <format>

namespace Processing {

    enum class REGISTER_FLAG : uint8_t {
        Z = 0b10000000,
        N = 0b01000000,
        H = 0b00100000,
        C = 0b00010000
    };
    
    template<size_t Bits> requires (Bits == 8 || Bits == 16)
    class RegisterTag {};
    
    template<size_t Bits> requires (Bits == 8 || Bits == 16)
    class RegisterBase : public RegisterTag<Bits> {
        public:
            using ValueType = std::conditional_t<Bits == 8, uint8_t, uint16_t>;
            RegisterBase(ValueType val = 0) : reg(val) {}
    
            RegisterBase<Bits>& operator=(ValueType val)    { reg = val; return *this; }
            RegisterBase<Bits>& operator+=(ValueType val)   { reg += val; return *this; }
            RegisterBase<Bits>& operator+=(int8_t val)      { reg += val; return *this; }
            RegisterBase<Bits>& operator-=(ValueType val)   { reg -= val; return *this; }
            RegisterBase<Bits>& operator-=(int8_t val)      { reg -= val; return *this; }
            RegisterBase<Bits>& operator&=(ValueType rhs)   { reg &= rhs; return *this; }
            RegisterBase<Bits>& operator|=(ValueType rhs)   { reg |= rhs; return *this; }
            RegisterBase<Bits>& operator^=(ValueType rhs)   { reg ^= rhs; return *this; }
            RegisterBase<Bits>& operator<<=(ValueType rhs)  { reg <<= rhs; return *this; }
            RegisterBase<Bits>& operator>>=(ValueType rhs)  { reg >>= rhs; return *this; }
    
            RegisterBase<Bits>& operator++()    { ++reg; return *this; }
            ValueType operator++(int)  { ValueType temp = *this; reg++; return temp; }
            RegisterBase<Bits>& operator--()    { --reg; return *this; }
            ValueType operator--(int)  { ValueType temp = *this; reg--; return temp; }
    
            operator ValueType() const { return reg; }
    
        protected:
            ValueType reg = 0;
    };

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
            Register16(ValueType value = 0) : RegisterBase<16>(value) { }
            uint8_t getHi() { return reg >> 8; };
            uint8_t getLo() { return reg & 0xFF; };
            void setHi(uint8_t val) {
                reg &= 0x00FF; // mask hi
                reg |= (static_cast<uint16_t>(val) << 8);
            }
            void setLo(uint8_t val) {
                reg &= 0xFF00; // mask lo
                reg |= static_cast<uint16_t>(val);
            }
    };
    
    class RegisterPair : public RegisterTag<16> {
        public:
            constexpr RegisterPair(Register8& hi, Register8& lo) : hi(hi), lo(lo) {}

            uint8_t getHi() { return hi; };
            uint8_t getLo() { return lo; };
            void setHi(uint8_t val) { hi = val; }
            void setLo(uint8_t val) { lo = val; }
    
            RegisterPair& operator=(uint16_t rhs) {
                hi = rhs >> 8;
                lo = rhs & 0xFF;
                return *this;
            }
            
            RegisterPair& operator+=(uint16_t rhs)  { return *this = *this + rhs; }
            RegisterPair& operator-=(uint16_t rhs)  { return *this = *this - rhs; }
            RegisterPair& operator&=(uint16_t rhs)  { return *this = *this & rhs; }
            RegisterPair& operator|=(uint16_t rhs)  { return *this = *this | rhs; }
            RegisterPair& operator^=(uint16_t rhs)  { return *this = *this ^ rhs; }
            RegisterPair& operator<<=(uint16_t rhs) { return *this = *this << rhs; }
            RegisterPair& operator>>=(uint16_t rhs) { return *this = *this >> rhs; }
    
            RegisterPair& operator++()  { return *this = *this + 1; }
            uint16_t operator++(int)    { uint16_t temp = *this; *this = *this + 1; return temp; }
            RegisterPair& operator--()  { return *this = *this - 1; }
            uint16_t operator--(int)    { uint16_t temp = *this; *this = *this - 1; return temp; }
    
            operator uint16_t() const { return hi << 8 | lo; }
    
        private:
            Register8& hi;
            Register8& lo;
    };
    
}

template <Processing::Register<8> T>
struct std::formatter<T> : std::formatter<uint8_t> {
    auto format(const T& reg, std::format_context& ctx) const {
        return std::formatter<uint8_t>::format(reg, ctx);
    }
};

template <Processing::Register<16> T>
struct std::formatter<T> : std::formatter<uint16_t> {
    auto format(const T& reg, std::format_context& ctx) const {
        return std::formatter<uint16_t>::format(reg, ctx);
    }
};