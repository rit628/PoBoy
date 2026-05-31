#include "CPU.hpp"
#include "Register.hpp"
#include <cstdint>
#include <stdexcept>

bool CPU::NOP_0x00(int) {
    return true;
}

bool CPU::LD_0x01(Register16& BC, uint16_t n16, int) {
    load<16>(BC, n16);
    return true;
}

bool CPU::LD_0x02(Register16& BC, RegisterView& A, int) {
    loadIndirect(BC, A);
    return true;
}

bool CPU::INC_0x03(Register16& BC, int) {
    increment(BC);
    return true;
}

bool CPU::INC_0x04(RegisterView& B, int) {
    increment(B);
    return true;
}

bool CPU::DEC_0x05(RegisterView& B, int) {
    decrement(B);
    return true;
}

bool CPU::LD_0x06(RegisterView& B, uint8_t n8, int) {
    load<8>(B, n8);
    return true;
}

bool CPU::RLCA_0x07(int) {
    rotateLeftCircular(A);
    using enum REGISTER_FLAG;
    F.clear(Z);
    F.clear(N);
    F.clear(H);
    return true;
}

bool CPU::LD_0x08(uint16_t a16, Register16& SP, int) {
    loadIndirect(a16, SP); // might need specific impl due to sp being 2 bytes wide
    return true;
}

bool CPU::ADD_0x09(Register16& HL, Register16& BC, int) {
    add<16>(HL, BC);
    return true;
}

bool CPU::LD_0x0A(RegisterView& A, Register16& BC, int) {
    loadIndirect(A, BC);
    return true;
}

bool CPU::DEC_0x0B(Register16& BC, int) {
    decrement(BC);
    return true;
}

bool CPU::INC_0x0C(RegisterView& C, int) {
    increment(C);
    return true;
}

bool CPU::DEC_0x0D(RegisterView& C, int) {
    decrement(C);
    return true;
}

bool CPU::LD_0x0E(RegisterView& C, uint8_t n8, int) {
    load<8>(C, n8);
    return true;
}

bool CPU::RRCA_0x0F(int) {
    rotateRightCircular(A);
    using enum REGISTER_FLAG;
    F.clear(Z);
    F.clear(N);
    F.clear(H);
    return true;
}

bool CPU::STOP_0x10(uint8_t n8 [[ maybe_unused ]], int) {
    stop();
    return true;
}

bool CPU::LD_0x11(Register16& DE, uint16_t n16, int) {
    load<16>(DE, n16);
    return true;
}

bool CPU::LD_0x12(Register16& DE, RegisterView& A, int) {
    loadIndirect(DE, A);
    return true;
}

bool CPU::INC_0x13(Register16& DE, int) {
    increment(DE);
    return true;
}

bool CPU::INC_0x14(RegisterView& D, int) {
    increment(D);
    return true;
}

bool CPU::DEC_0x15(RegisterView& D, int) {
    decrement(D);
    return true;
}

bool CPU::LD_0x16(RegisterView& D, uint8_t n8, int) {
    load<8>(D, n8);
    return true;
}

bool CPU::RLA_0x17(int) {
    rotateLeft(A);
    using enum REGISTER_FLAG;
    F.clear(Z);
    F.clear(N);
    F.clear(H);
    return true;
}

bool CPU::JR_0x18(int8_t e8, int) {
    jumpRelative(e8);
    return true;
}

bool CPU::ADD_0x19(Register16& HL, Register16& DE, int) {
    add<16>(HL, DE);
    return true;
}

bool CPU::LD_0x1A(RegisterView& A, Register16& DE, int) {
    loadIndirect(A, DE);
    return true;
}

bool CPU::DEC_0x1B(Register16& DE, int) {
    decrement(DE);
    return true;
}

bool CPU::INC_0x1C(RegisterView& E, int) {
    increment(E);
    return true;
}

bool CPU::DEC_0x1D(RegisterView& E, int) {
    decrement(E);
    return true;
}

bool CPU::LD_0x1E(RegisterView& E, uint8_t n8, int) {
    load<8>(E, n8);
    return true;
}

bool CPU::RRA_0x1F(int) {
    rotateRight(A);
    using enum REGISTER_FLAG;
    F.clear(Z);
    F.clear(N);
    F.clear(H);
    return true;
}

