#pragma once
#include "GraphicsConstants.hpp"
#include "StaticString.hpp"
#include <cstdint>
#include <string>

namespace Memory {

    constexpr uint32_t MEMORY_SIZE           = 0x10000;
    constexpr uint16_t BOOTROM_SIZE          = 0x0100;
    constexpr uint16_t ROM_BANK_SIZE         = 0x4000;
    constexpr uint16_t SRAM_BANK_SIZE        = 0x2000;
    constexpr uint16_t WRAM_SIZE             = 0x2000;
    constexpr uint8_t  IO_SIZE               = 0x80;
    constexpr uint8_t  HRAM_SIZE             = 0x7F;

    constexpr uint16_t ROM_BANK_0_START      = 0;
    constexpr uint16_t ROM_BANK_0_END        = ROM_BANK_0_START + ROM_BANK_SIZE;
    constexpr uint16_t ROM_BANK_1_START      = ROM_BANK_0_END;
    constexpr uint16_t ROM_BANK_1_END        = ROM_BANK_1_START + ROM_BANK_SIZE;
    constexpr uint16_t VRAM_START            = ROM_BANK_1_END;
    constexpr uint16_t VRAM_END              = VRAM_START + Graphics::VRAM_SIZE;
    constexpr uint16_t CARTRIDGE_RAM_START   = VRAM_END;
    constexpr uint16_t CARTRIDGE_RAM_END     = CARTRIDGE_RAM_START + SRAM_BANK_SIZE;
    constexpr uint16_t WRAM_START            = CARTRIDGE_RAM_END;
    constexpr uint16_t WRAM_END              = WRAM_START + WRAM_SIZE;
    constexpr uint16_t ECHO_RAM_START        = WRAM_END;
    constexpr uint16_t ECHO_RAM_END          = ECHO_RAM_START + WRAM_SIZE - 0X0200;
    constexpr uint16_t OAM_START             = ECHO_RAM_END;
    constexpr uint16_t OAM_END               = OAM_START + Graphics::OAM_SIZE;
    constexpr uint16_t PROHIBITED_START      = OAM_END;
    constexpr uint16_t PROHIBITED_END        = PROHIBITED_START + 0x60;
    constexpr uint16_t IO_START              = PROHIBITED_END;
    constexpr uint16_t IO_END                = IO_START + IO_SIZE;
    constexpr uint16_t HRAM_START            = IO_END;
    constexpr uint16_t HRAM_END              = HRAM_START + HRAM_SIZE;

