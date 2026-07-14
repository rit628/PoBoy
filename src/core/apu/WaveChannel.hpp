#pragma once
#include "Channel.hpp"
#include <array>
#include <cstdint>

namespace Audio {

    class WaveChannel : public Channel<VOLUME_TYPE::SHIFT, TICK_RATE::HALF_M_CYCLE> {
        friend class Channel<VOLUME_TYPE::SHIFT, TICK_RATE::HALF_M_CYCLE>;
        public:
            template<uint8_t Register>
            uint8_t readIO();
            template<uint8_t Register>
            void writeIO(uint8_t value);
            uint8_t readWaveRAM(uint8_t address);
            void writeWaveRAM(uint8_t address, uint8_t value);
        
        private:
            void advanceOutput();
            uint8_t sample();
            void trigger();

            static constexpr uint8_t WAVE_RAM_SIZE          = 0x10;
            static constexpr uint8_t WAVE_RAM_INDEX_MAX     = 2 * WAVE_RAM_SIZE - 1;

            std::array<uint8_t, WAVE_RAM_SIZE> waveram;
            uint8_t currentOutputLevel = 0;
            uint8_t waveRamIndex = 0;
            uint8_t currentSample = 0;

            /* NR30 Register Components */
            bool dacEnable = false;     // NR30 bit 7

            /* NR32 Register Components */
            uint8_t outputLevel = 0;    // NR32 bits 6-5
    };

}