bool CPU::JR_0x20(RegisterView& F_NZ, int8_t e8, int) {
    if (!F_NZ.test(REGISTER_FLAG::Z)) {
        jumpRelative(e8);
        return true;
    }
    return false;
}

bool CPU::LD_0x21(Register16& HL, uint16_t n16, int) {
    load<16>(HL, n16);
    return true;
}

bool CPU::LD_0x22(Register16& HL, RegisterView& A, int) {
    loadIncrement(HL, A);
    return true;
}

bool CPU::INC_0x23(Register16& HL, int) {
    increment(HL);
    return true;
}

bool CPU::INC_0x24(RegisterView& H, int) {
    increment(H);
    return true;
}

bool CPU::DEC_0x25(RegisterView& H, int) {
    decrement(H);
    return true;
}

bool CPU::LD_0x26(RegisterView& H, uint8_t n8, int) {
    load<8>(H, n8);
    return true;
}

bool CPU::DAA_0x27(int) {
    decimalAdjustAccumulator();
    return true;
}

bool CPU::JR_0x28(RegisterView& F_Z, int8_t e8, int) {
    if (F_Z.test(REGISTER_FLAG::Z)) {
        jumpRelative(e8);
        return true;
    }
    return false;
}

bool CPU::ADD_0x29(Register16& HL_1, Register16& HL_2, int) {
    add<16>(HL_1, HL_2);
    return true;
}

bool CPU::LD_0x2A(RegisterView& A, Register16& HL, int) {
    loadIncrement(A, HL);
    return true;
}

bool CPU::DEC_0x2B(Register16& HL, int) {
    decrement(HL);
    return true;
}

bool CPU::INC_0x2C(RegisterView& L, int) {
    increment(L);
    return true;
}

bool CPU::DEC_0x2D(RegisterView& L, int) {
    decrement(L);
    return true;
}

bool CPU::LD_0x2E(RegisterView& L, uint8_t n8, int) {
    load<8>(L, n8);
    return true;
}

bool CPU::CPL_0x2F(int) {
    bitNot(A);
    return true;
}

bool CPU::JR_0x30(RegisterView& F_NC, int8_t e8, int) {
    if (!F_NC.test(REGISTER_FLAG::C)) {
        jumpRelative(e8);
        return true;
    }
    return false;
}

bool CPU::LD_0x31(Register16& SP, uint16_t n16, int) {
    load<16>(SP, n16);
    return true;
}

bool CPU::LD_0x32(Register16& HL, RegisterView& A, int) {
    loadDecrement(HL, A);
    return true;
}

bool CPU::INC_0x33(Register16& SP, int) {
    increment(SP);
    return true;
}

bool CPU::INC_0x34(Register16& HL, int) {
    incrementIndirect(HL);
    return true;
}

bool CPU::DEC_0x35(Register16& HL, int) {
    decrementIndirect(HL);
    return true;
}

bool CPU::LD_0x36(Register16& HL, uint8_t n8, int) {
    loadIndirect(HL, n8);
    return true;
}

bool CPU::SCF_0x37(int) {
    setCarryFlag();
    return true;
}

bool CPU::JR_0x38(RegisterView& F_C, int8_t e8, int) {
    if (F_C.test(REGISTER_FLAG::C)) {
        jumpRelative(e8);
        return true;
    }
    return false;
}

bool CPU::ADD_0x39(Register16& HL, Register16& SP, int) {
    add<16>(HL, SP);
    return true;
}

bool CPU::LD_0x3A(RegisterView& A, Register16& HL, int) {
    loadDecrement(A, HL);
    return true;
}

bool CPU::DEC_0x3B(Register16& SP, int) {
    decrement(SP);
    return true;
}

bool CPU::INC_0x3C(RegisterView& A, int) {
    increment(A);
    return true;
}

bool CPU::DEC_0x3D(RegisterView& A, int) {
    decrement(A);
    return true;
}

bool CPU::LD_0x3E(RegisterView& A, uint8_t n8, int) {
    load<8>(A, n8);
    return true;
}

bool CPU::CCF_0x3F(int) {
    complementCarryFlag();
    return true;
}

bool CPU::LD_0x40(RegisterView& B_1, RegisterView& B_2, int) {
    load<8>(B_1, B_2);
    return true;
}

bool CPU::LD_0x41(RegisterView& B, RegisterView& C, int) {
    load<8>(B, C);
    return true;
}

