#pragma once
#include "MemoryConstants.hpp"
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
            uint8_t readSRAM(uint16_t address);
            void writeSRAM(uint16_t address, uint8_t value);

        protected:
            MBC(std::span<uint8_t> rom, uint8_t encodedRomSize, uint8_t encodedRamSize);

            std::span<uint8_t> rom;
            uint8_t encodedRomSize = 0, encodedRamSize = 0;
            bool ramEnabled = false;

            std::vector<uint8_t> sram;
            std::span<uint8_t, ROM_BANK_SIZE> bank0, bank1;
            std::span<uint8_t, SRAM_BANK_SIZE> sramBank;
    };

    template<SRAM_TYPE RamType = SRAM_TYPE::NONE, MBC_HARDWARE AdditionalHardware = MBC_HARDWARE::NONE>
    class MBC0 : public MBC<RamType, AdditionalHardware> {
        friend class MBC<RamType, AdditionalHardware>;
        public:
            MBC0(std::span<uint8_t> rom, uint8_t encodedRomSize, uint8_t encodedRamSize);

        private:
            void handleWrite(uint16_t address, uint8_t value);

    };

    template<SRAM_TYPE RamType = SRAM_TYPE::NONE, MBC_HARDWARE AdditionalHardware = MBC_HARDWARE::NONE>
    class MBC1 : public MBC<RamType, AdditionalHardware> {
        friend class MBC<RamType, AdditionalHardware>;
        public:
            MBC1(std::span<uint8_t> rom, uint8_t encodedRomSize, uint8_t encodedRamSize);

        private:
            static constexpr uint16_t ENABLE_RAM_REGION_END         = 0x2000;
            static constexpr uint16_t ROM_BANK_SWITCH_REGION_END    = 0x4000;
            static constexpr uint16_t RAM_BANK_SWITCH_REGION_END    = 0x6000;
            static constexpr uint16_t MODE_FLAG_REGION_END          = 0x8000;

            void handleWrite(uint16_t address, uint8_t value);
            void updateBanks();

            uint8_t romBankNumber = 1;
            uint8_t ramBankNumber = 1;
            bool modeFlag = false;

    };
    
    using MemoryBankController = std::variant<MBC0<>
                                            , MBC0<SRAM_TYPE::UNBUFFERED>
                                            , MBC0<SRAM_TYPE::BATTERY_BUFFERED>
                                            , MBC1<>
                                            , MBC1<SRAM_TYPE::UNBUFFERED>
                                            , MBC1<SRAM_TYPE::BATTERY_BUFFERED>>;

}

#include "MBC.tpp"