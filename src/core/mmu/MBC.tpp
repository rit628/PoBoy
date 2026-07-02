#pragma once
#include "MBC.hpp"
#include "MemoryConstants.hpp"
#include <cstdint>

namespace Memory {

    template<SRAM_TYPE RamType, MBC_HARDWARE AdditionalHardware>
    MBC<RamType, AdditionalHardware>::MBC(std::span<uint8_t> rom, uint8_t encodedRomSize, uint8_t encodedRamSize)
                                         : rom(rom), encodedRomSize(encodedRomSize), encodedRamSize(encodedRamSize)
                                         , bank0(rom.subspan<0, ROM_BANK_SIZE>()), bank1(rom.subspan<ROM_BANK_1_START, ROM_BANK_SIZE>())
                                         , sramBank(rom.subspan<0, SRAM_BANK_SIZE>()) // just to avoid ub
    {
        if constexpr (RamType != SRAM_TYPE::NONE) {
            sram.resize(decodeRamSize(encodedRamSize), 0xFF);
            sramBank = std::span(sram).subspan<0, SRAM_BANK_SIZE>();
        }
    }

    template<SRAM_TYPE RamType, MBC_HARDWARE AdditionalHardware>
    inline uint8_t MBC<RamType, AdditionalHardware>::readBank0(uint16_t address) {
        return bank0[address];
    }

    template<SRAM_TYPE RamType, MBC_HARDWARE AdditionalHardware>
    template<typename Self>
    inline void MBC<RamType, AdditionalHardware>::writeBank0(this Self&& self, uint16_t address, uint8_t value) {
        self.handleWrite(address, value);
    }

    template<SRAM_TYPE RamType, MBC_HARDWARE AdditionalHardware>
    inline uint8_t MBC<RamType, AdditionalHardware>::readBank1(uint16_t address) {
        return bank1[address];
    }

    template<SRAM_TYPE RamType, MBC_HARDWARE AdditionalHardware>
    template<typename Self>
    inline void MBC<RamType, AdditionalHardware>::writeBank1(this Self&& self, uint16_t address, uint8_t value) {
        self.handleWrite(address + ROM_BANK_SIZE, value);
    }

    template<SRAM_TYPE RamType, MBC_HARDWARE AdditionalHardware>
    inline uint8_t MBC<RamType, AdditionalHardware>::readSRAM(uint16_t address) {
        if constexpr (RamType == SRAM_TYPE::NONE) return 0xFF;
        return (ramEnabled) ? sramBank[address] : 0xFF;
    }

    template<SRAM_TYPE RamType, MBC_HARDWARE AdditionalHardware>
    inline void MBC<RamType, AdditionalHardware>::writeSRAM(uint16_t address, uint8_t value) {
        if constexpr (RamType == SRAM_TYPE::NONE) return;
        if (ramEnabled) sramBank[address] = value;
    }

    template<SRAM_TYPE RamType, MBC_HARDWARE AdditionalHardware>
    MBC0<RamType, AdditionalHardware>::MBC0(std::span<uint8_t> rom, uint8_t encodedRomSize, uint8_t encodedRamSize)
                                     : MBC<RamType, AdditionalHardware>(rom, encodedRomSize, encodedRamSize)
    {
        if constexpr (RamType != SRAM_TYPE::NONE) {
            this->ramEnabled = true; // no MBC chip available to map ram dynamically
        }
    }

    template<SRAM_TYPE RamType, MBC_HARDWARE AdditionalHardware>
    inline void MBC0<RamType, AdditionalHardware>::handleWrite(uint16_t, uint8_t) { /* no MBC chip */ }

    template<SRAM_TYPE RamType, MBC_HARDWARE AdditionalHardware>
    MBC1<RamType, AdditionalHardware>::MBC1(std::span<uint8_t> rom, uint8_t encodedRomSize, uint8_t encodedRamSize)
                                     : MBC<RamType, AdditionalHardware>(rom, encodedRomSize, encodedRamSize) {}

    template<SRAM_TYPE RamType, MBC_HARDWARE AdditionalHardware>
    inline void MBC1<RamType, AdditionalHardware>::handleWrite(uint16_t address, uint8_t value) {
        if (address < ENABLE_RAM_REGION_END) {
            this->ramEnabled = value & 0x0A;
        }
        else if (address < ROM_BANK_SWITCH_REGION_END) {
            uint8_t bankMask = (0xFF >> (7 - this->encodedRomSize)) & 0x1F; // bits 5 and 6 are determined by ram bank number
            romBankNumber = (value > 0) ? value & bankMask : 1;
        }
        else if (address < RAM_BANK_SWITCH_REGION_END) {
            ramBankNumber = value & 0b11;
        }
        else if (address < MODE_FLAG_REGION_END) {
            modeFlag = value & 0b1;
        }
        updateBanks();
    }

    template<SRAM_TYPE RamType, MBC_HARDWARE AdditionalHardware>
    inline void MBC1<RamType, AdditionalHardware>::updateBanks() {
        uint8_t bankHiBits = 0;
        if (this->encodedRomSize == 5) bankHiBits = ramBankNumber & 0b1; // 1MB ROM
        else if (this->encodedRomSize == 6) bankHiBits = ramBankNumber;  // 2MB ROM
        bankHiBits <<= 5;

        uint8_t selectedBank0 = (modeFlag) ? bankHiBits : 0; // only switch bank 0 if mode flag is enabled
        this->bank0 = this->rom.subspan(ROM_BANK_SIZE * selectedBank0).template first<ROM_BANK_SIZE>();

        uint8_t selectedBank1 = bankHiBits | romBankNumber;
        this->bank1 = this->rom.subspan(ROM_BANK_SIZE * selectedBank1).template first<ROM_BANK_SIZE>();

        if constexpr (RamType != SRAM_TYPE::NONE) {
            uint8_t selectedRamBank = (modeFlag) ? ramBankNumber : 0;
            this->sramBank = std::span(this->sram).subspan(SRAM_BANK_SIZE * selectedRamBank).template first<SRAM_BANK_SIZE>();
        }
    }

}
