#pragma once
#include "HeaderInfo.hpp"
#include <filesystem>
#include <fstream>

class CartridgeLoader {
    public:
        void loadCartridge(const std::filesystem::path& romFile);

    private:
        void readRomMetadata();

        std::ifstream rom;
        boost::static_string<TITLE_SIZE> title;
        boost::static_string<MANUFACTURER_CODE_SIZE> manufacturerCode;
        boost::static_string<LICENSEE_CODE_SIZE> licenseeCode;
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