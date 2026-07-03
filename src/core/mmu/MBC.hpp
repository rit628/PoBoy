#pragma once
#include "MemoryConstants.hpp"
#include <array>
#include <cstdint>
#include <span>
#include <variant>
#include <vector>

namespace Memory {

    enum class SRAM_TYPE { NONE, UNBUFFERED, BATTERY_BUFFERED };
    enum class MBC_HARDWARE { NONE, RTC, RUMBLE, ACCELEROMETER };

    template<SRAM_TYPE RamType = SRAM_TYPE::NONE, MBC_HARDWARE AdditionalHardware = MBC_HARDWARE::NONE>
    class MBC {
        public:
            uint8_t readBank0(uint16_t address);
            template<typename Self>
            void writeBank0(this Self&& self, uint16_t address, uint8_t value);
            uint8_t readBank1(uint16_t address);
            template<typename Self>
            void writeBank1(this Self&& self, uint16_t address, uint8_t value);
            template<typename Self>
            uint8_t readSRAM(this Self&& self, uint16_t address);
            template<typename Self>
            void writeSRAM(this Self&& self, uint16_t address, uint8_t value);

        protected:
            MBC(std::span<uint8_t> rom, uint8_t encodedRomSize, uint8_t encodedRamSize);
            static constexpr bool hasSRAM();
            static constexpr bool hasHardware();
            static constexpr bool hasMappedIO();
            template<uint8_t TargetBank>
            void setRomBank(uint16_t bankNumber);
            void setRamBank(uint16_t bankNumber);

            std::span<uint8_t> rom;
            uint8_t encodedRomSize = 0, encodedRamSize = 0;

            std::vector<uint8_t> sram;
            std::span<uint8_t, ROM_BANK_SIZE> bank0, bank1;
            std::span<uint8_t, SRAM_BANK_SIZE> sramBank;
    };

    template<SRAM_TYPE RamType = SRAM_TYPE::NONE, MBC_HARDWARE AdditionalHardware = MBC_HARDWARE::NONE>
    class MBC0 : public MBC<RamType, AdditionalHardware> {
        using Base = MBC<RamType, AdditionalHardware>;
        friend class MBC<RamType, AdditionalHardware>;
        public:
            MBC0(std::span<uint8_t> rom, uint8_t encodedRomSize, uint8_t encodedRamSize);

        private:
            void handleBankWrite(uint16_t address, uint8_t value);
            uint8_t readMappedIO(uint16_t address) requires (Base::hasMappedIO());
            void writeMappedIO(uint16_t address, uint8_t value) requires (Base::hasMappedIO());
    };

    template<SRAM_TYPE RamType = SRAM_TYPE::NONE, MBC_HARDWARE AdditionalHardware = MBC_HARDWARE::NONE>
    class MBC1 : public MBC<RamType, AdditionalHardware> {
        using Base = MBC<RamType, AdditionalHardware>;
        friend class MBC<RamType, AdditionalHardware>;
        public:
            MBC1(std::span<uint8_t> rom, uint8_t encodedRomSize, uint8_t encodedRamSize);

        private:
            static constexpr uint16_t ENABLE_RAM_REGION_END         = 0x2000;
            static constexpr uint16_t ROM_BANK_SWITCH_REGION_END    = 0x4000;
            static constexpr uint16_t RAM_BANK_SWITCH_REGION_END    = 0x6000;
            static constexpr uint16_t MODE_FLAG_REGION_END          = 0x8000;

            void handleBankWrite(uint16_t address, uint8_t value);
            uint8_t readMappedIO(uint16_t address) requires (Base::hasMappedIO());
            void writeMappedIO(uint16_t address, uint8_t value) requires (Base::hasMappedIO());
            void updateBanks();

            uint8_t romBankNumber = 1;
            uint8_t ramBankNumber = 1;
            bool ramEnabled = false;
            bool modeFlag = false;
    };

    template<SRAM_TYPE RamType = SRAM_TYPE::NONE, MBC_HARDWARE AdditionalHardware = MBC_HARDWARE::NONE>
    class MBC3 : public MBC<RamType, AdditionalHardware> {
        using Base = MBC<RamType, AdditionalHardware>;
        friend class MBC<RamType, AdditionalHardware>;
        public:
            MBC3(std::span<uint8_t> rom, uint8_t encodedRomSize, uint8_t encodedRamSize);
            static constexpr bool hasRTC();
            void tick() requires (hasRTC());

