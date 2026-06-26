#include "DMG.hpp"
#include "TerminalRenderer.hpp"
#include <iostream>
#include <print>

int main(int argc, char** argv) {
    const std::filesystem::path romFile = (argc > 1) ? argv[1] : "test.gb";
    std::println(std::cerr, "Running {}", romFile.filename().c_str());
    DMG gb{renderFrame};
    gb.run(romFile);
}