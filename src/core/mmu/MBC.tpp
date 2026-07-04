#pragma once
#include "MBC.hpp"
#include "MemoryConstants.hpp"
#include <cstdint>

namespace Memory {

    template<SRAM_TYPE RamType, MBC_HARDWARE AdditionalHardware>
    MBC<RamType, AdditionalHardware>::MBC(std::span<uint8_t> rom, uint8_t encodedRomSize, uint8_t encodedRamSize)
                                         : rom(rom)
                                         , romBankCount(decodeRomBankCount(encodedRomSize))
                                         , ramBankCount(decodeRamBankCount(encodedRamSize))
                                         , bank0(rom.subspan<0, ROM_BANK_SIZE>())
                                         , bank1(rom.subspan<ROM_BANK_1_START, ROM_BANK_SIZE>())
                                         , sramBank(rom.subspan<0, SRAM_BANK_SIZE>()) // just to prevent some builds from throwing a fit about ub
    {
        if constexpr (hasSRAM()) {
            auto ramSize = decodeRamSize(encodedRamSize);
            sram.resize(ramSize, 0xFF);
            if (ramSize < SRAM_BANK_SIZE) return; // prevent span ub for mbc2 and roms with header mismatch
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
            // redundant ram size check for roms with header mismatch
            return (self.sram.size() == 0) ? 0xFF : self.readMappedIO(address);
        }
        else {
            return 0xFF;
        }
    }

    template<SRAM_TYPE RamType, MBC_HARDWARE AdditionalHardware>
    template<typename Self>
    inline void MBC<RamType, AdditionalHardware>::writeSRAM(this Self&& self, uint16_t address, uint8_t value) {
        if constexpr (hasMappedIO()) {
            // redundant ram size check for roms with header mismatch
            return (self.sram.size() == 0) ? void() : self.writeMappedIO(address, value);
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
        uint16_t wrappedBank = bankNumber & (romBankCount - 1);
        auto bank = rom.subspan(ROM_BANK_SIZE * wrappedBank).template first<ROM_BANK_SIZE>();
        if constexpr (TargetBank == 0) bank0 = bank;
        if constexpr (TargetBank == 1) bank1 = bank;
    }

    template<SRAM_TYPE RamType, MBC_HARDWARE AdditionalHardware>
    inline void MBC<RamType, AdditionalHardware>::setRamBank(uint16_t bankNumber) {
        if constexpr (hasSRAM()) {
            uint16_t wrappedBank = bankNumber & (ramBankCount - 1);
            sramBank = std::span(sram).subspan(SRAM_BANK_SIZE * wrappedBank).template first<SRAM_BANK_SIZE>();
        }
    }

    template<SRAM_TYPE RamType, MBC_HARDWARE AdditionalHardware>
    bool MBC<RamType, AdditionalHardware>::enableRam(uint8_t ramGateRegister) {
        return (ramGateRegister & 0x0F) == 0x0A;
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
            ramEnabled = Base::enableRam(value);
        }
        else if (address < ROM_BANK_SWITCH_REGION_END) {
            romBankNumber = std::max(1, value & 0x1F);  // bits 5 and 6 are determined by ram bank number
        }
        else if (address < RAM_BANK_SWITCH_REGION_END) {
            ramBankNumber = value & 0b11;
        }
        else if (address < MODE_FLAG_REGION_END) {
            modeFlag = value & 0b1;
        }
        uint8_t bankHiBits = ramBankNumber << 5;
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

    /* MBC2 */
    template<SRAM_TYPE RamType, MBC_HARDWARE AdditionalHardware>
    MBC2<RamType, AdditionalHardware>::MBC2(std::span<uint8_t> rom, uint8_t encodedRomSize, uint8_t encodedRamSize)
                                     : MBC<RamType, AdditionalHardware>(rom, encodedRomSize, encodedRamSize)
    { 
        /* mbc2 always comes with 512 half bytes of sram */
        this->sram.resize(512, 0xFF);
    }

    template<SRAM_TYPE RamType, MBC_HARDWARE AdditionalHardware>
    inline void MBC2<RamType, AdditionalHardware>::handleBankWrite(uint16_t address, uint8_t value) {
        if (address < ENABLE_RAM_AND_ROM_BANK_SWITCH_REGION_END) {
            bool writeRomBank = address & 0x100;
            if (writeRomBank) {
                romBankNumber = std::max(1, value & 0x0F);
                this->template setRomBank<1>(romBankNumber);
            }
            else {
                ramEnabled = Base::enableRam(value);
            }
        }
    }

    template<SRAM_TYPE RamType, MBC_HARDWARE AdditionalHardware>
    inline uint8_t MBC2<RamType, AdditionalHardware>::readMappedIO(uint16_t address) requires (Base::hasMappedIO()) {
        if (!ramEnabled) return 0xFF;
        return this->sram[address & 0x1FF] | 0xF0; // mbc2 ram only has 512 4 bit addresses
    }

    template<SRAM_TYPE RamType, MBC_HARDWARE AdditionalHardware>
    inline void MBC2<RamType, AdditionalHardware>::writeMappedIO(uint16_t address, uint8_t value) requires (Base::hasMappedIO()) {
        if (!ramEnabled) return;
        this->sram[address & 0x1FF] = value & 0x0F; // mbc2 ram only has 512 4 bit addresses
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
            ramEnabled = Base::enableRam(value);
        }
        else if (address < ROM_BANK_SWITCH_REGION_END) {
            romBankNumber = std::max(1, value & 0x7F);
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
            ramEnabled = Base::enableRam(value);
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