        private:
            static constexpr uint16_t ENABLE_RAM_AND_RTC_REGISTER_REGION_END        = 0x2000;
            static constexpr uint16_t ROM_BANK_SWITCH_REGION_END                    = 0x4000;
            static constexpr uint16_t RAM_BANK_AND_RTC_REGISTER_SWITCH_REGION_END   = 0x6000;
            static constexpr uint16_t RTC_DATA_LATCH_REGION_END                     = 0x8000;
            static constexpr uint32_t CYCLES_PER_SECOND                             = 4194304; // synced with soc clock
            static constexpr uint8_t  RTC_HALT_BIT                                  = 0x40;

            void handleBankWrite(uint16_t address, uint8_t value);
            uint8_t readMappedIO(uint16_t address) requires (Base::hasMappedIO());
            void writeMappedIO(uint16_t address, uint8_t value) requires (Base::hasMappedIO());
            void latchRtcRegisters(uint8_t latchCommand);

            uint8_t romBankNumber = 1;
            uint8_t ramBankNumber = 1;
            bool ramEnabled = false;

            uint32_t cycleCount = 0;
            bool latchPrimed = false;
            uint8_t selectedRegister = 0;
            std::array<uint8_t, 5> rtcRegisters{}, rtcRegisterLatches{}; // rtcS, rtcM, rtcH, rtcDL, rtcDH
            static constexpr std::array<uint8_t, 5> rtcMasks = {0x3F, 0x3F, 0x1F, 0xFF, 0xC1};
    };

    template<SRAM_TYPE RamType = SRAM_TYPE::NONE, MBC_HARDWARE AdditionalHardware = MBC_HARDWARE::NONE>
    class MBC5 : public MBC<RamType, AdditionalHardware> {
        using Base = MBC<RamType, AdditionalHardware>;
        friend class MBC<RamType, AdditionalHardware>;
        public:
            MBC5(std::span<uint8_t> rom, uint8_t encodedRomSize, uint8_t encodedRamSize);

        private:
            static constexpr uint16_t ENABLE_RAM_REGION_END         = 0x2000;
            static constexpr uint16_t ROM_BANK_SWITCH_LO_REGION_END = 0x3000;
            static constexpr uint16_t ROM_BANK_SWITCH_HI_REGION_END = 0x4000;
            static constexpr uint16_t RAM_BANK_SWITCH_REGION_END    = 0x6000;

            void handleBankWrite(uint16_t address, uint8_t value);
            uint8_t readMappedIO(uint16_t address) requires (Base::hasMappedIO());
            void writeMappedIO(uint16_t address, uint8_t value) requires (Base::hasMappedIO());

            uint16_t romBankNumber = 1;
            uint8_t ramBankNumber = 1;
            bool ramEnabled = false;
    };
    
    using MemoryBankController = std::variant<MBC0<>
                                            , MBC0<SRAM_TYPE::UNBUFFERED>
                                            , MBC0<SRAM_TYPE::BATTERY_BUFFERED>
                                            , MBC1<>
                                            , MBC1<SRAM_TYPE::UNBUFFERED>
                                            , MBC1<SRAM_TYPE::BATTERY_BUFFERED>
                                            , MBC3<>
                                            , MBC3<SRAM_TYPE::UNBUFFERED>
                                            , MBC3<SRAM_TYPE::BATTERY_BUFFERED>
                                            , MBC3<SRAM_TYPE::NONE, MBC_HARDWARE::RTC>
                                            , MBC3<SRAM_TYPE::BATTERY_BUFFERED, MBC_HARDWARE::RTC>
                                            , MBC5<>
                                            , MBC5<SRAM_TYPE::UNBUFFERED>
                                            , MBC5<SRAM_TYPE::BATTERY_BUFFERED>
                                            , MBC5<SRAM_TYPE::NONE, MBC_HARDWARE::RUMBLE>
                                            , MBC5<SRAM_TYPE::BATTERY_BUFFERED, MBC_HARDWARE::RUMBLE>>;

}

#include "MBC.tpp"