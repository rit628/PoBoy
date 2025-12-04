#pragma once
#include <cstdint>
#include <string>
#include "StaticString.hpp"

#define HEADER_START 0x0100
#define ENTRYPOINT_SIZE 0x0004
#define NINTENDO_LOGO_SIZE 0x0030
#define TITLE_SIZE 0x0010
#define MANUFACTURER_CODE_SIZE 0x0004
#define LICENSEE_CODE_SIZE 0x0002
#define NEW_LICENSEE_CODE_FLAG 0x33

inline uint32_t getRomSize(uint8_t encodedSize) {
    return 32*1024 * (1 << encodedSize);
}

inline uint32_t getRamSize(uint8_t encodedSize) {
    switch (encodedSize) {
        case 0x00:
            return 0;
        break;

        case 0x02:
            return 8*1024;
        break;

        case 0x03:
            return 32*1024;
        break;

        case 0x04:
            return 128*1024;
        break;

        case 0x05:
            return 64*1024;
        break;

        default:
            return 0;
        break;
    }
}

constexpr std::string getRegion(uint8_t destinationCode) {
    switch (destinationCode) {
        case 0x00:
            return "Japan (and possibly overseas)";
        break;
        case 0x01:
            return "Overseas only";
        break;
        default:
            return "Invalid Destination Code";
        break;
    }
}

