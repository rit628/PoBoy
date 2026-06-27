#pragma once
#include <cstdint>
#include <string>
#include "StaticString.hpp"

namespace Cartridge {

    constexpr uint16_t HEADER_START             = 0x0100;
    constexpr uint16_t ENTRYPOINT_SIZE          = 0x0004;
    constexpr uint16_t NINTENDO_LOGO_SIZE       = 0x0030;
    constexpr uint16_t TITLE_SIZE               = 0x0010;
    constexpr uint16_t MANUFACTURER_CODE_SIZE   = 0x0004;
    constexpr uint16_t LICENSEE_CODE_SIZE       = 0x0002;
    constexpr uint8_t  NEW_LICENSEE_CODE_FLAG   = 0x33;
    
    constexpr uint32_t decodeRomSize(uint8_t encodedSize) {
        return 32 * 1024 * (1 << encodedSize);
    }
    
    constexpr uint32_t decodeRamSize(uint8_t encodedSize) {
        switch (encodedSize) {
            case 0x00: return 0;
            case 0x02: return 8 * 1024;
            case 0x03: return 32 * 1024;
            case 0x04: return 128 * 1024;
            case 0x05: return 64 * 1024;
        }
        return 0;
    }
    
    constexpr std::string decodeRegion(uint8_t destinationCode) {
        switch (destinationCode) {
            case 0x00: return "Japan (and possibly overseas)";
            case 0x01: return "Overseas only";
        }
        return "Invalid Destination Code";
    }
    
