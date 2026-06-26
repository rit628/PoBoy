#include "DMG.hpp"
#include "InputReader.hpp"
#include "TerminalRenderer.hpp"
#include <iostream>
#include <print>
#include <thread>

int main(int argc, char** argv) {
    const std::filesystem::path romFile = (argc > 1) ? argv[1] : "test.gb";
    std::println(std::cerr, "Running {}", romFile.filename().c_str());
    InputReader inputReader;
    std::thread inputThread(std::bind(&InputReader::run, std::ref(inputReader)));
    inputThread.detach();
    DMG gb{std::bind(&InputReader::readInput, std::ref(inputReader)), renderFrame};
    gb.run(romFile);
}