constexpr std::string getLicensee(boost::static_string<LICENSEE_CODE_SIZE>& licenseeCode) {
    if (licenseeCode.size() == 1) { // use old licensee code
        uint8_t code = licenseeCode[0];
        switch (code) {
            case 0x00:
                return "None";
            break;
            case 0x01:
                return "Nintendo";
            break;
            case 0x08:
                return "Capcom";
            break;
            case 0x09:
                return "HOT-B";
            break;
            case 0x0A:
                return "Jaleco";
            break;
            case 0x0B:
                return "Coconuts Japan";
            break;
            case 0x0C:
                return "Elite Systems";
            break;
            case 0x13:
                return "EA (Electronic Arts)";
            break;
            case 0x18:
                return "Hudson Soft";
            break;
            case 0x19:
                return "ITC Entertainment";
            break;
            case 0x1A:
                return "Yanoman";
            break;
            case 0x1D:
                return "Japan Clary";
            break;
            case 0x1F:
                return "Virgin Games Ltd.";
            break;
            case 0x24:
                return "PCM Complete";
            break;
            case 0x25:
                return "San-X";
            break;
            case 0x28:
                return "Kemco";
            break;
            case 0x29:
                return "SETA Corporation";
            break;
            case 0x30:
                return "Infogrames";
            break;
            case 0x31:
                return "Nintendo";
            break;
            case 0x32:
                return "Bandai";
            break;
            case 0x34:
                return "Konami";
            break;
            case 0x35:
                return "HectorSoft";
            break;
            case 0x38:
                return "Capcom";
            break;
            case 0x39:
                return "Banpresto";
            break;
            case 0x3C:
                return "Entertainment Interactive (stub)";
            break;
            case 0x3E:
                return "Gremlin";
            break;
            case 0x41:
                return "Ubi Soft";
            break;
            case 0x42:
                return "Atlus";
            break;
            case 0x44:
                return "Malibu Interactive";
            break;
            case 0x46:
                return "Angel";
            break;
            case 0x47:
                return "Spectrum HoloByte";
            break;
            case 0x49:
                return "Irem";
            break;
            case 0x4A:
                return "Virgin Games Ltd.";
            break;
            case 0x4D:
                return "Malibu Interactive";
            break;
            case 0x4F:
                return "U.S. Gold";
            break;
            case 0x50:
                return "Absolute";
            break;
            case 0x51:
                return "Acclaim Entertainment";
            break;
            case 0x52:
                return "Activision";
            break;
            case 0x53:
                return "Sammy USA Corporation";
            break;
            case 0x54:
                return "GameTek";
            break;
            case 0x55:
                return "Park Place";
            break;
            case 0x56:
                return "LJN";
            break;
            case 0x57:
                return "Matchbox";
            break;
            case 0x59:
                return "Milton Bradley Company";
            break;
            case 0x5A:
                return "Mindscape";
            break;
            case 0x5B:
                return "Romstar";
            break;
            case 0x5C:
                return "Naxat Soft";
            break;
            case 0x5D:
                return "Tradewest";
            break;
            case 0x60:
                return "Titus Interactive";
            break;
            case 0x61:
                return "Virgin Games Ltd.";
            break;
            case 0x67:
                return "Ocean Software";
            break;
            case 0x69:
                return "EA (Electronic Arts)";
            break;
            case 0x6E:
                return "Elite Systems";
            break;
            case 0x6F:
                return "Electro Brain";
            break;
            case 0x70:
                return "Infogrames";
            break;
            case 0x71:
                return "Interplay Entertainment";
            break;
            case 0x72:
                return "Broderbund";
            break;
            case 0x73:
                return "Sculptured Software";
            break;
            case 0x75:
                return "The Sales Curve Limited";
            break;
            case 0x78:
                return "THQ";
            break;
            case 0x79:
                return "Accolade";
            break;
            case 0x7A:
                return "Triffix Entertainment";
            break;
            case 0x7C:
                return "MicroProse";
            break;
            case 0x7F:
                return "Kemco";
            break;
            case 0x80:
                return "Misawa Entertainment";
            break;
            case 0x83:
                return "LOZC G.";
            break;
            case 0x86:
                return "Tokuma Shoten";
            break;
            case 0x8B:
                return "Bullet-Proof Software";
            break;
            case 0x8C:
                return "Vic Tokai Corp.";
            break;
            case 0x8E:
                return "Ape Inc.";
            break;
            case 0x8F:
                return "I'Max";
            break;
            case 0x91:
                return "Chunsoft Co.";
            break;
            case 0x92:
                return "Video System";
            break;
            case 0x93:
                return "Tsubaraya Productions";
            break;
            case 0x95:
                return "Varie";
            break;
            case 0x96:
                return "Yonezawa/S'Pal";
            break;
            case 0x97:
                return "Kemco";
            break;
            case 0x99:
                return "Arc";
            break;
            case 0x9A:
                return "Nihon Bussan";
            break;
            case 0x9B:
                return "Tecmo";
            break;
            case 0x9C:
                return "Imagineer";
            break;
            case 0x9D:
                return "Banpresto";
            break;
            case 0x9F:
                return "Nova";
            break;
            case 0xA1:
                return "Hori Electric";
            break;
            case 0xA2:
                return "Bandai";
            break;
            case 0xA4:
                return "Konami";
            break;
            case 0xA6:
                return "Kawada";
            break;
            case 0xA7:
                return "Takara";
            break;
            case 0xA9:
                return "Technos Japan";
            break;
            case 0xAA:
                return "Broderbund";
            break;
            case 0xAC:
                return "Toei Animation";
            break;
            case 0xAD:
                return "Toho";
            break;
            case 0xAF:
                return "Namco";
            break;
            case 0xB0:
                return "Acclaim Entertainment";
            break;
            case 0xB1:
                return "ASCII Corporation or Nexsoft";
            break;
            case 0xB2:
                return "Bandai";
            break;
            case 0xB4:
                return "Square Enix";
            break;
            case 0xB6:
                return "HAL Laboratory";
            break;
            case 0xB7:
                return "SNK";
            break;
            case 0xB9:
                return "Pony Canyon";
            break;
            case 0xBA:
                return "Culture Brain";
            break;
            case 0xBB:
                return "Sunsoft";
            break;
            case 0xBD:
                return "Sony Imagesoft";
            break;
            case 0xBF:
                return "Sammy Corporation";
            break;
            case 0xC0:
                return "Taito";
            break;
            case 0xC2:
                return "Kemco";
            break;
            case 0xC3:
                return "Square";
            break;
            case 0xC4:
                return "Tokuma Shoten";
            break;
            case 0xC5:
                return "Data East";
            break;
            case 0xC6:
                return "Tonkin House";
            break;
            case 0xC8:
                return "Koei";
            break;
            case 0xC9:
                return "UFL";
            break;
            case 0xCA:
                return "Ultra Games";
            break;
            case 0xCB:
                return "VAP, Inc.";
            break;
            case 0xCC:
                return "Use Corporation";
            break;
            case 0xCD:
                return "Meldac";
            break;
            case 0xCE:
                return "Pony Canyon";
            break;
            case 0xCF:
                return "Angel";
            break;
            case 0xD0:
                return "Taito";
            break;
            case 0xD1:
                return "SOFEL (Software Engineering Lab)";
            break;
            case 0xD2:
                return "Quest";
            break;
            case 0xD3:
                return "Sigma Enterprises";
            break;
            case 0xD4:
                return "ASK Kodansha Co.";
            break;
            case 0xD6:
                return "Naxat Soft";
            break;
            case 0xD7:
                return "Copya System";
            break;
            case 0xD9:
                return "Banpresto";
            break;
            case 0xDA:
                return "Tomy";
            break;
            case 0xDB:
                return "LJN";
            break;
            case 0xDD:
                return "Nippon Computer Systems";
            break;
            case 0xDE:
                return "Human Ent.";
            break;
            case 0xDF:
                return "Altron";
            break;
            case 0xE0:
                return "Jaleco";
            break;
            case 0xE1:
                return "Towa Chiki";
            break;
            case 0xE2:
                return "Yutaka";
            break;
            case 0xE3:
                return "Varie";
            break;
            case 0xE5:
                return "Epoch";
            break;
            case 0xE7:
                return "Athena";
            break;
            case 0xE8:
                return "Asmik Ace Entertainment";
            break;
            case 0xE9:
                return "Natsume";
            break;
            case 0xEA:
                return "King Records";
            break;
            case 0xEB:
                return "Atlus";
            break;
            case 0xEC:
                return "Epic/Sony Records";
            break;
            case 0xEE:
                return "IGS";
            break;
            case 0xF0:
                return "A Wave";
            break;
            case 0xF3:
                return "Extreme Entertainment";
            break;
            case 0xFF:
                return "LJN";
            break;
            default:
                return "Invalid Licensee Code";
            break;
        }
    }

    // use new licensee code
    if (licenseeCode == "00") { 
        return "None";
    }
    else if (licenseeCode == "01") { 
        return "Nintendo Research & Development 1";
    }
    else if (licenseeCode == "08") { 
        return "Capcom";
    }
    else if (licenseeCode == "13") { 
        return "EA (Electronic Arts)";
    }
    else if (licenseeCode == "18") { 
        return "Hudson Soft";
    }
    else if (licenseeCode == "19") { 
        return "B-AI";
    }
    else if (licenseeCode == "20") { 
        return "KSS";
    }
    else if (licenseeCode == "22") { 
        return "Planning Office WADA";
    }
    else if (licenseeCode == "24") { 
        return "PCM Complete";
    }
    else if (licenseeCode == "25") { 
        return "San-X";
    }
    else if (licenseeCode == "28") { 
        return "Kemco";
    }
    else if (licenseeCode == "29") { 
        return "SETA Corporation";
    }
    else if (licenseeCode == "30") { 
        return "Viacom";
    }
    else if (licenseeCode == "31") { 
        return "Nintendo";
    }
    else if (licenseeCode == "32") { 
        return "Bandai";
    }
    else if (licenseeCode == "33") { 
        return "Ocean Software/Acclaim Entertainment";
    }
    else if (licenseeCode == "34") { 
        return "Konami";
    }
    else if (licenseeCode == "35") { 
        return "HectorSoft";
    }
    else if (licenseeCode == "37") { 
        return "Taito";
    }
    else if (licenseeCode == "38") { 
        return "Hudson Soft";
    }
    else if (licenseeCode == "39") { 
        return "Banpresto";
    }
    else if (licenseeCode == "41") { 
        return "Ubi Soft";
    }
    else if (licenseeCode == "42") { 
        return "Atlus";
    }
    else if (licenseeCode == "44") { 
        return "Malibu Interactive";
    }
    else if (licenseeCode == "46") { 
        return "Angel";
    }
    else if (licenseeCode == "47") { 
        return "Bullet-Proof Software";
    }
    else if (licenseeCode == "49") { 
        return "Irem";
    }
    else if (licenseeCode == "50") { 
        return "Absolute";
    }
    else if (licenseeCode == "51") { 
        return "Acclaim Entertainment";
    }
    else if (licenseeCode == "52") { 
        return "Activision";
    }
    else if (licenseeCode == "53") { 
        return "Sammy USA Corporation";
    }
    else if (licenseeCode == "54") { 
        return "Konami";
    }
    else if (licenseeCode == "55") { 
        return "Hi Tech Expressions";
    }
    else if (licenseeCode == "56") { 
        return "LJN";
    }
    else if (licenseeCode == "57") { 
        return "Matchbox";
    }
    else if (licenseeCode == "58") { 
        return "Mattel";
    }
    else if (licenseeCode == "59") { 
        return "Milton Bradley Company";
    }
    else if (licenseeCode == "60") { 
        return "Titus Interactive";
    }
    else if (licenseeCode == "61") { 
        return "Virgin Games Ltd.";
    }
    else if (licenseeCode == "64") { 
        return "Lucasfilm Games";
    }
    else if (licenseeCode == "67") { 
        return "Ocean Software";
    }
    else if (licenseeCode == "69") { 
        return "EA (Electronic Arts)";
    }
    else if (licenseeCode == "70") { 
        return "Infogrames";
    }
    else if (licenseeCode == "71") { 
        return "Interplay Entertainment";
    }
    else if (licenseeCode == "72") { 
        return "Broderbund";
    }
    else if (licenseeCode == "73") { 
        return "Sculptured Software";
    }
    else if (licenseeCode == "75") { 
        return "The Sales Curve Limited";
    }
    else if (licenseeCode == "78") { 
        return "THQ";
    }
    else if (licenseeCode == "79") { 
        return "Accolade";
    }
    else if (licenseeCode == "80") { 
        return "Misawa Entertainment";
    }
    else if (licenseeCode == "83") { 
        return "LOZC G.";
    }
    else if (licenseeCode == "86") { 
        return "Tokuma Shoten";
    }
    else if (licenseeCode == "87") { 
        return "Tsukuda Original";
    }
    else if (licenseeCode == "91") { 
        return "Chunsoft Co.";
    }
    else if (licenseeCode == "92") { 
        return "Video System";
    }
    else if (licenseeCode == "93") { 
        return "Ocean Software/Acclaim Entertainment";
    }
    else if (licenseeCode == "95") { 
        return "Varie";
    }
    else if (licenseeCode == "96") { 
        return "Yonezawa/S'Pal";
    }
    else if (licenseeCode == "97") { 
        return "Kaneko";
    }
    else if (licenseeCode == "99") { 
        return "Pack-In-Video";
    }
    else if (licenseeCode == "9H") { 
        return "Bottom Up";
    }
    else if (licenseeCode == "A4") { 
        return "Konami (Yu-Gi-Oh!)";
    }
    else if (licenseeCode == "BL") { 
        return "MTO";
    }
    else if (licenseeCode == "DK") { 
        return "Kodansha";
    }
    return "Invalid Licensee Code";
}

