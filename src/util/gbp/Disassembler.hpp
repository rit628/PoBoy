#pragma once
#include <cstdint>
#include "MemoryConstants.hpp"
#include "StaticString.hpp"

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
        StaticString<Memory::TITLE_SIZE> title;
        StaticString<Memory::MANUFACTURER_CODE_SIZE> manufacturerCode;
        StaticString<Memory::LICENSEE_CODE_SIZE> licenseeCode;
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