    constexpr std::string decodeLicensee(StaticString<LICENSEE_CODE_SIZE>& licenseeCode) {
        if (licenseeCode.size() == 1) { // use old licensee code
            uint8_t code = static_cast<uint8_t>(licenseeCode.at(0));
            switch (code) {
                case 0x00: return "None";
                case 0x01: return "Nintendo";
                case 0x08: return "Capcom";
                case 0x09: return "HOT-B";
                case 0x0A: return "Jaleco";
                case 0x0B: return "Coconuts Japan";
                case 0x0C: return "Elite Systems";
                case 0x13: return "EA (Electronic Arts)";
                case 0x18: return "Hudson Soft";
                case 0x19: return "ITC Entertainment";
                case 0x1A: return "Yanoman";
                case 0x1D: return "Japan Clary";
                case 0x1F: return "Virgin Games Ltd.";
                case 0x24: return "PCM Complete";
                case 0x25: return "San-X";
                case 0x28: return "Kemco";
                case 0x29: return "SETA Corporation";
                case 0x30: return "Infogrames";
                case 0x31: return "Nintendo";
                case 0x32: return "Bandai";
                case 0x34: return "Konami";
                case 0x35: return "HectorSoft";
                case 0x38: return "Capcom";
                case 0x39: return "Banpresto";
                case 0x3C: return "Entertainment Interactive (stub)";
                case 0x3E: return "Gremlin";
                case 0x41: return "Ubi Soft";
                case 0x42: return "Atlus";
                case 0x44: return "Malibu Interactive";
                case 0x46: return "Angel";
                case 0x47: return "Spectrum HoloByte";
                case 0x49: return "Irem";
                case 0x4A: return "Virgin Games Ltd.";
                case 0x4D: return "Malibu Interactive";
                case 0x4F: return "U.S. Gold";
                case 0x50: return "Absolute";
                case 0x51: return "Acclaim Entertainment";
                case 0x52: return "Activision";
                case 0x53: return "Sammy USA Corporation";
                case 0x54: return "GameTek";
                case 0x55: return "Park Place";
                case 0x56: return "LJN";
                case 0x57: return "Matchbox";
                case 0x59: return "Milton Bradley Company";
                case 0x5A: return "Mindscape";
                case 0x5B: return "Romstar";
                case 0x5C: return "Naxat Soft";
                case 0x5D: return "Tradewest";
                case 0x60: return "Titus Interactive";
                case 0x61: return "Virgin Games Ltd.";
                case 0x67: return "Ocean Software";
                case 0x69: return "EA (Electronic Arts)";
                case 0x6E: return "Elite Systems";
                case 0x6F: return "Electro Brain";
                case 0x70: return "Infogrames";
                case 0x71: return "Interplay Entertainment";
                case 0x72: return "Broderbund";
                case 0x73: return "Sculptured Software";
                case 0x75: return "The Sales Curve Limited";
                case 0x78: return "THQ";
                case 0x79: return "Accolade";
                case 0x7A: return "Triffix Entertainment";
                case 0x7C: return "MicroProse";
                case 0x7F: return "Kemco";
                case 0x80: return "Misawa Entertainment";
                case 0x83: return "LOZC G.";
                case 0x86: return "Tokuma Shoten";
                case 0x8B: return "Bullet-Proof Software";
                case 0x8C: return "Vic Tokai Corp.";
                case 0x8E: return "Ape Inc.";
                case 0x8F: return "I'Max";
                case 0x91: return "Chunsoft Co.";
                case 0x92: return "Video System";
                case 0x93: return "Tsubaraya Productions";
                case 0x95: return "Varie";
                case 0x96: return "Yonezawa/S'Pal";
                case 0x97: return "Kemco";
                case 0x99: return "Arc";
                case 0x9A: return "Nihon Bussan";
                case 0x9B: return "Tecmo";
                case 0x9C: return "Imagineer";
                case 0x9D: return "Banpresto";
                case 0x9F: return "Nova";
                case 0xA1: return "Hori Electric";
                case 0xA2: return "Bandai";
                case 0xA4: return "Konami";
                case 0xA6: return "Kawada";
                case 0xA7: return "Takara";
                case 0xA9: return "Technos Japan";
                case 0xAA: return "Broderbund";
                case 0xAC: return "Toei Animation";
                case 0xAD: return "Toho";
                case 0xAF: return "Namco";
                case 0xB0: return "Acclaim Entertainment";
                case 0xB1: return "ASCII Corporation or Nexsoft";
                case 0xB2: return "Bandai";
                case 0xB4: return "Square Enix";
                case 0xB6: return "HAL Laboratory";
                case 0xB7: return "SNK";
                case 0xB9: return "Pony Canyon";
                case 0xBA: return "Culture Brain";
                case 0xBB: return "Sunsoft";
                case 0xBD: return "Sony Imagesoft";
                case 0xBF: return "Sammy Corporation";
                case 0xC0: return "Taito";
                case 0xC2: return "Kemco";
                case 0xC3: return "Square";
                case 0xC4: return "Tokuma Shoten";
                case 0xC5: return "Data East";
                case 0xC6: return "Tonkin House";
                case 0xC8: return "Koei";
                case 0xC9: return "UFL";
                case 0xCA: return "Ultra Games";
                case 0xCB: return "VAP, Inc.";
                case 0xCC: return "Use Corporation";
                case 0xCD: return "Meldac";
                case 0xCE: return "Pony Canyon";
                case 0xCF: return "Angel";
                case 0xD0: return "Taito";
                case 0xD1: return "SOFEL (Software Engineering Lab)";
                case 0xD2: return "Quest";
                case 0xD3: return "Sigma Enterprises";
                case 0xD4: return "ASK Kodansha Co.";
                case 0xD6: return "Naxat Soft";
                case 0xD7: return "Copya System";
                case 0xD9: return "Banpresto";
                case 0xDA: return "Tomy";
                case 0xDB: return "LJN";
                case 0xDD: return "Nippon Computer Systems";
                case 0xDE: return "Human Ent.";
                case 0xDF: return "Altron";
                case 0xE0: return "Jaleco";
                case 0xE1: return "Towa Chiki";
                case 0xE2: return "Yutaka";
                case 0xE3: return "Varie";
                case 0xE5: return "Epoch";
                case 0xE7: return "Athena";
                case 0xE8: return "Asmik Ace Entertainment";
                case 0xE9: return "Natsume";
                case 0xEA: return "King Records";
                case 0xEB: return "Atlus";
                case 0xEC: return "Epic/Sony Records";
                case 0xEE: return "IGS";
                case 0xF0: return "A Wave";
                case 0xF3: return "Extreme Entertainment";
                case 0xFF: return "LJN";
            }
            return "Invalid Licensee Code";
        }
    
        // use new licensee code
        if (licenseeCode == "00") return "None";
        if (licenseeCode == "01") return "Nintendo Research & Development 1";
        if (licenseeCode == "08") return "Capcom";
        if (licenseeCode == "13") return "EA (Electronic Arts)";
        if (licenseeCode == "18") return "Hudson Soft";
        if (licenseeCode == "19") return "B-AI";
        if (licenseeCode == "20") return "KSS";
        if (licenseeCode == "22") return "Planning Office WADA";
        if (licenseeCode == "24") return "PCM Complete";
        if (licenseeCode == "25") return "San-X";
        if (licenseeCode == "28") return "Kemco";
        if (licenseeCode == "29") return "SETA Corporation";
        if (licenseeCode == "30") return "Viacom";
        if (licenseeCode == "31") return "Nintendo";
        if (licenseeCode == "32") return "Bandai";
        if (licenseeCode == "33") return "Ocean Software/Acclaim Entertainment";
        if (licenseeCode == "34") return "Konami";
        if (licenseeCode == "35") return "HectorSoft";
        if (licenseeCode == "37") return "Taito";
        if (licenseeCode == "38") return "Hudson Soft";
        if (licenseeCode == "39") return "Banpresto";
        if (licenseeCode == "41") return "Ubi Soft";
        if (licenseeCode == "42") return "Atlus";
        if (licenseeCode == "44") return "Malibu Interactive";
        if (licenseeCode == "46") return "Angel";
        if (licenseeCode == "47") return "Bullet-Proof Software";
        if (licenseeCode == "49") return "Irem";
        if (licenseeCode == "50") return "Absolute";
        if (licenseeCode == "51") return "Acclaim Entertainment";
        if (licenseeCode == "52") return "Activision";
        if (licenseeCode == "53") return "Sammy USA Corporation";
        if (licenseeCode == "54") return "Konami";
        if (licenseeCode == "55") return "Hi Tech Expressions";
        if (licenseeCode == "56") return "LJN";
        if (licenseeCode == "57") return "Matchbox";
        if (licenseeCode == "58") return "Mattel";
        if (licenseeCode == "59") return "Milton Bradley Company";
        if (licenseeCode == "60") return "Titus Interactive";
        if (licenseeCode == "61") return "Virgin Games Ltd.";
        if (licenseeCode == "64") return "Lucasfilm Games";
        if (licenseeCode == "67") return "Ocean Software";
        if (licenseeCode == "69") return "EA (Electronic Arts)";
        if (licenseeCode == "70") return "Infogrames";
        if (licenseeCode == "71") return "Interplay Entertainment";
        if (licenseeCode == "72") return "Broderbund";
        if (licenseeCode == "73") return "Sculptured Software";
        if (licenseeCode == "75") return "The Sales Curve Limited";
        if (licenseeCode == "78") return "THQ";
        if (licenseeCode == "79") return "Accolade";
        if (licenseeCode == "80") return "Misawa Entertainment";
        if (licenseeCode == "83") return "LOZC G.";
        if (licenseeCode == "86") return "Tokuma Shoten";
        if (licenseeCode == "87") return "Tsukuda Original";
        if (licenseeCode == "91") return "Chunsoft Co.";
        if (licenseeCode == "92") return "Video System";
        if (licenseeCode == "93") return "Ocean Software/Acclaim Entertainment";
        if (licenseeCode == "95") return "Varie";
        if (licenseeCode == "96") return "Yonezawa/S'Pal";
        if (licenseeCode == "97") return "Kaneko";
        if (licenseeCode == "99") return "Pack-In-Video";
        if (licenseeCode == "9H") return "Bottom Up";
        if (licenseeCode == "A4") return "Konami (Yu-Gi-Oh!)";
        if (licenseeCode == "BL") return "MTO";
        if (licenseeCode == "DK") return "Kodansha";
        return "Invalid Licensee Code";
    }
    