bool CPU::LD_0x42(RegisterView& B, RegisterView& D, int) {
    load<8>(B, D);
    return true;
}

bool CPU::LD_0x43(RegisterView& B, RegisterView& E, int) {
    load<8>(B, E);
    return true;
}

bool CPU::LD_0x44(RegisterView& B, RegisterView& H, int) {
    load<8>(B, H);
    return true;
}

bool CPU::LD_0x45(RegisterView& B, RegisterView& L, int) {
    load<8>(B, L);
    return true;
}

bool CPU::LD_0x46(RegisterView& B, Register16& HL, int) {
    loadIndirect(B, HL);
    return true;
}

bool CPU::LD_0x47(RegisterView& B, RegisterView& A, int) {
    load<8>(B, A);
    return true;
}

bool CPU::LD_0x48(RegisterView& C, RegisterView& B, int) {
    load<8>(C, B);
    return true;
}

bool CPU::LD_0x49(RegisterView& C, RegisterView& C2, int) {
    load<8>(C, C2);
    return true;
}

bool CPU::LD_0x4A(RegisterView& C, RegisterView& D, int) {
    load<8>(C, D);
    return true;
}

bool CPU::LD_0x4B(RegisterView& C, RegisterView& E, int) {
    load<8>(C, E);
    return true;
}

bool CPU::LD_0x4C(RegisterView& C, RegisterView& H, int) {
    load<8>(C, H);
    return true;
}

bool CPU::LD_0x4D(RegisterView& C, RegisterView& L, int) {
    load<8>(C, L);
    return true;
}

bool CPU::LD_0x4E(RegisterView& C, Register16& HL, int) {
    loadIndirect(C, HL);
    return true;
}

bool CPU::LD_0x4F(RegisterView& C, RegisterView& A, int) {
    load<8>(C, A);
    return true;
}

bool CPU::LD_0x50(RegisterView& D, RegisterView& B, int) {
    load<8>(D, B);
    return true;
}

bool CPU::LD_0x51(RegisterView& D, RegisterView& C, int) {
    load<8>(D, C);
    return true;
}

bool CPU::LD_0x52(RegisterView& D, RegisterView& D2, int) {
    load<8>(D, D2);
    return true;
}

bool CPU::LD_0x53(RegisterView& D, RegisterView& E, int) {
    load<8>(D, E);
    return true;
}

bool CPU::LD_0x54(RegisterView& D, RegisterView& H, int) {
    load<8>(D, H);
    return true;
}

bool CPU::LD_0x55(RegisterView& D, RegisterView& L, int) {
    load<8>(D, L);
    return true;
}

bool CPU::LD_0x56(RegisterView& D, Register16& HL, int) {
    loadIndirect(D, HL);
    return true;
}

bool CPU::LD_0x57(RegisterView& D, RegisterView& A, int) {
    load<8>(D, A);
    return true;
}

bool CPU::LD_0x58(RegisterView& E, RegisterView& B, int) {
    load<8>(E, B);
    return true;
}

bool CPU::LD_0x59(RegisterView& E, RegisterView& C, int) {
    load<8>(E, C);
    return true;
}

bool CPU::LD_0x5A(RegisterView& E, RegisterView& D, int) {
    load<8>(E, D);
    return true;
}

bool CPU::LD_0x5B(RegisterView& E, RegisterView& E2, int) {
    load<8>(E, E2);
    return true;
}

bool CPU::LD_0x5C(RegisterView& E, RegisterView& H, int) {
    load<8>(E, H);
    return true;
}

bool CPU::LD_0x5D(RegisterView& E, RegisterView& L, int) {
    load<8>(E, L);
    return true;
}

bool CPU::LD_0x5E(RegisterView& E, Register16& HL, int) {
    loadIndirect(E, HL);
    return true;
}

bool CPU::LD_0x5F(RegisterView& E, RegisterView& A, int) {
    load<8>(E, A);
    return true;
}

bool CPU::LD_0x60(RegisterView& H, RegisterView& B, int) {
    load<8>(H, B);
    return true;
}

bool CPU::LD_0x61(RegisterView& H, RegisterView& C, int) {
    load<8>(H, C);
    return true;
}

bool CPU::LD_0x62(RegisterView& H, RegisterView& D, int) {
    load<8>(H, D);
    return true;
}

