#pragma once
#include "CartridgeConstants.hpp"
#include "StaticString.hpp"
#include <cstdint>

namespace Memory {

    struct RomMetadata {
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

}