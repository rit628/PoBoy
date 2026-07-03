#pragma once
#include "MBC.hpp"
#include "MemoryConstants.hpp"
#include <cstdint>

namespace Memory {

    template<SRAM_TYPE RamType, MBC_HARDWARE AdditionalHardware>
    MBC<RamType, AdditionalHardware>::MBC(std::span<uint8_t> rom, uint8_t encodedRomSize, uint8_t encodedRamSize)
                                         : rom(rom), encodedRomSize(encodedRomSize), encodedRamSize(encodedRamSize)
                                         , bank0(rom.subspan<0, ROM_BANK_SIZE>()), bank1(rom.subspan<ROM_BANK_1_START, ROM_BANK_SIZE>())
                                         , sramBank(rom.subspan<0, SRAM_BANK_SIZE>()) // just to prevent some builds from throwing a fit about ub
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
        self.handleBankWrite(address, value);
    }

    template<SRAM_TYPE RamType, MBC_HARDWARE AdditionalHardware>
    inline uint8_t MBC<RamType, AdditionalHardware>::readBank1(uint16_t address) {
        return bank1[address];
    }

    template<SRAM_TYPE RamType, MBC_HARDWARE AdditionalHardware>
    template<typename Self>
    inline void MBC<RamType, AdditionalHardware>::writeBank1(this Self&& self, uint16_t address, uint8_t value) {
        self.handleBankWrite(address + ROM_BANK_SIZE, value);
    }

    template<SRAM_TYPE RamType, MBC_HARDWARE AdditionalHardware>
    template<typename Self>
    inline uint8_t MBC<RamType, AdditionalHardware>::readSRAM(this Self&& self, uint16_t address) {
        if constexpr (RamType == SRAM_TYPE::NONE && AdditionalHardware == MBC_HARDWARE::NONE) return 0xFF;
        if constexpr (AdditionalHardware == MBC_HARDWARE::NONE) if (self.encodedRamSize == 0) return 0xFF; // extra check for sketchy roms
        return self.readMappedHardware(address);
    }

    template<SRAM_TYPE RamType, MBC_HARDWARE AdditionalHardware>
    template<typename Self>
    inline void MBC<RamType, AdditionalHardware>::writeSRAM(this Self&& self, uint16_t address, uint8_t value) {
        if constexpr (RamType == SRAM_TYPE::NONE && AdditionalHardware == MBC_HARDWARE::NONE) return;
        if constexpr (AdditionalHardware == MBC_HARDWARE::NONE) if (self.encodedRamSize == 0) return; // extra check for sketchy roms
        self.writeMappedHardware(address, value);
    }

    /* MBC0 */
    template<SRAM_TYPE RamType, MBC_HARDWARE AdditionalHardware>
    MBC0<RamType, AdditionalHardware>::MBC0(std::span<uint8_t> rom, uint8_t encodedRomSize, uint8_t encodedRamSize)
                                     : MBC<RamType, AdditionalHardware>(rom, encodedRomSize, encodedRamSize) {}

    template<SRAM_TYPE RamType, MBC_HARDWARE AdditionalHardware>
    inline void MBC0<RamType, AdditionalHardware>::handleBankWrite(uint16_t, uint8_t) { /* no MBC chip */ }

    template<SRAM_TYPE RamType, MBC_HARDWARE AdditionalHardware>
    inline uint8_t MBC0<RamType, AdditionalHardware>::readMappedHardware(uint16_t address) {
        return this->sramBank[address];
    }

    template<SRAM_TYPE RamType, MBC_HARDWARE AdditionalHardware>
    inline void MBC0<RamType, AdditionalHardware>::writeMappedHardware(uint16_t address, uint8_t value) {
        this->sramBank[address] = value;
    }

    /* MBC1 */
    template<SRAM_TYPE RamType, MBC_HARDWARE AdditionalHardware>
    MBC1<RamType, AdditionalHardware>::MBC1(std::span<uint8_t> rom, uint8_t encodedRomSize, uint8_t encodedRamSize)
                                     : MBC<RamType, AdditionalHardware>(rom, encodedRomSize, encodedRamSize) {}

    template<SRAM_TYPE RamType, MBC_HARDWARE AdditionalHardware>
    inline void MBC1<RamType, AdditionalHardware>::handleBankWrite(uint16_t address, uint8_t value) {
        if (address < ENABLE_RAM_REGION_END) {
            ramEnabled = value & 0x0A;
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

    template<SRAM_TYPE RamType, MBC_HARDWARE AdditionalHardware>
    inline uint8_t MBC1<RamType, AdditionalHardware>::readMappedHardware(uint16_t address) {
        if (!ramEnabled) return 0xFF;
        return this->sramBank[address];
    }

    template<SRAM_TYPE RamType, MBC_HARDWARE AdditionalHardware>
    inline void MBC1<RamType, AdditionalHardware>::writeMappedHardware(uint16_t address, uint8_t value) {
        if (!ramEnabled) return;
        this->sramBank[address] = value;
    }

    /* MBC3 */
    template<SRAM_TYPE RamType, MBC_HARDWARE AdditionalHardware>
    MBC3<RamType, AdditionalHardware>::MBC3(std::span<uint8_t> rom, uint8_t encodedRomSize, uint8_t encodedRamSize)
                                     : MBC<RamType, AdditionalHardware>(rom, encodedRomSize, encodedRamSize) {}

    template<SRAM_TYPE RamType, MBC_HARDWARE AdditionalHardware>
    inline void MBC3<RamType, AdditionalHardware>::tick() {
        if constexpr (AdditionalHardware == MBC_HARDWARE::RTC) {
            auto& rtcDH = rtcRegisters.at(4);
            if (rtcDH & RTC_HALT_BIT) return;

            static auto rtcTick = [](auto& reg, auto mask, auto period) {
                bool overflow = ++reg == period;
                reg &= mask;
                if (overflow) reg = 0;
                return overflow;
            };
            
            if (rtcTick(cycleCount, UINT32_MAX, CYCLES_PER_SECOND)     // sub seconds
             && rtcTick(rtcRegisters.at(0), rtcMasks.at(0), 60)   // seconds
             && rtcTick(rtcRegisters.at(1), rtcMasks.at(1), 60)   // minutes
             && rtcTick(rtcRegisters.at(2), rtcMasks.at(2), 24)   // hours
             && rtcTick(rtcRegisters.at(3), rtcMasks.at(3), 0)    // days lo 8 bits
             && (++rtcDH & 1) == 0                                     // days 9th bit
            ) {
                // set carry bit and reset 9th bit
                rtcDH = (rtcDH | 1 << 7) & rtcMasks.at(4);
            }
        }
    }

    template<SRAM_TYPE RamType, MBC_HARDWARE AdditionalHardware>
    inline void MBC3<RamType, AdditionalHardware>::handleBankWrite(uint16_t address, uint8_t value) {
        if (address < ENABLE_RAM_AND_RTC_REGISTER_REGION_END) {
            ramEnabled = value & 0x0A;
        }
        else if (address < ROM_BANK_SWITCH_REGION_END) {
            romBankNumber = (value > 0) ? value & 0x7F : 1;
            this->bank1 = this->rom.subspan(ROM_BANK_SIZE * romBankNumber).template first<ROM_BANK_SIZE>();
        }
        else if (address < RAM_BANK_AND_RTC_REGISTER_SWITCH_REGION_END) {
            if constexpr (RamType != SRAM_TYPE::NONE) {
                ramBankNumber = (value <= 0b11) ? value : ramBankNumber;
                this->sramBank = std::span(this->sram).subspan(SRAM_BANK_SIZE * ramBankNumber).template first<SRAM_BANK_SIZE>();
            }
            if constexpr (AdditionalHardware == MBC_HARDWARE::RTC) {
                selectedRegister = (0x08 <= value && value <= 0x0C) ? value - 0x08 : UINT8_MAX;
            }
        }
        else if (address < RTC_DATA_LATCH_REGION_END) {
            if constexpr (AdditionalHardware == MBC_HARDWARE::RTC) {
                if (!latchPrimed && value == 0) {
                    latchPrimed = true;
                }
                else if (latchPrimed && value == 1) {
                    rtcRegisterLatches = rtcRegisters;
                    latchPrimed = false;
                }
                else {
                    latchPrimed = false;
                }
            }
        }
    }

    template<SRAM_TYPE RamType, MBC_HARDWARE AdditionalHardware>
    inline uint8_t MBC3<RamType, AdditionalHardware>::readMappedHardware(uint16_t address) {
        if (!ramEnabled) return 0xFF;
        if constexpr (AdditionalHardware == MBC_HARDWARE::RTC) {
            if (selectedRegister < rtcRegisterLatches.size()) { // rtc takes precedence if mapped
                return rtcRegisterLatches.at(selectedRegister);
            }
        }
        if constexpr (RamType != SRAM_TYPE::NONE) return this->sramBank[address];
        return 0xFF;
    }

    template<SRAM_TYPE RamType, MBC_HARDWARE AdditionalHardware>
    inline void MBC3<RamType, AdditionalHardware>::writeMappedHardware(uint16_t address, uint8_t value) {
        if (!ramEnabled) return;
        if constexpr (AdditionalHardware == MBC_HARDWARE::RTC) {
            if (selectedRegister < rtcRegisterLatches.size()) { // rtc takes precedence if mapped
                if (selectedRegister == 0) cycleCount = 0; // reset sub second counter when second counter is modified
                rtcRegisters.at(selectedRegister)
                = rtcRegisterLatches.at(selectedRegister)
                = value & rtcMasks.at(selectedRegister);
                return;
            }
        }
        if constexpr (RamType != SRAM_TYPE::NONE) this->sramBank[address] = value;
    }
}