bool CPU::LD_0x63(RegisterView& H, RegisterView& E, int) {
    load<8>(H, E);
    return true;
}

bool CPU::LD_0x64(RegisterView& H, RegisterView& H2, int) {
    load<8>(H, H2);
    return true;
}

bool CPU::LD_0x65(RegisterView& H, RegisterView& L, int) {
    load<8>(H, L);
    return true;
}

bool CPU::LD_0x66(RegisterView& H, Register16& HL, int) {
    loadIndirect(H, HL);
    return true;
}

bool CPU::LD_0x67(RegisterView& H, RegisterView& A, int) {
    load<8>(H, A);
    return true;
}

bool CPU::LD_0x68(RegisterView& L, RegisterView& B, int) {
    load<8>(L, B);
    return true;
}

bool CPU::LD_0x69(RegisterView& L, RegisterView& C, int) {
    load<8>(L, C);
    return true;
}

bool CPU::LD_0x6A(RegisterView& L, RegisterView& D, int) {
    load<8>(L, D);
    return true;
}

bool CPU::LD_0x6B(RegisterView& L, RegisterView& E, int) {
    load<8>(L, E);
    return true;
}

bool CPU::LD_0x6C(RegisterView& L, RegisterView& H, int) {
    load<8>(L, H);
    return true;
}

bool CPU::LD_0x6D(RegisterView& L, RegisterView& L2, int) {
    load<8>(L, L2);
    return true;
}

bool CPU::LD_0x6E(RegisterView& L, Register16& HL, int) {
    loadIndirect(L, HL);
    return true;
}

bool CPU::LD_0x6F(RegisterView& L, RegisterView& A, int) {
    load<8>(L, A);
    return true;
}

bool CPU::LD_0x70(Register16& HL, RegisterView& B, int) {
    loadIndirect(HL, B);
    return true;
}

bool CPU::LD_0x71(Register16& HL, RegisterView& C, int) {
    loadIndirect(HL, C);
    return true;
}

bool CPU::LD_0x72(Register16& HL, RegisterView& D, int) {
    loadIndirect(HL, D);
    return true;
}

bool CPU::LD_0x73(Register16& HL, RegisterView& E, int) {
    loadIndirect(HL, E);
    return true;
}

bool CPU::LD_0x74(Register16& HL, RegisterView& H, int) {
    loadIndirect(HL, H);
    return true;
}

bool CPU::LD_0x75(Register16& HL, RegisterView& L, int) {
    loadIndirect(HL, L);
    return true;
}

bool CPU::HALT_0x76(int) {
    halt();
    return true;
}

bool CPU::LD_0x77(Register16& HL, RegisterView& A, int) {
    loadIndirect(HL, A);
    return true;
}

bool CPU::LD_0x78(RegisterView& A, RegisterView& B, int) {
    load<8>(A, B);
    return true;
}

bool CPU::LD_0x79(RegisterView& A, RegisterView& C, int) {
    load<8>(A, C);
    return true;
}

bool CPU::LD_0x7A(RegisterView& A, RegisterView& D, int) {
    load<8>(A, D);
    return true;
}

bool CPU::LD_0x7B(RegisterView& A, RegisterView& E, int) {
    load<8>(A, E);
    return true;
}

bool CPU::LD_0x7C(RegisterView& A, RegisterView& H, int) {
    load<8>(A, H);
    return true;
}

bool CPU::LD_0x7D(RegisterView& A, RegisterView& L, int) {
    load<8>(A, L);
    return true;
}

bool CPU::LD_0x7E(RegisterView& A, Register16& HL, int) {
    loadIndirect(A, HL);
    return true;
}

bool CPU::LD_0x7F(RegisterView& A, RegisterView& A2, int) {
    load<8>(A, A2);
    return true;
}

bool CPU::ADD_0x80(RegisterView& A, RegisterView& B, int) {
    add<8>(A, B);
    return true;
}

bool CPU::ADD_0x81(RegisterView& A, RegisterView& C, int) {
    add<8>(A, C);
    return true;
}

bool CPU::ADD_0x82(RegisterView& A, RegisterView& D, int) {
    add<8>(A, D);
    return true;
}

bool CPU::ADD_0x83(RegisterView& A, RegisterView& E, int) {
    add<8>(A, E);
    return true;
}

