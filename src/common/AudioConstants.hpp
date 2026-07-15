#pragma once
#include <cstdint>

namespace Audio {

    constexpr uint32_t INTERNAL_SAMPLE_RATE         = 1 << 22;
    constexpr uint32_t OUTPUT_SAMPLE_RATE           = 1 << 17;  // factor of internal rate for trivial downsampling and filtering
    constexpr uint8_t  SAMPLES_TO_DISCARD           = INTERNAL_SAMPLE_RATE / OUTPUT_SAMPLE_RATE;

    constexpr uint8_t  BITS_PER_SAMPLE              = 4;
    constexpr uint8_t  SAMPLES_PER_BYTE             = 8 / BITS_PER_SAMPLE;
    constexpr uint8_t  DIGITAL_SAMPLE_MAX           = (1 << BITS_PER_SAMPLE) - 1;

    constexpr uint8_t  CHANNEL_COUNT                = 4;
    constexpr float    VOLUME_MAX                   = 8.0f;
}