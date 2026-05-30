#include "GameBoy.hpp"
#include <print>

int main() {
    GameBoy gb;
    std::println("Running Bootrom Test");
    gb.runBootRomTest();
}