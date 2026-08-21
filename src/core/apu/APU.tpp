#pragma once
#include "APU.hpp"
#include "MemoryConstants.hpp"

namespace Audio {

    template<MODEL Model>
    template<uint16_t Register>
    uint8_t APU<Model>::readIO() {
        using namespace Memory;

        if constexpr (Register == NR52) {
            return 0x70
                | audioEnabled << 7
                | channel4.on() << 3
                | channel3.on() << 2
                | channel2.on() << 1
                | channel1.on();
        }

        if constexpr (Register == NR50) return masterVolumeControl;
        if constexpr (Register == NR51) return soundPanControl;
        if constexpr (NR10 <= Register && Register <= NR14) return channel1.readIO<Register - NR10>();
        if constexpr (NR21 <= Register && Register <= NR24) return channel2.readIO<Register - NR21 + 1>();
        if constexpr (NR30 <= Register && Register <= NR34) return channel3.readIO<Register - NR30>();
        if constexpr (NR41 <= Register && Register <= NR44) return channel4.readIO<Register - NR41 + 1>();
    }

    template<MODEL Model>
    template<uint16_t Register>
    void APU<Model>::writeIO(uint8_t value) {
        using namespace Memory;

        if constexpr (Register == NR52) {
            bool prevEnabled = audioEnabled;
            audioEnabled = value & 0x80;
            if (prevEnabled && !audioEnabled) disableAudio();
            else if (!prevEnabled && audioEnabled) enableAudio();
            return;
        }

        if (!audioEnabled) return;  // register writes (outside of NR52) ignored when powered off
        if constexpr (Register == NR50) masterVolumeControl = value;
        if constexpr (Register == NR51) soundPanControl = value;
        if constexpr (NR10 <= Register && Register <= NR14) channel1.writeIO<Register - NR10>(value);
        if constexpr (NR21 <= Register && Register <= NR24) channel2.writeIO<Register - NR21 + 1>(value);
        if constexpr (NR30 <= Register && Register <= NR34) channel3.writeIO<Register - NR30>(value);
        if constexpr (NR41 <= Register && Register <= NR44) channel4.writeIO<Register - NR41 + 1>(value);
    }

}