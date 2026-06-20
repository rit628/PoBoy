#pragma once

#include "CPU.hpp"
#include <boost/json.hpp>
#include <cstddef>
#include <cstdint>
#include <print>
#include <iostream>
#include <stdexcept>
#include <string>
#include <utility>

#define CHECK(expr) \
if (!(expr)) { \
    std::println("Test {} failed: {}", currentTest, #expr); \
    std::cout.flush(); \
}

class Harness : Processing::CPU<true> {
    public:
        Harness();
        void test(boost::json::value& testConfig);

    private:
        void init(boost::json::value& initial);
        void run(boost::json::value& cycles);
        void compare(boost::json::value& final);

        std::string currentTest;
        uint8_t ticksThisInstruction = 0;
};

inline Harness::Harness()
    : CPU<true>([this](){ticksThisInstruction++;}) {}

inline void Harness::test(boost::json::value& testConfig) {
    currentTest = testConfig.at("name").as_string();
    try {
        init(testConfig.at("initial"));
        run(testConfig.at("cycles"));
        compare(testConfig.at("final"));
    }
    catch (std::runtime_error& e) {
        std::println("Test {} failed with error {}.", currentTest, e.what());
        std::cout.flush();
    }
}

inline void Harness::init(boost::json::value& initial) {
    mmu.fill(0);
    state = STATE::RUNNING;

    PC = initial.at("pc").as_int64();
    SP = initial.at("sp").as_int64();
    A = initial.at("a").as_int64();
    B = initial.at("b").as_int64();
    C = initial.at("c").as_int64();
    D = initial.at("d").as_int64();
    E = initial.at("e").as_int64();
    F = initial.at("f").as_int64();
    H = initial.at("h").as_int64();
    L = initial.at("l").as_int64();
    IME = static_cast<INTERRUPT_MASTER_FLAG>(initial.at("ime").as_int64());

    auto ram = initial.at("ram").as_array();
    for (auto&& addressValue : ram) {
        uint16_t address = addressValue.at(0).as_int64();
        uint8_t value = addressValue.at(1).as_int64();
        mmu.at(address) = value;
    }
}

inline void Harness::run(boost::json::value& cycles) {
    auto totalMCycles = cycles.as_array().size();
    size_t elapsedMCycles = 0;
    ticksThisInstruction = 0;
    while (elapsedMCycles != totalMCycles) {
        ticksThisInstruction = 0;
        tick();
        elapsedMCycles += ticksThisInstruction;
        if (elapsedMCycles > totalMCycles) {
            throw std::runtime_error("invalid cycle count: " + std::to_string(elapsedMCycles) + " expected: " + std::to_string(totalMCycles));
        }
    }
}

inline void Harness::compare(boost::json::value& final) {
    CHECK(PC == final.at("pc").as_int64());
    CHECK(SP == final.at("sp").as_int64());
    CHECK(A == final.at("a").as_int64());
    CHECK(B == final.at("b").as_int64());
    CHECK(C == final.at("c").as_int64());
    CHECK(D == final.at("d").as_int64());
    CHECK(E == final.at("e").as_int64());
    CHECK(F == final.at("f").as_int64());
    CHECK(H == final.at("h").as_int64());
    CHECK(L == final.at("l").as_int64());
    CHECK((std::to_underlying(IME) & 1) == final.at("ime").as_int64());

    auto expectedRam = final.at("ram").as_array();
    for (auto&& addressValue : expectedRam) {
        uint16_t address = addressValue.at(0).as_int64();
        uint8_t value = addressValue.at(1).as_int64();
        CHECK(mmu.at(address) == value);
    }
}

#undef CHECK