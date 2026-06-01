#include "GameBoy.hpp"
#include <print>

int main(int argc, char** argv) {
    const std::filesystem::path romFile = (argc > 1) ? argv[1] : "test.gb";
    std::println("Running {}", romFile.filename().c_str());
    GameBoy gb;
    gb.run(romFile);
}