bool CPU::ADD_0x84(RegisterView& A, RegisterView& H, int) {
    add<8>(A, H);
    return true;
}

bool CPU::ADD_0x85(RegisterView& A, RegisterView& L, int) {
    add<8>(A, L);
    return true;
}

bool CPU::ADD_0x86(RegisterView& A, Register16& HL, int) {
    addIndirect(A, HL);
    return true;
}

bool CPU::ADD_0x87(RegisterView& A, RegisterView& A2, int) {
    add<8>(A, A2);
    return true;
}

bool CPU::ADC_0x88(RegisterView& A, RegisterView& B, int) {
    adc(A, B);
    return true;
}

bool CPU::ADC_0x89(RegisterView& A, RegisterView& C, int) {
    adc(A, C);
    return true;
}

bool CPU::ADC_0x8A(RegisterView& A, RegisterView& D, int) {
    adc(A, D);
    return true;
}

bool CPU::ADC_0x8B(RegisterView& A, RegisterView& E, int) {
    adc(A, E);
    return true;
}

bool CPU::ADC_0x8C(RegisterView& A, RegisterView& H, int) {
    adc(A, H);
    return true;
}

bool CPU::ADC_0x8D(RegisterView& A, RegisterView& L, int) {
    adc(A, L);
    return true;
}

bool CPU::ADC_0x8E(RegisterView& A, Register16& HL, int) {
    adcIndirect(A, HL);
    return true;
}

bool CPU::ADC_0x8F(RegisterView& A, RegisterView& A2, int) {
    adc(A, A2);
    return true;
}

bool CPU::SUB_0x90(RegisterView& A, RegisterView& B, int) {
    sub(A, B);
    return true;
}

bool CPU::SUB_0x91(RegisterView& A, RegisterView& C, int) {
    sub(A, C);
    return true;
}

bool CPU::SUB_0x92(RegisterView& A, RegisterView& D, int) {
    sub(A, D);
    return true;
}

bool CPU::SUB_0x93(RegisterView& A, RegisterView& E, int) {
    sub(A, E);
    return true;
}

bool CPU::SUB_0x94(RegisterView& A, RegisterView& H, int) {
    sub(A, H);
    return true;
}

bool CPU::SUB_0x95(RegisterView& A, RegisterView& L, int) {
    sub(A, L);
    return true;
}

bool CPU::SUB_0x96(RegisterView& A, Register16& HL, int) {
    subIndirect(A, HL);
    return true;
}

bool CPU::SUB_0x97(RegisterView& A, RegisterView& A2, int) {
    sub(A, A2);
    return true;
}

bool CPU::SBC_0x98(RegisterView& A, RegisterView& B, int) {
    sbc(A, B);
    return true;
}

bool CPU::SBC_0x99(RegisterView& A, RegisterView& C, int) {
    sbc(A, C);
    return true;
}

bool CPU::SBC_0x9A(RegisterView& A, RegisterView& D, int) {
    sbc(A, D);
    return true;
}

bool CPU::SBC_0x9B(RegisterView& A, RegisterView& E, int) {
    sbc(A, E);
    return true;
}

bool CPU::SBC_0x9C(RegisterView& A, RegisterView& H, int) {
    sbc(A, H);
    return true;
}

bool CPU::SBC_0x9D(RegisterView& A, RegisterView& L, int) {
    sbc(A, L);
    return true;
}

bool CPU::SBC_0x9E(RegisterView& A, Register16& HL, int) {
    sbcIndirect(A, HL);
    return true;
}

bool CPU::SBC_0x9F(RegisterView& A, RegisterView& A2, int) {
    sbc(A, A2);
    return true;
}

bool CPU::AND_0xA0(RegisterView& A, RegisterView& B, int) {
    bitAnd(A, B);
    return true;
}

bool CPU::AND_0xA1(RegisterView& A, RegisterView& C, int) {
    bitAnd(A, C);
    return true;
}

bool CPU::AND_0xA2(RegisterView& A, RegisterView& D, int) {
    bitAnd(A, D);
    return true;
}

bool CPU::AND_0xA3(RegisterView& A, RegisterView& E, int) {
    bitAnd(A, E);
    return true;
}

bool CPU::AND_0xA4(RegisterView& A, RegisterView& H, int) {
    bitAnd(A, H);
    return true;
}