    constexpr uint16_t P1       = 0xFF00;   // Joypad
    constexpr uint16_t SB       = 0xFF01;   // Serial transfer data
    constexpr uint16_t SC       = 0xFF02;   // Serial transfer control
    constexpr uint16_t DIV      = 0xFF04;   // Divider register
    constexpr uint16_t TIMA     = 0xFF05;   // Timer counter
    constexpr uint16_t TMA      = 0xFF06;   // Timer modulo
    constexpr uint16_t TAC      = 0xFF07;   // Timer control
    constexpr uint16_t IF       = 0xFF0F;   // Interrupt flag
    constexpr uint16_t NR10     = 0xFF10;   // Sound channel 1 sweep
    constexpr uint16_t NR11     = 0xFF11;   // Sound channel 1 length timer & duty cycle
    constexpr uint16_t NR12     = 0xFF12;   // Sound channel 1 volume & envelope
    constexpr uint16_t NR13     = 0xFF13;   // Sound channel 1 period low
    constexpr uint16_t NR14     = 0xFF14;   // Sound channel 1 period high & control
    constexpr uint16_t NR21     = 0xFF16;   // Sound channel 2 length timer & duty cycle
    constexpr uint16_t NR22     = 0xFF17;   // Sound channel 2 volume & envelope
    constexpr uint16_t NR23     = 0xFF18;   // Sound channel 2 period low
    constexpr uint16_t NR24     = 0xFF19;   // Sound channel 2 period high & control
    constexpr uint16_t NR30     = 0xFF1A;   // Sound channel 3 DAC enable
    constexpr uint16_t NR31     = 0xFF1B;   // Sound channel 3 length timer
    constexpr uint16_t NR32     = 0xFF1C;   // Sound channel 3 output level
    constexpr uint16_t NR33     = 0xFF1D;   // Sound channel 3 period low
    constexpr uint16_t NR34     = 0xFF1E;   // Sound channel 3 period high & control
    constexpr uint16_t NR41     = 0xFF20;   // Sound channel 4 length timer
    constexpr uint16_t NR42     = 0xFF21;   // Sound channel 4 volume & envelope
    constexpr uint16_t NR43     = 0xFF22;   // Sound channel 4 frequency & randomness
    constexpr uint16_t NR44     = 0xFF23;   // Sound channel 4 control
    constexpr uint16_t NR50     = 0xFF24;   // Master volume & VIN panning
    constexpr uint16_t NR51     = 0xFF25;   // Sound panning
    constexpr uint16_t NR52     = 0xFF26;   // Sound on/off
    constexpr uint16_t FF3F     = 0xFF30;   // Wave RAM Storage for one of the sound channels' waveform
    constexpr uint16_t LCDC     = 0xFF40;   // LCD control
    constexpr uint16_t STAT     = 0xFF41;   // LCD status
    constexpr uint16_t SCY      = 0xFF42;   // Viewport Y position
    constexpr uint16_t SCX      = 0xFF43;   // Viewport X position
    constexpr uint16_t LY       = 0xFF44;   // LCD Y coordinate
    constexpr uint16_t LYC      = 0xFF45;   // LY compare
    constexpr uint16_t DMA      = 0xFF46;   // OAM DMA source address & start
    constexpr uint16_t BGP      = 0xFF47;   // BG palette data
    constexpr uint16_t OBP0     = 0xFF48;   // OBJ palette 0 data
    constexpr uint16_t OBP1     = 0xFF49;   // OBJ palette 1 data
    constexpr uint16_t WY       = 0xFF4A;   // Window Y position
    constexpr uint16_t WX       = 0xFF4B;   // Window X position plus 7
    constexpr uint16_t KEY0     = 0xFF4C;   // CPU mode select
    constexpr uint16_t KEY1     = 0xFF4D;   // Prepare speed switch
    constexpr uint16_t VBK      = 0xFF4F;   // VRAM bank
    constexpr uint16_t BANK     = 0xFF50;   // Boot ROM mapping control
    constexpr uint16_t HDMA1    = 0xFF51;   // VRAM DMA source high
    constexpr uint16_t HDMA2    = 0xFF52;   // VRAM DMA source low
    constexpr uint16_t HDMA3    = 0xFF53;   // VRAM DMA destination high
    constexpr uint16_t HDMA4    = 0xFF54;   // VRAM DMA destination low
    constexpr uint16_t HDMA5    = 0xFF55;   // VRAM DMA length/mode/start
    constexpr uint16_t RP       = 0xFF56;   // Infrared communications port
    constexpr uint16_t BCPS     = 0xFF68;   // Background color palette specification / Background palette index
    constexpr uint16_t BCPD     = 0xFF69;   // Background color palette data / Background palette data
    constexpr uint16_t OCPS     = 0xFF6A;   // OBJ color palette specification / OBJ palette index
    constexpr uint16_t OCPD     = 0xFF6B;   // OBJ color palette data / OBJ palette data
    constexpr uint16_t OPRI     = 0xFF6C;   // Object priority mode
    constexpr uint16_t SVBK     = 0xFF70;   // WRAM bank
    constexpr uint16_t PCM12    = 0xFF76;   // Audio digital outputs 1 & 2
    constexpr uint16_t PCM34    = 0xFF77;   // Audio digital outputs 3 & 4
    constexpr uint16_t IE       = 0xFFFF;   // Interrupt enable

    constexpr uint16_t CARTRIDGE_HEADER_START   = 0x0100;
    constexpr uint16_t ENTRYPOINT_SIZE          = 0x0004;
    constexpr uint16_t NINTENDO_LOGO_SIZE       = 0x0030;
    constexpr uint16_t TITLE_SIZE               = 0x0010;
    constexpr uint16_t MANUFACTURER_CODE_SIZE   = 0x0004;
    constexpr uint16_t LICENSEE_CODE_SIZE       = 0x0002;
    constexpr uint8_t  NEW_LICENSEE_CODE_FLAG   = 0x33;

