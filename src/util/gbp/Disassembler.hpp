#pragma once
#include <cstdint>
#include "StaticString.hpp"
#include "CartridgeConstants.hpp"

class Disassembler {
    public:
        void readCartridge(std::istream& rom);
        
    private:
        void readEntrypoint(std::istream& rom);
        void readLogo(std::istream& rom);
        void readHeader(std::istream& rom);
        void readInstruction(std::istream& rom);
        void readPrefixedInstruction(std::istream& rom);

        uint16_t entrypointJumpAddress = 0x0150;
        StaticString<Cartridge::TITLE_SIZE> title;
        StaticString<Cartridge::MANUFACTURER_CODE_SIZE> manufacturerCode;
        StaticString<Cartridge::LICENSEE_CODE_SIZE> licenseeCode;
        uint8_t cgbFlag = 0;
        uint8_t sgbFlag = 0;
        uint8_t cartridgeType = 0;
        uint32_t romSize = 0;
        uint32_t ramSize = 0;
        uint8_t destinationCode = 0;
        uint8_t romVersion = 0;
        uint8_t headerChecksum = 0;
        uint16_t globalChecksum = 0;
};