bool CPU::AND_0xA5(RegisterView& A, RegisterView& L, int) {
    bitAnd(A, L);
    return true;
}

bool CPU::AND_0xA6(RegisterView& A, Register16& HL, int) {
    bitAndIndirect(A, HL);
    return true;
}

bool CPU::AND_0xA7(RegisterView& A, RegisterView& A2, int) {
    bitAnd(A, A2);
    return true;
}

bool CPU::XOR_0xA8(RegisterView& A, RegisterView& B, int) {
    bitXor(A, B);
    return true;
}


bool CPU::XOR_0xA9(RegisterView& A, RegisterView& C, int) {
    bitXor(A, C);
    return true;
}


bool CPU::XOR_0xAA(RegisterView& A, RegisterView& D, int) {
    bitXor(A, D);
    return true;
}


bool CPU::XOR_0xAB(RegisterView& A, RegisterView& E, int) {
    bitXor(A, E);
    return true;
}


bool CPU::XOR_0xAC(RegisterView& A, RegisterView& H, int) {
    bitXor(A, H);
    return true;
}


bool CPU::XOR_0xAD(RegisterView& A, RegisterView& L, int) {
    bitXor(A, L);
    return true;
}


bool CPU::XOR_0xAE(RegisterView& A, Register16& HL, int) {
    bitXorIndirect(A, HL);
    return true;
}

bool CPU::XOR_0xAF(RegisterView& A, RegisterView& A2, int) {
    bitXor(A, A2);
    return true;
}

bool CPU::OR_0xB0(RegisterView& A, RegisterView& B, int) {
    bitOr(A, B);
    return true;
}

bool CPU::OR_0xB1(RegisterView& A, RegisterView& C, int) {
    bitOr(A, C);
    return true;
}

bool CPU::OR_0xB2(RegisterView& A, RegisterView& D, int) {
    bitOr(A, D);
    return true;
}

bool CPU::OR_0xB3(RegisterView& A, RegisterView& E, int) {
    bitOr(A, E);
    return true;
}

bool CPU::OR_0xB4(RegisterView& A, RegisterView& H, int) {
    bitOr(A, H);
    return true;
}

bool CPU::OR_0xB5(RegisterView& A, RegisterView& L, int) {
    bitOr(A, L);
    return true;
}

bool CPU::OR_0xB6(RegisterView& A, Register16& HL, int) {
    bitOrIndirect(A, HL);
    return true;
}

bool CPU::OR_0xB7(RegisterView& A, RegisterView& A2, int) {
    bitOr(A, A2);
    return true;
}

bool CPU::CP_0xB8(RegisterView& A, RegisterView& B, int) {
    compare(A, B);
    return true;
}

bool CPU::CP_0xB9(RegisterView& A, RegisterView& C, int) {
    compare(A, C);
    return true;
}

bool CPU::CP_0xBA(RegisterView& A, RegisterView& D, int) {
    compare(A, D);
    return true;
}

bool CPU::CP_0xBB(RegisterView& A, RegisterView& E, int) {
    compare(A, E);
    return true;
}

bool CPU::CP_0xBC(RegisterView& A, RegisterView& H, int) {
    compare(A, H);
    return true;
}

bool CPU::CP_0xBD(RegisterView& A, RegisterView& L, int) {
    compare(A, L);
    return true;
}

bool CPU::CP_0xBE(RegisterView& A, Register16& HL, int) {
    compareIndirect(A, HL);
    return true;
}

bool CPU::CP_0xBF(RegisterView& A, RegisterView& A2, int) {
    compare(A, A2);
    return true;
}

bool CPU::RET_0xC0(RegisterView& F_NZ, int) {
    if (!F_NZ.test(REGISTER_FLAG::Z)) {
        ret();
        return true;
    }
    return false;
}

bool CPU::POP_0xC1(Register16& BC, int) {
    pop(BC);
    return true;
}

bool CPU::JP_0xC2(RegisterView& F_NZ, uint16_t a16, int) {
    if (!F_NZ.test(REGISTER_FLAG::Z)) {
        jump(a16);
        return true;
    }
    return false;
}

bool CPU::JP_0xC3(uint16_t a16, int) {
    jump(a16);
    return true;
}

bool CPU::CALL_0xC4(RegisterView& F_NZ, uint16_t a16, int) {
    if (!F_NZ.test(REGISTER_FLAG::Z)) {
        call(a16);
        return true;
    }
    return false;
}

