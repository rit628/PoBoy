#pragma once
#include <cstdint>

namespace Graphics {

    struct Pixel {
        uint8_t color;
        uint8_t palette;    // unused in dmg bg fetcher
        uint8_t sourceObject;   // unused in dmg pipeline
        uint8_t priority;   // unused in dmg bg fetcher
    };

}