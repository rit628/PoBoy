#include "Disassembler.hpp"
#include <fstream>

int main(int argc, char** argv) {
    Disassembler dasm;
    std::string romName = (argc > 1) ? argv[1] : "test.gb"; 
    std::ifstream rom(romName);
    dasm.readCartridge(rom);
    rom.close();
}