    constexpr std::string decodeCartridgeType(uint8_t code) {
        switch (code) {
            case 0x00: return "ROM ONLY";
            case 0x01: return "MBC1";
            case 0x02: return "MBC1+RAM";
            case 0x03: return "MBC1+RAM+BATTERY";
            case 0x05: return "MBC2";
            case 0x06: return "MBC2+BATTERY";
            case 0x08: return "ROM+RAM 11";
            case 0x09: return "ROM+RAM+BATTERY 11";
            case 0x0B: return "MMM01";
            case 0x0C: return "MMM01+RAM";
            case 0x0D: return "MMM01+RAM+BATTERY";
            case 0x0F: return "MBC3+TIMER+BATTERY";
            case 0x10: return "MBC3+TIMER+RAM+BATTERY 12";
            case 0x11: return "MBC3";
            case 0x12: return "MBC3+RAM 12";
            case 0x13: return "MBC3+RAM+BATTERY 12";
            case 0x19: return "MBC5";
            case 0x1A: return "MBC5+RAM";
            case 0x1B: return "MBC5+RAM+BATTERY";
            case 0x1C: return "MBC5+RUMBLE";
            case 0x1D: return "MBC5+RUMBLE+RAM";
            case 0x1E: return "MBC5+RUMBLE+RAM+BATTERY";
            case 0x20: return "MBC6";
            case 0x22: return "MBC7+SENSOR+RUMBLE+RAM+BATTERY";
            case 0xFC: return "POCKET CAMERA";
            case 0xFD: return "BANDAI TAMA5";
            case 0xFE: return "HuC3";
            case 0xFF: return "HuC1+RAM+BATTERY";
        }
        return "Invalid Cartridge Type";
    }

}