bool CPU::PUSH_0xC5(Register16& BC, int) {
    push(BC);
    return true;
}

bool CPU::ADD_0xC6(RegisterView& A, uint8_t n8, int) {
    add<8>(A, n8);
    return true;
}

bool CPU::RST_0xC7(uint8_t $0x00, int) {
    restart($0x00);
    return true;
}

bool CPU::RET_0xC8(RegisterView& F_Z, int) {
    if (F_Z.test(REGISTER_FLAG::Z)) {
        ret();
        return true;
    }
    return false;
}

bool CPU::RET_0xC9(int) {
    ret();
    return true;
}

bool CPU::JP_0xCA(RegisterView& F_Z, uint16_t a16, int) {
    if (F_Z.test(REGISTER_FLAG::Z)) {
        jump(a16);
        return true;
    }
    return false;
}

bool CPU::PREFIX_0xCB(int) {
    readPrefixed = true;
    return true;
}

bool CPU::CALL_0xCC(RegisterView& F_Z, uint16_t a16, int) {
    if (F_Z.test(REGISTER_FLAG::Z)) {
        call(a16);
        return true;
    }
    return false;
}

bool CPU::CALL_0xCD(uint16_t address, int) {
    call(address);
    return true;
}

bool CPU::ADC_0xCE(RegisterView& A, uint8_t n8, int) {
    adc(A, n8);
    return true;
}

bool CPU::RST_0xCF(uint8_t $0x08, int) {
    restart($0x08);
    return true;
}

bool CPU::RET_0xD0(RegisterView& F_NC, int) {
    if (!F_NC.test(REGISTER_FLAG::C)) {
        ret();
        return true;
    }
    return false;
}

bool CPU::POP_0xD1(Register16& DE, int) {
    pop(DE);
    return true;
}

bool CPU::JP_0xD2(RegisterView& F_NC, uint16_t a16, int) {
    if (!F_NC.test(REGISTER_FLAG::C)) {
        jump(a16);
        return true;
    }
    return false;
}

bool CPU::ILLEGAL_D3_0xD3(int) {
    throw std::runtime_error("ILLEGAL OPCODE D3");
}

bool CPU::CALL_0xD4(RegisterView& F_NC, uint16_t a16, int) {
    if (!F_NC.test(REGISTER_FLAG::C)) {
        call(a16);
        return true;
    }
    return false;
}

bool CPU::PUSH_0xD5(Register16& DE, int) {
    push(DE);
    return true;
}

bool CPU::SUB_0xD6(RegisterView& A, uint8_t n8, int) {
    sub(A, n8);
    return true;
}

bool CPU::RST_0xD7(uint8_t $0x10, int) {
    restart($0x10);
    return true;
}

bool CPU::RET_0xD8(RegisterView& F_C, int) {
    if (F_C.test(REGISTER_FLAG::C)) {
        ret();
        return true;
    }
    return false;
}

bool CPU::RETI_0xD9(int) {
    reti();
    return true;
}

bool CPU::JP_0xDA(RegisterView& F_C, uint16_t a16, int) {
    if (F_C.test(REGISTER_FLAG::C)) {
        jump(a16);
        return true;
    }
    return false;
}

bool CPU::ILLEGAL_DB_0xDB(int) {
    throw std::runtime_error("ILLEGAL OPCODE DB");
}

bool CPU::CALL_0xDC(RegisterView& F_C, uint16_t a16, int) {
    if (F_C.test(REGISTER_FLAG::C)) {
        call(a16);
        return true;
    }
    return false;
}

bool CPU::ILLEGAL_DD_0xDD(int) {
    throw std::runtime_error("ILLEGAL OPCODE DD");
}

bool CPU::SBC_0xDE(RegisterView& A, uint8_t n8, int) {
    sbc(A, n8);
    return true;
}

bool CPU::RST_0xDF(uint8_t $0x18, int) {
    restart($0x18);
    return true;
}

bool CPU::LDH_0xE0(uint8_t a8, RegisterView& A, int) {
    loadHiIndirect(a8, A);
    return true;
}

bool CPU::POP_0xE1(Register16& HL, int) {
    pop(HL);
    return true;
}

