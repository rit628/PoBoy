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
        if constexpr (hasSRAM()) {
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
        if constexpr (hasMappedIO()) {
            // redundant ram size check for sketchy roms
            return (self.encodedRamSize == 0) ? 0xFF : self.readMappedIO(address);
        }
        else {
            return 0xFF;
        }
    }

    template<SRAM_TYPE RamType, MBC_HARDWARE AdditionalHardware>
    template<typename Self>
    inline void MBC<RamType, AdditionalHardware>::writeSRAM(this Self&& self, uint16_t address, uint8_t value) {
        if constexpr (hasMappedIO()) {
            // redundant ram size check for sketchy roms
            return (self.encodedRamSize == 0) ? void() : self.writeMappedIO(address, value);
        }
    }

    template<SRAM_TYPE RamType, MBC_HARDWARE AdditionalHardware>
    inline constexpr bool MBC<RamType, AdditionalHardware>::hasSRAM() {
        return RamType != SRAM_TYPE::NONE;
    }

    template<SRAM_TYPE RamType, MBC_HARDWARE AdditionalHardware>
    inline constexpr bool MBC<RamType, AdditionalHardware>::hasHardware() {
        return AdditionalHardware != MBC_HARDWARE::NONE;
    }

    template<SRAM_TYPE RamType, MBC_HARDWARE AdditionalHardware>
    inline constexpr bool MBC<RamType, AdditionalHardware>::hasMappedIO() {
        return hasSRAM() || hasHardware();
    }

    template<SRAM_TYPE RamType, MBC_HARDWARE AdditionalHardware>
    template<uint8_t TargetBank>
    inline void MBC<RamType, AdditionalHardware>::setRomBank(uint16_t bankNumber) {
        auto bank = rom.subspan(ROM_BANK_SIZE * bankNumber).template first<ROM_BANK_SIZE>();
        if constexpr (TargetBank == 0) bank0 = bank;
        if constexpr (TargetBank == 1) bank1 = bank;
    }

    template<SRAM_TYPE RamType, MBC_HARDWARE AdditionalHardware>
    inline void MBC<RamType, AdditionalHardware>::setRamBank(uint16_t bankNumber) {
        if constexpr (hasSRAM()) {
            sramBank = std::span(sram).subspan(SRAM_BANK_SIZE * bankNumber).template first<SRAM_BANK_SIZE>();
        }
    }

    /* MBC0 */
    template<SRAM_TYPE RamType, MBC_HARDWARE AdditionalHardware>
    MBC0<RamType, AdditionalHardware>::MBC0(std::span<uint8_t> rom, uint8_t encodedRomSize, uint8_t encodedRamSize)
                                     : MBC<RamType, AdditionalHardware>(rom, encodedRomSize, encodedRamSize) {}

    template<SRAM_TYPE RamType, MBC_HARDWARE AdditionalHardware>
    inline void MBC0<RamType, AdditionalHardware>::handleBankWrite(uint16_t, uint8_t) { /* no MBC chip */ }

    template<SRAM_TYPE RamType, MBC_HARDWARE AdditionalHardware>
    inline uint8_t MBC0<RamType, AdditionalHardware>::readMappedIO(uint16_t address) requires (Base::hasMappedIO()) {
        return this->sramBank[address];
    }

    template<SRAM_TYPE RamType, MBC_HARDWARE AdditionalHardware>
    inline void MBC0<RamType, AdditionalHardware>::writeMappedIO(uint16_t address, uint8_t value) requires (Base::hasMappedIO()) {
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

        this->template setRomBank<0>((modeFlag) ? bankHiBits : 0);
        this->template setRomBank<1>(bankHiBits | romBankNumber);
        this->setRamBank((modeFlag) ? ramBankNumber : 0);
    }

    template<SRAM_TYPE RamType, MBC_HARDWARE AdditionalHardware>
    inline uint8_t MBC1<RamType, AdditionalHardware>::readMappedIO(uint16_t address) requires (Base::hasMappedIO()) {
        if (!ramEnabled) return 0xFF;
        return this->sramBank[address];
    }

    template<SRAM_TYPE RamType, MBC_HARDWARE AdditionalHardware>
    inline void MBC1<RamType, AdditionalHardware>::writeMappedIO(uint16_t address, uint8_t value) requires (Base::hasMappedIO()) {
        if (!ramEnabled) return;
        this->sramBank[address] = value;
    }

    /* MBC3 */
    template<SRAM_TYPE RamType, MBC_HARDWARE AdditionalHardware>
    MBC3<RamType, AdditionalHardware>::MBC3(std::span<uint8_t> rom, uint8_t encodedRomSize, uint8_t encodedRamSize)
                                     : MBC<RamType, AdditionalHardware>(rom, encodedRomSize, encodedRamSize) {}

    template<SRAM_TYPE RamType, MBC_HARDWARE AdditionalHardware>
    inline constexpr bool MBC3<RamType, AdditionalHardware>::hasRTC() {
        return AdditionalHardware == MBC_HARDWARE::RTC;
    }

    template<SRAM_TYPE RamType, MBC_HARDWARE AdditionalHardware>
    inline void MBC3<RamType, AdditionalHardware>::tick() requires (hasRTC()) {
        auto& rtcDH = rtcRegisters.at(4);
        if (rtcDH & RTC_HALT_BIT) return;

        static auto rtcTick = [](auto& reg, auto mask, auto period) {
            bool overflow = ++reg == period;
            reg &= mask;
            if (overflow) reg = 0;
            return overflow;
        };
        
        if (rtcTick(cycleCount, UINT32_MAX, CYCLES_PER_SECOND)        // sub seconds
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

    template<SRAM_TYPE RamType, MBC_HARDWARE AdditionalHardware>
    inline void MBC3<RamType, AdditionalHardware>::handleBankWrite(uint16_t address, uint8_t value) {
        if (address < ENABLE_RAM_AND_RTC_REGISTER_REGION_END) {
            ramEnabled = value & 0x0A;
        }
        else if (address < ROM_BANK_SWITCH_REGION_END) {
            romBankNumber = (value > 0) ? value & 0x7F : 1;
            this->template setRomBank<1>(romBankNumber);
        }
        else if (address < RAM_BANK_AND_RTC_REGISTER_SWITCH_REGION_END) {
            ramBankNumber = (value <= 0b11) ? value : ramBankNumber;
            this->setRamBank(ramBankNumber);
            selectedRegister = (0x08 <= value && value <= 0x0C) ? value - 0x08 : UINT8_MAX;
        }
        else if (address < RTC_DATA_LATCH_REGION_END) {
            latchRtcRegisters(value);
        }
    }

    template<SRAM_TYPE RamType, MBC_HARDWARE AdditionalHardware>
    inline void MBC3<RamType, AdditionalHardware>::latchRtcRegisters(uint8_t latchCommand) {
        if constexpr (!hasRTC()) return;

        if (!latchPrimed && latchCommand == 0) {
            latchPrimed = true;
        }
        else if (latchPrimed && latchCommand == 1) {
            rtcRegisterLatches = rtcRegisters;
            latchPrimed = false;
        }
        else {
            latchPrimed = false;
        }
    }

    template<SRAM_TYPE RamType, MBC_HARDWARE AdditionalHardware>
    inline uint8_t MBC3<RamType, AdditionalHardware>::readMappedIO(uint16_t address) requires (Base::hasMappedIO()) {
        if (!ramEnabled) return 0xFF;
        if constexpr (hasRTC()) {
            if (selectedRegister < rtcRegisterLatches.size()) {
                // rtc takes precedence if mapped
                return rtcRegisterLatches.at(selectedRegister);
            }
        }
        if constexpr (Base::hasSRAM()) {
            return this->sramBank[address];
        }
        return 0xFF;
    }

    template<SRAM_TYPE RamType, MBC_HARDWARE AdditionalHardware>
    inline void MBC3<RamType, AdditionalHardware>::writeMappedIO(uint16_t address, uint8_t value) requires (Base::hasMappedIO()) {
        if (!ramEnabled) return;
        if constexpr (hasRTC()) {
            if (selectedRegister < rtcRegisterLatches.size()) {
                cycleCount = (selectedRegister == 0) ? 0 : cycleCount; // reset sub second counter when second counter is modified
                rtcRegisters.at(selectedRegister)
                = rtcRegisterLatches.at(selectedRegister)
                = value & rtcMasks.at(selectedRegister);
                return; // rtc takes precedence if mapped
            }
        }
        if constexpr (Base::hasSRAM()) { 
            this->sramBank[address] = value;
        }
    }

    /* MBC5 */
    template<SRAM_TYPE RamType, MBC_HARDWARE AdditionalHardware>
    MBC5<RamType, AdditionalHardware>::MBC5(std::span<uint8_t> rom, uint8_t encodedRomSize, uint8_t encodedRamSize)
                                     : MBC<RamType, AdditionalHardware>(rom, encodedRomSize, encodedRamSize) {}

    template<SRAM_TYPE RamType, MBC_HARDWARE AdditionalHardware>
    inline void MBC5<RamType, AdditionalHardware>::handleBankWrite(uint16_t address, uint8_t value) {
        if (address < ENABLE_RAM_REGION_END) {
            ramEnabled = value & 0x0A;
        }
        else if (address < ROM_BANK_SWITCH_LO_REGION_END) {
            romBankNumber = (romBankNumber & 0xFF00) | value;
        }
        else if (address < ROM_BANK_SWITCH_HI_REGION_END) {
            romBankNumber = (romBankNumber & 0xFF) | uint16_t(value & 0b1) << 8;
        }
        else if (address < RAM_BANK_SWITCH_REGION_END) {
            ramBankNumber = value & 0x0F;
        }
        this->template setRomBank<1>(romBankNumber);
        this->setRamBank(ramBankNumber);
    }

    template<SRAM_TYPE RamType, MBC_HARDWARE AdditionalHardware>
    inline uint8_t MBC5<RamType, AdditionalHardware>::readMappedIO(uint16_t address) requires (Base::hasMappedIO()) {
        if (!ramEnabled) return 0xFF;
        return this->sramBank[address];
    }

    template<SRAM_TYPE RamType, MBC_HARDWARE AdditionalHardware>
    inline void MBC5<RamType, AdditionalHardware>::writeMappedIO(uint16_t address, uint8_t value) requires (Base::hasMappedIO()) {
        if (!ramEnabled) return;
        this->sramBank[address] = value;
    }
}