    enum class MBC_TYPE : uint8_t {
        MBC0                            = 0x00,
        MBC1                            = 0x01,
        MBC1_RAM                        = 0x02,
        MBC1_RAM_BATTERY                = 0x03,
        MBC2                            = 0x05,
        MBC2_BATTERY                    = 0x06,
        MBC0_RAM                        = 0x08,
        MBC0_RAM_BATTERY                = 0x09,
        MMM01                           = 0x0B,
        MMM01_RAM                       = 0x0C,
        MMM01_RAM_BATTERY               = 0x0D,
        MBC3_RTC                        = 0x0F,
        MBC3_RTC_RAM_BATTERY            = 0x10,
        MBC3                            = 0x11,
        MBC3_RAM                        = 0x12,
        MBC3_RAM_BATTERY                = 0x13,
        MBC5                            = 0x19,
        MBC5_RAM                        = 0x1A,
        MBC5_RAM_BATTERY                = 0x1B,
        MBC5_RUMBLE                     = 0x1C,
        MBC5_RUMBLE_RAM                 = 0x1D,
        MBC5_RUMBLE_RAM_BATTERY         = 0x1E,
        MBC6                            = 0x20,
        MBC7_ACCELEROMETER_RAM_BATTERY  = 0x22,
        POCKET_CAMERA                   = 0xFC,
        BANDAI_TAMA5                    = 0xFD,
        HuC3                            = 0xFE,
        HuC1_RAM_BATTERY                = 0xFF
    };

    struct CartridgeMetadata {
        StaticString<TITLE_SIZE> title;
        StaticString<MANUFACTURER_CODE_SIZE> manufacturerCode;
        StaticString<LICENSEE_CODE_SIZE> licenseeCode;
        uint8_t cgbFlag = 0;
        uint8_t sgbFlag = 0;
        MBC_TYPE cartridgeType = MBC_TYPE::MBC0;
        uint8_t encodedRomSize = 0;
        uint8_t encodedRamSize = 0;
        uint8_t destinationCode = 0;
        uint8_t romVersion = 0;
        uint8_t headerChecksum = 0;
        uint16_t globalChecksum = 0;
    };
    
    constexpr uint32_t decodeRomSize(uint8_t encodedSize) {
        return 32 * 1024 * (1 << encodedSize);
    }

    constexpr uint32_t decodeRomBankCount(uint8_t encodedSize) {
        return decodeRomSize(encodedSize) / ROM_BANK_SIZE;
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

    constexpr std::string decodeCartridgeType(MBC_TYPE code) {
        switch (code) {
            using enum MBC_TYPE;
            case MBC0: return "ROM ONLY";
            case MBC1: return "MBC1";
            case MBC1_RAM: return "MBC1+RAM";
            case MBC1_RAM_BATTERY: return "MBC1+RAM+BATTERY";
            case MBC2: return "MBC2";
            case MBC2_BATTERY: return "MBC2+BATTERY";
            case MBC0_RAM: return "ROM+RAM";
            case MBC0_RAM_BATTERY: return "ROM+RAM+BATTERY";
            case MMM01: return "MMM01";
            case MMM01_RAM: return "MMM01+RAM";
            case MMM01_RAM_BATTERY: return "MMM01+RAM+BATTERY";
            case MBC3_RTC: return "MBC3+RTC";
            case MBC3_RTC_RAM_BATTERY: return "MBC3+RTC+RAM+BATTERY";
            case MBC3: return "MBC3";
            case MBC3_RAM: return "MBC3+RAM";
            case MBC3_RAM_BATTERY: return "MBC3+RAM+BATTERY";
            case MBC5: return "MBC5";
            case MBC5_RAM: return "MBC5+RAM";
            case MBC5_RAM_BATTERY: return "MBC5+RAM+BATTERY";
            case MBC5_RUMBLE: return "MBC5+RUMBLE";
            case MBC5_RUMBLE_RAM: return "MBC5+RUMBLE+RAM";
            case MBC5_RUMBLE_RAM_BATTERY: return "MBC5+RUMBLE+RAM+BATTERY";
            case MBC6: return "MBC6";
            case MBC7_ACCELEROMETER_RAM_BATTERY: return "MBC7+ACCELEROMETER+RAM+BATTERY";
            case POCKET_CAMERA: return "POCKET CAMERA";
            case BANDAI_TAMA5: return "BANDAI TAMA5";
            case HuC3: return "HuC3";
            case HuC1_RAM_BATTERY: return "HuC1+RAM+BATTERY";
        }
    }

    constexpr std::string decodeCartridgeType(uint8_t code) {
        return decodeCartridgeType(static_cast<MBC_TYPE>(code));
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

}