bool CPU::LDH_0xE2(RegisterView& C, RegisterView& A, int) {
    loadHiIndirect(static_cast<uint16_t>(C), A);
    return true;
}

bool CPU::ILLEGAL_E3_0xE3(int) {
    throw std::runtime_error("ILLEGAL OPCODE E3");
}

bool CPU::ILLEGAL_E4_0xE4(int) {
    throw std::runtime_error("ILLEGAL OPCODE E4");
}

bool CPU::PUSH_0xE5(Register16& HL, int) {
    push(HL);
    return true;
}

bool CPU::AND_0xE6(RegisterView& A, uint8_t n8, int) {
    bitAnd(A, n8);
    return true;
}

bool CPU::RST_0xE7(uint8_t $0x20, int) {
    restart($0x20);
    return true;
}

bool CPU::ADD_0xE8(Register16& SP, int8_t e8, int) {
    add<16>(SP, e8);
    using enum REGISTER_FLAG;
    F.clear(Z);
    F.clear(N);
    return true;
}

bool CPU::JP_0xE9(Register16& HL, int) {
    jump(HL);
    return true;
}

bool CPU::LD_0xEA(uint16_t a16, RegisterView& A, int) {
    loadIndirect(a16, A);
    return true;
}

bool CPU::ILLEGAL_EB_0xEB(int) {
    throw std::runtime_error("ILLEGAL OPCODE EB");
}

bool CPU::ILLEGAL_EC_0xEC(int) {
    throw std::runtime_error("ILLEGAL OPCODE EC");
}

bool CPU::ILLEGAL_ED_0xED(int) {
    throw std::runtime_error("ILLEGAL OPCODE ED");
}

bool CPU::XOR_0xEE(RegisterView& A, uint8_t n8, int) {
    bitXor(A, n8);
    return true;
}

bool CPU::RST_0xEF(uint8_t $0x28, int) {
    restart($0x28);
    return true;
}

bool CPU::LDH_0xF0(RegisterView& A, uint8_t a8, int) {
    loadHiIndirect(A, a8);
    return true;
}

bool CPU::POP_0xF1(Register16& AF, int) {
    pop(AF);
    return true;
}

bool CPU::LDH_0xF2(RegisterView& A, RegisterView& C, int) {
    loadHiIndirect(A, C);
    return true;
}

bool CPU::DI_0xF3(int) {
    disableInterrupts();
    return true;
}

bool CPU::ILLEGAL_F4_0xF4(int) {
    throw std::runtime_error("ILLEGAL OPCODE F4");
}

bool CPU::PUSH_0xF5(Register16& AF, int) {
    push(AF);
    return true;
}

bool CPU::OR_0xF6(RegisterView& A, uint8_t n8, int) {
    bitOr(A, n8);
    return true;
}

bool CPU::RST_0xF7(uint8_t $0x30, int) {
    restart($0x30);
    return true;
}

bool CPU::LD_0xF8(Register16& HL, Register16& SP, int8_t e8, int) {
    add<16>(HL, SP + e8);
    using enum REGISTER_FLAG;
    F.clear(Z);
    F.clear(N);
    return true;
}

bool CPU::LD_0xF9(Register16& SP, Register16& HL, int) {
    load<16>(SP, HL);
    return true;
}

bool CPU::LD_0xFA(RegisterView& A, uint16_t a16, int) {
    loadIndirect(A, a16);
    return true;
}

bool CPU::EI_0xFB(int) {
    enableInterrupts();
    return true;
}

bool CPU::ILLEGAL_FC_0xFC(int) {
    throw std::runtime_error("ILLEGAL OPCODE FC");
}

bool CPU::ILLEGAL_FD_0xFD(int) {
    throw std::runtime_error("ILLEGAL OPCODE FD");
}

bool CPU::CP_0xFE(RegisterView& A, uint8_t n8, int) {
    compare(A, n8);
    return true;
}

bool CPU::RST_0xFF(uint8_t $0x38, int) {
    restart($0x38);
    return true;
}

/* some CB prefix implementations to test bootrom */

bool CPU::BIT_0x7C(uint8_t $0b7, RegisterView& H, int) {
    bitTest($0b7, H);
    return true;
}

bool CPU::SWAP_0x37(RegisterView& A, int) {
    swap(A);
    return true;
}

bool CPU::RL_0x10(RegisterView& B, int) {
    rotateLeft(B);
    return true;
}