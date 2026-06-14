#include "CPU.hpp"
#include "Register.hpp"
#include <cstdint>
#include <stdexcept>

using namespace Processing;

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
    F.clear(REGISTER_FLAG::Z); // set Z unconditionally
    return true;
}

bool CPU::LD_0x08(uint16_t a16, Register16& SP, int) {
    loadIndirect(a16, SP);
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
    F.clear(REGISTER_FLAG::Z); // set Z unconditionally
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
    F.clear(REGISTER_FLAG::Z); // set Z unconditionally
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
    F.clear(REGISTER_FLAG::Z); // set Z unconditionally
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
    addRelative(SP, e8);
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
    AF.setLo(F & 0xF0); // lower 4 bits of F must always be 0
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
    loadAdjusted(HL, SP, e8);
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

/* 0xCB prefixed */

bool CPU::RLC_0x00(RegisterView& B, int) {
    rotateLeftCircular(B);
    return true;
}

bool CPU::RLC_0x01(RegisterView& C, int) {
    rotateLeftCircular(C);
    return true;
}

bool CPU::RLC_0x02(RegisterView& D, int) {
    rotateLeftCircular(D);
    return true;
}

bool CPU::RLC_0x03(RegisterView& E, int) {
    rotateLeftCircular(E);
    return true;
}

bool CPU::RLC_0x04(RegisterView& H, int) {
    rotateLeftCircular(H);
    return true;
}

bool CPU::RLC_0x05(RegisterView& L, int) {
    rotateLeftCircular(L);
    return true;
}

bool CPU::RLC_0x06(Register16& HL, int) {
    rotateLeftCircularIndirect(HL);
    return true;
}

bool CPU::RLC_0x07(RegisterView& A, int) {
    rotateLeftCircular(A);
    return true;
}

bool CPU::RRC_0x08(RegisterView& B, int) {
    rotateRightCircular(B);
    return true;
}

bool CPU::RRC_0x09(RegisterView& C, int) {
    rotateRightCircular(C);
    return true;
}

bool CPU::RRC_0x0A(RegisterView& D, int) {
    rotateRightCircular(D);
    return true;
}

bool CPU::RRC_0x0B(RegisterView& E, int) {
    rotateRightCircular(E);
    return true;
}

bool CPU::RRC_0x0C(RegisterView& H, int) {
    rotateRightCircular(H);
    return true;
}

bool CPU::RRC_0x0D(RegisterView& L, int) {
    rotateRightCircular(L);
    return true;
}

bool CPU::RRC_0x0E(Register16& HL, int) {
    rotateRightCircularIndirect(HL);
    return true;
}

bool CPU::RRC_0x0F(RegisterView& A, int) {
    rotateRightCircular(A);
    return true;
}

bool CPU::RL_0x10(RegisterView& B, int) {
    rotateLeft(B);
    return true;
}

bool CPU::RL_0x11(RegisterView& C, int) {
    rotateLeft(C);
    return true;
}

bool CPU::RL_0x12(RegisterView& D, int) {
    rotateLeft(D);
    return true;
}

bool CPU::RL_0x13(RegisterView& E, int) {
    rotateLeft(E);
    return true;
}

bool CPU::RL_0x14(RegisterView& H, int) {
    rotateLeft(H);
    return true;
}

bool CPU::RL_0x15(RegisterView& L, int) {
    rotateLeft(L);
    return true;
}

bool CPU::RL_0x16(Register16& HL, int) {
    rotateLeftIndirect(HL);
    return true;
}

bool CPU::RL_0x17(RegisterView& A, int) {
    rotateLeft(A);
    return true;
}

bool CPU::RR_0x18(RegisterView& B, int) {
    rotateRight(B);
    return true;
}

bool CPU::RR_0x19(RegisterView& C, int) {
    rotateRight(C);
    return true;
}

bool CPU::RR_0x1A(RegisterView& D, int) {
    rotateRight(D);
    return true;
}

bool CPU::RR_0x1B(RegisterView& E, int) {
    rotateRight(E);
    return true;
}

bool CPU::RR_0x1C(RegisterView& H, int) {
    rotateRight(H);
    return true;
}

bool CPU::RR_0x1D(RegisterView& L, int) {
    rotateRight(L);
    return true;
}

bool CPU::RR_0x1E(Register16& HL, int) {
    rotateRightIndirect(HL);
    return true;
}

bool CPU::RR_0x1F(RegisterView& A, int) {
    rotateRight(A);
    return true;
}

bool CPU::SLA_0x20(RegisterView& B, int) {
    shiftLeftArithmetic(B);
    return true;
}

bool CPU::SLA_0x21(RegisterView& C, int) {
    shiftLeftArithmetic(C);
    return true;
}

bool CPU::SLA_0x22(RegisterView& D, int) {
    shiftLeftArithmetic(D);
    return true;
}

bool CPU::SLA_0x23(RegisterView& E, int) {
    shiftLeftArithmetic(E);
    return true;
}

bool CPU::SLA_0x24(RegisterView& H, int) {
    shiftLeftArithmetic(H);
    return true;
}

bool CPU::SLA_0x25(RegisterView& L, int) {
    shiftLeftArithmetic(L);
    return true;
}

bool CPU::SLA_0x26(Register16& HL, int) {
    shiftLeftArithmeticIndirect(HL);
    return true;
}

bool CPU::SLA_0x27(RegisterView& A, int) {
    shiftLeftArithmetic(A);
    return true;
}

bool CPU::SRA_0x28(RegisterView& B, int) {
    shiftRightArithmetic(B);
    return true;
}

bool CPU::SRA_0x29(RegisterView& C, int) {
    shiftRightArithmetic(C);
    return true;
}

bool CPU::SRA_0x2A(RegisterView& D, int) {
    shiftRightArithmetic(D);
    return true;
}

bool CPU::SRA_0x2B(RegisterView& E, int) {
    shiftRightArithmetic(E);
    return true;
}

bool CPU::SRA_0x2C(RegisterView& H, int) {
    shiftRightArithmetic(H);
    return true;
}

bool CPU::SRA_0x2D(RegisterView& L, int) {
    shiftRightArithmetic(L);
    return true;
}

bool CPU::SRA_0x2E(Register16& HL, int) {
    shiftRightArithmeticIndirect(HL);
    return true;
}

bool CPU::SRA_0x2F(RegisterView& A, int) {
    shiftRightArithmetic(A);
    return true;
}

bool CPU::SWAP_0x30(RegisterView& B, int) {
    swap(B);
    return true;
}

bool CPU::SWAP_0x31(RegisterView& C, int) {
    swap(C);
    return true;
}

bool CPU::SWAP_0x32(RegisterView& D, int) {
    swap(D);
    return true;
}

bool CPU::SWAP_0x33(RegisterView& E, int) {
    swap(E);
    return true;
}

bool CPU::SWAP_0x34(RegisterView& H, int) {
    swap(H);
    return true;
}

bool CPU::SWAP_0x35(RegisterView& L, int) {
    swap(L);
    return true;
}

bool CPU::SWAP_0x36(Register16& HL, int) {
    swapIndirect(HL);
    return true;
}

bool CPU::SWAP_0x37(RegisterView& A, int) {
    swap(A);
    return true;
}

bool CPU::SRL_0x38(RegisterView& B, int) {
    shiftRightLogical(B);
    return true;
}

bool CPU::SRL_0x39(RegisterView& C, int) {
    shiftRightLogical(C);
    return true;
}

bool CPU::SRL_0x3A(RegisterView& D, int) {
    shiftRightLogical(D);
    return true;
}

bool CPU::SRL_0x3B(RegisterView& E, int) {
    shiftRightLogical(E);
    return true;
}

bool CPU::SRL_0x3C(RegisterView& H, int) {
    shiftRightLogical(H);
    return true;
}

bool CPU::SRL_0x3D(RegisterView& L, int) {
    shiftRightLogical(L);
    return true;
}

bool CPU::SRL_0x3E(Register16& HL, int) {
    shiftRightLogicalIndirect(HL);
    return true;
}

bool CPU::SRL_0x3F(RegisterView& A, int) {
    shiftRightLogical(A);
    return true;
}

bool CPU::BIT_0x40(uint8_t _0, RegisterView& B, int) {
    bitTest(_0, B);
    return true;
}

bool CPU::BIT_0x41(uint8_t _0, RegisterView& C, int) {
    bitTest(_0, C);
    return true;
}

bool CPU::BIT_0x42(uint8_t _0, RegisterView& D, int) {
    bitTest(_0, D);
    return true;
}

bool CPU::BIT_0x43(uint8_t _0, RegisterView& E, int) {
    bitTest(_0, E);
    return true;
}

bool CPU::BIT_0x44(uint8_t _0, RegisterView& H, int) {
    bitTest(_0, H);
    return true;
}

bool CPU::BIT_0x45(uint8_t _0, RegisterView& L, int) {
    bitTest(_0, L);
    return true;
}

bool CPU::BIT_0x46(uint8_t _0, Register16& HL, int) {
    bitTestIndirect(_0, HL);
    return true;
}

bool CPU::BIT_0x47(uint8_t _0, RegisterView& A, int) {
    bitTest(_0, A);
    return true;
}

bool CPU::BIT_0x48(uint8_t _1, RegisterView& B, int) {
    bitTest(_1, B);
    return true;
}

bool CPU::BIT_0x49(uint8_t _1, RegisterView& C, int) {
    bitTest(_1, C);
    return true;
}

bool CPU::BIT_0x4A(uint8_t _1, RegisterView& D, int) {
    bitTest(_1, D);
    return true;
}

bool CPU::BIT_0x4B(uint8_t _1, RegisterView& E, int) {
    bitTest(_1, E);
    return true;
}

bool CPU::BIT_0x4C(uint8_t _1, RegisterView& H, int) {
    bitTest(_1, H);
    return true;
}

bool CPU::BIT_0x4D(uint8_t _1, RegisterView& L, int) {
    bitTest(_1, L);
    return true;
}

bool CPU::BIT_0x4E(uint8_t _1, Register16& HL, int) {
    bitTestIndirect(_1, HL);
    return true;
}

bool CPU::BIT_0x4F(uint8_t _1, RegisterView& A, int) {
    bitTest(_1, A);
    return true;
}

bool CPU::BIT_0x50(uint8_t _2, RegisterView& B, int) {
    bitTest(_2, B);
    return true;
}

bool CPU::BIT_0x51(uint8_t _2, RegisterView& C, int) {
    bitTest(_2, C);
    return true;
}

bool CPU::BIT_0x52(uint8_t _2, RegisterView& D, int) {
    bitTest(_2, D);
    return true;
}

bool CPU::BIT_0x53(uint8_t _2, RegisterView& E, int) {
    bitTest(_2, E);
    return true;
}

bool CPU::BIT_0x54(uint8_t _2, RegisterView& H, int) {
    bitTest(_2, H);
    return true;
}

bool CPU::BIT_0x55(uint8_t _2, RegisterView& L, int) {
    bitTest(_2, L);
    return true;
}

bool CPU::BIT_0x56(uint8_t _2, Register16& HL, int) {
    bitTestIndirect(_2, HL);
    return true;
}

bool CPU::BIT_0x57(uint8_t _2, RegisterView& A, int) {
    bitTest(_2, A);
    return true;
}

bool CPU::BIT_0x58(uint8_t _3, RegisterView& B, int) {
    bitTest(_3, B);
    return true;
}

bool CPU::BIT_0x59(uint8_t _3, RegisterView& C, int) {
    bitTest(_3, C);
    return true;
}

bool CPU::BIT_0x5A(uint8_t _3, RegisterView& D, int) {
    bitTest(_3, D);
    return true;
}

bool CPU::BIT_0x5B(uint8_t _3, RegisterView& E, int) {
    bitTest(_3, E);
    return true;
}

bool CPU::BIT_0x5C(uint8_t _3, RegisterView& H, int) {
    bitTest(_3, H);
    return true;
}

bool CPU::BIT_0x5D(uint8_t _3, RegisterView& L, int) {
    bitTest(_3, L);
    return true;
}

bool CPU::BIT_0x5E(uint8_t _3, Register16& HL, int) {
    bitTestIndirect(_3, HL);
    return true;
}

bool CPU::BIT_0x5F(uint8_t _3, RegisterView& A, int) {
    bitTest(_3, A);
    return true;
}

bool CPU::BIT_0x60(uint8_t _4, RegisterView& B, int) {
    bitTest(_4, B);
    return true;
}

bool CPU::BIT_0x61(uint8_t _4, RegisterView& C, int) {
    bitTest(_4, C);
    return true;
}

bool CPU::BIT_0x62(uint8_t _4, RegisterView& D, int) {
    bitTest(_4, D);
    return true;
}

bool CPU::BIT_0x63(uint8_t _4, RegisterView& E, int) {
    bitTest(_4, E);
    return true;
}

bool CPU::BIT_0x64(uint8_t _4, RegisterView& H, int) {
    bitTest(_4, H);
    return true;
}

bool CPU::BIT_0x65(uint8_t _4, RegisterView& L, int) {
    bitTest(_4, L);
    return true;
}

bool CPU::BIT_0x66(uint8_t _4, Register16& HL, int) {
    bitTestIndirect(_4, HL);
    return true;
}

bool CPU::BIT_0x67(uint8_t _4, RegisterView& A, int) {
    bitTest(_4, A);
    return true;
}

bool CPU::BIT_0x68(uint8_t _5, RegisterView& B, int) {
    bitTest(_5, B);
    return true;
}

bool CPU::BIT_0x69(uint8_t _5, RegisterView& C, int) {
    bitTest(_5, C);
    return true;
}

bool CPU::BIT_0x6A(uint8_t _5, RegisterView& D, int) {
    bitTest(_5, D);
    return true;
}

bool CPU::BIT_0x6B(uint8_t _5, RegisterView& E, int) {
    bitTest(_5, E);
    return true;
}

bool CPU::BIT_0x6C(uint8_t _5, RegisterView& H, int) {
    bitTest(_5, H);
    return true;
}

bool CPU::BIT_0x6D(uint8_t _5, RegisterView& L, int) {
    bitTest(_5, L);
    return true;
}

bool CPU::BIT_0x6E(uint8_t _5, Register16& HL, int) {
    bitTestIndirect(_5, HL);
    return true;
}

bool CPU::BIT_0x6F(uint8_t _5, RegisterView& A, int) {
    bitTest(_5, A);
    return true;
}

bool CPU::BIT_0x70(uint8_t _6, RegisterView& B, int) {
    bitTest(_6, B);
    return true;
}

bool CPU::BIT_0x71(uint8_t _6, RegisterView& C, int) {
    bitTest(_6, C);
    return true;
}

bool CPU::BIT_0x72(uint8_t _6, RegisterView& D, int) {
    bitTest(_6, D);
    return true;
}

bool CPU::BIT_0x73(uint8_t _6, RegisterView& E, int) {
    bitTest(_6, E);
    return true;
}

bool CPU::BIT_0x74(uint8_t _6, RegisterView& H, int) {
    bitTest(_6, H);
    return true;
}

bool CPU::BIT_0x75(uint8_t _6, RegisterView& L, int) {
    bitTest(_6, L);
    return true;
}

bool CPU::BIT_0x76(uint8_t _6, Register16& HL, int) {
    bitTestIndirect(_6, HL);
    return true;
}

bool CPU::BIT_0x77(uint8_t _6, RegisterView& A, int) {
    bitTest(_6, A);
    return true;
}

bool CPU::BIT_0x78(uint8_t _7, RegisterView& B, int) {
    bitTest(_7, B);
    return true;
}

bool CPU::BIT_0x79(uint8_t _7, RegisterView& C, int) {
    bitTest(_7, C);
    return true;
}

bool CPU::BIT_0x7A(uint8_t _7, RegisterView& D, int) {
    bitTest(_7, D);
    return true;
}

bool CPU::BIT_0x7B(uint8_t _7, RegisterView& E, int) {
    bitTest(_7, E);
    return true;
}

bool CPU::BIT_0x7C(uint8_t _7, RegisterView& H, int) {
    bitTest(_7, H);
    return true;
}

bool CPU::BIT_0x7D(uint8_t _7, RegisterView& L, int) {
    bitTest(_7, L);
    return true;
}

bool CPU::BIT_0x7E(uint8_t _7, Register16& HL, int) {
    bitTestIndirect(_7, HL);
    return true;
}

bool CPU::BIT_0x7F(uint8_t _7, RegisterView& A, int) {
    bitTest(_7, A);
    return true;
}

bool CPU::RES_0x80(uint8_t _0, RegisterView& B, int) {
    bitReset(_0, B);
    return true;
}

bool CPU::RES_0x81(uint8_t _0, RegisterView& C, int) {
    bitReset(_0, C);
    return true;
}

bool CPU::RES_0x82(uint8_t _0, RegisterView& D, int) {
    bitReset(_0, D);
    return true;
}

bool CPU::RES_0x83(uint8_t _0, RegisterView& E, int) {
    bitReset(_0, E);
    return true;
}

bool CPU::RES_0x84(uint8_t _0, RegisterView& H, int) {
    bitReset(_0, H);
    return true;
}

bool CPU::RES_0x85(uint8_t _0, RegisterView& L, int) {
    bitReset(_0, L);
    return true;
}

bool CPU::RES_0x86(uint8_t _0, Register16& HL, int) {
    bitResetIndirect(_0, HL);
    return true;
}

bool CPU::RES_0x87(uint8_t _0, RegisterView& A, int) {
    bitReset(_0, A);
    return true;
}

bool CPU::RES_0x88(uint8_t _1, RegisterView& B, int) {
    bitReset(_1, B);
    return true;
}

bool CPU::RES_0x89(uint8_t _1, RegisterView& C, int) {
    bitReset(_1, C);
    return true;
}

bool CPU::RES_0x8A(uint8_t _1, RegisterView& D, int) {
    bitReset(_1, D);
    return true;
}

bool CPU::RES_0x8B(uint8_t _1, RegisterView& E, int) {
    bitReset(_1, E);
    return true;
}

bool CPU::RES_0x8C(uint8_t _1, RegisterView& H, int) {
    bitReset(_1, H);
    return true;
}

bool CPU::RES_0x8D(uint8_t _1, RegisterView& L, int) {
    bitReset(_1, L);
    return true;
}

bool CPU::RES_0x8E(uint8_t _1, Register16& HL, int) {
    bitResetIndirect(_1, HL);
    return true;
}

bool CPU::RES_0x8F(uint8_t _1, RegisterView& A, int) {
    bitReset(_1, A);
    return true;
}

bool CPU::RES_0x90(uint8_t _2, RegisterView& B, int) {
    bitReset(_2, B);
    return true;
}

bool CPU::RES_0x91(uint8_t _2, RegisterView& C, int) {
    bitReset(_2, C);
    return true;
}

bool CPU::RES_0x92(uint8_t _2, RegisterView& D, int) {
    bitReset(_2, D);
    return true;
}

bool CPU::RES_0x93(uint8_t _2, RegisterView& E, int) {
    bitReset(_2, E);
    return true;
}

bool CPU::RES_0x94(uint8_t _2, RegisterView& H, int) {
    bitReset(_2, H);
    return true;
}

bool CPU::RES_0x95(uint8_t _2, RegisterView& L, int) {
    bitReset(_2, L);
    return true;
}

bool CPU::RES_0x96(uint8_t _2, Register16& HL, int) {
    bitResetIndirect(_2, HL);
    return true;
}

bool CPU::RES_0x97(uint8_t _2, RegisterView& A, int) {
    bitReset(_2, A);
    return true;
}

bool CPU::RES_0x98(uint8_t _3, RegisterView& B, int) {
    bitReset(_3, B);
    return true;
}

bool CPU::RES_0x99(uint8_t _3, RegisterView& C, int) {
    bitReset(_3, C);
    return true;
}

bool CPU::RES_0x9A(uint8_t _3, RegisterView& D, int) {
    bitReset(_3, D);
    return true;
}

bool CPU::RES_0x9B(uint8_t _3, RegisterView& E, int) {
    bitReset(_3, E);
    return true;
}

bool CPU::RES_0x9C(uint8_t _3, RegisterView& H, int) {
    bitReset(_3, H);
    return true;
}

bool CPU::RES_0x9D(uint8_t _3, RegisterView& L, int) {
    bitReset(_3, L);
    return true;
}

bool CPU::RES_0x9E(uint8_t _3, Register16& HL, int) {
    bitResetIndirect(_3, HL);
    return true;
}

bool CPU::RES_0x9F(uint8_t _3, RegisterView& A, int) {
    bitReset(_3, A);
    return true;
}

bool CPU::RES_0xA0(uint8_t _4, RegisterView& B, int) {
    bitReset(_4, B);
    return true;
}

bool CPU::RES_0xA1(uint8_t _4, RegisterView& C, int) {
    bitReset(_4, C);
    return true;
}

bool CPU::RES_0xA2(uint8_t _4, RegisterView& D, int) {
    bitReset(_4, D);
    return true;
}

bool CPU::RES_0xA3(uint8_t _4, RegisterView& E, int) {
    bitReset(_4, E);
    return true;
}

bool CPU::RES_0xA4(uint8_t _4, RegisterView& H, int) {
    bitReset(_4, H);
    return true;
}

bool CPU::RES_0xA5(uint8_t _4, RegisterView& L, int) {
    bitReset(_4, L);
    return true;
}

bool CPU::RES_0xA6(uint8_t _4, Register16& HL, int) {
    bitResetIndirect(_4, HL);
    return true;
}

bool CPU::RES_0xA7(uint8_t _4, RegisterView& A, int) {
    bitReset(_4, A);
    return true;
}

bool CPU::RES_0xA8(uint8_t _5, RegisterView& B, int) {
    bitReset(_5, B);
    return true;
}

bool CPU::RES_0xA9(uint8_t _5, RegisterView& C, int) {
    bitReset(_5, C);
    return true;
}

bool CPU::RES_0xAA(uint8_t _5, RegisterView& D, int) {
    bitReset(_5, D);
    return true;
}

bool CPU::RES_0xAB(uint8_t _5, RegisterView& E, int) {
    bitReset(_5, E);
    return true;
}

bool CPU::RES_0xAC(uint8_t _5, RegisterView& H, int) {
    bitReset(_5, H);
    return true;
}

bool CPU::RES_0xAD(uint8_t _5, RegisterView& L, int) {
    bitReset(_5, L);
    return true;
}

bool CPU::RES_0xAE(uint8_t _5, Register16& HL, int) {
    bitResetIndirect(_5, HL);
    return true;
}

bool CPU::RES_0xAF(uint8_t _5, RegisterView& A, int) {
    bitReset(_5, A);
    return true;
}

bool CPU::RES_0xB0(uint8_t _6, RegisterView& B, int) {
    bitReset(_6, B);
    return true;
}

bool CPU::RES_0xB1(uint8_t _6, RegisterView& C, int) {
    bitReset(_6, C);
    return true;
}

bool CPU::RES_0xB2(uint8_t _6, RegisterView& D, int) {
    bitReset(_6, D);
    return true;
}

bool CPU::RES_0xB3(uint8_t _6, RegisterView& E, int) {
    bitReset(_6, E);
    return true;
}

bool CPU::RES_0xB4(uint8_t _6, RegisterView& H, int) {
    bitReset(_6, H);
    return true;
}

bool CPU::RES_0xB5(uint8_t _6, RegisterView& L, int) {
    bitReset(_6, L);
    return true;
}

bool CPU::RES_0xB6(uint8_t _6, Register16& HL, int) {
    bitResetIndirect(_6, HL);
    return true;
}

bool CPU::RES_0xB7(uint8_t _6, RegisterView& A, int) {
    bitReset(_6, A);
    return true;
}

bool CPU::RES_0xB8(uint8_t _7, RegisterView& B, int) {
    bitReset(_7, B);
    return true;
}

bool CPU::RES_0xB9(uint8_t _7, RegisterView& C, int) {
    bitReset(_7, C);
    return true;
}

bool CPU::RES_0xBA(uint8_t _7, RegisterView& D, int) {
    bitReset(_7, D);
    return true;
}

bool CPU::RES_0xBB(uint8_t _7, RegisterView& E, int) {
    bitReset(_7, E);
    return true;
}

bool CPU::RES_0xBC(uint8_t _7, RegisterView& H, int) {
    bitReset(_7, H);
    return true;
}

bool CPU::RES_0xBD(uint8_t _7, RegisterView& L, int) {
    bitReset(_7, L);
    return true;
}

bool CPU::RES_0xBE(uint8_t _7, Register16& HL, int) {
    bitResetIndirect(_7, HL);
    return true;
}

bool CPU::RES_0xBF(uint8_t _7, RegisterView& A, int) {
    bitReset(_7, A);
    return true;
}

bool CPU::SET_0xC0(uint8_t _0, RegisterView& B, int) {
    bitSet(_0, B);
    return true;
}

bool CPU::SET_0xC1(uint8_t _0, RegisterView& C, int) {
    bitSet(_0, C);
    return true;
}

bool CPU::SET_0xC2(uint8_t _0, RegisterView& D, int) {
    bitSet(_0, D);
    return true;
}

bool CPU::SET_0xC3(uint8_t _0, RegisterView& E, int) {
    bitSet(_0, E);
    return true;
}

bool CPU::SET_0xC4(uint8_t _0, RegisterView& H, int) {
    bitSet(_0, H);
    return true;
}

bool CPU::SET_0xC5(uint8_t _0, RegisterView& L, int) {
    bitSet(_0, L);
    return true;
}

bool CPU::SET_0xC6(uint8_t _0, Register16& HL, int) {
    bitSetIndirect(_0, HL);
    return true;
}

bool CPU::SET_0xC7(uint8_t _0, RegisterView& A, int) {
    bitSet(_0, A);
    return true;
}

bool CPU::SET_0xC8(uint8_t _1, RegisterView& B, int) {
    bitSet(_1, B);
    return true;
}

bool CPU::SET_0xC9(uint8_t _1, RegisterView& C, int) {
    bitSet(_1, C);
    return true;
}

bool CPU::SET_0xCA(uint8_t _1, RegisterView& D, int) {
    bitSet(_1, D);
    return true;
}

bool CPU::SET_0xCB(uint8_t _1, RegisterView& E, int) {
    bitSet(_1, E);
    return true;
}

bool CPU::SET_0xCC(uint8_t _1, RegisterView& H, int) {
    bitSet(_1, H);
    return true;
}

bool CPU::SET_0xCD(uint8_t _1, RegisterView& L, int) {
    bitSet(_1, L);
    return true;
}

bool CPU::SET_0xCE(uint8_t _1, Register16& HL, int) {
    bitSetIndirect(_1, HL);
    return true;
}

bool CPU::SET_0xCF(uint8_t _1, RegisterView& A, int) {
    bitSet(_1, A);
    return true;
}

bool CPU::SET_0xD0(uint8_t _2, RegisterView& B, int) {
    bitSet(_2, B);
    return true;
}

bool CPU::SET_0xD1(uint8_t _2, RegisterView& C, int) {
    bitSet(_2, C);
    return true;
}

bool CPU::SET_0xD2(uint8_t _2, RegisterView& D, int) {
    bitSet(_2, D);
    return true;
}

bool CPU::SET_0xD3(uint8_t _2, RegisterView& E, int) {
    bitSet(_2, E);
    return true;
}

bool CPU::SET_0xD4(uint8_t _2, RegisterView& H, int) {
    bitSet(_2, H);
    return true;
}

bool CPU::SET_0xD5(uint8_t _2, RegisterView& L, int) {
    bitSet(_2, L);
    return true;
}

bool CPU::SET_0xD6(uint8_t _2, Register16& HL, int) {
    bitSetIndirect(_2, HL);
    return true;
}

bool CPU::SET_0xD7(uint8_t _2, RegisterView& A, int) {
    bitSet(_2, A);
    return true;
}

bool CPU::SET_0xD8(uint8_t _3, RegisterView& B, int) {
    bitSet(_3, B);
    return true;
}

bool CPU::SET_0xD9(uint8_t _3, RegisterView& C, int) {
    bitSet(_3, C);
    return true;
}

bool CPU::SET_0xDA(uint8_t _3, RegisterView& D, int) {
    bitSet(_3, D);
    return true;
}

bool CPU::SET_0xDB(uint8_t _3, RegisterView& E, int) {
    bitSet(_3, E);
    return true;
}

bool CPU::SET_0xDC(uint8_t _3, RegisterView& H, int) {
    bitSet(_3, H);
    return true;
}

bool CPU::SET_0xDD(uint8_t _3, RegisterView& L, int) {
    bitSet(_3, L);
    return true;
}

bool CPU::SET_0xDE(uint8_t _3, Register16& HL, int) {
    bitSetIndirect(_3, HL);
    return true;
}

bool CPU::SET_0xDF(uint8_t _3, RegisterView& A, int) {
    bitSet(_3, A);
    return true;
}

bool CPU::SET_0xE0(uint8_t _4, RegisterView& B, int) {
    bitSet(_4, B);
    return true;
}

bool CPU::SET_0xE1(uint8_t _4, RegisterView& C, int) {
    bitSet(_4, C);
    return true;
}

bool CPU::SET_0xE2(uint8_t _4, RegisterView& D, int) {
    bitSet(_4, D);
    return true;
}

bool CPU::SET_0xE3(uint8_t _4, RegisterView& E, int) {
    bitSet(_4, E);
    return true;
}

bool CPU::SET_0xE4(uint8_t _4, RegisterView& H, int) {
    bitSet(_4, H);
    return true;
}

bool CPU::SET_0xE5(uint8_t _4, RegisterView& L, int) {
    bitSet(_4, L);
    return true;
}

bool CPU::SET_0xE6(uint8_t _4, Register16& HL, int) {
    bitSetIndirect(_4, HL);
    return true;
}

bool CPU::SET_0xE7(uint8_t _4, RegisterView& A, int) {
    bitSet(_4, A);
    return true;
}

bool CPU::SET_0xE8(uint8_t _5, RegisterView& B, int) {
    bitSet(_5, B);
    return true;
}

bool CPU::SET_0xE9(uint8_t _5, RegisterView& C, int) {
    bitSet(_5, C);
    return true;
}

bool CPU::SET_0xEA(uint8_t _5, RegisterView& D, int) {
    bitSet(_5, D);
    return true;
}

bool CPU::SET_0xEB(uint8_t _5, RegisterView& E, int) {
    bitSet(_5, E);
    return true;
}

bool CPU::SET_0xEC(uint8_t _5, RegisterView& H, int) {
    bitSet(_5, H);
    return true;
}

bool CPU::SET_0xED(uint8_t _5, RegisterView& L, int) {
    bitSet(_5, L);
    return true;
}

bool CPU::SET_0xEE(uint8_t _5, Register16& HL, int) {
    bitSetIndirect(_5, HL);
    return true;
}

bool CPU::SET_0xEF(uint8_t _5, RegisterView& A, int) {
    bitSet(_5, A);
    return true;
}

bool CPU::SET_0xF0(uint8_t _6, RegisterView& B, int) {
    bitSet(_6, B);
    return true;
}

bool CPU::SET_0xF1(uint8_t _6, RegisterView& C, int) {
    bitSet(_6, C);
    return true;
}

bool CPU::SET_0xF2(uint8_t _6, RegisterView& D, int) {
    bitSet(_6, D);
    return true;
}

bool CPU::SET_0xF3(uint8_t _6, RegisterView& E, int) {
    bitSet(_6, E);
    return true;
}

bool CPU::SET_0xF4(uint8_t _6, RegisterView& H, int) {
    bitSet(_6, H);
    return true;
}

bool CPU::SET_0xF5(uint8_t _6, RegisterView& L, int) {
    bitSet(_6, L);
    return true;
}

bool CPU::SET_0xF6(uint8_t _6, Register16& HL, int) {
    bitSetIndirect(_6, HL);
    return true;
}

bool CPU::SET_0xF7(uint8_t _6, RegisterView& A, int) {
    bitSet(_6, A);
    return true;
}

bool CPU::SET_0xF8(uint8_t _7, RegisterView& B, int) {
    bitSet(_7, B);
    return true;
}

bool CPU::SET_0xF9(uint8_t _7, RegisterView& C, int) {
    bitSet(_7, C);
    return true;
}

bool CPU::SET_0xFA(uint8_t _7, RegisterView& D, int) {
    bitSet(_7, D);
    return true;
}

bool CPU::SET_0xFB(uint8_t _7, RegisterView& E, int) {
    bitSet(_7, E);
    return true;
}

bool CPU::SET_0xFC(uint8_t _7, RegisterView& H, int) {
    bitSet(_7, H);
    return true;
}

bool CPU::SET_0xFD(uint8_t _7, RegisterView& L, int) {
    bitSet(_7, L);
    return true;
}

bool CPU::SET_0xFE(uint8_t _7, Register16& HL, int) {
    bitSetIndirect(_7, HL);
    return true;
}

bool CPU::SET_0xFF(uint8_t _7, RegisterView& A, int) {
    bitSet(_7, A);
    return true;
}