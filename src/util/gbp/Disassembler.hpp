#pragma once
#include <cstdint>
#include "StaticString.hpp"

#define HEADER_START 0x0100
#define ENTRYPOINT_SIZE 0x0004
#define NINTENDO_LOGO_SIZE 0x0030
#define TITLE_SIZE 0x0010
#define MANUFACTURER_CODE_SIZE 0x0004
#define LICENSEE_CODE_SIZE 0x0002

class Disassembler {
    public:
        void readHeader(std::istream& rom);

    private:
        boost::static_string<TITLE_SIZE> title;
        boost::static_string<MANUFACTURER_CODE_SIZE> manufacturerCode;
        boost::static_string<LICENSEE_CODE_SIZE> licenseeCode;
        uint8_t cgbFlag = 0;
        uint8_t sgbFlag = 0;
        uint8_t cartridgeType = 0;
        uint32_t romSize = 0;
        uint16_t ramSize = 0;
        uint8_t destinationCode = 0;
        uint8_t romVersion = 0;
        uint8_t headerChecksum = 0;
        uint16_t globalChecksum = 0;
};