constexpr std::string getCartridgeType(uint8_t code) {
    switch (code) {
        case 0x00:
            return "ROM ONLY";
        break;
        case 0x01:
            return "MBC1";
        break;
        case 0x02:
            return "MBC1+RAM";
        break;
        case 0x03:
            return "MBC1+RAM+BATTERY";
        break;
        case 0x05:
            return "MBC2";
        break;
        case 0x06:
            return "MBC2+BATTERY";
        break;
        case 0x08:
            return "ROM+RAM 11";
        break;
        case 0x09:
            return "ROM+RAM+BATTERY 11";
        break;
        case 0x0B:
            return "MMM01";
        break;
        case 0x0C:
            return "MMM01+RAM";
        break;
        case 0x0D:
            return "MMM01+RAM+BATTERY";
        break;
        case 0x0F:
            return "MBC3+TIMER+BATTERY";
        break;
        case 0x10:
            return "MBC3+TIMER+RAM+BATTERY 12";
        break;
        case 0x11:
            return "MBC3";
        break;
        case 0x12:
            return "MBC3+RAM 12";
        break;
        case 0x13:
            return "MBC3+RAM+BATTERY 12";
        break;
        case 0x19:
            return "MBC5";
        break;
        case 0x1A:
            return "MBC5+RAM";
        break;
        case 0x1B:
            return "MBC5+RAM+BATTERY";
        break;
        case 0x1C:
            return "MBC5+RUMBLE";
        break;
        case 0x1D:
            return "MBC5+RUMBLE+RAM";
        break;
        case 0x1E:
            return "MBC5+RUMBLE+RAM+BATTERY";
        break;
        case 0x20:
            return "MBC6";
        break;
        case 0x22:
            return "MBC7+SENSOR+RUMBLE+RAM+BATTERY";
        break;
        case 0xFC:
            return "POCKET CAMERA";
        break;
        case 0xFD:
            return "BANDAI TAMA5";
        break;
        case 0xFE:
            return "HuC3";
        break;
        case 0xFF:
            return "HuC1+RAM+BATTERY";
        break;
        default:
            return "Invalid Cartridge Type";
        break;
    }
}