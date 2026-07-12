#pragma once
#include "GraphicsConstants.hpp"
#include <cstdint>

namespace Audio {

    constexpr uint32_t SAMPLE_RATE                  = 4194304;
    constexpr uint8_t  BITS_PER_SAMPLE              = 4;
    constexpr uint8_t  SAMPLES_PER_BYTE             = 8 / BITS_PER_SAMPLE;
    constexpr uint8_t  DIGITAL_SAMPLE_MAX           = (1 << BITS_PER_SAMPLE) - 1;

    constexpr uint8_t  CHANNEL_COUNT                = 4;
    constexpr float    VOLUME_MAX                   = 8.0f;
}